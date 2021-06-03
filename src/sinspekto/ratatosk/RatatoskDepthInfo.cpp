#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskDepthInfo.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskDepthInfoSubscriber::RatatoskDepthInfoSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskDepthInfoSubscriber::~RatatoskDepthInfoSubscriber() = default;

void RatatoskDepthInfoSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::DepthInfo>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::DepthInfo>(
        std::bind(&RatatoskDepthInfoSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskDepthInfoSubscriber::eventHeard, this, &RatatoskDepthInfoSubscriber::updateValue);
  }
}

double RatatoskDepthInfoSubscriber::depth() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.depth();
}

double RatatoskDepthInfoSubscriber::depthBelowTransducer() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.depthBelowTransducer();
}

QDateTime RatatoskDepthInfoSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskDepthInfoSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::DepthInfo> samples =
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
      emit depthBelowTransducerChanged(m_reader->sample.depthBelowTransducer());

      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
