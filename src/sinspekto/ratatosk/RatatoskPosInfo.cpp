#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskPosInfo.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskPosInfoSubscriber::RatatoskPosInfoSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskPosInfoSubscriber::~RatatoskPosInfoSubscriber() = default;

void RatatoskPosInfoSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::PosInfo>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::PosInfo>(
        std::bind(&RatatoskPosInfoSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskPosInfoSubscriber::eventHeard, this, &RatatoskPosInfoSubscriber::updateValue);
  }
}

double RatatoskPosInfoSubscriber::lat() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.lat();
}

double RatatoskPosInfoSubscriber::lon() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.lon();
}

double RatatoskPosInfoSubscriber::sog() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.sog();
}

double RatatoskPosInfoSubscriber::cog() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.cog();
}

QDateTime RatatoskPosInfoSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskPosInfoSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::PosInfo> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit latChanged(m_reader->sample.lat());
      emit lonChanged(m_reader->sample.lon());
      emit sogChanged(m_reader->sample.sog());
      emit cogChanged(m_reader->sample.cog());

      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
