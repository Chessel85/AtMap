cmake_minimum_required(VERSION 3.16)

project(AtMap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Confirm if Winndows and give warning if not 
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

# Dependencies path definitions 
# Uncomment the following lines once you have put your own paths in 
# set(SQLITE3_ROOT "C:/<YourPath>/sqlite3" CACHE PATH "Path to SQLite3 source")
# set(OSGEO4W_ROOT "C:/<YourPath>/OSGeo4W" CACHE PATH "Path to OSGeo4W installation")

# Automatic handling of Qt specialized files (MOC, UI, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# QT path setup
# OPTION 1: Manual Override (Uncomment the line below for your local use)
# set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/msvc2022_64")
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/msvc2022_64")

# If CMAKE_PREFIX_PATH wasn't set on the command line OR uncommented above, 
find_package(Qt6 COMPONENTS Core Gui Widgets Sql Positioning QUIET)
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
    SearchLocationDialog.cpp
    CompassPane.cpp
    InfoPane.cpp
    MapWidget.cpp
    InfoWidget.cpp
    InfoTreeWidget.cpp
    MapManager.cpp
    WKTConverter.cpp
    Planet.cpp
    User.cpp
    LongLat.cpp
    GeoResult.cpp
    GeoToScreen.cpp
    IntelliLand.cpp
    LandByCoastlineDB.cpp
    LandDB.cpp
    WaterDB.cpp
    # Headers are usually not strictly required in PROJECT_SOURCES for CMake,
    # but adding them ensures they show up in the MSVC Solution Explorer.
    messageHandler.h
    initManager.h
    mainwindow.h
    GotoLocationDialog.h
    SearchLocationDialog.h
    CompassPane.h
    InfoPane.h
    MapWidget.h
    InfoWidget.h
    InfoTreeWidget.h
    MapManager.h
    WKTConverter.h
    Planet.h
    User.h
    LongLat.h
    GeoResult.h
    GeoToScreen.h
    StepSizes.h
    RedrawReason.h
    IntelliLand.h
    LandByCoastlineDB.h
    LandDB.h
    WaterDB.h
    aliases.h
)

# Create the Executable
add_executable(AtMap WIN32 ${PROJECT_SOURCES})

# Include Directories (Mapping to your INCLUDEPATH)
# Check path variabbles are set
# Check SQLite3 Path
if(NOT EXISTS "${SQLITE3_ROOT}/src/sqlite3.h")
    message(FATAL_ERROR "SQLITE3_ROOT is incorrect. Could not find src/sqlite3.h at: ${SQLITE3_ROOT}")
endif()

# Check OSGeo4W Path
if(NOT EXISTS "${OSGEO4W_ROOT}/include/proj.h")
    message(FATAL_ERROR "OSGEO4W_ROOT is incorrect. Could not find include/proj.h at: ${OSGEO4W_ROOT}")
endif()

# use path variables  to define  include paths 
target_include_directories(AtMap PRIVATE
    "${SQLITE3_ROOT}/src"
    "${OSGEO4W_ROOT}/include"
)

# Link Libraries (Mapping to your LIBS)
target_link_libraries(AtMap PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Positioning
    "${SQLITE3_ROOT}/lib/sqlite3.lib"
    "${OSGEO4W_ROOT}/lib/proj.lib"
)

# Deployment / Install Rules (Optional)
if(NOT MSVC)
    install(TARGETS AtMap RUNTIME DESTINATION bin)
endif()

# Copy Data and Scripts folders (Fixed syntax and target name)
add_custom_command(TARGET AtMap POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:AtMap>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
        "$<TARGET_FILE_DIR:AtMap>/scripts"
    COMMENT "Copying data and scripts folders to executable directory..."
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