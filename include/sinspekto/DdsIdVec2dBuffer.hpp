#pragma once
/**
   @file DdsIdVec2dBuffer.hpp
   @brief QML enabled buffer class for Vec2d
*/

#include <memory>

#include "sinspekto/QtToDds.hpp"
#include "sinspekto/DdsBuffer.hpp"

/**
   @brief Buffer class for IdVec2d or BatchIdVec2d

   This class subscribes to a DDS topic and makes sure the buffers are updated with incoming data.

   When updating a QAbstractSeries with DdsBuffer::updateSeries() in QML scripts, the
   following mapping from DDS member variables to qml_enums::DimId enums are
   used.

   \rst
   .. table::

     +------------+----------+
     | QML enum   | Variable |
     +------------+----------+
     | ``FKIN.T`` | time     |
     +------------+----------+
     | ``FKIN.X`` | x        |
     +------------+----------+
     | ``FKIN.Y`` | y        |
     +------------+----------+

   .. code-block:: qml

     aBuffer.updateSeries(someSeries, FKIN.T, FKIN.Y)

   \endrst

   // TODO: Add batch buffer.

*/
class DdsIdVec2dBuffer : public DdsBuffer
{
  Q_OBJECT
  Q_PROPERTY(QPointF rangeX READ rangeX NOTIFY rangeChanged) ///< Data range of variable x.
  Q_PROPERTY(QPointF rangeY READ rangeY NOTIFY rangeChanged) ///< Data range of variable y.


public:
  /**
     @brief Constructor

     The initialization of DDS instances are deferred to an init() function, but it
     initializes the data buffers.

     @param[in] parent QObject pointer.
  */
  explicit DdsIdVec2dBuffer(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsIdVec2dBuffer();
  /**
     @brief Property accessor for x variable range.
  */
  QPointF rangeX() const;
  /**
     @brief Property accessor for y variable range.
  */
  QPointF rangeY() const;
  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @note An instance of this class can only be buffer for a single DDS topic, so either
     type IdVec2d or BatchIdVec2d.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to subscribe.
     @param[in] id Key identifier for the topic instance.
     @param[in] buffer_size Number of data points to store in the circular buffer.
     @param[in] with_listener Use DdsReaderListener callback to trigger eventHeard() signal.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& id,
      int buffer_size,
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
  std::unique_ptr<sinspekto::Reader<fkin::IdVec2d>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.
};
