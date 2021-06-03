#include <cinttypes>
#include <functional>
#include <limits>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsIdVec1d.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsIdVec1dSubscriber::DdsIdVec1dSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsIdVec1dSubscriber::~DdsIdVec1dSubscriber() = default;

void DdsIdVec1dSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::IdVec1d>>(dds, topic);
  m_id = id;

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::IdVec1d>(
        std::bind(&DdsIdVec1dSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(this, &DdsIdVec1dSubscriber::eventHeard, this, &DdsIdVec1dSubscriber::updateValue);
  }
}

double DdsIdVec1dSubscriber::value() const
{
  if(!m_reader) return std::numeric_limits<double>::quiet_NaN();
  return m_reader->sample.vec().x();
}

QDateTime DdsIdVec1dSubscriber::timestamp() const
{
  if(!m_reader) return QDateTime();

  return [inTime = m_reader->timepoint.to_millisecs()]()
   -> QDateTime {
           auto ret = QDateTime();
           ret.setMSecsSinceEpoch(inTime);
           return ret;
         }();
}


void DdsIdVec1dSubscriber::updateValue()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::IdVec1d> samples =
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

      emit valueChanged(m_reader->sample.vec().x());
      emit timestampChanged(
          [inTime = m_reader->timepoint.to_millisecs()]()
          -> QDateTime {
            auto ret = QDateTime();
            ret.setMSecsSinceEpoch(inTime);
            return ret;
          }());
    }
}


DdsIdVec1dPublisher::DdsIdVec1dPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsIdVec1dPublisher::~DdsIdVec1dPublisher() = default;

void DdsIdVec1dPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    double start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::IdVec1d>>(dds, topic);

  m_writer->sample.vec().x() = start_value;
  m_writer->sample.id() = id.toStdString();
  setValue(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

double DdsIdVec1dPublisher::value() const
{
  if(!m_writer) return std::numeric_limits<double>::quiet_NaN();
  return m_writer->sample.vec().x();
}

void DdsIdVec1dPublisher::setValue(double value)
{
  if (!m_writer) return;
  else if(qFuzzyCompare(m_writer->sample.vec().x(), value))
    return;
  else
  {
    m_writer->sample.vec().x() = value;
    m_writer->writer << m_writer->sample;
    emit valueChanged(value);
  }
}

void DdsIdVec1dPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
