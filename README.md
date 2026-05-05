# ZombieGame

![CI](https://github.com/thespiderchef/ZombieGame/actions/workflows/ci.yml/badge.svg) ![License: CC BY 4.0](https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg)

A small C++ demo game. This repository contains a simple C++ source file for building and running the game.

## Academic note

This project was developed as an assignment for Plymouth University for the COMP1000 module (2025/2026 academic year). Future work, extensions, or refactors may be added to this repository; check the commit history or open issues for planned improvements.

## Repository layout

- `ZombieGame/` — project sources
  - `main.cpp` — game entrypoint

## Prerequisites

- Linux: `g++` (supports C++17) or `clang++`


## Build & Run (Linux)

You can build directly with `g++` for quick runs, or use `CMake` for a cross-platform build.

Quick compile:

```bash
# compile the single-source demo
g++ -std=c++17 -O2 -IZombieGame -o ZombieGame ZombieGame/main.cpp

# run
./ZombieGame
```

CMake (recommended):

```bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Release

# run (from build directory)
./ZombieGame
```

`CMake` enables building on Windows, Linux, and macOS from the same build files.

## Notes about Visual Studio files

The previous Visual Studio solution and project files have been removed to keep the repository platform-neutral. Use `CMake` (above) to generate platform-specific project files or IDE solutions if needed.

## Contributing

Contributions and improvements are welcome. Please open issues or pull requests describing changes.

