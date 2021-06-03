#pragma once
/**
   @file RatatoskWindInfo.hpp
   @brief DDS subscribe adapter for ratatosk's WindInfo in QML.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type ratatosk::types::WindInfo as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class RatatoskWindInfoSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double trueSpeed READ trueSpeed NOTIFY trueSpeedChanged) ///< True speed [m/s]
  Q_PROPERTY(double trueDir READ trueDir NOTIFY trueDirChanged) ///< True direction [rad]
  Q_PROPERTY(double relSpeed READ relSpeed NOTIFY relSpeedChanged) ///< Relative speed [m/s]
  Q_PROPERTY(double relDir READ relDir NOTIFY relDirChanged) ///< Relative direction [rad]
  Q_PROPERTY(double trueWindRelDir READ trueWindRelDir NOTIFY trueWindRelDirChanged) ///< True wind relative direction [m/s]
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit RatatoskWindInfoSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~RatatoskWindInfoSubscriber();
  /**
     @brief Property accessor for trueSpeed in QML.
     @return True speed [m/s]
  */
  double trueSpeed() const;
  /**
     @brief Property accessor for trueDir in QML.
     @return True direction [rad]
  */
  double trueDir() const;
  /**
     @brief Property accessor for relSpeed in QML.
     @return Relative speed [m/s]
  */
  double relSpeed() const;
  /**
     @brief Property accessor for relDir in QML.
     @return Relative direction [rad]
  */
  double relDir() const;
  /**
     @brief Property accessor for trueWindRelDir in QML.
     @return True wind relative direction [m/s]
  */
  double trueWindRelDir() const;
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

     This signal is connected to the QML RatatoskWindInfoSubscriber::trueSpeed property.

     @param[out] trueSpeed New value.
  */
  void trueSpeedChanged(double trueSpeed);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskWindInfoSubscriber::trueDir property.

     @param[out] trueDir New value.
  */
  void trueDirChanged(double trueDir);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskWindInfoSubscriber::relSpeed property.

     @param[out] relSpeed New value.
  */
  void relSpeedChanged(double relSpeed);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskWindInfoSubscriber::relDir property.

     @param[out] relDir New value.
  */
  void relDirChanged(double relDir);
  /**
     @brief DDS value has changed.

     This signal is connected to the QML RatatoskWindInfoSubscriber::trueWindRelDir property.

     @param[out] trueWindRelDir New value.
  */
  void trueWindRelDirChanged(double trueWindRelDir);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML RatatoskWindInfoSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If RatatoskWindInfoSubscriber is initialized with_listener=true, eventHeard() will be connected
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
  std::unique_ptr<sinspekto::Reader<ratatosk::types::WindInfo>> m_reader; ///< The DDS reader wrapper class.

};
