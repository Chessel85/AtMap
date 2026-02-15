# Contributing to AtMap

Thank you for your interest in helping with AtMap. 

The goal of this project is to create a desktop mapping application accessible to everyone, regardless of visual ability. By presenting geographic information through both high-contrast graphics, screen reader friendly text content and audio cues, AtMap aims to allow blind and partially sighted users to get spatial data with the same ease as sighted users.

To maintain the quality and accessibility of this screen-reader-friendly application, please follow these guidelines.

## How Can I Help?

* **Reporting Bugs:** Open an issue on GitHub describing the bug and how to reproduce it.
* **Feature Requests:** Suggestions for new mapping features or accessibility improvements are always welcome.
* **Pull Requests:** If you want to contribute code, please follow the steps below.

## Development Setup
AtMap requires:
* **C++17** and **Qt 6**
* **SQLite3**, **Spatialite** and **PROJ** (via OSGeo4W)

Please refer to the [README.md](README.md) for full build instructions using CMake.

## Code Style & Standards

* **Accessibility First:** Any UI changes must be tested with a screen reader (e.g., NVDA). Ensure widgets have proper accessible names and roles.
* **C++ Style:** We use standard C++17. Please use meaningful variable names and keep functions focused on a single task.
* **Qt Best Practices:** Use the signal/slot mechanism appropriately and ensure all `QObject`-derived classes use the `Q_OBJECT` macro.

## Submitting a Pull Request

1. Fork the repository and create your branch from `main`.
2. Ensure the code compiles without warnings using **MSVC** or **MinGW**.
3. Update the documentation (README) if you change any build dependencies.
4. Submit the Pull Request with a clear description of your changes.