#pragma once

#include <cinttypes>
#include <utility>
#include <boost/circular_buffer.hpp>

#include <QAbstractSeries>
#include <QXYSeries>

QT_CHARTS_USE_NAMESPACE

namespace sinspekto
{

  /**
     @brief A pair with two double ranges.

     Each element of the pair is also a pair. These pair indicate a range, with double as
     type.

  */
  typedef std::pair<std::pair<double, double>, std::pair<double, double>> RangeXY;

  /**
     @brief Replace data points in a QXYSeries.

     The given QXYSeries's data is replaced with the contents of the input buffers. Both
     input buffers must be of equal length.

     @param[in] b1 Buffer of real values to set.
     @param[in] b2 Buffer of real values to set.
     @param[in,out] xySeries QXYSeries pointer to update.
     @return The ranges of each dimens (min and max values).
  */
  RangeXY replace_double_points(
      boost::circular_buffer<double>& b1,
      boost::circular_buffer<double>& b2,
      QXYSeries *xySeries);
}

/**
   @brief Buffer class for holding floating point scaler data in a QML compatible data
   structure

   QPointF Qt data type is used for the data range. This makes it available in QML and
   compatible with e.g. QtChart.

   @note This class is not currently exposed as a QML element, but rather used by DdsBuffer.

*/
class DdsDoubleBuffer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QPointF range READ range NOTIFY rangeChanged) ///< The value range in the buffer

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit DdsDoubleBuffer(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsDoubleBuffer();
  /**
     @brief Access function for minimum and maximum value of the buffered data.
  */
  QPointF range() const;
  /**
     @brief Pre-allocate capacity of the circular buffer.
  */
  void setCapacity(int buffer_size);
  /**
     @brief Access function to the circular buffer that stores the data.

     @return Reference to the circular buffer.
  */
  boost::circular_buffer<double>& Buffer() { return m_buffer; }

signals:
  /**
     @brief Signal to indicate that data range has changed.
  */
  void rangeChanged(QPointF range);

public slots:
  /**
     @brief Updates the data range of the buffer.

     If provided min or max value is different from already registered range, the function
     emits rangeChanged(). It uses Qt qFuzzyCompare() to decide if the range has changed.

     @param[in] min New minimum data value.
     @param[in] max New maximum data value.

  */
  void updateRange(double min, double max);

private:
  boost::circular_buffer<double> m_buffer; ///< A ring buffer with time points, held as double.
  QPointF m_range; ///< Data range of buffer. QPointF.x() is minimum, QPoint.y() is maximum.
};
