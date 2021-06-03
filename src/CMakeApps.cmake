
message(STATUS "Building Example applications")

# =============================
# Set up language-aware program
# =============================

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/sinspekto.qrc
  ${PROJECT_SOURCE_DIR}/data/languages/fkin_en.ts
  ${PROJECT_SOURCE_DIR}/data/languages/fkin_no.ts)

set(TS_FILES
  ${PROJECT_SOURCE_DIR}/data/languages/fkin_en.ts
  ${PROJECT_SOURCE_DIR}/data/languages/fkin_no.ts)
qt5_add_translation(QM_FILES ${TS_FILES})

# You need to manually translate files with $> linguist fkin_*.ts
# Note: Beware that 'make clean' deletes the *.ts files

# Resources are found relative to its .qrc file.
# qm-files are the binary directory, this copies .qrc to the binary directory
configure_file(${PROJECT_SOURCE_DIR}/data/translations.qrc ${CMAKE_BINARY_DIR}/src COPYONLY)

# since qml-resources listed in the ${RESOURCES} files are not explicit dependencies,
# they will not be picked up by this.. You must edit one file in ${RESOURCES}
# for *.ts files (qt5_create_translation to be re-run).
#add_custom_target(translations ALL DEPENDS ${QM_FILES})
#add_custom_target(trans_resrc ALL DEPENDS ${RESOURCES})
#add_dependencies(trans_resrc translations)


#======================================
# Sinspekto Application
#======================================

set(RESOURCES
  qml/sinspekto.qrc
  ${PROJECT_SOURCE_DIR}/data/qtresources.qrc)

if(WIN32)
  set(RESOURCES ${RESOURCES} ${PROJECT_SOURCE_DIR}/data/app.rc)
endif()

set(HUMAN_APP_NAME sinspekto)
set(APP_NAME sinspekto-app)

set(PROG_FILES programs/sinspekto/sinspekto.cpp)
add_executable(${APP_NAME}
  ${PROG_FILES}
  ${RESOURCES}
  ${CMAKE_BINARY_DIR}/src/translations.qrc
  ${QM_FILES}
  )

#add_dependencies(${APP_NAME} trans_resrc)

target_link_libraries(${APP_NAME} PRIVATE
  ${SINSPEKTO_QT_TARGETS_APPS}
  sinspekto-api)

# CMAKE_DEBUG_POSTFIX does not apply to executable targets, so:
set_target_properties(${APP_NAME}
  PROPERTIES
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})


# If windows, run windeployqt to bundle Qt dll and other resources in bin
if(WIN32)

  find_program(WINDEPLOYQT_EXECUTABLE windeployqt REQUIRED DOC "Qt resource bundler")
  add_custom_target(qtdeploy ALL
    COMMAND ${WINDEPLOYQT_EXECUTABLE} ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX}
      --qmldir ${CMAKE_CURRENT_SOURCE_DIR}/qml
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Deploy Qt dependencies into binary directory for ${APP_NAME}"
  )
  add_dependencies(qtdeploy ${APP_NAME})
endif()

# This executable is not installed

#====================
# Minimal example

set(APP_NAME sinspekto-example)
set(PROG_FILES programs/example/minimal.cpp)
set(RESOURCES
  qml/minimal.qrc
  ${PROJECT_SOURCE_DIR}/data/qtresources.qrc)

add_executable(${APP_NAME}
  ${PROG_FILES}
  ${RESOURCES})

target_link_libraries(${APP_NAME} PRIVATE
  ${SINSPEKTO_QT_TARGETS_APPS}
  sinspekto-api)

set_target_properties(${APP_NAME}
  PROPERTIES
  DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# This executable is not installed

#====================
# Chart example

set(APP_NAME sinspekto-chart)
set(PROG_FILES programs/example/chart.cpp)
set(RESOURCES
  qml/chart.qrc
  ${PROJECT_SOURCE_DIR}/data/qtresources.qrc)

add_executable(${APP_NAME}
  ${PROG_FILES}
  ${RESOURCES})

target_link_libraries(${APP_NAME} PRIVATE
  ${SINSPEKTO_QT_TARGETS_APPS}
  sinspekto-api)

set_target_properties(${APP_NAME}
  PROPERTIES
  DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# This executable is not installed


#=====================
# Sinspekto Publisher

add_executable(sinspekto-pub programs/publisher.cpp)
target_link_libraries(sinspekto-pub PRIVATE
  sinspekto-api
  sinspekto-fkin)

set_target_properties(sinspekto-pub
  PROPERTIES
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# This executable is not installed

#======================
# Sinspekto Subscriber

add_executable(sinspekto-sub programs/subscriber.cpp)
target_link_libraries(sinspekto-sub PRIVATE sinspekto-fkin)

set_target_properties(sinspekto-sub
  PROPERTIES
  DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# This executable is not installed
