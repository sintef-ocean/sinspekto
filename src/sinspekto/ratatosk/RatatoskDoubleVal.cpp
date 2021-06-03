#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/ratatosk/RatatoskDoubleVal.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskDoubleValSubscriber::RatatoskDoubleValSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskDoubleValSubscriber::~RatatoskDoubleValSubscriber() = default;

void RatatoskDoubleValSubscriber::init(QtToDds* dds, const QString& topic, bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::DoubleVal>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::DoubleVal>(
        std::bind(&RatatoskDoubleValSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskDoubleValSubscriber::eventHeard, this, &RatatoskDoubleValSubscriber::updateValue);
  }
}

double RatatoskDoubleValSubscriber::val() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.val();
}

QDateTime RatatoskDoubleValSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskDoubleValSubscriber::updateValue()
{
  if(!m_reader) return;

  dds::sub::LoanedSamples<ratatosk::types::DoubleVal> samples =
    m_reader->reader.select().state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit valChanged(m_reader->sample.val());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


RatatoskDoubleValPublisher::RatatoskDoubleValPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

RatatoskDoubleValPublisher::~RatatoskDoubleValPublisher() = default;

void RatatoskDoubleValPublisher::init(QtToDds* dds, const QString& topic, double start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<ratatosk::types::DoubleVal>>(dds, topic);

  m_writer->sample.val() = start_value;
  setVal(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

double RatatoskDoubleValPublisher::val() const
{
  if(!m_writer) return false;
  return m_writer->sample.val();
}

void RatatoskDoubleValPublisher::setVal(double value)
{
  if (!m_writer) return;
  else if(qFuzzyCompare(m_writer->sample.val(), value))
    return;
  else
  {
    m_writer->sample.val() = value;
    m_writer->writer << m_writer->sample;
    emit valChanged(value);
  }
}

void RatatoskDoubleValPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
