# AtMap

A screen reader friendly mapping application written in C++ built with **Qt 6**, **SQLite3**, and **PROJ (OSGeo4W)**.

## Prerequisites

Before building this project, ensure you have the following installed on your system:

* **Compiler:** Visual Studio 2022 (MSVC) or a C++17 compatible compiler.
* **CMake:** Version 3.16 or higher.
* **Qt 6:** Components required: `Core`, `Gui`, `Widgets`, `Positioning`.
* **SQLite3:** Source code or development binaries.
* **OSGeo4W:** Including the `PROJ` library.

## Configuration & Building

The project uses CMake to manage dependencies. You have two ways to point the project to your local library installations.

### Option 1: Command Line 

This approach keeps your local paths out of the source code. From the project root, run:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/msvc2022_64" ^
  -DSQLITE3_ROOT="C:/Path/To/sqlite3" ^
  -DOSGEO4W_ROOT="C:/OSGeo4W"
```

### Option 2: Editing CMakeLists.txt

If you prefer not to type long commands, you can edit the CMakeLists.txt file directly:

1.  Locate the # Dependencies path definitions section.
2.  Update the SQLITE3_ROOT and OSGEO4W_ROOT paths.
3.  Uncomment the set(CMAKE_PREFIX_PATH ...) line for Qt.

Then simply run:

cmake -B build -S .


## Compiling

Once the configuration is successful, build the project using CMake:

cmake --build build --config Release

## Deployment

This project includes a Post-Build step for Windows users:

* Qt DLLs: windeployqt will automatically gather the necessary Qt libraries into your build folder.
* Assets: The data/ and scripts/ folders are automatically copied to the executable directory so the application can access its databases and logic files immediately upon launch.

## Post-build Action - the database

AtMap expects a Spatialite database in a 'data' subfolder from the executable called 'land.db'.  This can be downloaded from the Releases area of the gitHub for AtMap and manually placed in the stated location.

## Troubleshooting

* Qt Not Found: Ensure your CMAKE_PREFIX_PATH points specifically to the architecture folder (e.g., msvc2022_64), not just the base Qt directory.
* Missing Headers: If CMake throws a FATAL_ERROR regarding sqlite3.h or proj.h, double-check that your _ROOT variables point to the folders containing the src or include subdirectories.
* Non-Windows Platforms: While the project can be configured on Linux or macOS, you will receive a warning. You must manually ensure all dependencies are available in your system's library paths, as the automated Windows deployment tools will not run.

