#pragma once
/**
   @file DdsKinematics2DBuffer.hpp
   @brief QML enabled buffer for Kinematics2D
*/

#include "sinspekto/QtToDds.hpp"
#include "sinspekto/DdsBuffer.hpp"

/**
   @brief Buffer class for Kinematics2D or BatchKinematics2D

   This class subscribes to a DDS topic and makes sure the buffers are updated with incoming data.

   When updating a QAbstractSeries with DdsBuffer::updateSeries() in QML scripts, the
   following mapping from DDS member variables to qml_enums::DimId enums are
   used.

   \rst
   .. table::

     +------------------------------+
     | QML enum        | Variable   |
     +-----------------+------------+
     | ``FKIN.T``      | time       |
     +-----------------+------------+
     | ``FKIN.PosX``   | position.x |
     +-----------------+------------+
     | ``FKIN.PosY``   | position.y |
     +-----------------+------------+
     | ``FKIN.Speed``  | speed      |
     +-----------------+------------+
     | ``FKIN.Course`` | course     |
     +-----------------+------------+

   .. code-block:: qml

     aBuffer.updateSeries(someSeries, FKIN.T, FKIN.Course)

   \endrst

*/
class DdsKinematics2DBuffer : public DdsBuffer
{
  Q_OBJECT
  Q_PROPERTY(QPointF rangePosX READ rangePosX NOTIFY rangeChanged) ///< Data range of variable x.
  Q_PROPERTY(QPointF rangePosY READ rangePosY NOTIFY rangeChanged) ///< Data range of variable y.
  Q_PROPERTY(QPointF rangeCourse READ rangeCourse NOTIFY rangeChanged) ///< Data range of variable course.
  Q_PROPERTY(QPointF rangeSpeed READ rangeSpeed NOTIFY rangeChanged)  ///< Data range of variable speed.


public:
  /**
     @brief Constructor

     The initialization of DDS instances are deferred to an init() function, but it
     initializes the data buffers.

     @param[in] parent QObject pointer.
  */
  explicit DdsKinematics2DBuffer(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsKinematics2DBuffer();
  /**
     @brief Property accessor for position x variable range.
  */
  QPointF rangePosX() const;
  /**
     @brief Property accessor for position y variable range.
  */
  QPointF rangePosY() const;
  /**
     @brief Property accessor for course variable range.
  */
  QPointF rangeCourse() const;
    /**
     @brief Property accessor for speed variable range.
  */
  QPointF rangeSpeed() const;
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
  std::unique_ptr<sinspekto::Reader<fkin::Kinematics2D>> m_reader; ///< The DDS reader wrapper class.
  std::unique_ptr<sinspekto::Reader<fkin::BatchKinematics2D>> m_batchReader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};
