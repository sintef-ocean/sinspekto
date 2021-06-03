#pragma once
/**
   @file DdsIdVec1d.hpp
   @brief DDS publish and subscribe adapters for one-dimensional double vectors in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type IdVec1d as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsIdVec1dSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double value READ value NOTIFY valueChanged) ///< DDS 1d-vector as double QML property.
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsIdVec1dSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsIdVec1dSubscriber();

  /**
     @brief Property accessor for double value in QML.
     @return Real value as double.
  */
  double value() const;
  /**
     @brief Property accessor for timestamp in QML.
     @return Timestamp as QML compatible type.
  */
  QDateTime timestamp() const;
  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to subscribe.
     @param[in] id Key identifier for the topic instance.
     @param[in] with_listener Use DdsReaderListener callback to trigger eventHeard() signal.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& id,
      bool with_listener=true);

signals:
  /**
     @brief DDS value has changed.

     This signal is connected to the QML DdsIdVec1dSubscriber::value property.

     @param[out] value New Real value.
  */
  void valueChanged(double value);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML DdsIdVec1dSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If DdsIdVec1dSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValue().
  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current value. If there were a new sample it emits valueChanged().
  */
  void updateValue();

private:
  std::unique_ptr<sinspekto::Reader<fkin::IdVec1d>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};

/**
   @brief Publisher for DDS type IdVec1d as a QML element.

   This class is a QML element and enables QML applications to publish a
   DDS signal type from a writable QML property.
*/
class DdsIdVec1dPublisher : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged) ///< DDS 1d-vector as double QML property.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsIdVec1dPublisher(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsIdVec1dPublisher();

  /**
     @brief Property accessor for Real value as QML property.
     @return Real value as double.
  */
  double value() const;
  /**
     @brief Initializes DDS writer and sets initial value.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to publish.
     @param[in] id Key identifier for the topic instance.
     @param[in] start_value Initial value of the signal.
     @param[in] send_first Whether to forcefully send the initial value.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& id,
      double start_value,
      bool send_first);

signals:
  /**
     @brief DDS value has has changed.

     This signal is connected to the QML DdsIdVec1dPublisher::value property.

     @param[out] value New Real value.
  */
  void valueChanged(double value);

public slots:
  /**
     @brief Write function for the QML property, publishes value on DDS.

     This slot publishes the double value onto the DDS topic with the configured key
     identifier. It also emits a valueChanged().

     @note It only publishes or emits a signal if the QML property value has changed!

     @param[in] value New double value.
  */
  void setValue(double value);

  /**
     @brief Publish on DDS the value already set

  */
  void publish();


private:
  std::unique_ptr<sinspekto::Writer<fkin::IdVec1d>> m_writer; ///< The DDS writer wrapper class.
};
