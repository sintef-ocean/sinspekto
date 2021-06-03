#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "FKIN/fkin_types_DCPS.hpp"
#include <dds/domain/DomainParticipant.hpp>
#include <dds/sub/Subscriber.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>

template <typename T>
class MyReaderListener :
  public dds::sub::NoOpDataReaderListener<T>
{
public:
  MyReaderListener(
      std::function<void(dds::sub::DataReader<T>&)> DoWork)
  {
    m_workFcn = std::move(DoWork);
  }
  virtual void on_data_available(dds::sub::DataReader<T>& dataReader)
  {
    if(m_workFcn)
      m_workFcn(dataReader);
  }

private:
  std::function<void(dds::sub::DataReader<T>&)> m_workFcn;
};

void HandleSample(const dds::sub::Sample<fkin::Command>& sample)
{
  if (sample.info().valid())
  {
    switch(sample.data().command())
    {
    case fkin::CommandType::START_PROCESS:
      std::cout << "Start it " << std::endl;
      break;
    case fkin::CommandType::STOP_PROCESS:
      std::cout << "Stop it " << std::endl;
      break;
    default:
      break;
    }
    std::cout << sample.data().header().recipient() << ": ";
  }
  else
  {
    std::cout << "Sample invalid" << std::endl;
  }
}

template <typename T>
void MyWorkFcn(dds::sub::DataReader<T>& reader, const dds::sub::Query& query){

  const auto samples = reader.select()
   .state(dds::sub::status::DataState::new_data())
   .content(query)
   .take();

  std::for_each(
      samples.begin(),
      samples.end(),
      HandleSample);
}


int main(int argc, char *argv[])
{
  auto domainParticipant = dds::domain::DomainParticipant(0);
  auto subscriber = dds::sub::Subscriber(domainParticipant);

  std::string topic("fkinReal");
  std::string recipient("you");
  bool use_listener = false;

  // option to allow selection between waitset and listener.
  if (argc > 1)
  {
    std::stringstream ss(argv[1]);
    ss >> use_listener;
  }
  std::cout << "Use listener: " << use_listener << std::endl;

  if (argc > 2)
    topic = argv[2];

  if (argc > 3)
    recipient = argv[3];

  auto sampleTopic = dds::topic::Topic<fkin::Command>(
        domainParticipant,
        topic);
  auto sampleReaderQos = subscriber.default_datareader_qos();
  sampleReaderQos
   << dds::core::policy::Durability::TransientLocal()
   << dds::core::policy::Reliability::Reliable();
  auto sampleReader = dds::sub::DataReader<fkin::Command>(
      subscriber,
      sampleTopic,
      sampleReaderQos);

  // clear reader queue of any old samples
  sampleReader.wait_for_historical_data(dds::core::Duration::infinite());
  sampleReader.take();

  std::cout << "Query for recipient: " << recipient << std::endl;
  const auto query = dds::sub::Query(
      sampleReader,
      "header.recipient = %0",
      {recipient});

  using namespace std::placeholders;

  auto fcn = std::bind(MyWorkFcn<fkin::Command>, _1, query);

  MyReaderListener<fkin::Command> listener(fcn);
  auto waitSet = dds::core::cond::WaitSet();

  if (use_listener)
  {
    listener = MyReaderListener<fkin::Command>(fcn);
    sampleReader.listener(&listener, dds::core::status::StatusMask::data_available());
  }
  else
  {

    auto readCondition = dds::sub::cond::ReadCondition(
        sampleReader,
        dds::sub::status::DataState::new_data());
    auto condition = dds::sub::cond::QueryCondition(
        query,
        dds::sub::status::DataState::new_data());

    waitSet.attach_condition(readCondition);
    //  waitSet.attach_condition(condition);
  }

  while(true){

    if(use_listener)
    {
      std::this_thread::sleep_for(std::chrono::seconds(10));
      std::cout << "Just being alive" << std::endl;
    }
    else
    {
      waitSet.wait();
      const auto samples = sampleReader.select().content(query).take();
      std::for_each(
          samples.begin(),
          samples.end(),
          HandleSample);/*
          [](const dds::sub::Sample<fkin::Command>& sample)
          {
            if (sample.info().valid())
            {
              switch(sample.data().command())
              {
              case fkin::CommandType::START_PROCESS:
                std::cout << "Start it " << std::endl;
                break;
              case fkin::CommandType::STOP_PROCESS:
                std::cout << "Stop it " << std::endl;
                break;
              }
              std::cout << sample.data().header().recipient() << ": ";
            }
            else
            {
              std::cout << "Sample invalid" << std::endl;
            }
          });*/
    }



  }

  return 0;

}
