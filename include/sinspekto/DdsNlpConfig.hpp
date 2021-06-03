#pragma once
/**
   @file DdsNlpConfig.hpp
   @brief DDS subscriber for NLP configuration settings.
*/

#include "sinspekto/QtToDds.hpp"
#include "QDateTime"

/**
   @brief Subscriber for DDS type fkin::NlpConfig as a QML element.

   This class is a QML element and enables QML applications to gain access to a
   DDS signal type as a read-only QML property.

*/
class DdsNlpConfigSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString technique READ technique NOTIFY techniqueChanged)
  Q_PROPERTY(QString solver READ solver NOTIFY solverChanged)
  Q_PROPERTY(int degree READ degree NOTIFY degreeChanged)
  Q_PROPERTY(double horizon READ horizon NOTIFY horizonChanged)
  Q_PROPERTY(unsigned int elements READ elements NOTIFY elementsChanged)
  Q_PROPERTY(unsigned int nx READ nx NOTIFY nxChanged)
  Q_PROPERTY(unsigned int np READ np NOTIFY npChanged)
  Q_PROPERTY(unsigned int ng READ ng NOTIFY ngChanged)
  Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged) ///< Timestamp of sample.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param[in] parent QObject pointer.
  */
  explicit DdsNlpConfigSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsNlpConfigSubscriber();
  /**
     @brief Property accessor for technique in QML.
     @return Discretization technique.
  */
  QString technique() const;
  /**
     @brief Property accessor for solver in QML.
     @return Name of NLP solver.
  */
  QString solver() const;
  /**
     @brief Property accessor for polynomial degree in QML.
     @return degree.
  */
  int degree() const;
  /**
     @brief Property accessor for horizon [s] in QML.
     @return Horizon.
  */
  double horizon() const;
  /**
     @brief Property accessor for number of discretization elements [-] in QML.
     @return Elements.
  */
  unsigned int elements() const;
  /**
     @brief Property accessor for number of decision variables in QML.
     @return Number of x in NLP.
  */
  unsigned int nx() const;
  /**
     @brief Property accessor for number of parameters in QML.
     @return Number of p in NLP.
  */
  unsigned int np() const;
  /**
     @brief Property accessor for number of constraints in QML.
     @return Number of g in NLP.
  */
  unsigned int ng() const;
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

     This signal is connected to technique QML property.

     @param[out] technique New value.
  */
  void techniqueChanged(QString technique);
  /**
     @brief A new sample has arrived.

     This signal is connected to solver QML property.

     @param[out] solver New value.
  */
  void solverChanged(QString solver);
  /**
     @brief A new sample has arrived.

     This signal is connected to degree QML property.

     @param[out] degree New value.
  */
  void degreeChanged(int degree);
  /**
     @brief A new sample has arrived.

     This signal is connected to horizon QML property.

     @param[out] horizon New value.
  */
  void horizonChanged(double horizon);
  /**
     @brief A new sample has arrived.

     This signal is connected to elements QML property.

     @param[out] elements New value.
  */
  void elementsChanged(unsigned int elements);
  /**
     @brief A new sample has arrived.

     This signal is connected to nx QML property.

     @param[out] nx New value.
  */
  void nxChanged(unsigned int nx);
  /**
     @brief A new sample has arrived.

     This signal is connected to np QML property.

     @param[out] np New value.
  */
  void npChanged(unsigned int np);
  /**
     @brief A new sample has arrived.

     This signal is connected to ng QML property.

     @param[out] ng New value.
  */
  void ngChanged(unsigned int ng);
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
     current value. If there were a new sample it emits valueChanged().
  */
  void updateValues();

private:
  std::unique_ptr<sinspekto::Reader<fkin::NlpConfig>> m_reader; ///< The DDS reader wrapper class.
  QString m_id; ///< Key identifier for topic instance.

};
