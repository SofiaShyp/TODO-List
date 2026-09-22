# TODO List Application

A lightweight, modern desktop Task Manager built with C++ and Qt Quick (QML).

## Features
- Task management: Add, complete, and remove tasks
- Deadline tracking with automated validation
- Status filtering: View All, Active, or Completed tasks
- Intrusive alerting: System tray notifications, custom pop-ups, and audio alarms
- Persistent state management using JSON storage

## Demonstration
![App Demo](demo.gif)

## Requirements
- Qt 5.15+ or Qt 6.x
- CMake 3.16+
- C++17 compatible compiler (MinGW, MSVC, or GCC)

## How to Run Locally

### Option 1: Via Qt Creator
1. Launch **Qt Creator**.
2. Select **File > Open File or Project...** and choose `CMakeLists.txt`.
3. Select your build kit and click **Configure Project**.
4. Press `Ctrl + R` to build and run the application.

### Option 2: Via Terminal (CMake)
```bash
mkdir build
cd build
cmake ..
cmake --build .
