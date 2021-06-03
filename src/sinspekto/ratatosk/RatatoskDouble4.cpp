#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskDouble4.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskDouble4Subscriber::RatatoskDouble4Subscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskDouble4Subscriber::~RatatoskDouble4Subscriber() = default;

void RatatoskDouble4Subscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::Double4>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::Double4>(
        std::bind(&RatatoskDouble4Subscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskDouble4Subscriber::eventHeard, this, &RatatoskDouble4Subscriber::updateValue);
  }
}

QVector4D RatatoskDouble4Subscriber::value() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector4D(nan,nan,nan,nan);
  }
  return QVector4D(
      m_reader->sample.x(),
      m_reader->sample.y(),
      m_reader->sample.z(),
      m_reader->sample.w());
}

QDateTime RatatoskDouble4Subscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskDouble4Subscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::Double4> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit valueChanged(QVector4D(
           m_reader->sample.x(),
           m_reader->sample.y(),
           m_reader->sample.z(),
           m_reader->sample.w()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


RatatoskDouble4Publisher::RatatoskDouble4Publisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

RatatoskDouble4Publisher::~RatatoskDouble4Publisher() = default;

void RatatoskDouble4Publisher::init(
    QtToDds* dds,
    const QString& topic,
    QVector4D start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<ratatosk::types::Double4>>(dds, topic);

  m_writer->sample.x() = start_value.x();
  m_writer->sample.y() = start_value.y();
  m_writer->sample.z() = start_value.z();
  m_writer->sample.w() = start_value.w();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

QVector4D RatatoskDouble4Publisher::value() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector4D(nan,nan,nan,nan);
  }
  return QVector4D(
      m_writer->sample.x(),
      m_writer->sample.y(),
      m_writer->sample.z(),
      m_writer->sample.w());
}

void RatatoskDouble4Publisher::setValue(QVector4D value)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.x(), static_cast<double>(value.x())) &&
      qFuzzyCompare(m_writer->sample.y(), static_cast<double>(value.y())) &&
      qFuzzyCompare(m_writer->sample.z(), static_cast<double>(value.z())) &&
      qFuzzyCompare(m_writer->sample.w(), static_cast<double>(value.w())))
    return;
  else
  {
    m_writer->sample.x() = value.x();
    m_writer->sample.y() = value.y();
    m_writer->sample.z() = value.z();
    m_writer->sample.w() = value.w();
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void RatatoskDouble4Publisher::publish()
{
  m_writer->writer << m_writer->sample;
}
