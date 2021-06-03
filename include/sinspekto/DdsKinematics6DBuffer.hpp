#pragma once
/**
   @file DdsKinematics6DBuffer.hpp
   @brief QML enabled buffer for Kinematics6D.
*/

#include "sinspekto/QtToDds.hpp"
#include "sinspekto/DdsBuffer.hpp"

/**
   @brief Buffer class for Kinematics6D or BatchKinematics6D

   This class subscribes to a DDS topic and makes sure the buffers are updated with incoming data.

   When updating a QAbstractSeries with DdsBuffer::updateSeries() in QML scripts, the
   following mapping from DDS member variables to qml_enums::DimId enums are
   used.

   \rst
   .. table::

     +-----------------+------------+
     | QML enum        | Variable   |
     +-----------------+------------+
     | ``FKIN.T``      | time       |
     +-----------------+------------+
     | ``FKIN.PosX``   | position.x |
     +-----------------+------------+
     | ``FKIN.PosY``   | position.y |
     +-----------------+------------+
     | ``FKIN.PosZ``   | position.z |
     +-----------------+------------+
     | ``FKIN.VelX``   | velocity.x |
     +-----------------+------------+
     | ``FKIN.VelY``   | velocity.y |
     +-----------------+------------+
     | ``FKIN.VelZ``   | velocity.z |
     +-----------------+------------+
     | ``FKIN.EulerX`` | euler.x    |
     +-----------------+------------+
     | ``FKIN.EulerY`` | euler.y    |
     +-----------------+------------+
     | ``FKIN.EulerZ`` | euler.z    |
     +-----------------+------------+

   .. code-block:: qml

     aBuffer.updateSeries(someSeries, FKIN.T, FKIN.EulerX)

   \endrst

*/
class DdsKinematics6DBuffer : public DdsBuffer
{
  Q_OBJECT
  Q_PROPERTY(QPointF rangePosX READ rangePosX NOTIFY rangeChanged) ///< Data range of position x.
  Q_PROPERTY(QPointF rangePosY READ rangePosY NOTIFY rangeChanged) ///< Data range of position y.
  Q_PROPERTY(QPointF rangePosZ READ rangePosZ NOTIFY rangeChanged) ///< Data range of position z.
  Q_PROPERTY(QPointF rangeVelX READ rangeVelX NOTIFY rangeChanged) ///< Data range of velocity x.
  Q_PROPERTY(QPointF rangeVelY READ rangeVelY NOTIFY rangeChanged) ///< Data range of velocity y.
  Q_PROPERTY(QPointF rangeVelZ READ rangeVelZ NOTIFY rangeChanged) ///< Data range of velocity z.
  Q_PROPERTY(QPointF rangeEulerX READ rangeEulerX NOTIFY rangeChanged) ///< Data range of euler x.
  Q_PROPERTY(QPointF rangeEulerY READ rangeEulerY NOTIFY rangeChanged) ///< Data range of euler y.
  Q_PROPERTY(QPointF rangeEulerZ READ rangeEulerZ NOTIFY rangeChanged) ///< Data range of euler z.

public:
  /**
     @brief Constructor

     The initialization of DDS instances are deferred to an init() function, but it
     initializes the data buffers.

     @param[in] parent QObject pointer.
  */
  explicit DdsKinematics6DBuffer(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsKinematics6DBuffer();
  /**
     @brief Property accessor for position x range.
  */
  QPointF rangePosX() const;
  /**
     @brief Property accessor for position y range.
  */
  QPointF rangePosY() const;
  /**
     @brief Property accessor for position z range.
  */
  QPointF rangePosZ() const;
  /**
     @brief Property accessor for velocity x range.
  */
  QPointF rangeVelX() const;
  /**
     @brief Property accessor for velocity y range.
  */
  QPointF rangeVelY() const;
  /**
     @brief Property accessor for velocity z range.
  */
  QPointF rangeVelZ() const;
  /**
     @brief Property accessor for euler x range.
  */
  QPointF rangeEulerX() const;
  /**
     @brief Property accessor for euler y range.
  */
  QPointF rangeEulerY() const;
  /**
     @brief Property accessor for euler z range.
  */
  QPointF rangeEulerZ() const;
  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @note An instance of this class can only be buffer for a single DDS topic, so either
     type Kinematics2D or BatchKinematics2D.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to subscribe.
     @param[in] id Key identifier for the topic instance.
     @param[in] buffer_size Number of data points to store in the circular buffer.
     @param[in] use_batch If true, subscribe to BatchIdVec1d, otherwise IdVec1d.
     @param[in] with_listener Use DdsReaderListener callback to trigger eventHeard() signal.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& id,
      int buffer_size,
      bool use_batch,
      bool with_listener=true);

signals:
  /**
     @brief Triggered when the range has changed.

     See class documentation for supported DimId enums.

     @note For time dimension, use DdsBuffer::rangeTminChanged() and DdsBuffer::rangeTmaxChanged().
     @param[out] range New range for the dimension.
     @param[out] dim The dimension in question.
  */
  void rangeChanged(QPointF range, qml_enums::DimId dim);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If init() is called with_listener=true, eventHeard() will be connected
     to the slot updateBuffers().
  */
  void eventHeard();
  /**
     @brief Signal whenever new data is added to the buffers.
  */
  void newData();

public slots:
  /**
     @brief Adds samples to the appropriate buffer.

     Each data member of the DDS data structure is separated into an appropriate
     buffer. It picks the last sample and emits newData().
  */
  virtual void updateBuffers();

private:
  std::unique_ptr<sinspekto::Reader<fkin::Kinematics6D>> m_reader; ///< The DDS reader wrapper class.
  std::unique_ptr<sinspekto::Reader<fkin::BatchKinematics6D>> m_batchReader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};
