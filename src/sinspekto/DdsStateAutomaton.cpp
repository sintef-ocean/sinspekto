#include "sinspekto/FkinDds.hpp"
#include "sinspekto/DdsStateAutomaton.hpp"
#include "sinspekto/QtToDdsPriv.hpp"

// TODO: Move to namespace
typedef std::map<fkin::ProcessStateKind, QString> ProcessStateName;

ProcessStateName processStateName_t =
  {
   { fkin::ProcessStateKind::IDLE,         QObject::tr("Idle") },
   { fkin::ProcessStateKind::INITIALIZING, QObject::tr("Initializing") },
   { fkin::ProcessStateKind::RUNNING,      QObject::tr("Running") },
   { fkin::ProcessStateKind::FAILURE,      QObject::tr("Failure") },
   { fkin::ProcessStateKind::DEAD,         QObject::tr("Dead") },
   { fkin::ProcessStateKind::UNKNOWN,      QObject::tr("Unknown") }
  };

DdsStateAutomaton::DdsStateAutomaton(QObject *parent) :
  QObject(parent),
  m_reader(nullptr)
{}

DdsStateAutomaton::~DdsStateAutomaton() = default;

void DdsStateAutomaton::init(
    QtToDds* dds,
    const QString& notifyTopicName,
    const QString& stateIdentifier)
{
  m_identifier = stateIdentifier;

  m_reader = std::make_unique<sinspekto::Reader<fkin::ProcessStateAutomaton>>(dds, notifyTopicName, true);
  m_reader->listener = sinspekto::DdsReaderListener<fkin::ProcessStateAutomaton>(
      std::bind(&DdsStateAutomaton::eventHeard, this));
  m_reader->reader.listener(&m_reader->listener, dds::core::status::StatusMask::data_available());
  QObject::connect(this, &DdsStateAutomaton::eventHeard, this, &DdsStateAutomaton::updateState);
}

fkin::ProcessStateKind DdsStateAutomaton::state() const
{
  if(!m_reader) return fkin::ProcessStateKind::UNKNOWN;
  return m_reader->sample.state();
}

QString DdsStateAutomaton::stateName() const
{
  if(!m_reader) return processStateName_t[fkin::ProcessStateKind::UNKNOWN];
  return processStateName_t[m_reader->sample.state()];
}

void DdsStateAutomaton::updateState()
{
  if(!m_reader) return;

  const auto query = dds::sub::Query(
      m_reader->reader,
      "identifier = %0",
      {m_identifier.toStdString()});

  dds::sub::LoanedSamples<fkin::ProcessStateAutomaton> samples =
   m_reader->reader.select()
   .state(dds::sub::status::DataState::new_data())
   .content(query).take();

  if(samples.length() > 0)
  {
    auto old_state = m_reader->sample.state();

    m_reader->sample = (--samples.end())->data();
    if( old_state != m_reader->sample.state())
    {
      emit stateChanged(m_reader->sample.state());
      emit stateNameChanged(processStateName_t[m_reader->sample.state()]);
    }
  }
}
