#pragma once
/**
   @file RatatoskDoubleVal.hpp
   @brief DDS publish and subscribe adapters for basic Ratatosk types in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type Double as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskDoubleValSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double val READ val NOTIFY valChanged) ///< DDS DoubleVal as double QML property.
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskDoubleValSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskDoubleValSubscriber();

  /**
     @brief Property accessor for double value in QML.
     @return Real value as double.
  */
  double val() const;
  /**
     @brief Property accessor for timestamp in QML.
     @return Timestamp as QML QDateTime.
  */
  QDateTime timestamp() const;

  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to subscribe.
     @param[in] with_listener Use DdsReaderListener callback to trigger eventHeard() signal.
  */
  Q_INVOKABLE void init(QtToDds* dds, const QString& topic, bool with_listener=true);

signals:

  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskDoubleValSubscriber::val property.

     @param[out] val New Real value.
  */
  void valChanged(double val);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskDoubleValSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskDoubleValSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValue().

  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current Double value. If there were a new sample it emits valChanged().

  */
  void updateValue();

private:
  std::unique_ptr<sinspekto::Reader<ratatosk::types::DoubleVal>> m_reader; ///< The DDS reader wrapper class.

};

/**
   @brief Publisher for DDS type Double as a QML element.

   This class is a QML element and enables QML applications to publish a
   DDS signal type from a writable QML property.

*/
class RatatoskDoubleValPublisher : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double val READ val WRITE setVal NOTIFY valChanged) ///< DDS DoubleVal as double QML property.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit RatatoskDoubleValPublisher(QObject *parent = nullptr);
  /**
     @brief Destructor
  */
  virtual ~RatatoskDoubleValPublisher();

  /**
     @brief Property accessor for double value as QML property.
     @return Double value.
  */
  double val() const;
  /**
     @brief Initializes DDS writer and sets initial value.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to publish.
     @param[in] start_value Initial value of the signal.
     @param[in] send_first Whether to forcefully send the initial value.
  */
  Q_INVOKABLE void init(QtToDds* dds, const QString& topic, double start_value, bool send_first);

signals:
  /**
     @brief DDS value has has changed.

     This signal is connected to the QML RatatoskDoubleValPublisher::val property.

     @param[out] value New Double value.
  */
  void valChanged(double value);

public slots:
  /**
     @brief Write function for the QML property, publishes value on DDS.

     This slot publishes the double value onto the DDS topic. It also emits a valChanged().
     @note It only publishes or emits a signal if the QML property value has changed!

     @param[in] val New double value.
  */
  void setVal(double val);

  /**
     @brief Publish on DDS the value already set

  */
  void publish();

private:
  std::unique_ptr<sinspekto::Writer<ratatosk::types::DoubleVal>> m_writer; ///< The DDS writer wrapper class.
};
