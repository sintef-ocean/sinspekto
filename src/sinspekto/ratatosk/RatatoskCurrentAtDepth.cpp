#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskCurrentAtDepth.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskCurrentAtDepthSubscriber::RatatoskCurrentAtDepthSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskCurrentAtDepthSubscriber::~RatatoskCurrentAtDepthSubscriber() = default;

void RatatoskCurrentAtDepthSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::CurrentAtDepth>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::CurrentAtDepth>(
        std::bind(&RatatoskCurrentAtDepthSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskCurrentAtDepthSubscriber::eventHeard, this, &RatatoskCurrentAtDepthSubscriber::updateValue);
  }
}

double RatatoskCurrentAtDepthSubscriber::depth() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.depth();
}

double RatatoskCurrentAtDepthSubscriber::direction() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.direction();
}

double RatatoskCurrentAtDepthSubscriber::speed() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.speed();
}

QDateTime RatatoskCurrentAtDepthSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskCurrentAtDepthSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::CurrentAtDepth> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit depthChanged(m_reader->sample.depth());
      emit directionChanged(m_reader->sample.direction());
      emit speedChanged(m_reader->sample.speed());

      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
