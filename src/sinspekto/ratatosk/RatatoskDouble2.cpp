#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskDouble2.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskDouble2Subscriber::RatatoskDouble2Subscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskDouble2Subscriber::~RatatoskDouble2Subscriber() = default;

void RatatoskDouble2Subscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::Double2>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::Double2>(
        std::bind(&RatatoskDouble2Subscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskDouble2Subscriber::eventHeard, this, &RatatoskDouble2Subscriber::updateValue);
  }
}

QVector2D RatatoskDouble2Subscriber::value() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector2D(nan,nan);
  }
  return QVector2D(
      m_reader->sample.x(),
      m_reader->sample.y());
}

QDateTime RatatoskDouble2Subscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskDouble2Subscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::Double2> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit valueChanged(QVector2D(
           m_reader->sample.x(),
           m_reader->sample.y()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


RatatoskDouble2Publisher::RatatoskDouble2Publisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

RatatoskDouble2Publisher::~RatatoskDouble2Publisher() = default;

void RatatoskDouble2Publisher::init(
    QtToDds* dds,
    const QString& topic,
    QVector2D start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<ratatosk::types::Double2>>(dds, topic);

  m_writer->sample.x() = start_value.x();
  m_writer->sample.y() = start_value.y();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

QVector2D RatatoskDouble2Publisher::value() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector2D(nan,nan);
  }
  return QVector2D(
      m_writer->sample.x(),
      m_writer->sample.y());
}

void RatatoskDouble2Publisher::setValue(QVector2D value)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.x(), static_cast<double>(value.x())) &&
      qFuzzyCompare(m_writer->sample.y(), static_cast<double>(value.y())))
    return;
  else
  {
    m_writer->sample.x() = value.x();
    m_writer->sample.y() = value.y();
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void RatatoskDouble2Publisher::publish()
{
  m_writer->writer << m_writer->sample;
}
