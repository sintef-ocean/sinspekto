cmake_minimum_required(VERSION 3.12)

set(THE_PROJECT_AUTHOR "Joakim Haugen")
set(THE_PROJECT_MAINTAINER "SINTEF Ocean <joakim.haugen@sintef.no>")
set(THE_PROJECT_VENDOR "SINTEF Ocean")

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(kluster-scripts CONFIG QUIET)

file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" projectVersion)

if(kluster-scripts_FOUND)
  # Use git tag and synchronize version.txt
  kluster_get_version_from_git(projectTagVersion ${projectVersion})
  kluster_use_version(${projectTagVersion} "${CMAKE_SOURCE_DIR}/version.txt" projectVersion)
endif()

project(sinspekto
  VERSION ${projectVersion}
  LANGUAGES CXX
  DESCRIPTION "SINTEF Ocean Information inspector")

# Qt-specific convenience:
# Qt meta-object code generation. moc-files will be generated into
# AUTOGEN_BUILD_DIR/include, which is auto added to target's include directories (Uses
# AUTOMOC_MACRO_NAMES list of string to search for files: Q_OBJECT;Q_GADGET;Q_NAMESPACE by
# default). CMAKE_AUTOMOC_OPTIONS AUTOUIC: require uic to be run. Will search for and
# generate appropriate files ui_*.h in AUTOGEN_BUILD_DIR/include, which is auto added to
# target's include directories. CMAKE_AUTOUIC_OPTIONS
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# Qt preset AUTORCC for all targets. Executes rcc on *.qrc files. add_executable(x x.cpp
# rsrc.qrc). CMAKE_AUTORCC_OPTIONS set(CMAKE_INCLUDE_CURRENT_DIR ON) # Let generated files
# that are put the build dir to be included when compiling. CMake>3.7: not necessary for
# Qt-generated any more.

# https://cliutils.gitlab.io/modern-cmake/chapters/features/cpp11.html
# should be set as target properties instead
set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler flag extension (use -std=c++11 instead of -std=gnu++11)
set(CMAKE_CXX_STANDARD 17) # The default CXX_STANDARD (resolves to a flag -std=c++17)..
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Generate debug library name with a postfix.")
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC by default

# Undocumented cmake options to disable source changes and in-source build.
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

option(WITH_EXAMPLES "Compile example applications" OFF)
option(WITH_CONAN "Use conan for dependency management" OFF)
option(WITH_DOC "Add doc targets" OFF)
option(WITH_API_DOC "Add api doc targets" ON)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  if(NOT WITH_CONAN)
    set(WITH_CONAN ON)
    message(STATUS "Found conanbuildinfo.cmake, setting WITH_CONAN=ON")
  endif()
elseif(WITH_CONAN)
  message(FATAL_ERROR "WITH_CONAN=ON, but conanbuildinfo.cmake not found. Forgot 'conan install'?")
endif()

if(WITH_CONAN)
  list(REMOVE_AT CMAKE_MODULE_PATH 0) # Removes bundled cmake dir with FindOpenSplice.cmake
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
  include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
  # Seems to be necessary?
  #add_definitions("-DQT_NO_VERSION_TAGGING")
endif()

if(DEFINED ENV{ANDROID_ABI})
  message(STATUS "ANDROID_ABI defined, setting ANDROID ON")
  set(ANDROID ON CACHE INTERNAL "Enabled ANDROID")
endif()

if(WITH_EXAMPLES)
  set(SINSPEKTO_EXTRA_QT LinguistTools)
endif()

if(NOT ANDROID)
else()
  message(STATUS "Building for Android")
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ON)
  if(NOT ANDROID_PLATFORM)
    set(ANDROID_PLATFORM $ENV{ANDROID_PLATFORM})
  endif()
  if(NOT ANDROID_ABI)
    set(ANDROID_ABI $ENV{ANDROID_ABI})
  endif()
endif()

if(WITH_CONAN)
  message(STATUS "Using Conan for dependencies")
  find_package(qt MODULE REQUIRED)
  #find_package(Qt5Core 5.15 CONFIG REQUIRED)
  #set(SINSPEKTO_QT_TARGETS qt::qt)
  # There seems to be issues with the conan center Qt package
  set(SINSPEKTO_QT 5.15)
else()
  set(SINSPEKTO_QT 5.11)
endif()

find_package(Qt5 ${SINSPEKTO_QT} CONFIG COMPONENTS Widgets Qml Quick Charts ${SINSPEKTO_EXTRA_QT} REQUIRED)
set(SINSPEKTO_QT_TARGETS "Qt5::Qml;Qt5::Widgets;Qt5::Charts")
set(SINSPEKTO_QT_TARGETS_APPS "Qt5::Qml;Qt5::Widgets;Qt5::Quick")

