#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsIdVec4d.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsIdVec4dSubscriber::DdsIdVec4dSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsIdVec4dSubscriber::~DdsIdVec4dSubscriber() = default;

void DdsIdVec4dSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::IdVec4d>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::IdVec4d>(
        std::bind(&DdsIdVec4dSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsIdVec4dSubscriber::eventHeard, this, &DdsIdVec4dSubscriber::updateValue);
  }
}

QVector4D DdsIdVec4dSubscriber::value() const
{
  if(!m_reader)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
    return QVector4D(nan,nan,nan,nan);
  }
  return QVector4D(
      m_reader->sample.vec().x(),
      m_reader->sample.vec().y(),
      m_reader->sample.vec().z(),
      m_reader->sample.vec().w());
}

QDateTime DdsIdVec4dSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsIdVec4dSubscriber::updateValue()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::IdVec4d> samples =
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

      emit valueChanged(QVector4D(
           m_reader->sample.vec().x(),
           m_reader->sample.vec().y(),
           m_reader->sample.vec().z(),
           m_reader->sample.vec().w()));
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsIdVec4dPublisher::DdsIdVec4dPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsIdVec4dPublisher::~DdsIdVec4dPublisher() = default;

void DdsIdVec4dPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    QVector4D start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::IdVec4d>>(dds, topic);

  m_writer->sample.vec().x() = start_value.x();
  m_writer->sample.vec().y() = start_value.y();
  m_writer->sample.vec().z() = start_value.z();
  m_writer->sample.vec().w() = start_value.w();
  m_writer->sample.id() = id.toStdString();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

QVector4D DdsIdVec4dPublisher::value() const
{
  if(!m_writer)
  {
    float nan = std::numeric_limits<float>::quiet_NaN();
   return QVector4D(nan,nan,nan,nan);
  }
  return QVector4D(
      m_writer->sample.vec().x(),
      m_writer->sample.vec().y(),
      m_writer->sample.vec().z(),
      m_writer->sample.vec().w());
}

void DdsIdVec4dPublisher::setValue(QVector4D value)
{
  if (!m_writer) return;
  else if(
      qFuzzyCompare(m_writer->sample.vec().x(), static_cast<double>(value.x())) &&
      qFuzzyCompare(m_writer->sample.vec().y(), static_cast<double>(value.y())) &&
      qFuzzyCompare(m_writer->sample.vec().z(), static_cast<double>(value.z())) &&
      qFuzzyCompare(m_writer->sample.vec().w(), static_cast<double>(value.w())))
    return;
  else
  {
    m_writer->sample.vec().x() = value.x();
    m_writer->sample.vec().y() = value.y();
    m_writer->sample.vec().z() = value.z();
    m_writer->sample.vec().w() = value.w();
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void DdsIdVec4dPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
