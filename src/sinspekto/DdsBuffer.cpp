#include <iostream>
#include <QXYSeries>
#include "sinspekto/DdsBuffer.hpp"
#include "sinspekto/QtToDds.hpp"


DdsBuffer::DdsBuffer(QObject *parent) :
  QObject(parent),
  m_time(new DdsTimepointBuffer(parent))
{
  qRegisterMetaType<QAbstractSeries*>();
  qRegisterMetaType<QAbstractAxis*>(); // needed?
}

DdsBuffer::~DdsBuffer() = default;

QDateTime DdsBuffer::rangeTmin() const { return m_time->rangeTmin(); }
QDateTime DdsBuffer::rangeTmax() const { return m_time->rangeTmax(); }

void DdsBuffer::init(int buffer_size)
{
  m_time->setCapacity(buffer_size);

  QObject::connect(this->m_time, &DdsTimepointBuffer::rangeChanged,
  this, &DdsBuffer::rangeTChanged);
  QObject::connect(this->m_time, &DdsTimepointBuffer::rangeTminChanged,
   this, &DdsBuffer::rangeTminChanged);
  QObject::connect(this->m_time, &DdsTimepointBuffer::rangeTmaxChanged,
   this, &DdsBuffer::rangeTmaxChanged);
}

void DdsBuffer::updateSeries(
    QAbstractSeries *series, qml_enums::DimId xDim, qml_enums::DimId yDim)
{
  if(!series) return;

  QXYSeries *xySeries = static_cast<QXYSeries *>(series);

  if(yDim == qml_enums::DimId::T)
    std::cerr << "Buffer's yDim "
              << __FUNCTION__
              << " cannot be a time axis" << std::endl;

  try {

    if(xDim == qml_enums::DimId::T)
    {
      auto rangeTX = sinspekto::replace_data_points(
          m_time->Buffer(),
          m_buffers.at(yDim)->Buffer(), xySeries);
      m_time->updateRange(rangeTX.first.first, rangeTX.first.second);
      m_buffers.at(yDim)->updateRange(rangeTX.second.first, rangeTX.second.second);
    }
    else
    {
      auto rangeXY = sinspekto::replace_double_points(
          m_buffers.at(xDim)->Buffer(),
          m_buffers.at(yDim)->Buffer(), xySeries);
      m_buffers.at(xDim)->updateRange(rangeXY.first.first, rangeXY.first.second);
      m_buffers.at(yDim)->updateRange(rangeXY.second.first, rangeXY.second.second);
    }
  }
  catch(std::out_of_range &)
  {
    std::cerr
     << "You tried to access a dimension that DdsBuffer does not have."
     << " You supplied DimId with underlying type values: "
     << static_cast<std::underlying_type<qml_enums::DimId>::type>(xDim)
     << ", "
     << static_cast<std::underlying_type<qml_enums::DimId>::type>(yDim)
     << ". Think about which dimensions you want and inspect DimId enum"
     << std::endl;
  }

}

void DdsBuffer::clearBuffers()
{
  m_time->Buffer().clear();

  for (auto& buf : m_buffers)
  {
    buf.second->Buffer().clear();
  }
}
