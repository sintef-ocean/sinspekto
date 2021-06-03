#pragma once
/**
   @file RatatoskGyroInfo.hpp
   @brief DDS subscribe adapter for ratatosk's GryoInfo in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type ratatosk::types::GyroInfo as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskGyroInfoSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double hdt READ hdt NOTIFY hdtChanged) ///< Heading True [rad]
  Q_PROPERTY(double rot READ rot NOTIFY rotChanged) ///< Rate of turn [rad/s]
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskGyroInfoSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskGyroInfoSubscriber();
  /**
     @brief Property accessor for hdt in QML.
     @return true heading
  */
  double hdt() const;
  /**
     @brief Property accessor for rot in QML.
     @return rate of turn
  */
  double rot() const;
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

     This signal is connected to the QML RatatoskGyroInfoSubscriber::hdt property.

     @param[out] hdt New value.
  */
  void hdtChanged(double hdt);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskGyroInfoSubscriber::rot property.

     @param[out] rot New value.
  */
  void rotChanged(double rot);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskGyroInfoSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskGyroInfoSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValue().

  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current value. If there were a new sample it emits <...>Changed().

  */
  void updateValue();

private:
  std::unique_ptr<sinspekto::Reader<ratatosk::types::GyroInfo>> m_reader; ///< The DDS reader wrapper class.

};
