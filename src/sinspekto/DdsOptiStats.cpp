#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsOptiStats.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsOptiStatsSubscriber::DdsOptiStatsSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsOptiStatsSubscriber::~DdsOptiStatsSubscriber() = default;

void DdsOptiStatsSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::OptiStats>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::OptiStats>(
        std::bind(&DdsOptiStatsSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsOptiStatsSubscriber::eventHeard, this, &DdsOptiStatsSubscriber::updateValues);
  }
}

unsigned int DdsOptiStatsSubscriber::iterations() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.iterations();
}

bool DdsOptiStatsSubscriber::status() const
{
  if(!m_reader) return false;
  return m_reader->sample.status();
}

QString DdsOptiStatsSubscriber::status_text() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.status_text());
}

double DdsOptiStatsSubscriber::obj() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.obj();
}

double DdsOptiStatsSubscriber::solve_time() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.t_wall().total();
}

std::vector<double> DdsOptiStatsSubscriber::p() const
{
  if(!m_reader) return std::vector<double>();
  return m_reader->sample.p();
}

std::vector<double> DdsOptiStatsSubscriber::x0() const
{
  if(!m_reader) return std::vector<double>();
  return m_reader->sample.x0();
}

QDateTime DdsOptiStatsSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsOptiStatsSubscriber::updateValues()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::OptiStats> samples =
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

      emit iterationsChanged(m_reader->sample.iterations());
      emit statusChanged(m_reader->sample.status());
      emit status_textChanged(QString::fromStdString(m_reader->sample.status_text()));
      emit objChanged(m_reader->sample.obj());
      emit solve_timeChanged(m_reader->sample.t_wall().total());
      emit pChanged(m_reader->sample.p());
      emit x0Changed(m_reader->sample.x0());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
