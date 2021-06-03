#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsBit.hpp"
#include "sinspekto/QtToDdsPriv.hpp"

DdsBitSubscriber::DdsBitSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsBitSubscriber::~DdsBitSubscriber() = default;

void DdsBitSubscriber::init(QtToDds* dds, const QString& topic, bool with_listener)
{
  m_reader = std::make_unique<sinspekto::Reader<fkin::Bit>>(dds, topic);

  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::Bit>(
        std::bind(&DdsBitSubscriber::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());
    QObject::connect(this, &DdsBitSubscriber::eventHeard, this, &DdsBitSubscriber::updateSignal);
  }
}

bool DdsBitSubscriber::signal() const
{
  if(!m_reader) return false;
  return m_reader->sample.value();
}

void DdsBitSubscriber::updateSignal()
{
  if(!m_reader) return;
  dds::sub::LoanedSamples<fkin::Bit> samples =
    m_reader->reader.select().state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
  {
    m_reader->sample = (--samples.end())->data(); // picks the last sample
    emit signalChanged(m_reader->sample.value());
  }
}


DdsBitPublisher::DdsBitPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr)
{ }

DdsBitPublisher::~DdsBitPublisher() = default;

void DdsBitPublisher::init(QtToDds* dds, const QString& topic, bool start_value, bool send_first)
{
  m_writer = std::make_unique<sinspekto::Writer<fkin::Bit>>(dds, topic);

  m_writer->sample.value() = start_value;
  setSignal(start_value);

  if (send_first)
    m_writer->writer << m_writer->sample;

}

bool DdsBitPublisher::signal() const
{
  if(!m_writer) return false;
  return m_writer->sample.value();
}

void DdsBitPublisher::setSignal(bool value)
{
  if (!m_writer) return;
  else if(m_writer->sample.value() == value)
    return;
  else
  {
    m_writer->sample.value() = value;
    m_writer->writer << m_writer->sample;
    emit signalChanged(value);
  }
}

void DdsBitPublisher::publish()
{
  m_writer->writer << m_writer->sample;
}
