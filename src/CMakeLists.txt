#======================
# sinspekto-api library
#======================

set(SINSPEKTO_SRC
  sinspekto/SinspektoPriv.cpp
  sinspekto/SinspektoQml.cpp
  sinspekto/QtToDds.cpp
  sinspekto/AxisEqualizer.cpp
  sinspekto/DdsDouble.cpp
  sinspekto/DdsBit.cpp
  sinspekto/DdsIdVec1d.cpp
  sinspekto/DdsIdVec2d.cpp
  sinspekto/DdsIdVec3d.cpp
  sinspekto/DdsIdVec4d.cpp
  sinspekto/DdsKinematics2D.cpp
  sinspekto/DdsKinematics6D.cpp
  sinspekto/DdsDoubleBuffer.cpp
  sinspekto/DdsTimepointBuffer.cpp
  sinspekto/DdsBuffer.cpp
  sinspekto/DdsIdVec1dBuffer.cpp
  sinspekto/DdsIdVec2dBuffer.cpp
  sinspekto/DdsIdVec3dBuffer.cpp
  sinspekto/DdsIdVec4dBuffer.cpp
  sinspekto/DdsKinematics2DBuffer.cpp
  sinspekto/DdsKinematics6DBuffer.cpp
  sinspekto/DdsCommand.cpp
  sinspekto/DdsStateAutomaton.cpp
  sinspekto/DdsDataSource.cpp
  sinspekto/DdsNlpConfig.cpp
  sinspekto/DdsOptiStats.cpp
  sinspekto/DdsWeatherData.cpp
  sinspekto/ratatosk/RatatoskDoubleVal.cpp
  sinspekto/ratatosk/RatatoskDouble2.cpp
  sinspekto/ratatosk/RatatoskDouble3.cpp
  sinspekto/ratatosk/RatatoskDouble4.cpp
  sinspekto/ratatosk/RatatoskPosInfo.cpp
  sinspekto/ratatosk/RatatoskGyroInfo.cpp
  sinspekto/ratatosk/RatatoskWindInfo.cpp
  sinspekto/ratatosk/RatatoskDepthInfo.cpp
  sinspekto/ratatosk/RatatoskLogInfo.cpp
  sinspekto/ratatosk/RatatoskCurrentAtDepth.cpp
  sinspekto/ratatosk/RatatoskCurrentProfile.cpp
  )

set(SINSPEKTO_PUBLIC
  ${PROJECT_SOURCE_DIR}/include/sinspekto/sinspekto.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/SinspektoQml.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/QtToDds.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsDouble.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/AxisEqualizer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsBit.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec1d.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec2d.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec3d.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec4d.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsKinematics2D.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsKinematics6D.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsDoubleBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsTimepointBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec1dBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec2dBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec3dBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsIdVec4dBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsKinematics2DBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsKinematics6DBuffer.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsCommand.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsStateAutomaton.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsDataSource.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsNlpConfig.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsOptiStats.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/DdsWeatherData.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskDoubleVal.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskDouble2.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskDouble3.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskDouble4.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskPosInfo.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskGyroInfo.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskWindInfo.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskDepthInfo.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskLogInfo.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskCurrentAtDepth.hpp
  ${PROJECT_SOURCE_DIR}/include/sinspekto/ratatosk/RatatoskCurrentProfile.hpp
  )

# Need to add public headers, since autogen qml does not pick up target include directories properly (?)
add_library(sinspekto-api SHARED
  ${SINSPEKTO_SRC}
  ${SINSPEKTO_PUBLIC})

target_link_libraries(sinspekto-api PRIVATE
  ${SINSPEKTO_DDS_TARGETS}
  Boost::boost
  ${SINSPEKTO_QT_TARGETS})

# alias so that library can be used inside the build tree, e.g. with testing
add_library(sinspekto::sinspekto-api ALIAS sinspekto-api)

target_include_directories(sinspekto-api
PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> # must use absolute paths
  $<INSTALL_INTERFACE:include> # should not use absolute paths..
PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Cheat shit (sic):
#[[ A links B:
  - PRIVATE: A uses B in implementation, but not in A's public API
  - INTERFACE: A does not use B in its implementation, but B is used in A's public API.
               Also if A is INTERFACE library in CMake
  - PUBLIC: A uses B, and B is used in A's public API
#]]#

set_target_properties(sinspekto-api
  PROPERTIES
  PUBLIC_HEADER "${SINSPEKTO_PUBLIC}"
  # correct way of doing it? or install(FILES ...)
  EXPORT_NAME sinspekto
  OUTPUT_NAME sinspekto
  WINDOWS_EXPORT_ALL_SYMBOLS ON
  )

if(NOT ANDROID)
  set_target_properties(sinspekto-api
    PROPERTIES
    INSTALL_RPATH "/opt/opensplice-rts/lib;/opt/opensplice-hde/lib;/opt/ospl/lib")
endif()

install(TARGETS sinspekto-api ${SINSPEKTO_DDS_TARGETS}
  EXPORT "${PROJECT_NAME}"
  LIBRARY
  COMPONENT runtime
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE
  COMPONENT dev
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME
  COMPONENT runtime
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER
  COMPONENT dev
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sinspekto)

if(WITH_EXAMPLES)
  if(NOT ANDROID)
    include(CMakeApps.cmake)
  else()
    message(WARNING "WITH_EXAMPLES are not yet supported for Android")
  endif()

endif()
