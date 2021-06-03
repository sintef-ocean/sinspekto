#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsDouble.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsDoubleSubscriber::DdsDoubleSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsDoubleSubscriber::~DdsDoubleSubscriber() = default;

void DdsDoubleSubscriber::init(QtToDds* dds, const QString& topic, bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::Real>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::Real>(
        std::bind(&DdsDoubleSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsDoubleSubscriber::eventHeard, this, &DdsDoubleSubscriber::updateValue);
  }
}

double DdsDoubleSubscriber::value() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.value();
}

QDateTime DdsDoubleSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsDoubleSubscriber::updateValue()
{
  if(!m_reader) return;

  dds::sub::LoanedSamples<fkin::Real> samples =
    m_reader->reader.select().state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit valueChanged(m_reader->sample.value());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsDoublePublisher::DdsDoublePublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsDoublePublisher::~DdsDoublePublisher() = default;

void DdsDoublePublisher::init(QtToDds* dds, const QString& topic, double start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::Real>>(dds, topic);

  m_writer->sample.value() = start_value;
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

double DdsDoublePublisher::value() const
{
  if(!m_writer) return false;
  return m_writer->sample.value();
}

void DdsDoublePublisher::setValue(double value)
{
  if (!m_writer) return;
  else if(qFuzzyCompare(m_writer->sample.value(), value))
    return;
  else
  {
    m_writer->sample.value() = value;
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void DdsDoublePublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
