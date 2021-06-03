#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/ratatosk/RatatoskCurrentProfile.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


RatatoskCurrentProfileSubscriber::RatatoskCurrentProfileSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

RatatoskCurrentProfileSubscriber::~RatatoskCurrentProfileSubscriber() = default;

void RatatoskCurrentProfileSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<ratatosk::types::CurrentProfile>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<ratatosk::types::CurrentProfile>(
        std::bind(&RatatoskCurrentProfileSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &RatatoskCurrentProfileSubscriber::eventHeard, this, &RatatoskCurrentProfileSubscriber::updateValue);
  }
}

std::vector<double> RatatoskCurrentProfileSubscriber::depths() const
{
  if(!m_reader)
    return std::vector<double>();
  else
  {
    auto currents = m_reader->sample.currents();
    auto ret = std::vector<double>();
    for (auto &e : currents)
      ret.emplace_back(e.depth());
    return ret;
  }
}

std::vector<double> RatatoskCurrentProfileSubscriber::directions() const
{
  if(!m_reader)
    return std::vector<double>();
  else
  {
    auto currents = m_reader->sample.currents();
    auto ret = std::vector<double>();
    for (auto &e : currents)
      ret.emplace_back(e.direction());
    return ret;
  }
}

std::vector<double> RatatoskCurrentProfileSubscriber::speeds() const
{
  if(!m_reader)
    return std::vector<double>();
  else
  {
    auto currents = m_reader->sample.currents();
    auto ret = std::vector<double>();
    for (auto &e : currents)
      ret.emplace_back(e.speed());
    return ret;
  }
}

QDateTime RatatoskCurrentProfileSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void RatatoskCurrentProfileSubscriber::updateValue()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<ratatosk::types::CurrentProfile> samples =
    m_reader->reader.select()
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      auto currents = m_reader->sample.currents();
      std::vector<double> depths, directions, speeds;

      for (auto &e : currents)
      {
        depths.emplace_back(e.depth());
        directions.emplace_back(e.direction());
        speeds.emplace_back(e.speed());
      }

      emit depthsChanged(depths);
      emit directionsChanged(directions);
      emit speedsChanged(speeds);

      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
