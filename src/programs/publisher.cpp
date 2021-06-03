#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "FKIN/fkin_types_DCPS.hpp"
#include <dds/domain/DomainParticipant.hpp>
#include <dds/pub/Publisher.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <thread>

#include "sinspekto/sinspekto.hpp"

int main(int argc, char *argv[])
{
  auto domainParticipant = dds::domain::DomainParticipant(0);
  auto publisher = dds::pub::Publisher(domainParticipant);

  std::string topic("fkinReal");
  std::string recipient("you");

  if (argc > 1)
    topic = argv[1];

  if (argc > 2)
    recipient = argv[2];

  auto sampleTopic = dds::topic::Topic<fkin::Command>(
      domainParticipant,
      topic);
  auto sampleWriterQos = publisher.default_datawriter_qos();
  sampleWriterQos << dds::core::policy::Durability::TransientLocal();
  auto sampleWriter = dds::pub::DataWriter<fkin::Command>(
      publisher,
      sampleTopic,
      sampleWriterQos);

  const auto requestID = fkin::MessageID(sinspekto::generate_hex(6), 0);

  std::unique_ptr<fkin::Command> sample(new fkin::Command);
  sample->header() =  fkin::RequestHeader(requestID, recipient);
  sample->command() = fkin::CommandType::BOGUS_COMMAND;

  while(true)
  {

    if(sample->command() == fkin::CommandType::STOP_PROCESS)
      sample->command() = fkin::CommandType::START_PROCESS;
    else
      sample->command() = fkin::CommandType::STOP_PROCESS;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sampleWriter << *sample;
    std::cout << "Wrote sample with recipient " << sample->header().recipient() << std::endl;

  }

  return 0;
}
