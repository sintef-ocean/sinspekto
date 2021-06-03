#pragma once
/**
   @file DdsWeatherData.hpp
   @brief DDS subscribe adapter for Netatmo weather station data.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type weather::ModuleData as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsWeatherDataSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString module_name READ module_name NOTIFY module_nameChanged)
  Q_PROPERTY(QString type READ type NOTIFY typeChanged)
  Q_PROPERTY(QString temp_trend READ temp_trend NOTIFY temp_trendChanged)
  Q_PROPERTY(QString pressure_trend READ pressure_trend NOTIFY pressure_trendChanged)
  Q_PROPERTY(double CO2 READ CO2 NOTIFY CO2Changed)
  Q_PROPERTY(double noise READ noise NOTIFY noiseChanged)
  Q_PROPERTY(double humidity READ humidity NOTIFY humidityChanged)
  Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)
  Q_PROPERTY(double pressure READ pressure NOTIFY pressureChanged)
  Q_PROPERTY(unsigned int signal_strength READ signal_strength NOTIFY signal_strengthChanged)
  Q_PROPERTY(unsigned int battery READ battery NOTIFY batteryChanged)
  Q_PROPERTY(bool reachable READ reachable NOTIFY reachableChanged)
  Q_PROPERTY(QDateTime sampleTime READ sampleTime NOTIFY sampleTimeChanged)
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsWeatherDataSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsWeatherDataSubscriber();
  /**
     @brief Property accessor for module_name in QML.
     @return User-defined name of module.
  */
  QString module_name() const;
  /**
     @brief Property accessor for type in QML.
     @return Type of module.
  */
  QString type() const;
  /**
     @brief Property accessor for temp_trend in QML.
     @return Temperature trend last 12 h.
  */
  QString temp_trend() const;
  /**
     @brief Property accessor for pressure_trend in QML.
     @return Pressure trend last 12 h.
  */
  QString pressure_trend() const;

  /**
     @brief Property accessor for CO2 [ppm] in QML.
     @return CO2.
  */
  double CO2() const;
  /**
     @brief Property accessor for noise [dB] in QML.
     @return Noise.
  */
  double noise() const;
  /**
     @brief Property accessor form humidity [%] in QML.
     @return Humidity.
  */
  double humidity() const;
  /**
     @brief Property accessor for temperature [deg C] in QML.
     @return Temperature.
  */
  double temperature() const;
  /**
     @brief Property accessor for pressure [mbar] in QML.
     @return Pressure.
  */
  double pressure() const;
  /**
     @brief Property accessor for signal_strength [56 is good, 86 is bad]  in QML.
     @return Signal strength.
  */
  unsigned int signal_strength() const;
  /**
     @brief Property accessor for battery [%]  in QML.
     @return Battery percentage.
  */
  unsigned int battery() const;
  /**
     @brief Property accessor for reachable in QML.
     @return Is module connected to cloud within 5 h.
  */
  bool reachable() const;
  /**
     @brief Property accessor for sampleTime in QML.
     @return Timestamp as QML compatible type.
  */
  QDateTime sampleTime() const;

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
     @brief User-defined name of module.

     This signal is connected to module_name QML property.

     @param[out] module_name New value.
  */
  void module_nameChanged(QString module_name);
  /**
     @brief Type name of module.

     This signal is connected to type QML property.

     @param[out] type New value.
  */
  void typeChanged(QString type);
  /**
     @brief Temperature trend (up, down, stable)

     This signal is connected to temp_trend QML property.

     @param[out] temp_trend New value.
  */
  void temp_trendChanged(QString temp_trend);
  /**
     @brief Pressure trend (up, down, stable).

     This signal is connected to pressure_trend QML property.

     @param[out] pressure_trend New value.
  */
  void pressure_trendChanged(QString pressure_trend);
  /**
     @brief Is module connected to cloud within 5 h.

     This signal is connected to reachable QML property.

     @param[out] reachable New value.
  */
  void reachableChanged(bool reachable);

  /**
     @brief A new sample has arrived.

     This signal is connected to CO2 QML property.

     @param[out] CO2 New value.
  */
  void CO2Changed(double CO2);
  /**
     @brief A new sample has arrived.

     This signal is connected to noise QML property.

     @param[out] noise New value.
  */
  void noiseChanged(double noise);
  /**
     @brief A new sample has arrived.

     This signal is connected to humidity QML property.

     @param[out] humidity New value.
  */
  void humidityChanged(double humidity);
  /**
     @brief A new sample has arrived.

     This signal is connected to temperature QML property.

     @param[out] temperature New value.
  */
  void temperatureChanged(double temperature);
  /**
     @brief A new sample has arrived.

     This signal is connected to pressure QML property.

     @param[out] pressure New value.
  */
  void pressureChanged(double pressure);
  /**
  @brief A new sample has arrived.

  This signal is connected to signal_strength QML property.

  @param[out] signal_strength New value.
  */
  void signal_strengthChanged(unsigned int signal_strength);
  /**
     @brief A new sample has arrived.

     This signal is connected to battery QML property.

     @param[out] battery New value.
  */
  void batteryChanged(unsigned int battery);
  /**
     @brief Timestamp of Weather data has changed.

     This signal is connected to the QML sampleTime property.

     @param[out] sampleTime New timestamp value.
  */
  void sampleTimeChanged(QDateTime sampleTime);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML DdsWeatherDataSubscriber::timestamp property.

     @param[out] timestamp New timestamp value.
  */
  void timestampChanged(QDateTime timestamp);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If DdsIdVec1dSubscriber is initialized with_listener=true, eventHeard() will be connected
     to the slot updateValue().
  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current value. If there were a new sample it emits valueChanged().
  */
  void updateValues();

private:
  std::unique_ptr<sinspekto::Reader<weather::ModuleData>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};
