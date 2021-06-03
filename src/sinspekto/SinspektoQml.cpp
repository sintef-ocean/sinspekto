#include <QQmlEngine>

#include "sinspekto/QtToDds.hpp"
#include "sinspekto/AxisEqualizer.hpp"
#include "sinspekto/DdsDouble.hpp"
#include "sinspekto/DdsBit.hpp"
#include "sinspekto/DdsIdVec1d.hpp"
#include "sinspekto/DdsIdVec2d.hpp"
#include "sinspekto/DdsIdVec3d.hpp"
#include "sinspekto/DdsIdVec4d.hpp"
#include "sinspekto/DdsKinematics2D.hpp"
#include "sinspekto/DdsKinematics6D.hpp"
#include "sinspekto/DdsIdVec1dBuffer.hpp"
#include "sinspekto/DdsIdVec2dBuffer.hpp"
#include "sinspekto/DdsIdVec3dBuffer.hpp"
#include "sinspekto/DdsIdVec4dBuffer.hpp"
#include "sinspekto/DdsKinematics2DBuffer.hpp"
#include "sinspekto/DdsKinematics6DBuffer.hpp"
#include "sinspekto/DdsCommand.hpp"
#include "sinspekto/DdsStateAutomaton.hpp"
#include "sinspekto/DdsDataSource.hpp"
#include "sinspekto/DdsNlpConfig.hpp"
#include "sinspekto/DdsOptiStats.hpp"
#include "sinspekto/DdsWeatherData.hpp"
#include "sinspekto/ratatosk/RatatoskDoubleVal.hpp"
#include "sinspekto/ratatosk/RatatoskDouble2.hpp"
#include "sinspekto/ratatosk/RatatoskDouble3.hpp"
#include "sinspekto/ratatosk/RatatoskDouble4.hpp"
#include "sinspekto/ratatosk/RatatoskPosInfo.hpp"
#include "sinspekto/ratatosk/RatatoskGyroInfo.hpp"
#include "sinspekto/ratatosk/RatatoskWindInfo.hpp"
#include "sinspekto/ratatosk/RatatoskDepthInfo.hpp"
#include "sinspekto/ratatosk/RatatoskLogInfo.hpp"
#include "sinspekto/ratatosk/RatatoskCurrentAtDepth.hpp"
#include "sinspekto/ratatosk/RatatoskCurrentProfile.hpp"


