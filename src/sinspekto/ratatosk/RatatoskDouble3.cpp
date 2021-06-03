#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskDouble3.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskDouble3Subscriber::RatatoskDouble3Subscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskDouble3Subscriber::~RatatoskDouble3Subscriber() = default;

void RatatoskDouble3Subscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::Double3>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::Double3>(
        std::bind(&RatatoskDouble3Subscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskDouble3Subscriber::eventHeard, this, &RatatoskDouble3Subscriber::updateValue);
  }
}

QVector3D RatatoskDouble3Subscriber::value() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_reader->sample.x(),
      m_reader->sample.y(),
      m_reader->sample.z());
}

QDateTime RatatoskDouble3Subscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskDouble3Subscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::Double3> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit valueChanged(QVector3D(
           m_reader->sample.x(),
           m_reader->sample.y(),
           m_reader->sample.z()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


RatatoskDouble3Publisher::RatatoskDouble3Publisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

RatatoskDouble3Publisher::~RatatoskDouble3Publisher() = default;

void RatatoskDouble3Publisher::init(
    QtToDds* dds,
    const QString& topic,
    QVector3D start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<ratatosk::types::Double3>>(dds, topic);

  m_writer->sample.x() = start_value.x();
  m_writer->sample.y() = start_value.y();
  m_writer->sample.z() = start_value.z();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

QVector3D RatatoskDouble3Publisher::value() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_writer->sample.x(),
      m_writer->sample.y(),
      m_writer->sample.z());
}

void RatatoskDouble3Publisher::setValue(QVector3D value)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.x(), static_cast<double>(value.x())) &&
      qFuzzyCompare(m_writer->sample.y(), static_cast<double>(value.y())) &&
      qFuzzyCompare(m_writer->sample.z(), static_cast<double>(value.z())))
    return;
  else
  {
    m_writer->sample.x() = value.x();
    m_writer->sample.y() = value.y();
    m_writer->sample.z() = value.z();
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void RatatoskDouble3Publisher::publish()
{
  m_writer->writer << m_writer->sample;
}
