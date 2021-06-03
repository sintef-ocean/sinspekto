#pragma once
/**
   @file DdsKinematics6D.hpp
   @brief DDS publish and subscribe adapters for kinematics 6d in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"
#include "QVector3D"

/**
   @brief Subscriber for DDS type Kinematics6D as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsKinematics6DSubscriber : public QObject
{
  Q_OBJECT
  /// 3D position as QVector3D QML property.
  Q_PROPERTY(QVector3D position READ position NOTIFY positionChanged)
  /// 3D velocity as QVector3D QML property.
  Q_PROPERTY(QVector3D velocity READ velocity NOTIFY velocityChanged)
  /// 3D euler angles as QVector3D QML property.
  Q_PROPERTY(QVector3D euler READ euler NOTIFY eulerChanged)
  /// Timestamp of sample.
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged)

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsKinematics6DSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsKinematics6DSubscriber();
  /**
     @brief Property accessor for position in QML.
     @return Value as QML compatible vector.
  */
  QVector3D position() const;
  /**
     @brief Property accessor for velocity in QML.
     @return Value as QML compatible vector.
  */
  QVector3D velocity() const;
  /**
     @brief Property accessor for euler angles in QML.
     @return Value as QML compatible vector.
  */
  QVector3D euler() const;
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
     @brief DDS Kinematics6D's position has changed.

     This signal is connected to the QML DdsKinematics6DSubscriber::position property.

     @param[out] position New 3d position.
  */
  void positionChanged(QVector3D position);
  /**
     @brief DDS Kinematics6D's velocity has changed.

     This signal is connected to the QML DdsKinematics6DSubscriber::velocity property.

     @param[out] velocity New 3d velocity.
  */
  void velocityChanged(QVector3D velocity);
  /**
     @brief DDS Kinematics6D's euler angle has changed.

     This signal is connected to the QML DdsKinematics6DSubscriber::euler property.

     @param[out] euler New euler angles.
  */
  void eulerChanged(QVector3D euler);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML DdsKinematics6DSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If DdsKinematics6DSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValues().
  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current Real value. If there were a new sample it emits {position, velocity, euler, timestamp}Changed().
  */
  void updateValues();

private:
  std::unique_ptr<sinspekto::Reader<fkin::Kinematics6D>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};

/**
   @brief Publisher for DDS type Kinematics6D as a QML element.

   This class is a QML element and enables QML applications to publish a
   DDS signal type from a writable QML property.
*/
class DdsKinematics6DPublisher : public QObject
{
  Q_OBJECT
  /// 3D position as QVector3D QML property.
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  /// 3D velocity as QVector3D QML property.
  Q_PROPERTY(QVector3D velocity READ velocity WRITE setVelocity NOTIFY velocityChanged)
  /// 3D euler angles as QVector3D QML property.
  Q_PROPERTY(QVector3D euler READ euler WRITE setEuler NOTIFY eulerChanged)

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsKinematics6DPublisher(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsKinematics6DPublisher();
  /**
     @brief Property accessor for position in QML.
     @return Value as QML compatible vector.
  */
  QVector3D position() const;
  /**
     @brief Property accessor for velocity in QML.
     @return Value as QML compatible vector.
  */
  QVector3D velocity() const;
  /**
     @brief Property accessor for euler angles in QML.
     @return Value as QML compatible vector.
  */
  QVector3D euler() const;
  /**
     @brief Initializes DDS writer and sets initial value.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to publish.
     @param[in] id Key identifier for the topic instance.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& id);

signals:
  /**
     @brief DDS Kinematics6D's position has changed.

     This signal is connected to the QML DdsKinematics6DPublisher::position property.

     @param[out] position New 3d position.
  */
  void positionChanged(QVector3D position);
  /**
     @brief DDS Kinematics6D's velocity has changed.

     This signal is connected to the QML DdsKinematics6DPublisher::velocity property.

     @param[out] velocity New 3d velocity.
  */
  void velocityChanged(QVector3D velocity);
  /**
     @brief DDS Kinematics6D's euler angle has changed.

     This signal is connected to the QML DdsKinematics6DPublisher::euler property.

     @param[out] euler New euler angles.
  */
  void eulerChanged(QVector3D euler);

public slots:
  /**
     @brief Write function for the QML property, does not publish value on DDS.

     This slot updates the DDS sample with the new position. It emits a positionChanged().

     @note It only emits a signal if the QML property value has changed!
     @note This function does not publish the DDS sample. sendSample() must be called.

     @param[in] position New vector value.
  */
  void setPosition(QVector3D position);
  /**
     @brief Write function for the QML property, does not publish value on DDS.

     This slot updates the DDS sample with the new velocity. It emits a velocityChanged().

     @note It only emits a signal if the QML property value has changed!
     @note This function does not publish the DDS sample. sendSample() must be called.

     @param[in] velocity New vector value.
  */
  void setVelocity(QVector3D velocity);
  /**
     @brief Write function for the QML property, does not publish value on DDS.

     This slot updates the DDS sample with the new euler. It emits a eulerChanged().

     @note It only emits a signal if the QML property value has changed!
     @note This function does not publish the DDS sample. sendSample() must be called.

     @param[in] euler New vector value.
  */
  void setEuler(QVector3D euler);
  /**
     @brief Publishes the sample value on DDS.

     The currently stored sample is sent over DDS. Typically setPosition(), setVelocity(),
     and setEuler() are called before sending the sample.

  */
  void publish();

private:
  std::unique_ptr<sinspekto::Writer<fkin::Kinematics6D>> m_writer; ///< The DDS writer wrapper class.
};
