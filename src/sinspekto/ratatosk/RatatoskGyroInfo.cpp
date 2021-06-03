#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskGyroInfo.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskGyroInfoSubscriber::RatatoskGyroInfoSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskGyroInfoSubscriber::~RatatoskGyroInfoSubscriber() = default;

void RatatoskGyroInfoSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::GyroInfo>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::GyroInfo>(
        std::bind(&RatatoskGyroInfoSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskGyroInfoSubscriber::eventHeard, this, &RatatoskGyroInfoSubscriber::updateValue);
  }
}

double RatatoskGyroInfoSubscriber::hdt() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.hdt();
}

double RatatoskGyroInfoSubscriber::rot() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.rot();
}

QDateTime RatatoskGyroInfoSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskGyroInfoSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::GyroInfo> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit hdtChanged(m_reader->sample.hdt());
      emit rotChanged(m_reader->sample.rot());

      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
