FIND_PACKAGE(Qt4 REQUIRED)

clear( sasi_moc_header )
add_sources( sasi_moc_header 
  missionmodel.h
  maplistmodel.h
  qminimap.h
  sidemodel.h
  sasi_app.h
  battlelistmodel.h
  audio/audiomanager.h
  skirmishmodel.h
  qbattleroom.h
  qerrorwindow.h
  engineconfig.h
  engineconfig.h )

clear(sasi_sources )  
add_sources( sasi_sources
  battlelistmodel.cpp
  converters.cpp
  engineconfig.cpp
  exceptions.cpp
  FileHandler.cpp
  imageprovider.cpp
  maplistmodel.cpp
  missionmodel.cpp
  noguisingleplayerbattle.cpp
  qbattleroom.cpp
  qminimap.cpp
  sasi_app.cpp
  sasi_main.cpp
  sidemodel.cpp
  simpleserverevents.cpp
  skirmishmodel.cpp )
  
add_subdirectory( audio )

SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTDECLARATIVE TRUE)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
QT4_WRAP_CPP(sasi_processed_header ${sasi_moc_header})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS} -DSL_QT_MODE)

ADD_EXECUTABLE(sasi ${sasi_sources} 
    ${sasi_processed_header})
TARGET_LINK_LIBRARIES(sasi lsl-unitsync lsl-utils ${QT_LIBRARIES})

