#include <cinttypes>
#include <functional>
#include <limits>

#include <QXYSeries>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsIdVec1dBuffer.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsIdVec1dBuffer::DdsIdVec1dBuffer(QObject *parent) :
  DdsBuffer(parent),
  m_reader(nullptr),
  m_batchReader(nullptr)
{
  m_buffers[qml_enums::DimId::X] = new DdsDoubleBuffer(parent);
  qRegisterMetaType<QAbstractSeries*>();
  qRegisterMetaType<QAbstractAxis*>();
}

DdsIdVec1dBuffer::~DdsIdVec1dBuffer() = default;

QPointF DdsIdVec1dBuffer::rangeX() const { return m_buffers.at(qml_enums::DimId::X)->range(); }

void DdsIdVec1dBuffer::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    int buffer_size,
    bool use_batch,
    bool with_listener)
{
  DdsBuffer::init(buffer_size);

  m_id = id;
  m_buffers.at(qml_enums::DimId::X)->setCapacity(buffer_size);

  QObject::connect(
      this->m_buffers[qml_enums::DimId::X], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsIdVec1dBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::X));

  if(use_batch)
    m_batchReader = std::make_unique<sinspekto::Reader<fkin::BatchIdVec1d>>(dds, topic);
  else
    m_reader = std::make_unique<sinspekto::Reader<fkin::IdVec1d>>(dds, topic);

  // Optional listener and connection
  if (with_listener)
  {
    if(use_batch)
    {
      m_batchReader->listener = sinspekto::DdsReaderListener<fkin::BatchIdVec1d>(
          std::bind(&DdsIdVec1dBuffer::eventHeard, this));
      m_batchReader->reader.listener(
          &m_batchReader->listener,
          dds::core::status::StatusMask::data_available());
    }
    else
    {
      m_reader->listener = sinspekto::DdsReaderListener<fkin::IdVec1d>(
          std::bind(&DdsIdVec1dBuffer::eventHeard, this));
      m_reader->reader.listener(
          &m_reader->listener,
          dds::core::status::StatusMask::data_available());
    }
    QObject::connect(
        this, &DdsIdVec1dBuffer::eventHeard,
        this, &DdsIdVec1dBuffer::updateBuffers);
  }
}

void DdsIdVec1dBuffer::updateBuffers()
{
  if(!m_reader && !m_batchReader) return;

  auto addSampleToBuffers =
  [=](fkin::IdVec1d& sample)
  {
    m_buffers.at(qml_enums::DimId::X)->Buffer().push_back(sample.vec().x());
  };

  if(m_reader)
  {
    const auto query = dds::sub::Query(
        m_reader->reader,
        "id = %0", {m_id.toStdString()});
    dds::sub::LoanedSamples<fkin::IdVec1d> samples =
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
    dds::sub::LoanedSamples<fkin::BatchIdVec1d> samples =
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
