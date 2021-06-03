#pragma once
/**
   @file DdsOptiStats.hpp
   @brief DDS subscriber for NLP optimization statistics.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"
#include <vector>

/**
   @brief Subscriber for DDS type fkin::OptiStats as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsOptiStatsSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned int iterations READ iterations NOTIFY iterationsChanged)
  Q_PROPERTY(bool status READ status NOTIFY statusChanged)
  Q_PROPERTY(QString status_text READ status_text NOTIFY status_textChanged)
  Q_PROPERTY(double obj READ obj NOTIFY objChanged)
  Q_PROPERTY(double solve_time READ solve_time NOTIFY solve_timeChanged)
  Q_PROPERTY(std::vector<double> p READ p NOTIFY pChanged)
  Q_PROPERTY(std::vector<double> x0 READ x0 NOTIFY x0Changed)
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsOptiStatsSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsOptiStatsSubscriber();

  /**
     @brief Property accessor for number of iterations to solve NLP [-] in QML.
     @return Iteration count.
  */
  unsigned int iterations() const;
  /**
     @brief Property accessor for status of NLP solve in QML.
     @return status.
  */
  bool status() const;
  /**
     @brief Property accessor for status_text in QML.
     @return Status text for NLP.
  */
  QString status_text() const;
  /**
     @brief Property accessor for NLP objective function in QML.
     @return objective.
  */
  double obj() const;
  /**
     @brief Property accessor for wall clock solve time [s] for NLP in QML.
     @return solve time.
  */
  double solve_time() const;
  /**
     @brief Property accessor for NLP parameter vector in QML.
     @return parameters.
  */
  std::vector<double> p() const;
  /**
     @brief Property accessor for NLP state initial condition vector in QML.
     @return initial condition x0.
  */
  std::vector<double> x0() const;
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
     @brief A new sample has arrived.

     This signal is connected to iterations QML property.

     @param[out] iterations New value.
  */
  void iterationsChanged(unsigned long iterations);
  /**
     @brief A new sample has arrived.

     This signal is connected to status QML property.

     @param[out] status New value.
  */
  void statusChanged(bool status);
  /**
     @brief A new sample has arrived.

     This signal is connected to technique QML property.

     @param[out] status_text New value.
  */
  void status_textChanged(QString status_text);
  /**
     @brief A new sample has arrived.

     This signal is connected to obj QML property.

     @param[out] objective New value.
  */
  void objChanged(double objective);
  /**
     @brief A new sample has arrived.

     This signal is connected to solve_time QML property.

     @param[out] solve_time New value.
  */
  void solve_timeChanged(double solve_time);
  /**
     @brief A new sample has arrived.

     This signal is connected to p QML property.

     @param[out] parameters New value.
  */
  void pChanged(std::vector<double> parameters);
  /**
     @brief A new sample has arrived.

     This signal is connected to x0 QML property.

     @param[out] x0 New value.
  */
  void x0Changed(std::vector<double> x0);
  /**
     @brief Timestamp of DDS value has changed.

     This signal is connected to the QML DdsNlpConfigSubscriber::timestamp property.

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
     current value. If there were a new sample it emits various <..>Changed().
  */
  void updateValues();

private:
  std::unique_ptr<sinspekto::Reader<fkin::OptiStats>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};
