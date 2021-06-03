#pragma once
/**
   @file RatatoskPosInfo.hpp
   @brief DDS subscribe adapter for ratatosk's PosInfo in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type ratatosk::types::PosInfo as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskPosInfoSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double lat READ lat NOTIFY latChanged) ///< latitude [deg]
  Q_PROPERTY(double lon READ lon NOTIFY lonChanged) ///< longitude [deg]
  Q_PROPERTY(double sog READ sog NOTIFY sogChanged) ///< speed over ground [m/s]
  Q_PROPERTY(double cog READ cog NOTIFY cogChanged) ///< course over ground [rad]
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskPosInfoSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskPosInfoSubscriber();
  /**
     @brief Property accessor for lat in QML.
     @return Latitude
  */
  double lat() const;
  /**
     @brief Property accessor for lon in QML.
     @return Longitude
  */
  double lon() const;
  /**
     @brief Property accessor for sog in QML.
     @return Speed over ground [m/s]
  */
  double sog() const;
  /**
     @brief Property accessor for cog in QML.
     @return Course over ground [rad]
  */
  double cog() const;
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

     This signal is connected to the QML RatatoskPosInfoSubscriber::lat property.

     @param[out] lat New value.
  */
  void latChanged(double lat);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskPosInfoSubscriber::lon property.

     @param[out] lon New value.
  */
  void lonChanged(double lon);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskPosInfoSubscriber::sog property.

     @param[out] sog New value.
  */
  void sogChanged(double sog);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskPosInfoSubscriber::cog property.

     @param[out] cog New value.
  */
  void cogChanged(double cog);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskPosInfoSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskPosInfoSubscriber is initialized with_listener=true, eventHeard() will be connected
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
  std::unique_ptr<sinspekto::Reader<ratatosk::types::PosInfo>> m_reader; ///< The DDS reader wrapper class.

};
