#pragma once
/**
   @file DdsKinematics2D.hpp
   @brief DDS publish and subscribe adapters for kinematics 2d in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"
#include "QVector2D"

/**
   @brief Subscriber for DDS type Kinematics2D as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsKinematics2DSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVector2D position READ position NOTIFY positionChanged) ///< 2D position as QVector2D QML property.
  Q_PROPERTY(double speed READ speed NOTIFY speedChanged)             ///< Speed of vehicle as double.
  Q_PROPERTY(double course READ course NOTIFY courseChanged)          ///< Course of vehicle as double.
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsKinematics2DSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsKinematics2DSubscriber();
  /**
     @brief Property accessor for position in QML.
     @return Value as QML compatible vector.
  */
  QVector2D position() const;
  /**
     @brief Property accessor for speed in QML.
     @return Value as QML double.
  */
  double speed() const;
  /**
     @brief Property accessor for course in QML.
     @return Value as QML double.
  */
  double course() const;
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
     @brief DDS Kinematics2D's position has changed.

     This signal is connected to the QML DdsKinematics2DSubscriber::position property.

     @param[out] position New 2d position.
  */
  void positionChanged(QVector2D position);
  /**
     @brief DDS Kinematics2D's speed has changed.

     This signal is connected to the QML DdsKinematics2DSubscriber::speed property.

     @param[out] speed New speed.
  */
  void speedChanged(double speed);
  /**
     @brief DDS Kinematics2D's course has changed.

     This signal is connected to the QML DdsKinematics2DSubscriber::course property.

     @param[out] course New course.
  */
  void courseChanged(double course);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML DdsKinematics2DSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If DdsKinematics2DSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValues().
  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current Real value. If there were a new sample it emits {position, speed, course, timestamp}Changed().
  */
  void updateValues();

private:
  std::unique_ptr<sinspekto::Reader<fkin::Kinematics2D>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};

/**
   @brief Publisher for DDS type Kinematics2D as a QML element.

   This class is a QML element and enables QML applications to publish a
   DDS signal type from a writable QML property.
*/
class DdsKinematics2DPublisher : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVector2D position READ position WRITE setPosition NOTIFY positionChanged) ///< 2D position as QVector2D QML property.
  Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged) ///< Speed of vehicle as double.
  Q_PROPERTY(double course READ course WRITE setCourse NOTIFY courseChanged) ///< Course of vehicle as double.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsKinematics2DPublisher(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsKinematics2DPublisher();
  /**
     @brief Property accessor for position in QML.
     @return Value as QML compatible vector.
  */
  QVector2D position() const;
  /**
     @brief Property accessor for speed in QML.
     @return Value as QML double.
  */
  double speed() const;
  /**
     @brief Property accessor for course in QML.
     @return Value as QML double.
  */
  double course() const;
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
     @brief DDS Kinematics2D's position has changed.

     This signal is connected to the QML DdsKinematics2DPublisher::position property.

     @param[out] position New 2d position.
  */
  void positionChanged(QVector2D position);
  /**
     @brief DDS Kinematics2D's speed has changed.

     This signal is connected to the QML DdsKinematics2DPublisher::speed property.

     @param[out] speed New speed.
  */
  void speedChanged(double speed);
  /**
     @brief DDS Kinematics2D's course has changed.

     This signal is connected to the QML DdsKinematics2DPublisher::course property.

     @param[out] course New course.
  */
  void courseChanged(double course);

public slots:
  /**
     @brief Write function for the QML property, does not publish value on DDS.

     This slot updates the DDS sample with the new position. It emits a positionChanged().

     @note It only emits a signal if the QML property value has changed!
     @note This function does not publish the DDS sample. sendSample() must be called.

     @param[in] position New vector value.
  */
  void setPosition(QVector2D position);
  /**
     @brief Write function for the QML property, does not publish value on DDS.

     This slot updates the DDS sample with the new speed. It emits a speedChanged().

     @note It only emits a signal if the QML property value has changed!
     @note This function does not publish the DDS sample. sendSample() must be called.

     @param[in] speed New speed value.
  */
  void setSpeed(double speed);
  /**
     @brief Write function for the QML property, does not publish value on DDS.

     This slot updates the DDS sample with the new course. It emits a courseChanged().

     @note It only emits a signal if the QML property value has changed!
     @note This function does not publish the DDS sample. sendSample() must be called.

     @param[in] course New course value.
  */
  void setCourse(double course);
    /**
     @brief Publishes the sample value on DDS.

     The currently stored sample is sent over DDS. Typically setPosition(), setSpeed(),
     and setCourse() are called before sending the sample.

  */
  void publish();

private:
  std::unique_ptr<sinspekto::Writer<fkin::Kinematics2D>> m_writer; ///< The DDS writer wrapper class.
};