namespace sinspekto
{
  void LoadSinspektoQmlTypes()
  {
    qmlRegisterType<QtToDds>("fkin.Dds", 1, 0, "QtToDds");
    qmlRegisterType<AxisEqualizer>("fkin.Dds", 1, 0, "AxisEqualizer");
    qmlRegisterType<DdsBitSubscriber>("fkin.Dds", 1, 0, "DdsBitSubscriber");
    qmlRegisterType<DdsBitPublisher>("fkin.Dds", 1, 0, "DdsBitPublisher");
    qmlRegisterType<DdsDoubleSubscriber>("fkin.Dds", 1, 0, "DdsDoubleSubscriber");
    qmlRegisterType<DdsDoublePublisher>("fkin.Dds", 1, 0, "DdsDoublePublisher");
    qmlRegisterType<DdsIdVec1dSubscriber>("fkin.Dds", 1, 0, "DdsIdVec1dSubscriber");
    qmlRegisterType<DdsIdVec1dPublisher>("fkin.Dds", 1, 0, "DdsIdVec1dPublisher");
    qmlRegisterType<DdsIdVec2dSubscriber>("fkin.Dds", 1, 0, "DdsIdVec2dSubscriber");
    qmlRegisterType<DdsIdVec2dPublisher>("fkin.Dds", 1, 0, "DdsIdVec2dPublisher");
    qmlRegisterType<DdsIdVec3dSubscriber>("fkin.Dds", 1, 0, "DdsIdVec3dSubscriber");
    qmlRegisterType<DdsIdVec3dPublisher>("fkin.Dds", 1, 0, "DdsIdVec3dPublisher");
    qmlRegisterType<DdsIdVec4dSubscriber>("fkin.Dds", 1, 0, "DdsIdVec4dSubscriber");
    qmlRegisterType<DdsIdVec4dPublisher>("fkin.Dds", 1, 0, "DdsIdVec4dPublisher");
    qmlRegisterType<DdsKinematics2DSubscriber>("fkin.Dds", 1, 0, "DdsKinematics2DSubscriber");
    qmlRegisterType<DdsKinematics2DPublisher>("fkin.Dds", 1, 0, "DdsKinematics2DPublisher");
    qmlRegisterType<DdsKinematics6DSubscriber>("fkin.Dds", 1, 0, "DdsKinematics6DSubscriber");
    qmlRegisterType<DdsKinematics6DPublisher>("fkin.Dds", 1, 0, "DdsKinematics6DPublisher");
    qmlRegisterType<DdsIdVec1dBuffer>("fkin.Dds", 1, 0, "DdsIdVec1dBuffer");
    qmlRegisterType<DdsIdVec2dBuffer>("fkin.Dds", 1, 0, "DdsIdVec2dBuffer");
    qmlRegisterType<DdsIdVec3dBuffer>("fkin.Dds", 1, 0, "DdsIdVec3dBuffer");
    qmlRegisterType<DdsIdVec4dBuffer>("fkin.Dds", 1, 0, "DdsIdVec4dBuffer");
    qmlRegisterType<DdsKinematics2DBuffer>("fkin.Dds", 1, 0, "DdsKinematics2DBuffer");
    qmlRegisterType<DdsKinematics6DBuffer>("fkin.Dds", 1, 0, "DdsKinematics6DBuffer");
    qmlRegisterType<DdsCommandSubscriber>("fkin.Dds", 1, 0, "DdsCommandSubscriber");
    qmlRegisterType<DdsCommandPublisher>("fkin.Dds", 1, 0, "DdsCommandPublisher");
    qmlRegisterType<DdsStateAutomaton>("fkin.Dds", 1, 0, "DdsStateNotification");
    qmlRegisterType<DdsDataSource>("fkin.Dds", 1, 0, "DdsDataSource");
    qmlRegisterType<DdsNlpConfigSubscriber>("fkin.Dds", 1, 0, "DdsNlpConfigSubscriber");
    qmlRegisterType<DdsOptiStatsSubscriber>("fkin.Dds", 1, 0, "DdsOptiStatsSubscriber");

    // ratatosk
    qmlRegisterType<RatatoskDoubleValSubscriber>("ratatosk", 1, 0, "RatatoskDoubleValSubscriber");
    qmlRegisterType<RatatoskDoubleValPublisher>("ratatosk", 1, 0, "RatatoskDoubleValPublisher");
    qmlRegisterType<RatatoskDouble2Subscriber>("ratatosk", 1, 0, "RatatoskDouble2Subscriber");
    qmlRegisterType<RatatoskDouble2Publisher>("ratatosk", 1, 0, "RatatoskDouble2Publisher");
    qmlRegisterType<RatatoskDouble3Subscriber>("ratatosk", 1, 0, "RatatoskDouble3Subscriber");
    qmlRegisterType<RatatoskDouble3Publisher>("ratatosk", 1, 0, "RatatoskDouble3Publisher");
    qmlRegisterType<RatatoskDouble4Subscriber>("ratatosk", 1, 0, "RatatoskDouble4Subscriber");
    qmlRegisterType<RatatoskDouble4Publisher>("ratatosk", 1, 0, "RatatoskDouble4Publisher");
    qmlRegisterType<RatatoskPosInfoSubscriber>("ratatosk", 1, 0, "RatatoskPosInfoSubscriber");
    qmlRegisterType<RatatoskGyroInfoSubscriber>("ratatosk", 1, 0, "RatatoskGyroInfoSubscriber");
    qmlRegisterType<RatatoskWindInfoSubscriber>("ratatosk", 1, 0, "RatatoskWindInfoSubscriber");
    qmlRegisterType<RatatoskDepthInfoSubscriber>("ratatosk", 1, 0, "RatatoskDepthInfoSubscriber");
    qmlRegisterType<RatatoskLogInfoSubscriber>("ratatosk", 1, 0, "RatatoskLogInfoSubscriber");
    qmlRegisterType<RatatoskCurrentAtDepthSubscriber>("ratatosk", 1, 0, "RatatoskCurrentAtDepthSubscriber");
    qmlRegisterType<RatatoskCurrentProfileSubscriber>("ratatosk", 1, 0, "RatatoskCurrentProfileSubscriber");

    // netatmo
    qmlRegisterType<DdsWeatherDataSubscriber>("weather.Dds", 1, 0, "DdsWeatherDataSubscriber");

    // Define enums identical to idl enums that can be used in qml javascript.
    // These must be registered before fkin::staticMetaObject
    // I believe this is a hack and should be somehow fixed
    // Currently, all dds enums to be used in qml, must be manually "redefined" in another namespace
    qmlRegisterUncreatableMetaObject(qml_enums::staticMetaObject,
     "fkin.Dds",
     1, 0,
     "FKIN", // Mapped to same qml namespace as fkin.
     "Error: only enums");

    qRegisterMetaType<qml_enums::ProcessStateKind>();
    qRegisterMetaType<qml_enums::CommandType>();
    qRegisterMetaType<qml_enums::DimId>();

    // For some reason any enum declared in fkin namespace fails to properly register in qml
    // Forward declared DDS enums are unknown in qml
    qmlRegisterUncreatableMetaObject(fkin::staticMetaObject,
     "fkin.Dds",
     1, 0,
     "Fkin", // Cannot use the same namespace name as qml_enums, conflict
     "Error: only enums");

    qRegisterMetaType<fkin::ProcessStateKind>();
    qRegisterMetaType<fkin::CommandType>();
    qRegisterMetaType<fkin::DimId>();



  }
}
