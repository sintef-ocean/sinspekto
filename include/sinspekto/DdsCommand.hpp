#pragma once
/**
   @file DdsCommand.hpp
   @brief DDS publish and subscribe adapters for Command signals in QML.
*/

#include <cinttypes>
#include "sinspekto/QtToDds.hpp"
#include <QTimer>

/**
   @brief Subscriber for DDS type Command as a QML element.

   This class is a QML element and enables QML applications to gain access to a DDS signal
   type as a read-only QML properties.

   @rst
   .. note ::

     Unlike most other adapters in sinspekto, the DDS QoS of this are implemented with
     writers with :code:`dds::core::policy::Durability::TransientLocal()` and for readers
     also :code:`dds::core::policy::Reliability::Reliable();`, see `DDS QoS
     <http://download.prismtech.com/docs/Vortex/apis/ospl/isocpp2/html/a02530.html>`_ for
     details.
   @endrst

*/
class DdsCommandSubscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(fkin::CommandType command READ command NOTIFY commandChanged) ///< Subscribed command.
  Q_PROPERTY(QString commandName READ commandName NOTIFY commandNameChanged) ///< Human readable command name.

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit DdsCommandSubscriber(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsCommandSubscriber();

  /**
     @brief Property accessor for command.
     @return command as a CommandType enum.
  */
  fkin::CommandType command() const;
  /**
     @brief Property accessor for command name.
     @return Command as a human readable string.
  */
  QString commandName() const;
  /**
     @brief Initializes DDS reader and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic to subscribe to.
     @param[in] recipient Key identifier for the command.
     @param[in] replyTopic Name of DDS topic to publish command replies.
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& recipient,
      const QString& replyTopic=QString());

signals:
  /**
     @brief DDS command has changed.

     This signal is connected to the QML DdsCommandSubscriber::command property.

     @param[out] command New command.
  */
  void commandChanged(fkin::CommandType command);
  /**
     @brief DDS command name has changed.

     This signal is connected to the QML DdsCommandSubscriber::commandName property.

     @param[out] commandName Human readable command name.
  */
  void commandNameChanged(QString commandName);
  /**
     @brief DdsReaderListener calls this signal when there is data available on the subscribed topic.

     If DdsCommandSubscriber is initialized with a non-empty replyTopic, eventHeard() will be connected
     to the slot updateCommand().
  */
  void eventHeard();

public slots:
  /**
     @brief Takes all new samples from the subscribed topic and uses the last sample.

     This slot calls take() from the DDS reader and stores the last new sample as the
     current command value. If there were a new sample it emits commandChanged(),
     commandNameChanged() and sendReply().
  */
  void updateCommand();
  /**
     @brief Sends a command acknowledgment on DDS

     This slot is called from updateCommand().
  */
  void sendReply();

private:
  std::unique_ptr<sinspekto::Reader<fkin::Command>> m_reader; ///< The DDS reader wrapper class.
  std::unique_ptr<sinspekto::Writer<fkin::CommandResponse>> m_writer; ///< The DDS writer wrapper class.
  QString m_recipient; ///< Recipient identifier string.
};

