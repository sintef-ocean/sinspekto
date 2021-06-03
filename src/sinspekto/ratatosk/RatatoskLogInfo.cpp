#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskLogInfo.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskLogInfoSubscriber::RatatoskLogInfoSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskLogInfoSubscriber::~RatatoskLogInfoSubscriber() = default;

void RatatoskLogInfoSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::LogInfo>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::LogInfo>(
        std::bind(&RatatoskLogInfoSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskLogInfoSubscriber::eventHeard, this, &RatatoskLogInfoSubscriber::updateValue);
  }
}

double RatatoskLogInfoSubscriber::speed() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.speed();
}

QDateTime RatatoskLogInfoSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskLogInfoSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::LogInfo> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

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
