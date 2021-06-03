#include <cinttypes>
#include <functional>
#include <limits>

#include <QXYSeries>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsIdVec4dBuffer.hpp"
#include "sinspekto/QtToDdsPriv.hpp"


DdsIdVec4dBuffer::DdsIdVec4dBuffer(QObject *parent) :
  DdsBuffer(parent),
  m_reader(nullptr)
{
  m_buffers[qml_enums::DimId::X] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::Y] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::Z] = new DdsDoubleBuffer(parent);
  m_buffers[qml_enums::DimId::W] = new DdsDoubleBuffer(parent);
  qRegisterMetaType<QAbstractSeries*>();
  qRegisterMetaType<QAbstractAxis*>();
}

DdsIdVec4dBuffer::~DdsIdVec4dBuffer() = default;

QPointF DdsIdVec4dBuffer::rangeX() const { return m_buffers.at(qml_enums::DimId::X)->range(); }
QPointF DdsIdVec4dBuffer::rangeY() const { return m_buffers.at(qml_enums::DimId::Y)->range(); }
QPointF DdsIdVec4dBuffer::rangeZ() const { return m_buffers.at(qml_enums::DimId::Z)->range(); }
QPointF DdsIdVec4dBuffer::rangeW() const { return m_buffers.at(qml_enums::DimId::W)->range(); }

void DdsIdVec4dBuffer::init(
    QtToDds* dds,
    const QString& topic,
    const QString& id,
    int buffer_size,
    bool with_listener)
{
  DdsBuffer::init(buffer_size);

  m_reader = std::make_unique<sinspekto::Reader<fkin::IdVec4d>>(dds, topic);
  m_id = id;
  m_buffers.at(qml_enums::DimId::X)->setCapacity(buffer_size);
  m_buffers.at(qml_enums::DimId::Y)->setCapacity(buffer_size);
  m_buffers.at(qml_enums::DimId::Z)->setCapacity(buffer_size);
  m_buffers.at(qml_enums::DimId::W)->setCapacity(buffer_size);

  // Optional listener and connection
  if (with_listener)
  {
    m_reader->listener = sinspekto::DdsReaderListener<fkin::IdVec4d>(
        std::bind(&DdsIdVec4dBuffer::eventHeard, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());

    QObject::connect(
        this, &DdsIdVec4dBuffer::eventHeard,
        this, &DdsIdVec4dBuffer::updateBuffers);
  }

  QObject::connect(
      this->m_buffers[qml_enums::DimId::X], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsIdVec4dBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::X));
  QObject::connect(
      this->m_buffers[qml_enums::DimId::Y], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsIdVec4dBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::Y));
  QObject::connect(
      this->m_buffers[qml_enums::DimId::Z], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsIdVec4dBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::Z));
  QObject::connect(
      this->m_buffers[qml_enums::DimId::W], &DdsDoubleBuffer::rangeChanged,
      std::bind( &DdsIdVec4dBuffer::rangeChanged, this, std::placeholders::_1, qml_enums::DimId::W));
}

void DdsIdVec4dBuffer::updateBuffers()
{
  if(!m_reader) return;

  const auto query = dds::sub::Query(
      m_reader->reader,
      "id = %0", {m_id.toStdString()});
  dds::sub::LoanedSamples<fkin::IdVec4d> samples =
   m_reader->reader.select()
   .content(query)
   .state(dds::sub::status::DataState::new_data()).take();

  if(samples.length() > 0)
  {
    auto sample = (--samples.end()); // picks the last sample
    m_reader->sample = sample->data();
    m_reader->timepoint = sample->info().timestamp();
    m_buffers.at(qml_enums::DimId::X)->Buffer().push_back(m_reader->sample.vec().x());
    m_buffers.at(qml_enums::DimId::Y)->Buffer().push_back(m_reader->sample.vec().y());
    m_buffers.at(qml_enums::DimId::Z)->Buffer().push_back(m_reader->sample.vec().z());
    m_buffers.at(qml_enums::DimId::W)->Buffer().push_back(m_reader->sample.vec().w());
    m_time->Buffer().push_back(m_reader->timepoint.to_millisecs());
    emit newData();
  }
}
