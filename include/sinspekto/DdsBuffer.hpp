#pragma once

#include <map>
#include <QObject>
#include <QDateTime>
#include <QAbstractSeries>

#include "sinspekto/DdsDoubleBuffer.hpp"
#include "sinspekto/DdsTimepointBuffer.hpp"

namespace qml_enums{ enum class DimId; }

QT_CHARTS_USE_NAMESPACE

/**
   @brief Buffer class for holding data in QtChart compatible data structures

   This class has a virtual slot function updateBuffers() that needs to be defined in the
   derived class.

*/
class DdsBuffer : public QObject
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
  explicit DdsBuffer(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsBuffer();

  /**
     @brief Access function for minimum time point in buffer as QML property.
  */
  QDateTime rangeTmin() const;
  /**
     @brief Access function for maximal time point in buffer as QML property.
  */
  QDateTime rangeTmax() const;

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
  void rangeTChanged();

public slots:

  /**
     @brief Updates a QAbstractSeries with the data currently in the buffer

     This slots updates the input data series structure with the data in the buffers held
     by this class. It calls either replace_data_points() and replace_double_points()
     depending on the dimension type. The buffer member variable can hold several buffers,
     and are fetched using a specific DimId enum. This enables indexing in QML scripting
     using e.g. `Fkin.Course`, instead of integer indexing.

     @note yDim cannot be a time axis, that is, not DimId::T.

     @param[in,out] series Pointer to series to be updated.
     @param[in] xDim First dimension identifier.
     @param[in] yDim Second dimension identifier.
  */
  void updateSeries(
      QAbstractSeries *series,
      qml_enums::DimId xDim,
      qml_enums::DimId yDim);

  /**
     @brief A function that should update the buffers with new data.

     A possible use case would be to read data from a DDS topic and add them to
     appropriately mapped buffers in this class.

     For example a Vec2d DDS type would append three buffers for each sample, two in the
     mapped buffer (DimId::X, DimId::Y) and one for time point.

  */
  virtual void updateBuffers() = 0;

  /**
     @brief Clears all buffers
  */
  void clearBuffers();

  /**
     @brief Initialize buffer and connect signals and slots for the time range properties.

     @param[in] buffer_size Set capacity of DdsTimepointBuffer
  */
  void init(int buffer_size);

protected:
  std::map<qml_enums::DimId, DdsDoubleBuffer *> m_buffers; ///< A map of buffers, key is DimId, value is DdsDoubleBuffer
  DdsTimepointBuffer* m_time; ///< Pointer to time point buffer.
};
