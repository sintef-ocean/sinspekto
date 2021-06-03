#pragma once
/**
   @file RatatoskCurrentProfile.hpp
   @brief DDS subscribe adapter for ratatosk's CurrentProfile in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"
#include <vector>

/**
   @brief Subscriber for DDS type ratatosk::types::CurrentProfile as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskCurrentProfileSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(std::vector<double> depths READ depths NOTIFY depthsChanged) ///< Depths [m]
  Q_PROPERTY(std::vector<double> directions READ directions NOTIFY directionsChanged) ///< Directions [rad]
  Q_PROPERTY(std::vector<double> speeds READ speeds NOTIFY speedsChanged) ///< Speeds [m/s]
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskCurrentProfileSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskCurrentProfileSubscriber();
  /**
     @brief Property accessor for current depths in QML.
     @return depths [m]
  */
  std::vector<double> depths() const;
  /**
     @brief Property accessor for current directions in QML.
     @return directions [rad]
  */
  std::vector<double> directions() const;
  /**
     @brief Property accessor for current speeds in QML.
     @return speeds [m/s]
  */
  std::vector<double> speeds() const;
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

     This signal is connected to the QML RatatoskCurrentProfileSubscriber::depths property.

     @param[out] depths New values.
  */
  void depthsChanged(std::vector<double> depths);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskCurrentProfileSubscriber::directions property.

     @param[out] directions New values.
  */
  void directionsChanged(std::vector<double> directions);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskCurrentProfileSubscriber::speeds property.

     @param[out] speeds New values.
  */
  void speedsChanged(std::vector<double> speeds);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskCurrentProfileSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskCurrentProfileSubscriber is initialized with_listener=true, eventHeard() will be connected
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
  std::unique_ptr<sinspekto::Reader<ratatosk::types::CurrentProfile>> m_reader; ///< The DDS reader wrapper class.

};
