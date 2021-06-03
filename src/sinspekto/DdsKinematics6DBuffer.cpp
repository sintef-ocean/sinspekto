#include <cinttypes>
#include <functional>
#include <limits>

#include <QXYSeries>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsKinematics6DBuffer.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsKinematics6DBuffer::DdsKinematics6DBuffer(QObject *parent) :
  DdsBuffer(parent),
  m_reader(nullptr),
  m_batchReader(nullptr)
{
  m_buffers[qml_enums::DimId::PosX] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::PosY] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::PosZ] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::VelX] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::VelY] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::VelZ] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::EulerX] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::EulerY] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::EulerZ] = new DdsDoubleBuffer(parent);
  qRegisterMetaType<QAbstractSeries*>();
  qRegisterMetaType<QAbstractAxis*>();
}

DdsKinematics6DBuffer::~DdsKinematics6DBuffer() = default;

QPointF DdsKinematics6DBuffer::rangePosX() const { return m_buffers.at(qml_enums::DimId::PosX)->range(); }
QPointF DdsKinematics6DBuffer::rangePosY() const { return m_buffers.at(qml_enums::DimId::PosY)->range(); }
QPointF DdsKinematics6DBuffer::rangePosZ() const { return m_buffers.at(qml_enums::DimId::PosZ)->range(); }
QPointF DdsKinematics6DBuffer::rangeVelX() const { return m_buffers.at(qml_enums::DimId::VelX)->range(); }
QPointF DdsKinematics6DBuffer::rangeVelY() const { return m_buffers.at(qml_enums::DimId::VelY)->range(); }
QPointF DdsKinematics6DBuffer::rangeVelZ() const { return m_buffers.at(qml_enums::DimId::VelZ)->range(); }
QPointF DdsKinematics6DBuffer::rangeEulerX() const { return m_buffers.at(qml_enums::DimId::EulerX)->range(); }
QPointF DdsKinematics6DBuffer::rangeEulerY() const { return m_buffers.at(qml_enums::DimId::EulerY)->range(); }
QPointF DdsKinematics6DBuffer::rangeEulerZ() const { return m_buffers.at(qml_enums::DimId::EulerZ)->range(); }

void DdsKinematics6DBuffer::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    int buffer_size,
    bool use_batch,
    bool with_listener)
{
  DdsBuffer::init(buffer_size);
  m_id = id;
  using namespace qml_enums;

  std::vector<DimId> dims =
   {
    DimId::PosX, DimId::PosY, DimId::PosZ,
    DimId::VelX, DimId::VelY, DimId::VelZ,
    DimId::EulerX, DimId::EulerY, DimId::EulerZ
   };

  for( auto dim : dims)
  {
    m_buffers.at(dim)->setCapacity(buffer_size);

    QObject::connect(
        this->m_buffers.at(dim), &DdsDoubleBuffer::rangeChanged,
        std::bind( &DdsKinematics6DBuffer::rangeChanged, this, std::placeholders::_1, dim));
  }

  if (use_batch)
    m_batchReader = std::make_unique<sinspekto::Reader<fkin::BatchKinematics6D>>(dds, topic);
  else
    m_reader = std::make_unique<sinspekto::Reader<fkin::Kinematics6D>>(dds, topic);

  if (with_listener)
  {
    if (use_batch)
    {
      m_batchReader->listener = sinspekto::DdsReaderListener<fkin::BatchKinematics6D>(
          std::bind(&DdsKinematics6DBuffer::eventHeard, this));
      m_batchReader->reader.listener(
          &m_batchReader->listener,
          dds::core::status::StatusMask::data_available());
    }
    else
    {
      m_reader->listener = sinspekto::DdsReaderListener<fkin::Kinematics6D>(
          std::bind(&DdsKinematics6DBuffer::eventHeard, this));
      m_reader->reader.listener(
          &m_reader->listener,
          dds::core::status::StatusMask::data_available());
    }

    QObject::connect(
        this, &DdsKinematics6DBuffer::eventHeard,
        this, &DdsKinematics6DBuffer::updateBuffers);
  }
}

void DdsKinematics6DBuffer::updateBuffers()
{
  if(!m_reader && !m_batchReader) return;

  auto addSampleToBuffers =
  [=](fkin::Kinematics6D& sample)
  {
    m_buffers.at(qml_enums::DimId::PosX)->Buffer().push_back(sample.position().x());
    m_buffers.at(qml_enums::DimId::PosY)->Buffer().push_back(sample.position().y());
    m_buffers.at(qml_enums::DimId::PosZ)->Buffer().push_back(sample.position().z());
    m_buffers.at(qml_enums::DimId::VelX)->Buffer().push_back(sample.velocity().x());
    m_buffers.at(qml_enums::DimId::VelY)->Buffer().push_back(sample.velocity().y());
    m_buffers.at(qml_enums::DimId::VelZ)->Buffer().push_back(sample.velocity().z());
    m_buffers.at(qml_enums::DimId::EulerX)->Buffer().push_back(sample.euler().x());
    m_buffers.at(qml_enums::DimId::EulerY)->Buffer().push_back(sample.euler().y());
    m_buffers.at(qml_enums::DimId::EulerZ)->Buffer().push_back(sample.euler().z());
  };

  if(m_reader)
  {
    const auto query = dds::sub::Query(
        m_reader->reader,
        "id = %0", {m_id.toStdString()});

    dds::sub::LoanedSamples<fkin::Kinematics6D> samples =
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

    dds::sub::LoanedSamples<fkin::BatchKinematics6D> samples =
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
         << " for BatchKinematics6D with id: " << m_id.toStdString()
         << std::endl;

      for(size_t i = 0; i < m_batchReader->sample.batch().size(); ++i)
      {
        addSampleToBuffers(m_batchReader->sample.batch()[i]);
        m_time->Buffer().push_back(m_batchReader->sample.timestamps()[i].unixMillis());
      }
      emit newData();
    }
  }
}
