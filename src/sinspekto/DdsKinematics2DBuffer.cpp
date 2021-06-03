#include <cinttypes>
#include <functional>
#include <limits>

#include <QXYSeries>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsKinematics2DBuffer.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsKinematics2DBuffer::DdsKinematics2DBuffer(QObject *parent) :
  DdsBuffer(parent),
  m_reader(nullptr),
  m_batchReader(nullptr)
{
  m_buffers[qml_enums::DimId::PosX] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::PosY] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::Speed] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::Course] = new DdsDoubleBuffer(parent);
  qRegisterMetaType<QAbstractSeries*>();
  qRegisterMetaType<QAbstractAxis*>();
}

DdsKinematics2DBuffer::~DdsKinematics2DBuffer() = default;

QPointF DdsKinematics2DBuffer::rangePosX() const { return m_buffers.at(qml_enums::DimId::PosX)->range(); }
QPointF DdsKinematics2DBuffer::rangePosY() const { return m_buffers.at(qml_enums::DimId::PosY)->range(); }
QPointF DdsKinematics2DBuffer::rangeSpeed() const { return m_buffers.at(qml_enums::DimId::Speed)->range(); }
QPointF DdsKinematics2DBuffer::rangeCourse() const { return m_buffers.at(qml_enums::DimId::Course)->range(); }

void DdsKinematics2DBuffer::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    int buffer_size,
    bool use_batch,
    bool with_listener)
{
  DdsBuffer::init(buffer_size);

  m_id = id;
  m_buffers.at(qml_enums::DimId::PosX)->setCapacity(buffer_size);
  m_buffers.at(qml_enums::DimId::PosY)->setCapacity(buffer_size);
  m_buffers.at(qml_enums::DimId::Speed)->setCapacity(buffer_size);
  m_buffers.at(qml_enums::DimId::Course)->setCapacity(buffer_size);

  QObject::connect(
      this->m_buffers[qml_enums::DimId::PosX], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsKinematics2DBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::PosX));
  QObject::connect(
      this->m_buffers[qml_enums::DimId::PosY], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsKinematics2DBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::PosY));
  QObject::connect(
      this->m_buffers[qml_enums::DimId::Speed], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsKinematics2DBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::Speed));
  QObject::connect(
      this->m_buffers[qml_enums::DimId::Course], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsKinematics2DBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::Course));

  if(use_batch)
    m_batchReader = std::make_unique<sinspekto::Reader<fkin::BatchKinematics2D>>(dds, topic);
  else
    m_reader = std::make_unique<sinspekto::Reader<fkin::Kinematics2D>>(dds, topic);

  if (with_listener)
  {

    if(use_batch)
    {
      m_batchReader->listener = sinspekto::DdsReaderListener<fkin::BatchKinematics2D>(
          std::bind(&DdsKinematics2DBuffer::eventHeard, this));
      m_batchReader->reader.listener(
          &m_batchReader->listener,
          dds::core::status::StatusMask::data_available());
    }
    else
    {
      m_reader->listener = sinspekto::DdsReaderListener<fkin::Kinematics2D>(
          std::bind(&DdsKinematics2DBuffer::eventHeard, this));
      m_reader->reader.listener(
          &m_reader->listener,
          dds::core::status::StatusMask::data_available());
    }

    QObject::connect(
        this, &DdsKinematics2DBuffer::eventHeard,
        this, &DdsKinematics2DBuffer::updateBuffers);
  }
}

void DdsKinematics2DBuffer::updateBuffers()
{
  if(!m_reader && !m_batchReader) return;

  auto addSampleToBuffers =
   [=](fkin::Kinematics2D& sample)
   {
     m_buffers.at(qml_enums::DimId::PosX)->Buffer().push_back(sample.position().x());
     m_buffers.at(qml_enums::DimId::PosY)->Buffer().push_back(sample.position().y());
     m_buffers.at(qml_enums::DimId::Speed)->Buffer().push_back(sample.speed().x());
     m_buffers.at(qml_enums::DimId::Course)->Buffer().push_back(sample.course().x());
   };

  if(m_reader)
  {
    const auto query = dds::sub::Query(
        m_reader->reader,
        "id = %0", {m_id.toStdString()});


    dds::sub::LoanedSamples<fkin::Kinematics2D> samples =
     m_reader->reader.select()
     .content(query)
     .state(dds::sub::status::DataState::new_data()).take();

    if(samples.length() > 0)
    {
      auto sample = (--samples.end()); // picks the last sample
      m_reader->sample = sample->data();
      m_reader->timepoint = sample->info().timestamp();
      addSampleToBuffers(m_reader->sample);
      m_time->Buffer().push_back(m_reader->timepoint.to_millisecs());
      emit newData();
    }
  }

  if(m_batchReader)
  {
    const auto query = dds::sub::Query(
        m_batchReader->reader,
        "id = %0", {m_id.toStdString()});

    dds::sub::LoanedSamples<fkin::BatchKinematics2D> samples =
     m_batchReader->reader.select()
     .content(query)
     .state(dds::sub::status::DataState::new_data()).take();

    if(samples.length() > 0)
    {
      auto batchSample = (--samples.end()); // picks the last sample
      m_batchReader->sample = batchSample->data();
      m_batchReader->timepoint = batchSample->info().timestamp();

      if(m_batchReader->sample.batch().size() != m_batchReader->sample.timestamps().size())
        std::cerr
         << "Inconsistent length of batch sequence and timestamps"
         << " for BatchKinematics2D with id: " << m_id.toStdString()
         << std::endl;

      for(size_t i = 0; i < m_batchReader->sample.batch().size(); ++i)
      {
        //auto& sample = m_batchReader->sample.batch()[i];
        addSampleToBuffers(m_batchReader->sample.batch()[i]);
        m_time->Buffer().push_back(m_batchReader->sample.timestamps()[i].unixMillis());
      }
      emit newData();
    }
  }
}
