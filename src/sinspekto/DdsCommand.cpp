#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsCommand.hpp"
#include "sinspekto/QtToDdsPriv.hpp"

#include <iostream>
typedef std::map<fkin::CommandType, QString> CommandTypeName;

CommandTypeName commandName_t =
  {
   { fkin::CommandType::START_PROCESS, QObject::tr("Start") },
   { fkin::CommandType::STOP_PROCESS, QObject::tr("Stop") },
   { fkin::CommandType::TERMINATE_PROCESS, QObject::tr("Terminate") },
   { fkin::CommandType::BOGUS_COMMAND, QObject::tr("Noop") }
  };


DdsCommandSubscriber::DdsCommandSubscriber(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{ }

DdsCommandSubscriber::~DdsCommandSubscriber() = default;

void DdsCommandSubscriber::init(
    QtToDds* dds,
    const QString& topic,
    const QString& recipient,
    const QString& replyTopic)
{
  m_recipient = recipient;
  m_reader = std::make_unique<sinspekto::Reader<fkin::Command>>(dds, topic, true);

  m_reader->listener = sinspekto::DdsReaderListener<fkin::Command>(
        std::bind(&DdsCommandSubscriber::eventHeard, this));
  m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());
  QObject::connect(this, &DdsCommandSubscriber::eventHeard, this, &DdsCommandSubscriber::updateCommand);

  if ( replyTopic.length() > 0)
  {
    m_writer = std::make_unique<sinspekto::Writer<fkin::CommandResponse>>(dds, replyTopic);
  }

}

fkin::CommandType DdsCommandSubscriber::command() const
{
  if(!m_reader) return fkin::CommandType::BOGUS_COMMAND;
  return m_reader->sample.command();
}

QString DdsCommandSubscriber::commandName() const
{
  if(!m_reader) return commandName_t[fkin::CommandType::BOGUS_COMMAND];
  return commandName_t[m_reader->sample.command()];
}

void DdsCommandSubscriber::updateCommand()
{
  if(!m_reader) return;
  const auto query = dds::sub::Query(
      m_reader->reader,
      "header.recipient = %0", {m_recipient.toStdString()});
  dds::sub::LoanedSamples<fkin::Command> samples =
    m_reader->reader.select()
    .content(query)
    .state(dds::sub::status::DataState::new_data()).take();
  uint32_t num = samples.length();

  if(num > 0)
  {
    m_reader->sample = (--samples.end())->data(); // picks the last sample
    emit commandChanged(m_reader->sample.command());
    emit commandNameChanged(commandName_t[m_reader->sample.command()]);
    emit sendReply();
  }
}

void DdsCommandSubscriber::sendReply()
{
  if(!m_writer || !m_reader) return;

  m_writer->sample = fkin::CommandResponse(
      fkin::ReplyHeader(m_reader->sample.header().requestID()),
      true,
      QObject::tr("Command acknowledged").toStdString());

  m_writer->writer << m_writer->sample;
}


DdsCommandPublisher::DdsCommandPublisher(QObject *parent) :
  QObject(parent),
  m_writer(nullptr),
  m_reader(nullptr),
  m_responseStatus(false),
  m_awaitingSeqNr(-0),
  m_timer(this)
{ }

DdsCommandPublisher::~DdsCommandPublisher() = default;

