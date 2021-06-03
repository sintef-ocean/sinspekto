#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsNlpConfig.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsNlpConfigSubscriber::DdsNlpConfigSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsNlpConfigSubscriber::~DdsNlpConfigSubscriber() = default;

void DdsNlpConfigSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::NlpConfig>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::NlpConfig>(
        std::bind(&DdsNlpConfigSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsNlpConfigSubscriber::eventHeard, this, &DdsNlpConfigSubscriber::updateValues);
  }
}

QString DdsNlpConfigSubscriber::technique() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.technique());
}

QString DdsNlpConfigSubscriber::solver() const
{
  if(!m_reader) return QString();
  return QString::fromStdString(m_reader->sample.solver());
}

int DdsNlpConfigSubscriber::degree() const
{
  if(!m_reader) return std::int32_t(0);
  return m_reader->sample.degree();
}

double DdsNlpConfigSubscriber::horizon() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.horizon();
}

unsigned int DdsNlpConfigSubscriber::elements() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.elements();
}

unsigned int DdsNlpConfigSubscriber::nx() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.nx();
}

unsigned int DdsNlpConfigSubscriber::np() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.np();
}

unsigned int DdsNlpConfigSubscriber::ng() const
{
  if(!m_reader) return std::uint32_t(0);
  return m_reader->sample.ng();
}

QDateTime DdsNlpConfigSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsNlpConfigSubscriber::updateValues()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::NlpConfig> samples =
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

      emit techniqueChanged(QString::fromStdString(m_reader->sample.technique()));
      emit solverChanged(QString::fromStdString(m_reader->sample.solver()));
      emit degreeChanged(m_reader->sample.degree());
      emit horizonChanged(m_reader->sample.horizon());
      emit elementsChanged(m_reader->sample.elements());
      emit nxChanged(m_reader->sample.nx());
      emit npChanged(m_reader->sample.np());
      emit ngChanged(m_reader->sample.ng());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}
