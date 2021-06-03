#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsKinematics6D.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsKinematics6DSubscriber::DdsKinematics6DSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsKinematics6DSubscriber::~DdsKinematics6DSubscriber() = default;

void DdsKinematics6DSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::Kinematics6D>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::Kinematics6D>(
        std::bind(&DdsKinematics6DSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsKinematics6DSubscriber::eventHeard, this, &DdsKinematics6DSubscriber::updateValues);
  }
}

QVector3D DdsKinematics6DSubscriber::position() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_reader->sample.position().x(),
      m_reader->sample.position().y(),
      m_reader->sample.position().z());
}

QVector3D DdsKinematics6DSubscriber::velocity() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_reader->sample.velocity().x(),
      m_reader->sample.velocity().y(),
      m_reader->sample.velocity().z());
}

QVector3D DdsKinematics6DSubscriber::euler() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_reader->sample.euler().x(),
      m_reader->sample.euler().y(),
      m_reader->sample.euler().z());
}

QDateTime DdsKinematics6DSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}

void DdsKinematics6DSubscriber::updateValues()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::Kinematics6D> samples =
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

      emit positionChanged(QVector3D(
           m_reader->sample.position().x(),
           m_reader->sample.position().y(),
           m_reader->sample.position().z()));
      emit velocityChanged(QVector3D(
           m_reader->sample.velocity().x(),
           m_reader->sample.velocity().y(),
           m_reader->sample.velocity().z()));
      emit eulerChanged(QVector3D(
           m_reader->sample.euler().x(),
           m_reader->sample.euler().y(),
           m_reader->sample.euler().z()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsKinematics6DPublisher::DdsKinematics6DPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsKinematics6DPublisher::~DdsKinematics6DPublisher() = default;

void DdsKinematics6DPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::Kinematics6D>>(dds, topic);

  m_writer->sample.position().x() = 0;
  m_writer->sample.position().y() = 0;
  m_writer->sample.position().z() = 0;
  m_writer->sample.velocity().x() = 0;
  m_writer->sample.velocity().y() = 0;
  m_writer->sample.velocity().z() = 0;
  m_writer->sample.euler().x() = 0;
  m_writer->sample.euler().y() = 0;
  m_writer->sample.euler().z() = 0;
  m_writer->sample.id() = id.toStdString();

}

QVector3D DdsKinematics6DPublisher::position() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_writer->sample.position().x(),
      m_writer->sample.position().y(),
      m_writer->sample.position().z());
}

QVector3D DdsKinematics6DPublisher::velocity() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_writer->sample.velocity().x(),
      m_writer->sample.velocity().y(),
      m_writer->sample.velocity().z());
}

QVector3D DdsKinematics6DPublisher::euler() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector3D(nan,nan,nan);
  }
  return QVector3D(
      m_writer->sample.euler().x(),
      m_writer->sample.euler().y(),
      m_writer->sample.euler().z());
}

void DdsKinematics6DPublisher::setPosition(QVector3D position)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.position().x(), static_cast<double>(position.x())) &&
      qFuzzyCompare(m_writer->sample.position().y(), static_cast<double>(position.y())) &&
      qFuzzyCompare(m_writer->sample.position().z(), static_cast<double>(position.z())))
      // Will the ever be equal?
    return;
  else
  {
    m_writer->sample.position().x() = position.x();
    m_writer->sample.position().y() = position.y();
    m_writer->sample.position().z() = position.z();
    emit positionChanged(position);
  }
}

void DdsKinematics6DPublisher::setVelocity(QVector3D velocity)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.velocity().x(), static_cast<double>(velocity.x())) &&
      qFuzzyCompare(m_writer->sample.velocity().y(), static_cast<double>(velocity.y())) &&
      qFuzzyCompare(m_writer->sample.velocity().z(), static_cast<double>(velocity.z())))
    // Will the ever be equal?
    return;
  else
  {
    m_writer->sample.velocity().x() = velocity.x();
    m_writer->sample.velocity().y() = velocity.y();
    m_writer->sample.velocity().z() = velocity.z();
    emit velocityChanged(velocity);
  }
}

void DdsKinematics6DPublisher::setEuler(QVector3D euler)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.euler().x(), static_cast<double>(euler.x())) &&
      qFuzzyCompare(m_writer->sample.euler().y(), static_cast<double>(euler.y())) &&
      qFuzzyCompare(m_writer->sample.euler().z(), static_cast<double>(euler.z())))
    // Will the ever be equal?
    return;
  else
  {
    m_writer->sample.euler().x() = euler.x();
    m_writer->sample.euler().y() = euler.y();
    m_writer->sample.euler().z() = euler.z();
    emit eulerChanged(euler);
  }
}

void DdsKinematics6DPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
