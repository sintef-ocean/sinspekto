#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskWindInfo.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskWindInfoSubscriber::RatatoskWindInfoSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskWindInfoSubscriber::~RatatoskWindInfoSubscriber() = default;

void RatatoskWindInfoSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::WindInfo>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::WindInfo>(
        std::bind(&RatatoskWindInfoSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskWindInfoSubscriber::eventHeard, this, &RatatoskWindInfoSubscriber::updateValue);
  }
}

double RatatoskWindInfoSubscriber::trueSpeed() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.trueSpeed();
}

double RatatoskWindInfoSubscriber::trueDir() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.trueDir();
}

double RatatoskWindInfoSubscriber::relSpeed() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.relSpeed();
}

double RatatoskWindInfoSubscriber::relDir() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.relDir();
}

double RatatoskWindInfoSubscriber::trueWindRelDir() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  else
    return m_reader->sample.trueWindRelDir();
}


QDateTime RatatoskWindInfoSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskWindInfoSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::WindInfo> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit trueSpeedChanged(m_reader->sample.trueSpeed());
      emit trueDirChanged(m_reader->sample.trueDir());
      emit relSpeedChanged(m_reader->sample.relSpeed());
      emit relDirChanged(m_reader->sample.relDir());
      emit trueWindRelDirChanged(m_reader->sample.trueWindRelDir());

      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
