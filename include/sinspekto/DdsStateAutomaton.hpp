#pragma once
/**
   @file DdsStateAutomaton.hpp
   @brief DDS subscribe adapter for ProcessStateAutomaton signals in QML.
*/

#include "sinspekto/QtToDds.hpp"

/**
   @brief Subscriber for DDS type ProcessStateAutomaton as a QML element.

   This class is a QML element and enables QML applications to gain access to a DDS signal
   type as a read-only QML properties.

   This element is useful to gain access to the process state of external applications
   that use this DDS type to report its state in a QML application.

   @rst
   .. note ::

     Unlike most other adapters in sinspekto, the DDS QoS of this are implemented with
     a reader with :code:`dds::core::policy::Durability::TransientLocal()` and
     :code:`dds::core::policy::Reliability::Reliable();`, see `DDS QoS
     <http://download.prismtech.com/docs/Vortex/apis/ospl/isocpp2/html/a02530.html>`_ for
     details.
   @endrst

*/
class DdsStateAutomaton : public QObject
{
  Q_OBJECT
  Q_PROPERTY(fkin::ProcessStateKind state READ state NOTIFY stateChanged) ///< Subscribed process state kind.
  Q_PROPERTY(QString stateName READ stateName NOTIFY stateNameChanged) ///< Human readable name of state.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit DdsStateAutomaton(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsStateAutomaton();
  /**
     @brief Property accessor for process state kind as an enum.
     @return state as an enum.
  */
  fkin::ProcessStateKind state() const;
  /**
     @brief Property accessor for process state name.
     @return State as a human readable string.
  */
  QString stateName() const;
  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to subscribe.
     @param[in] stateIdentifier Key identifier for the topic instance.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& stateIdentifier);

signals:
  /**
     @brief DDS process state value has changed.

     This signal is connected to the QML DdsStateAutomaton::state property.

     @param[out] state New state value.
  */
  void stateChanged(fkin::ProcessStateKind state);
  /**
     @brief DDS process state name has changed.

     This signal is connected to the QML DdsStateAutomaton::stateName property.

     @param[out] stateName New state name.
  */
  void stateNameChanged(QString stateName);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     signal eventHeard() is connected to the slot updateState().

  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current state. If there were a new sample it emits stateChanged() and
     stateNameChanged().

  */
  void updateState();

private:
  std::unique_ptr<sinspekto::Reader<fkin::ProcessStateAutomaton>> m_reader; ///< The DDS reader wrapper class.
  QString m_identifier; ///< Key identifier for topic instance.
};
