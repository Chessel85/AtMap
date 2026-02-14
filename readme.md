# Accessible Atlas

A modern C++ mapping application built with **Qt 6**, **SQLite3**, and **PROJ (OSGeo4W)**.

---

## Prerequisites

* **Compiler:** Visual Studio 2022 (MSVC)
* **CMake:** Version 3.16+
* **Qt 6:** Core, Gui, Widgets, Sql, Positioning
* **SQLite3:** Source/Binaries
* **OSGeo4W:** PROJ library

---

## Configuration & Building

### Option 1: Command Line (Recommended)

From the project root, run:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/msvc2022_64" ^
  -DSQLITE3_ROOT="C:/Path/To/sqlite3" ^
  -DOSGEO4W_ROOT="C:/OSGeo4W"