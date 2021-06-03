#pragma once
/**
   @file DdsBit.hpp
   @brief DDS publish and subscribe adapters for Bit signals in QML.
*/

#include "sinspekto/QtToDds.hpp"

/**
   @brief Subscriber for DDS type Bit as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsBitSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool signal READ signal NOTIFY signalChanged) ///< DDS Bit as bool QML property.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsBitSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsBitSubscriber();

  /**
     @brief Property accessor for bit value in QML.
     @return bit value as bool.
  */
  bool signal() const;

  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to subscribe to.
     @param[in] with_listener Use DdsReaderListener callback to trigger eventHeard() signal.
  */
  Q_INVOKABLE void init(QtToDds* dds, const QString& topic, bool with_listener=true);

signals:
  /**
     @brief DDS value has changed.

     This signal is connected to the QML DdsBitSubscriber::signal property.

     @param[out] value New bit value.
  */
  void signalChanged(bool value);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If DdsBitSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateSignal().

  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current bit value. If there were a new sample it emits signalChanged().

  */
  void updateSignal();

private:
  std::unique_ptr<sinspekto::Reader<fkin::Bit>> m_reader; ///< The DDS reader wrapper class.
};

/**
   @brief Publisher for DDS type Bit as a QML element.

   This class is a QML element and enables QML applications to publish a
   DDS signal type from a writable QML property.

*/
class DdsBitPublisher : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool signal READ signal WRITE setSignal NOTIFY signalChanged) ///< DDS Bit as bool QML property.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit DdsBitPublisher(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~DdsBitPublisher();

  /**
     @brief Signaling bit value as QML property.
     @return bit value as bool.
  */
  bool signal() const;

  /**
     @brief Initializes DDS writer and sets initial value.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to publish.
     @param[in] start_value Initial value of the signal.
     @param[in] send_first Whether to forcefully send the initial value.
  */
  Q_INVOKABLE void init(QtToDds* dds, const QString& topic, bool start_value, bool send_first);

signals:
  /**
     @brief DDS value has has changed.

     This signal is connected to the QML DdsBitPublisher::signal property.

     @param[out] value New bit value.
  */
  void signalChanged(bool value);

public slots:
  /**
     @brief Write function for the QML property, publishes value on DDS

     This slot publishes the bit value onto the DDS topic. It also emits a signalChanged().
     @note It only publishes or emits a signal if the QML property value has changed!

     @param[in] value New bool value.
  */
  void setSignal(bool value);

  /**
     @brief Publish on DDS the value already set

  */
  void publish();

private:
  std::unique_ptr<sinspekto::Writer<fkin::Bit>> m_writer; ///< The DDS writer wrapper class.
};
