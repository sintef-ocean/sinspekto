#pragma once
/**
   @file RatatoskCurrentAtDepth.hpp
   @brief DDS subscribe adapter for ratatosk's CurrentAtDepth in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type ratatosk::types::CurrentAtDepth as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskCurrentAtDepthSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double depth READ depth NOTIFY depthChanged) ///< Depth [m]
  Q_PROPERTY(double direction READ direction NOTIFY directionChanged) ///< Direction [rad]
  Q_PROPERTY(double speed READ speed NOTIFY speedChanged) ///< Speed [m/s]
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskCurrentAtDepthSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskCurrentAtDepthSubscriber();
  /**
     @brief Property accessor for depth in QML.
     @return depth [m]
  */
  double depth() const;
  /**
     @brief Property accessor for direction in QML.
     @return direction [rad]
  */
  double direction() const;
  /**
     @brief Property accessor for speed in QML.
     @return speed [m/s]
  */
  double speed() const;
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

     This signal is connected to the QML RatatoskCurrentAtDepthSubscriber::depth property.

     @param[out] depth New value.
  */
  void depthChanged(double depth);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskCurrentAtDepthSubscriber::direction property.

     @param[out] direction New value.
  */
  void directionChanged(double direction);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskCurrentAtDepthSubscriber::speed property.

     @param[out] speed New value.
  */
  void speedChanged(double speed);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskCurrentAtDepthSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskCurrentAtDepthSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValue().

  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current value. If there were a new sample it emits {depth, direction, speed, timestampChanged}Changed().

  */
  void updateValue();

private:
  std::unique_ptr<sinspekto::Reader<ratatosk::types::CurrentAtDepth>> m_reader; ///< The DDS reader wrapper class.

};
