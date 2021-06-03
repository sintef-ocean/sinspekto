#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/WeatherDds.hpp"
#include "sinspekto/DdsWeatherData.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsWeatherDataSubscriber::DdsWeatherDataSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsWeatherDataSubscriber::~DdsWeatherDataSubscriber() = default;

void DdsWeatherDataSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<weather::ModuleData>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<weather::ModuleData>(
        std::bind(&DdsWeatherDataSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsWeatherDataSubscriber::eventHeard, this, &DdsWeatherDataSubscriber::updateValues);
  }
}

QString DdsWeatherDataSubscriber::module_name() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.module_name());
}

QString DdsWeatherDataSubscriber::type() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.type());
}

QString DdsWeatherDataSubscriber::temp_trend() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.temp_trend());
}

QString DdsWeatherDataSubscriber::pressure_trend() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.pressure_trend());
}

double DdsWeatherDataSubscriber::CO2() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.CO2();
}

double DdsWeatherDataSubscriber::noise() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.noise();
}

double DdsWeatherDataSubscriber::humidity() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.humidity();
}

double DdsWeatherDataSubscriber::temperature() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.temperature();
}

double DdsWeatherDataSubscriber::pressure() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.pressure();
}

unsigned int DdsWeatherDataSubscriber::signal_strength() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.signal_strength();
}

unsigned int DdsWeatherDataSubscriber::battery() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.battery_percent();
}

bool DdsWeatherDataSubscriber::reachable() const
{
  if(!m_reader) return false;
  return m_reader->sample.reachable();
}

QDateTime DdsWeatherDataSubscriber::sampleTime() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->sample.time().unixMillis()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}

QDateTime DdsWeatherDataSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsWeatherDataSubscriber::updateValues()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "module_name = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<weather::ModuleData> samples =
    m_reader->reader.select()
    .content(query)
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      auto datetime = QDateTime();

      emit CO2Changed(m_reader->sample.CO2());
      emit noiseChanged(m_reader->sample.noise());
      emit humidityChanged(m_reader->sample.humidity());
      emit temperatureChanged(m_reader->sample.temperature());
      emit pressureChanged(m_reader->sample.pressure());
      emit reachableChanged(m_reader->sample.reachable());
      emit signal_strengthChanged(m_reader->sample.signal_strength());
      emit batteryChanged(m_reader->sample.battery_percent());
      emit temp_trendChanged(QString::fromStdString(m_reader->sample.temp_trend()));
      emit pressure_trendChanged(QString::fromStdString(m_reader->sample.pressure_trend()));
      emit module_nameChanged(QString::fromStdString(m_reader->sample.module_name()));
      emit typeChanged(QString::fromStdString(m_reader->sample.type()));
      emit sampleTimeChanged(
          [inTime = m_reader->sample.time().unixMillis()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
