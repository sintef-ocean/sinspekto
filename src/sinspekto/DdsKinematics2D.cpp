#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsKinematics2D.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsKinematics2DSubscriber::DdsKinematics2DSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsKinematics2DSubscriber::~DdsKinematics2DSubscriber() = default;

void DdsKinematics2DSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::Kinematics2D>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::Kinematics2D>(
        std::bind(&DdsKinematics2DSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsKinematics2DSubscriber::eventHeard, this, &DdsKinematics2DSubscriber::updateValues);
  }
}

QVector2D DdsKinematics2DSubscriber::position() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector2D(nan,nan);
  }
  return QVector2D(
      m_reader->sample.position().x(),
      m_reader->sample.position().y());
}

double DdsKinematics2DSubscriber::speed() const
{
  if(!m_reader)
     return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.speed().x();
}

double DdsKinematics2DSubscriber::course() const
{
  if(!m_reader)
    return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.course().x();
}

QDateTime DdsKinematics2DSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsKinematics2DSubscriber::updateValues()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::Kinematics2D> samples =
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

      emit positionChanged(QVector2D(
           m_reader->sample.position().x(),
           m_reader->sample.position().y()));
      emit speedChanged(m_reader->sample.speed().x());
      emit courseChanged(m_reader->sample.course().x());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsKinematics2DPublisher::DdsKinematics2DPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsKinematics2DPublisher::~DdsKinematics2DPublisher() = default;

void DdsKinematics2DPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::Kinematics2D>>(dds, topic);

  m_writer->sample.position().x() = 0;
  m_writer->sample.position().y() = 0;
  m_writer->sample.course().x() = 0;
  m_writer->sample.speed().x() = 0;
  m_writer->sample.id() = id.toStdString();

}

QVector2D DdsKinematics2DPublisher::position() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector2D(nan,nan);
  }
  return QVector2D(
      m_writer->sample.position().x(),
      m_writer->sample.position().y());
}

double DdsKinematics2DPublisher::speed() const
{
  if(!m_writer)
  {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return m_writer->sample.speed().x();
}

double DdsKinematics2DPublisher::course() const
{
  if(!m_writer)
  {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return m_writer->sample.course().x();
}

void DdsKinematics2DPublisher::setPosition(QVector2D position)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.position().x(), static_cast<double>(position.x())) &&
      qFuzzyCompare(m_writer->sample.position().y(), static_cast<double>(position.y())))
      // Will the ever be equal?
    return;
  else
  {
    m_writer->sample.position().x() = position.x();
    m_writer->sample.position().y() = position.y();
    emit positionChanged(position);
  }
}

void DdsKinematics2DPublisher::setSpeed(double speed)
{
  if (!m_writer) return;
  else if(qFuzzyCompare(m_writer->sample.speed().x(), speed))
    return;
  else
  {
    m_writer->sample.speed().x() = speed;
    emit speedChanged(speed);
  }
}

void DdsKinematics2DPublisher::setCourse(double course)
{
  if (!m_writer) return;
  else if(qFuzzyCompare(m_writer->sample.course().x(), course))
    return;
  else
  {
    m_writer->sample.course().x() = course;
    emit courseChanged(course);
  }
}

void DdsKinematics2DPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
