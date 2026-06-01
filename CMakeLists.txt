cmake_minimum_required(VERSION 3.16)

project(AtMap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Confirm if Windows and give warning if not 
if(WIN32)
    message(STATUS "Configuring for Windows platform...")
else()
    message(WARNING 
        "\n--------------------------------------------------------------------------\n"
        " WARNING: This project is primarily designed and tested for Windows.\n"
        " You are building on a non-Windows platform.\n\n"
        " You may need to:\n"
        " 1. Manually install Qt6, SQLite3, and PROJ via your package manager.\n"
        " 2. Ensure your system's library paths include these dependencies.\n"
        " 3. Note that 'windeployqt' automation will not run here.\n"
        "--------------------------------------------------------------------------"
    )
endif()

# Automatic handling of Qt specialized files (MOC, UI, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# QT path setup
find_package(Qt6 COMPONENTS Core Gui Widgets Positioning QUIET)
if(NOT Qt6_FOUND)
    message(FATAL_ERROR 
        "\n--------------------------------------------------------------------------\n"
        " ERROR: Qt6 was not found!\n\n"
        " Please provide the path to your Qt installation using one of these methods:\n"
        " 1. Pass it via command line:\n"
        "    cmake .. -DCMAKE_PREFIX_PATH=\"C:/Path/To/Qt/msvc2022_64\"\n\n"
        " 2. Edit CMakeLists.txt and uncomment the 'set(CMAKE_PREFIX_PATH ...)' line.\n"
        "--------------------------------------------------------------------------"
    )
endif()

# Define Source and Header files
set(PROJECT_SOURCES
    main.cpp
    messageHandler.cpp
    initManager.cpp
    mainwindow.cpp
    GotoLocationDialog.cpp
    outlineDialog.cpp
    SearchLocationDialog.cpp
    layersDialog.cpp
    CompassPane.cpp
    InfoPane.cpp
    MapWidget.cpp
    InfoWidget.cpp
    InfoTreeWidget.cpp
    MapManager.cpp
    layerManager.cpp
    layerGroup.cpp
    layer.cpp
    WKTConverter.cpp
    Planet.cpp
    User.cpp
    LongLat.cpp
    GeoResult.cpp
    GeoToScreen.cpp
    geoProjection.cpp
    LandDB.cpp
    soundEngine.cpp
    sourceManager.cpp
    soundSource.cpp
    directSource.cpp
    stereoSource.cpp
    spatialSource.cpp
    soundBuffer.cpp
    beacon.cpp
    # adding header files ensures they show up in the MSVC Solution Explorer.
    messageHandler.h
    initManager.h
    mainwindow.h
    GotoLocationDialog.h
    outlineDialog.h
    SearchLocationDialog.h
    layersDialog.h
    CompassPane.h
    InfoPane.h
    MapWidget.h
    InfoWidget.h
    InfoTreeWidget.h
    MapManager.h
    layerManager.h
    layerGroup.h
    layer.h
    WKTConverter.h
    Planet.h
    User.h
    LongLat.h
    GeoResult.h
    GeoToScreen.h
    geoProjection.h
    StepSizes.h
    RedrawReason.h
    LandDB.h
    soundEngine.h
    sourceManager.h
    soundSource.h
    directSource.h
    stereoSource.h
    spatialSource.h
    soundBuffer.h
    beacon.h
    soundTypes.h
    soundSourceCategories.h
    aliases.h
    AtMap.rc
)

# Create the Executable
# this version does not have a console window qt_add_executable(AtMap WIN32 ${PROJECT_SOURCES})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    qt_add_executable(AtMap ${PROJECT_SOURCES})
else()
    qt_add_executable(AtMap WIN32 ${PROJECT_SOURCES})
endif()


# Include Directories (Mapping to your INCLUDEPATH)
# Check path variables are set
# Check SQLite3 Path
if(NOT EXISTS "${SQLITE3_ROOT}/src/sqlite3.h")
    message(FATAL_ERROR "SQLITE3_ROOT is incorrect. Could not find src/sqlite3.h at: ${SQLITE3_ROOT}")
endif()

# Check OSGeo4W Path
if(NOT EXISTS "${OSGEO4W_ROOT}/include/proj.h")
    message(FATAL_ERROR "OSGEO4W_ROOT is incorrect. Could not find include/proj.h at: ${OSGEO4W_ROOT}")
endif()

if(NOT EXISTS "${SYNTHIZER_ROOT}/include/synthizer.h")
    message(FATAL_ERROR "SYNTHIZER_ROOT is missing or incorrect: ${SYNTHIZER_ROOT}")
endif()

# use path variables  to define  include paths 
target_include_directories(AtMap PRIVATE
    "${SYNTHIZER_ROOT}/include"
    "${SQLITE3_ROOT}/src"
    "${OSGEO4W_ROOT}/include"
)

# Link Libraries (Mapping to your LIBS)
target_link_libraries(AtMap PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Positioning
    "${SYNTHIZER_ROOT}/lib/synthizer.lib"
    "${SQLITE3_ROOT}/lib/sqlite3.lib"
    "${OSGEO4W_ROOT}/lib/proj.lib"
)

set_target_properties(AtMap PROPERTIES
    WIN32_EXECUTABLE FALSE  # Keep the console window for debugging
)


# Deployment / Install Rules (Optional)
if(NOT MSVC)
    install(TARGETS AtMap RUNTIME DESTINATION bin)
endif()

# Sets AtMap as the default "Startup Project" when you open the .sln
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT AtMap)

# Copy document folders (Fixed syntax and target name)
add_custom_command(TARGET AtMap POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/documents"
        "$<TARGET_FILE_DIR:AtMap>/documents"
    COMMENT "Copying document folder to executable directory..."
)

# This block automates the deployment of Qt DLLs to the build folder
if(WIN32)
    # Use the Qt path variable defined earlier to find the tool
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt 
        HINTS "${CMAKE_PREFIX_PATH}/bin"
    )

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET AtMap POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-compiler-runtime
                    "$<TARGET_FILE:AtMap>"
            COMMENT "Running windeployqt to gather dependencies..."
        )
    endif()
endif()