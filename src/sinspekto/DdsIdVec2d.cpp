#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsIdVec2d.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsIdVec2dSubscriber::DdsIdVec2dSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsIdVec2dSubscriber::~DdsIdVec2dSubscriber() = default;

void DdsIdVec2dSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::IdVec2d>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::IdVec2d>(
        std::bind(&DdsIdVec2dSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsIdVec2dSubscriber::eventHeard, this, &DdsIdVec2dSubscriber::updateValue);
  }
}

QVector2D DdsIdVec2dSubscriber::value() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector2D(nan,nan);
  }
  return QVector2D(
      m_reader->sample.vec().x(),
      m_reader->sample.vec().y());
}

QDateTime DdsIdVec2dSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsIdVec2dSubscriber::updateValue()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::IdVec2d> samples =
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

      emit valueChanged(QVector2D(
           m_reader->sample.vec().x(),
           m_reader->sample.vec().y()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsIdVec2dPublisher::DdsIdVec2dPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsIdVec2dPublisher::~DdsIdVec2dPublisher() = default;

void DdsIdVec2dPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    QVector2D start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::IdVec2d>>(dds, topic);

  m_writer->sample.vec().x() = start_value.x();
  m_writer->sample.vec().y() = start_value.y();
  m_writer->sample.id() = id.toStdString();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

QVector2D DdsIdVec2dPublisher::value() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector2D(nan,nan);
  }
  return QVector2D(
      m_writer->sample.vec().x(),
      m_writer->sample.vec().y());
}

void DdsIdVec2dPublisher::setValue(QVector2D value)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.vec().x(), static_cast<double>(value.x())) &&
      qFuzzyCompare(m_writer->sample.vec().y(), static_cast<double>(value.y())))
    return;
  else
  {
    m_writer->sample.vec().x() = value.x();
    m_writer->sample.vec().y() = value.y();
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void DdsIdVec2dPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
