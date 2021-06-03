#pragma once
/**
   @file QtToDds.hpp
   @brief This file provides declarations that are needed to implement DDS to Qt (QML) adapters.

*/

#include <QObject>
#include <QString>
#include <memory>

/**
   @brief Ratatosk data acquisition data types, a selection

   Consists of a selection of data types used for ratatosk data acquisition software.
*/
namespace ratatosk {
  namespace types {
    class DoubleVal;
    class Double2;
    class Double3;
    class Double4;
    class PosInfo;
    class GyroInfo;
    class WindInfo;
    class DepthInfo;
    class LogInfo;
    class CurrentAtDepth;
    class CurrentProfile;

  }
}


/**
   @brief Catch control in purse seine interface defintion types

   fangstkontroll i not (fkin) translates to catch control in purse seine and consists of
   a set of classes and data types, which are used for inter process data sharing.

*/
namespace fkin {
  class Bit;
  class Real;
  class IdVec1d;
  class IdVec2d;
  class IdVec3d;
  class IdVec4d;
  class Kinematics2D;
  class Kinematics6D;
  class BatchIdVec1d;
  class BatchKinematics2D;
  class BatchKinematics6D;
  class ProcessStateAutomaton;
  class Command;
  class CommandResponse;
  class NlpConfig;
  class OptiStats;

  // Do not know how to forward declare dds enums to be used by qt..
  enum class ProcessStateKind;
  enum class CommandType;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  Q_NAMESPACE
  enum class ProcessStateKind;
  Q_ENUM_NS(ProcessStateKind);

  Q_NAMESPACE
  enum class CommandType;
  Q_ENUM_NS(CommandType);

  Q_NAMESPACE
  enum class DimId
    {
     X,
     Y,
     Z,
     W,
     T, ///< Time.
     PosX,
     PosY,
     PosZ,
     Course,
     Speed,
     VelX,
     VelY,
     VelZ,
     EulerX,
     EulerY,
     EulerZ
    };
  Q_ENUM_NS(DimId);

#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
}

namespace weather {
  class ModuleData;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace sinspekto {
  template <typename T> struct Writer;
  template <typename T> struct Reader;
  template <typename T> class DdsReaderListener;
}
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
/**
   @brief Helper namespace for enums that should be accessible from QML.

   This namespace consists of copies of IDL-defined enum classes. They exist because there
   are some limitations to QML (or the implementor) on how to forward declare enums to be
   used by Qt.
   @warning These enums must be identical to the ones defined in the IDL files for consistence.

*/
namespace qml_enums
{

  /**
     @brief Holds the states for a state machine, used in ProcessStateAutomaton IDL type.

     This is a copy of the same enum defined in an IDL file. It exists because it is
     needed as a workaround to expose them to QML.

  */
  Q_NAMESPACE
  enum class ProcessStateKind
  {
   IDLE,         ///< Idle state, waiting for events
   INITIALIZING, ///< The process is initializing
   RUNNING,      ///< Running process
   FAILURE,      ///< Failed state, the process has failed
   DEAD,         ///< Killed state, the process is not running
   UNKNOWN       ///< Undefined state
  };
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  Q_ENUM_NS(ProcessStateKind);
#endif

  /**
     @brief Actions to send to a process used in Command.

     This is a copy of the same enum defined in an IDL file. It exists because it is
     needed as a workaround to expose them to QML.
  */
  Q_NAMESPACE
  enum class CommandType
  {
   START_PROCESS,     ///< Start the process.
   STOP_PROCESS,      ///< Stop the process.
   TERMINATE_PROCESS, ///< Terminate the process.
   BOGUS_COMMAND      ///< No operation.
  };
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  Q_ENUM_NS(CommandType);
#endif

  /**
     @brief Convencience enums used in QML to indicate a dimension in buffers.

     The enumerations are used to indicate a variable in multi-dimensional buffers.

  */
  Q_NAMESPACE
  enum class DimId
    {
     X,
     Y,
     Z,
     W,
     T, ///< Time.
     PosX,
     PosY,
     PosZ,
     Course,
     Speed,
     VelX,
     VelY,
     VelZ,
     EulerX,
     EulerY,
     EulerZ
    };
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  Q_ENUM_NS(DimId);
#endif
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/**
   @brief Core Qt to DDS instance class

   This class is an essential QML element that holds a DDS wrapper class, with instances
   of dds::domain::DomainParticipant, dds::sub::Subscriber, and dds::pub::Publisher. These
   are classes from the DDS API, which enables DDS communication.

   Every QML application that intents to communicate with DDS using sinspekto must add
   this component and initalize it before adding any sinspekto DDS adapters, <a
   href="../rst/intro.html#minimal-qml-example">see the minimal example</a>.

*/
struct QtToDds : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged) ///< Indicator whether QtToDds has been initialized.

  /**
     @brief Access function used by QML property.
     @return boolean whether DDS is initialized.
  */
  bool initialized() const;

public:
  /**
     @brief Constructor

     The initialization is deferred to an init() function.

     @param [in] parent QObject pointer.
  */
  QtToDds(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~QtToDds();


  /**
     @brief Initializes DDS instance for the given domain.

     @param[in] domain The DDS domain for which to enlist.
  */
  Q_INVOKABLE void init(int domain);

signals:
  /**
     @brief DDS initialized state has has changed.

     This signal is connected to the QML QtToDds::initialized property.

     @param[out] ready Is DDS initialized?
  */
  void initializedChanged(bool ready);

public:
  struct Dds; ///< Dds functionality not defined here. QtToDds::Dds.
  std::unique_ptr<Dds> dds; ///< Opaque pointer to QtToDds::Dds.
private:
  bool m_ready; ///< Holds the property on whether DDS is ready.

};
