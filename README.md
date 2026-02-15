# AtMap

A screen reader friendly mapping application written in C++ built with the Qt 6, Spatialite, and OSGeo4W libraries.

There is a Windows MSI install package on the Releases page.  Click on the Releases heading below and then search the page for 'Assets' and arrow down to the AtMapSetup.msi link.

## Prerequisites

Before building this project, ensure the following are installed:

* **Compiler:** [Visual Studio 2022 (MSVC)](https://visualstudio.microsoft.com/downloads/) or a [MinGW-w64](https://www.mingw-w64.org/) (C++17 compatible) compiler.
* **CMake:** [Version 3.16](https://cmake.org/download/) or higher.
* **Qt 6:** [Qt online installer.](https://www.qt.io/development/download-open-source) Components required: `Core`, `Gui`, `Widgets`, `Positioning`.
* **SQLite3:** [Source code or development binaries](https://www.sqlite.org/download.html).
* **OSGeo4W:** [Network installer](https://trac.osgeo.org/osgeo4w/) Including the 'Spatialite' and `PROJ` libraries.

## Configuration & Building

The project uses CMake to manage dependencies. There are two ways to point the project to the local library installations.

### Option 1: Command Line 

This approach avoids editing the `CMakeLists.txt` file.  Copy the following code and set the paths correctly for each library.  Then run the command from the project root.

MSVC users:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_PREFIX_PATH="C:/Path/To/Qt/6.9.1/msvc2022_64" ^
  -DSQLITE3_ROOT="C:/Path/To/sqlite3" ^
  -DOSGEO4W_ROOT="C:/Path/To/OSGeo4W"
```

MINGW users:

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH="C:/Path/To/Qt/6.x.x/mingw_64" ^
  -DCMAKE_PREFIX_PATH="C:/Path/To/Qt/6.9.1/msvc2022_64" ^
  -DSQLITE3_ROOT="C:/Path/To/sqlite3" ^
  -DOSGEO4W_ROOT="C:/Path/To/OSGeo4W"
```

### Option 2: Editing CMakeLists.txt

The 'CMakeLists.txt' file contains variables that can be updated before building the solution.

1.  Locate the `# Dependencies path definitions` section.
2.  Update the SQLITE3_ROOT and OSGEO4W_ROOT paths. 
3.  Uncomment the set(CMAKE_PREFIX_PATH ...) line for Qt.

Then run the following from the project root folder:

```bash
cmake -B build -S .
```

## Compiling

Once the configuration is successful, build the project using CMake:

```bash
cmake --build build --config Release
```

Or use an IDE.

## Deployment

This project includes a Post-Build step for Windows users:

* Qt DLLs: windeployqt will automatically gather the necessary Qt libraries into your build folder.
* Assets: The data/ and scripts/ folders are automatically copied to the executable directory so the application can access its databases and logic files immediately upon launch.

## Post-build Actions 

### The database

AtMap expects a Spatialite database in a 'data' subfolder from the executable called 'land.db'.  This can be downloaded from the Releases area of the gitHub for AtMap and manually placed in the stated location.

### proj DLL and DB files

Geographic transformations are carried out by the 'proj_9.dll' which must be accompanied with a matching 'proj.db' file.  

* 'proj_9.dll' is located in 'OSGeo4W/bin'
* 'proj.db' is located in 'OSGeo4W/share/proj'

Copy these files to the executable folder.

## Troubleshooting

* Qt Not Found: Ensure your CMAKE_PREFIX_PATH points specifically to the architecture folder e.g. 'msvc2022_64' or 'mingw_64', not just the base Qt directory.
* Missing Headers: If CMake throws a FATAL_ERROR regarding sqlite3.h or proj.h, double-check that your _ROOT variables point to the folders containing the src or include subdirectories.
* Non-Windows Platforms: While the project can be configured on Linux or macOS, you will receive a warning. You must manually ensure all dependencies are available in your system's library paths, as the automated Windows deployment tools will not run.