void DdsCommandPublisher::init(
    QtToDds* dds,
    const QString& topic,
    const QString& recipient,
    const QString& responseTopic,
    int responseTimeout_ms)
{
  m_recipient = recipient;

  m_writer = std::make_unique<sinspekto::Writer<fkin::Command>>(dds, topic, true);

  const auto requestID = fkin::MessageID(sinspekto::generate_hex(6), 0);

  m_writer->sample = fkin::Command(
      fkin::RequestHeader(requestID, recipient.toStdString()),
      fkin::CommandType::BOGUS_COMMAND);
  m_writer->sample.command() = fkin::CommandType::BOGUS_COMMAND;

  if (responseTopic.length() > 0)
  {
    m_reader = std::make_unique<sinspekto::Reader<fkin::CommandResponse>>(dds, responseTopic);

    m_reader->listener = sinspekto::DdsReaderListener<fkin::CommandResponse>(
        std::bind(&DdsCommandPublisher::gotResponse, this));
    m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());
    QObject::connect(this, &DdsCommandPublisher::gotResponse, this, &DdsCommandPublisher::updateResponse);

    // clear reader queue of any old samples
    m_reader->reader.wait_for_historical_data(dds::core::Duration::infinite());
    m_reader->reader.take();

    m_timer.setSingleShot(true);
    m_timer.setInterval(responseTimeout_ms);

    QObject::connect(&m_timer, &QTimer::timeout, this, &DdsCommandPublisher::handleNoResponse);
    QObject::connect(this, &DdsCommandPublisher::confirmedResponseSeqNr, &m_timer, &QTimer::stop);

  }

  m_writer->sample.command() = fkin::CommandType::BOGUS_COMMAND;

}

fkin::CommandType DdsCommandPublisher::command() const
{
  if(!m_writer) return fkin::CommandType::BOGUS_COMMAND;
  return m_writer->sample.command();
}

QString DdsCommandPublisher::commandName() const
{
  if(!m_writer) return commandName_t[fkin::CommandType::BOGUS_COMMAND];
  return commandName_t[m_writer->sample.command()];
}

QString DdsCommandPublisher::responseMessage() const
{
  return m_responseMessage;
}

bool DdsCommandPublisher::responseStatus() const
{
  return m_responseStatus;
}

void DdsCommandPublisher::setCommand(fkin::CommandType command)
{
  if(!m_writer) return;
  else
  {
    // should not be possible to change command until success response (if enabled)?
    auto seqNr = m_writer->sample.header().requestID().sequenceNumber();
    m_writer->sample.header().requestID().sequenceNumber() = seqNr + 1;
    m_writer->sample.command() = command;
    m_writer->writer << m_writer->sample;
    m_awaitingSeqNr = seqNr + 1;
    emit commandChanged(command);
    emit commandNameChanged(commandName_t[command]);
    if(m_reader) setResponseMessage(QString());

    if (command != fkin::CommandType::BOGUS_COMMAND && m_reader)
      m_timer.start();
  }
}

void DdsCommandPublisher::updateResponse()
{
  if(!m_reader) return;

  const auto query = dds::sub::Query(
      m_reader->reader,
      "header.relatedRequestID.senderUUID = %0 "
      "AND header.relatedRequestID.sequenceNumber = %1",
      {m_writer->sample.header().requestID().senderUUID(),
       std::to_string(m_awaitingSeqNr)});
  dds::sub::LoanedSamples<fkin::CommandResponse> samples =
   m_reader->reader.select()
   .state(dds::sub::status::DataState::new_data())
   .content(query).take();

  // Query should contain all awaiting sequence numbers, really.
  // setCommand new commands disabled awaiting response or timeout?

  if(samples.length() > 0)
  {
    // cannot skip samples here need consecutive sequence numbers.
    m_reader->sample = (--samples.end())->data(); // picks the last sample
    setResponseMessage(QString::fromStdString(m_reader->sample.message()));
    setResponseStatus(m_reader->sample.success());
    emit confirmedResponseSeqNr(m_reader->sample.header().relatedRequestID().sequenceNumber());
  }

}

void DdsCommandPublisher::handleNoResponse()
{
  setResponseMessage(QString("Timeout waiting for response"));
  setResponseStatus(false);
  emit noResponse();
}

void DdsCommandPublisher::setResponseMessage(const QString& responseMessage)
{
  if(!m_reader) return;
  else if(QString::compare(
       m_responseMessage,
       responseMessage) != 0)
  {
    m_responseMessage = responseMessage;
    emit responseMessageChanged(responseMessage);
  }
}

void DdsCommandPublisher::setResponseStatus(bool responseStatus)
{
  if(!m_reader) return;
  else if(m_responseStatus != responseStatus)
  {
    m_responseStatus = responseStatus;
    emit responseStatusChanged(responseStatus);
  }
}