/**
   @brief Publisher for DDS type Command as a QML element.

   This class is a QML element and enables QML applications to gain access to a DDS signal
   type as a read-only QML properties.

   @rst
   .. note ::

     Unlike most other adapters in sinspekto, the DDS QoS of this are implemented with
     writers with :code:`dds::core::policy::Durability::TransientLocal()` and for readers
     also :code:`dds::core::policy::Reliability::Reliable();`, see `DDS QoS
     <http://download.prismtech.com/docs/Vortex/apis/ospl/isocpp2/html/a02530.html>`_ for
     details.
   @endrst

*/
class DdsCommandPublisher : public QObject
{
  Q_OBJECT
  Q_PROPERTY(fkin::CommandType command READ command WRITE setCommand NOTIFY commandChanged) ///< Command enum
  Q_PROPERTY(QString commandName READ commandName NOTIFY commandNameChanged) ///< Human readable command name.
  Q_PROPERTY(QString responseMessage READ responseMessage NOTIFY responseMessageChanged) ///< Response message on a command.
  Q_PROPERTY(bool responseStatus READ responseStatus NOTIFY responseStatusChanged) ///< Response status of a sent command.
public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  explicit DdsCommandPublisher(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~DdsCommandPublisher();
  /**
     @brief Property accessor for command.
     @return command as a CommandType enum.
  */
  fkin::CommandType command() const;
  /**
     @brief Property accessor for command name.
     @return Command as a human readable string.
  */
  QString commandName() const;
  /**
     @brief Property accessor for response message.
     @return Response message string.
  */
  QString responseMessage() const;
  /**
     @brief Property accessor for response status.
     @return Response status. True is OK.
  */
  bool responseStatus() const;
  /**
     @brief Initializes DDS writer and connects Qt signals and slots.

     @param[in] dds Pointer to QtToDds instance.
     @param[in] topic Name of DDS topic for which to publish.
     @param[in] recipient Key identifier for the command.
     @param[in] responseTopic Name of DDS topic to subscribe to command responses, acknowledgments. (optional)
     @param[in] responseTimeout_ms Time in milliseconds to wait for an acknowledgment. (default: 1000)
  */
  Q_INVOKABLE void init(
      QtToDds* dds,
      const QString& topic,
      const QString& recipient,
      const QString& responseTopic=QString(),
      int responseTimeout_ms=1000);

signals:
  /**
     @brief DDS command has changed.

     This signal is connected to the QML DdsCommandPublisher::command property.

     @param[out] command New command.
  */
  void commandChanged(fkin::CommandType command);
  /**
     @brief DDS command name has changed.

     This signal is connected to the QML DdsCommandPublisher::commandName property.

     @param[out] commandName Human readable command name.
  */
  void commandNameChanged(const QString& commandName);
  /**
     @brief DDS response message name has changed.

     This signal is connected to the QML DdsCommandPublisher::responseMessage property.

     @param[out] responseMessage Human readable response message.
  */
  void responseMessageChanged(const QString& responseMessage);
  /**
     @brief DDS response status has changed.

     This signal is connected to the QML DdsCommandPublisher::responseStatus property.

     @param[out] responseStatus Is the response OK?
  */
  void responseStatusChanged(bool responseStatus);
  /**
     @brief Triggered when a response is received.
  */
  void gotResponse();
  /**
     @brief Triggered when no response is received.
  */
  void noResponse();
  /**
     @brief Triggered when a response is received.

     @param[out] sequenceNr Sequence number of confirmed command.
  */
  void confirmedResponseSeqNr(int sequenceNr);

public slots:
  /**
     @brief Sets command and sends over DDS

     Sends requested command over DDS. Emits commandChanged() and commandNameChanged().
     If a responseTopic is defined in init(), starts the response timer.

     @param[in] command Command to send.
  */
  void setCommand(fkin::CommandType command);
  /**
     @brief Reads the command response from DDS.

     If there is a response, emits confirmedResponseSeqNr()
  */
  void updateResponse();
  /**
     @brief Sets the new response message and emits responseMessageChanged().

     @param[in] responseMessage The response message to set.
  */
  void setResponseMessage(const QString& responseMessage);
  /**
     @brief Sets the new response status and emits responseStatusChanged().

     @param[in] status The response status to set.
  */
  void setResponseStatus(bool status);
  /**
     @brief Sets timout message and sets response status false. Emits noResponse().
  */
  void handleNoResponse();

private:
  std::unique_ptr<sinspekto::Writer<fkin::Command>> m_writer; ///< The DDS writer wrapper class.
  std::unique_ptr<sinspekto::Reader<fkin::CommandResponse>> m_reader; ///< The DDS writer wrapper class.
  QString m_recipient; ///< Recipient identifier string.
  QString m_responseMessage; ///< Response message variable.
  bool m_responseStatus; ///< Response status.
  std::int32_t m_awaitingSeqNr; ///< Sequence number of message.
  QTimer m_timer; ///< Response timer.
};
