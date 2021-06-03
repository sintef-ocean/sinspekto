#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <map>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/QtToDds.hpp"
#include "sinspekto/sinspekto.hpp"

/**
   @brief Dds container class

   Simple container class that holds instances of DDS-related classes. For details on DDS,
   please consult e.g. <a href="http://download.prismtech.com/docs/Vortex/apis/ospl/isocpp2/html/index.html">OpenSplice ISO C++ 2 DCPS documentation.</a>

   This class is instanced by QtToDds.

*/
struct QtToDds::Dds {

  /**
     @brief Constructor.

     @param id domain id for which to enlist. Often 0.
  */
  Dds(uint32_t id) :
    domainParticipant(id),
    subscriber(domainParticipant),
    publisher(domainParticipant)
  { }

  /**
     @brief Destructor.
  */
  ~Dds() = default;
  dds::domain::DomainParticipant domainParticipant; ///< DDS domain participant.
  dds::sub::Subscriber subscriber; ///< DDS subscriber.
  dds::pub::Publisher publisher; ///< DDS publisher.
};


/**
   @brief Helper classes and functions to enable DDS and QML interconnectivity.

*/
namespace sinspekto {

  /**
     @brief Listener class to dispatch callbacks to a Qt signal function

     When data becomes available the callback function emits a Qt signal so that new data
     event is registered in Qt's event loop.

  */
  template <typename T>
  class DdsReaderListener :
    public dds::sub::NoOpDataReaderListener<T>
  {
  public:
    /**
       @brief Constructor.

       Takes a function point to a Qt signal function.
    */
    DdsReaderListener(std::function<void()> QtSignalFcn = nullptr)
    {
      m_qtEventSignal = std::move(QtSignalFcn);
    }

    /**
       @brief Called when data becomes available.

       This is an overloaded function that dispatches an new data available callback to
       the Qt event system.
    */
    virtual void on_data_available(dds::sub::DataReader<T>&)
    {
      if(m_qtEventSignal)
         emit m_qtEventSignal();
    }

  private:
    std::function<void()> m_qtEventSignal; ///< Function pointer to Qt signal to emit.
};

  /// Wrapper class that sets up a DDS data writer
  template <typename T>
  struct Writer
  {
    /// Constructor that sets up a writer on the provided domain.
    Writer<T>(QtToDds * const dds, const QString &topic, bool transient_local=false) :
      writer(dds::pub::DataWriter<T>(dds::core::null))
    {
      if(dds->dds == nullptr)
      {
        std::cerr
          << "Topic: " << topic.toStdString()
            << ". QtToDds participant was not initialized first"
            << std::endl;
        throw std::runtime_error(std::string(
            "QtToDds::init must be called before writers: ")
               + std::string(__FUNCTION__));
      }

      auto signalTopic = dds::topic::Topic<T>(
          dds->dds->domainParticipant,
          topic.toStdString());

      auto signalWriterQos = dds->dds->publisher.default_datawriter_qos();
      if(transient_local){
        signalWriterQos << dds::core::policy::Durability::TransientLocal();
      }

      writer = dds::pub::DataWriter<T>(
          dds->dds->publisher,
          signalTopic,
          signalWriterQos);
    }
    dds::pub::DataWriter<T> writer; ///< The DDS data writer.
    T sample; ///< A sample of the DDS type the Data writer manages.
  };

  /// Wrapper class that sets up DDS data reader
  template <typename T>
  struct Reader
  {
    /// Constructor that sets up a reader on the provided domain.
    Reader<T>(QtToDds * const dds, const QString &topic, bool transient_local=false) :
      reader(dds::sub::DataReader<T>(dds::core::null))
    {
      if(dds->dds == nullptr)
      {
        std::cerr
         << "Topic: " << topic.toStdString()
         << ". QtToDds participant was not initialized first"
         << std::endl;
        throw std::runtime_error(std::string(
             "QtToDds::init must be called before readers: ")
         + std::string(__FUNCTION__));
      }

      auto signalTopic = dds::topic::Topic<T>(
          dds->dds->domainParticipant,
          topic.toStdString());

      auto signalReaderQos = dds->dds->subscriber.default_datareader_qos();
      if(transient_local){
        signalReaderQos
         << dds::core::policy::Durability::TransientLocal()
         << dds::core::policy::Reliability::Reliable();
      }
      reader = dds::sub::DataReader<T>(
          dds->dds->subscriber,
          signalTopic,
          signalReaderQos);
    }
    dds::sub::DataReader<T> reader; ///< DDS data reader.
    T sample; ///<
    dds::core::Time timepoint; ///< time point with type used in DDS
    sinspekto::DdsReaderListener<T> listener; ///< Wrapper class to dispatch Qt events on data available.

    /// Destructor
    ~Reader<T>()
    {
      reader.listener(nullptr, dds::core::status::StatusMask::none());
    }

  };
}
