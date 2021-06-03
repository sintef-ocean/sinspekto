#pragma once

#include <cinttypes>
#include <boost/circular_buffer.hpp>

#include <QAbstractSeries>
#include <QXYSeries>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE

namespace sinspekto
{

  /**
     @brief A pair with a time range and a double range.

     The first element of the pair is a pair of the Qt type QDateTime, which is readable
     in QML.
     The second element of the pair is a pair of doubles.
  */
  typedef std::pair<std::pair<QDateTime, QDateTime>, std::pair<double, double>> RangeTX;

  /**
     @brief Replace data points in a QXYSeries.

     The given QXYSeries's data is replaced with the contents of the input buffers.
     Both the time points and real value buffers must be of equal length.

     @param[in] b1 Buffer of time points to set.
     @param[in] b2 Buffer of real values to set.
     @param[in,out] xySeries QXYSeries pointer to update.
     @return The range of time and range of data value (min and max values).
  */
  RangeTX replace_data_points(
      boost::circular_buffer<int64_t>& b1,
      boost::circular_buffer<double>& b2,
      QXYSeries *xySeries);
}

/**
   @brief Buffer class for holding time point data in a QML compatible data structure

   QDateTime is the Qt time point data type, which is available in QML and compatible with
   e.g. QtChart.

   @note This class is not currently exposed as a QML element, but rather used by DdsBuffer.

*/
class DdsTimepointBuffer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QDateTime rangeTmin READ rangeTmin NOTIFY rangeTminChanged) ///< Minimum time point in buffer.
  Q_PROPERTY(QDateTime rangeTmax READ rangeTmax NOTIFY rangeTmaxChanged) ///< Maximal time point in buffer.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit DdsTimepointBuffer(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsTimepointBuffer();
  /**
     @brief Access function for minimum time point in buffer as QML property.
  */
  QDateTime rangeTmin() const;
  /**
     @brief Access function for maximum time point in buffer as QML property.
  */
  QDateTime rangeTmax() const;
  /**
     @brief Pre-allocate capacity of the circular buffer.
  */
  void setCapacity(int buffer_size);
  /**
     @brief Access function to the circular buffer that stores the time points.

     @note the time points are stored as int64_t.
     @return Reference to the circular buffer.
  */
  boost::circular_buffer<int64_t>& Buffer() { return m_buffer; }

signals:
  /**
     @brief Signal to indicate that minimum time point has changed

     @param[out] min Minimum time point as a QDateTime.
  */
  void rangeTminChanged(QDateTime min);
  /**
     @brief Signal to indicate that maximum time point has changed

     @param[out] max Maximum time point as a QDateTime.
  */
  void rangeTmaxChanged(QDateTime max);
  /**
     @brief Signal to indicate that time range has changed.
  */
  void rangeChanged();

public slots:
  /**
     @brief Updates the time interval of the buffer.

     If provided min or max value is different from already registered range, the function
     emits rangeTminChanged() and rangeTmaxChanged().

     @param[in] min New minimum time point value.
     @param[in] max New maximum time point value.

  */
  void updateRange(QDateTime min, QDateTime max);

private:
  boost::circular_buffer<int64_t> m_buffer; ///< A ring buffer with time points, held as int64_t.
  QDateTime m_min_t, ///< Minimum time point value.
    m_max_t; ///< Maximal time point value.
};
