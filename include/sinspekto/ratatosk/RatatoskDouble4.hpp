#pragma once
/**
   @file RatatoskDouble4.hpp
   @brief DDS publish and subscribe adapters for four-dimensional double vectors in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"
#include "QVector4D"

/**
   @brief Subscriber for DDS type ratatosk::types::Double4 as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskDouble4Subscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVector4D value READ value NOTIFY valueChanged) ///< DDS 4d-vector as QVector4D QML property.
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskDouble4Subscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskDouble4Subscriber();
  /**
     @brief Property accessor for vector in QML.
     @return Value as QML compatible vector.
  */
  QVector4D value() const;
  /**
     @brief Property accessor for timestamp in QML.
     @return Timestamp as QML compatible type.
  */
  QDateTime timestamp() const;
  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to subscribe.
     @param[in] with_listener Use DdsReaderListener callback to trigger eventHeard() signal.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      bool with_listener=true);

signals:
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskDouble4Subscriber::value property.

     @param[out] value New vector value.
  */
  void valueChanged(QVector4D value);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskDouble4Subscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskDouble4Subscriber is initialized with_listener=true, eventHeard() will be connected
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
  std::unique_ptr<sinspekto::Reader<ratatosk::types::Double4>> m_reader; ///< The DDS reader wrapper class.

};

/**
   @brief Publisher for DDS type ratatosk::types::Double4 as a QML element.

   This class is a QML element and enables QML applications to publish a
   DDS signal type from a writable QML property.

*/
class RatatoskDouble4Publisher : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVector4D value READ value WRITE setValue NOTIFY valueChanged) ///< DDS 3d-vector as QVector4D QML property.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskDouble4Publisher(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskDouble4Publisher();

  /**
     @brief Property accessor for vector as QML property.
     @return QML compatible vector.
  */
  QVector4D value() const;
  /**
     @brief Initializes DDS writer and sets initial value.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to publish.
     @param[in] start_value Initial value of the signal.
     @param[in] send_first Whether to forcefully send the initial value.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      QVector4D start_value,
      bool send_first);

signals:
  /**
     @brief DDS value has has changed.

     This signal is connected to the QML RatatoskDouble4Publisher::value property.

     @param[out] value New vector value.
  */
  void valueChanged(QVector4D value);

public slots:
  /**
     @brief Write function for the QML property, publishes value on DDS.

     This slot publishes the vector onto the DDS topic with the configured key
     identifier. It also emits a valueChanged().

     @note It only publishes or emits a signal if the QML property value has changed!

     @param[in] value New vector value.
  */
  void setValue(QVector4D value);

  /**
     @brief Publish on DDS the value already set

  */
  void publish();

private:
  std::unique_ptr<sinspekto::Writer<ratatosk::types::Double4>> m_writer; ///< The DDS writer wrapper class.
};
