#include <limits>

#include "sinspekto/DdsTimepointBuffer.hpp"


namespace sinspekto
{

  RangeTX replace_data_points(
      boost::circular_buffer<int64_t>& b1,
      boost::circular_buffer<double>& b2,
      QXYSeries *xySeries)
  {
    assert(b1.size() == b2.size());

    QVector<QPointF> bufferData;
    bufferData.reserve(static_cast<int>(b1.size()));

    auto itX = b1.begin();
    auto itY = b2.begin();

    double
     min_y = std::numeric_limits<double>::infinity(),
     max_y = -std::numeric_limits<double>::infinity();
    int64_t
     min_x = std::numeric_limits<int64_t>::max(),
     max_x = std::numeric_limits<int64_t>::min();

    int64_t val_x;
    double val_y;
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

    QDateTime min_t, max_t;
    min_t.setMSecsSinceEpoch(min_x);
    max_t.setMSecsSinceEpoch(max_x);

    return std::make_pair(std::make_pair(min_t, max_t), std::make_pair(min_y, max_y));
  }
}


DdsTimepointBuffer::DdsTimepointBuffer(QObject *parent) :
  QObject(parent),
  m_buffer(0),
  m_min_t(QDateTime::currentDateTime()),
  m_max_t(QDateTime::currentDateTime())
{}

DdsTimepointBuffer::~DdsTimepointBuffer() = default;

void DdsTimepointBuffer::setCapacity(int buffer_size)
{
  m_buffer.set_capacity(buffer_size);
}

QDateTime DdsTimepointBuffer::rangeTmin() const { return m_min_t; }
QDateTime DdsTimepointBuffer::rangeTmax() const { return m_max_t; }


void DdsTimepointBuffer::updateRange(QDateTime min, QDateTime max)
{
  bool minC = false, maxC = false;
  if (min != m_min_t)
  {
    minC = true;
    m_min_t = min;
    emit rangeTminChanged(m_min_t);
  }
  if (max != m_max_t)
  {
    maxC = true;
    m_max_t = max;
    emit rangeTmaxChanged(m_max_t);
  }
  if (minC || maxC)
    emit rangeChanged();

}