find_package(OpenSplice 6.9 REQUIRED)
find_package(Boost 1.66 REQUIRED)
find_package(RatatoskIDL CONFIG REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-Werror")
  add_compile_options("-Wno-parentheses")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/W4" "/WX")
  add_compile_options("/wd4251")
  add_compile_options("/wd4127")
  add_compile_options("/wd5240")
  add_definitions("-D_SCL_SECURE_NO_WARNINGS" "-D_CRT_SECURE_NO_WARNINGS")
  add_definitions("-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING")
endif()


# --------------------------------------------------------------------
#  DDS code generation

ratatosk_add_idl_library(sinspekto-fkin FKIN)
ratatosk_add_idl_library(sinspekto-weather Netatmo)
ratatosk_add_idl_library(sinspekto-ratatosk Ratatosk)

set(SINSPEKTO_DDS_TARGETS
  "sinspekto-fkin;sinspekto-weather;sinspekto-ratatosk")

# --------------------------------------------------------------------
#  Libraries, export targets and install files
# --------------------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

add_subdirectory(src)

if(WITH_DOC)
  add_subdirectory(docs)
endif()

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion)

configure_file(cmake/projectConfig.cmake.in
  cmake/${PROJECT_NAME}Config.cmake.in @ONLY)

configure_package_config_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
  PATH_VARS INSTALL_CONFIGDIR
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
  COMPONENT dev
  DESTINATION ${INSTALL_CONFIGDIR})

install(EXPORT ${PROJECT_NAME}
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE "${PROJECT_NAME}::"
  COMPONENT dev
  DESTINATION ${INSTALL_CONFIGDIR})

install(FILES LICENSE
  COMPONENT runtime
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})


# --------------------------------------------------------------------
# Packaging
# --------------------------------------------------------------------

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} COMPILER)
set(COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})
string(REPLACE "." ";" comp_list "${COMPILER_VERSION}")
list(GET comp_list 0 COMP_MAJOR)
set(COMPILER ${COMPILER}-${COMP_MAJOR})

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  set(ARCHITECTURE amd64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
  set(ARCHITECTURE armhf)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
  set(ARCHITECTURE arm64)
else()
  set(ARCHITECTURE i386)
endif()

set(THE_PACKAGE_LINUX_INSTALL_PREFIX /usr/local)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "${THE_PROJECT_VENDOR}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${COMPILER}_${ARCHITECTURE})
set(THE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}) # needed, since include CPack changes things

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL runtime dev doc)
set(CPACK_COMPONENT_DEV_DEPENDS runtime doc)

set(CPACK_COMPONENT_DEV_DESCRIPTION "${PROJECT_DESCRIPTION} - development headers")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "${PROJECT_DESCRIPTION} - runtime")
set(CPACK_COMPONENT_DOC_DESCRIPTION "${PROJECT_DESCRIPTION} - documentation")

if(WIN32 AND NOT UNIX)
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY /${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR})
  set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${VERSION_NUMBER})

else(WIN32 AND NOT UNIX)

  set(CPACK_GENERATOR "DEB;TGZ")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCHITECTURE})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${THE_PROJECT_MAINTAINER}")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY optional)
  set(CPACK_PACKAGING_INSTALL_PREFIX "${THE_PACKAGE_LINUX_INSTALL_PREFIX}")
  set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)

  set(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS "\
  libqt5charts5, \
  libqt5core5a, \
  libqt5gui5, \
  libqt5qml5, \
  libqt5widgets5, \
  libqt5waylandclient5, \
  qml-module-qtcharts, \
  qml-module-qtquick2, \
  qml-module-qtquick-controls2, \
  qml-module-qtquick-layouts, \
  qml-module-qtquick-window2, \
  qtwayland5, \
  opensplice-rts (>= 6.9.190925)")

  # add trigger for ldconfig
  set(CPACK_DEBIAN_RUNTIME_PACKAGE_CONTROL_EXTRA
    "${CMAKE_CURRENT_SOURCE_DIR}/tools/cpack/trigger")


  # kbd and libgles2-mesa
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "\
  libqt5charts5-dev, \
  libqt5waylandclient5-dev, \
  pkg-config, \
  qt5-default, \
  qtdeclarative5-dev, \
  qttools5-dev, \
  qttools5-dev-tools, \
  qml-module-qtquick2, \
  qml-module-qtcharts, \
  qml-module-qtquick-controls2, \
  qml-module-qtquick-layouts, \
  qml-module-qtquick-window2, \
  opensplice-hde (>= 6.9.190925)")

  if(WITH_CONAN)
    # Because these are not the dependencies the library is built with (version)
    unset(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS)
    unset(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS)
  endif()

endif(WIN32 AND NOT UNIX)

include(CPack)

#---------------------------------------------------------------------
# If kluster-scripts found, add upload, deliver, and deploy targets
#---------------------------------------------------------------------

if(kluster-scripts_FOUND)

  if(WITH_DOC)
    set(SINSPEKTO_DOC doc)
  endif()

  kluster_add_artifactory_target(${SINSPEKTO_DOC})
  kluster_add_aptly_targets(${SINSPEKTO_DOC})

else()

  add_custom_target(package_it
    COMMAND cmake --build . --target package --config Release
    COMMENT "Package project ${CMAKE_PROJECT_NAME}"
    VERBATIM)

  if(WITH_DOC)
    add_dependencies(package_it doc)
  endif()

endif()
