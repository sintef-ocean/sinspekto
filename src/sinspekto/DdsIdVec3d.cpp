#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsIdVec3d.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsIdVec3dSubscriber::DdsIdVec3dSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsIdVec3dSubscriber::~DdsIdVec3dSubscriber() = default;

void DdsIdVec3dSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::IdVec3d>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::IdVec3d>(
        std::bind(&DdsIdVec3dSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsIdVec3dSubscriber::eventHeard, this, &DdsIdVec3dSubscriber::updateValue);
  }
}

QVector3D DdsIdVec3dSubscriber::value() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_reader->sample.vec().x(),
      m_reader->sample.vec().y(),
      m_reader->sample.vec().z());
}

QDateTime DdsIdVec3dSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsIdVec3dSubscriber::updateValue()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::IdVec3d> samples =
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

      emit valueChanged(QVector3D(
           m_reader->sample.vec().x(),
           m_reader->sample.vec().y(),
           m_reader->sample.vec().z()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsIdVec3dPublisher::DdsIdVec3dPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsIdVec3dPublisher::~DdsIdVec3dPublisher() = default;

void DdsIdVec3dPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    QVector3D start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::IdVec3d>>(dds, topic);

  m_writer->sample.vec().x() = start_value.x();
  m_writer->sample.vec().y() = start_value.y();
  m_writer->sample.vec().z() = start_value.z();
  m_writer->sample.id() = id.toStdString();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

QVector3D DdsIdVec3dPublisher::value() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_writer->sample.vec().x(),
      m_writer->sample.vec().y(),
      m_writer->sample.vec().z());
}

void DdsIdVec3dPublisher::setValue(QVector3D value)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.vec().x(), static_cast<double>(value.x())) &&
      qFuzzyCompare(m_writer->sample.vec().y(), static_cast<double>(value.y())) &&
      qFuzzyCompare(m_writer->sample.vec().z(), static_cast<double>(value.z())))
    return;
  else
  {
    m_writer->sample.vec().x() = value.x();
    m_writer->sample.vec().y() = value.y();
    m_writer->sample.vec().z() = value.z();
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void DdsIdVec3dPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
