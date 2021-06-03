#include <limits>

#include "sinspekto/DdsDoubleBuffer.hpp"

namespace sinspekto
{

  RangeXY replace_double_points(
      boost::circular_buffer<double>& b1,
      boost::circular_buffer<double>& b2,
      QXYSeries *xySeries)
  {
    assert(b1.size() == b2.size());

    QVector<QPointF> bufferData;
    bufferData.reserve(static_cast<int>(b1.size()));

    auto itX = b1.begin();
    auto itY = b2.begin();

    double
     min_x = std::numeric_limits<double>::infinity(),
     min_y = min_x,
     max_x = -std::numeric_limits<double>::infinity(),
     max_y = max_x;

    double val_x, val_y;
    while(itX != b1.end())
    {
      val_x = *itX++;
      val_y = *itY++;
      if(val_x < min_x) min_x = val_x;
      if(val_x > max_x) max_x = val_x;
      if(val_y < min_y) min_y = val_y;
      if(val_y > max_y) max_y = val_y;
      bufferData.push_back(QPointF(val_x, val_y));
    }
    xySeries->replace(bufferData);

    return std::make_pair(std::make_pair(min_x, max_x), std::make_pair(min_y, max_y));
  }
}


DdsDoubleBuffer::DdsDoubleBuffer(QObject *parent) :
  QObject(parent),
  m_buffer(0),
  m_range(0,1)
{}

DdsDoubleBuffer::~DdsDoubleBuffer() = default;

void DdsDoubleBuffer::setCapacity(int buffer_size)
{
  m_buffer.set_capacity(buffer_size);
}

QPointF DdsDoubleBuffer::range() const { return m_range; }

void DdsDoubleBuffer::updateRange(double min, double max)
{
  if (
      !qFuzzyCompare(min, m_range.x()) ||
      !qFuzzyCompare(max, m_range.y()))
  {
    m_range.setX(min);
    m_range.setY(max);
    emit rangeChanged(m_range);
  }
}
