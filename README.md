# ZombieGame

A small C++ demo game. This repository contains a Visual Studio solution and a simple C++ source file for building and running the game.

## Academic note

This project was developed as an assignment for Plymouth University for the COMP1000 module (2025/2026 academic year). Future work, extensions, or refactors may be added to this repository; check the commit history or open issues for planned improvements.

## Repository layout

- `ZombieGame.sln` — Visual Studio solution
- `ZombieGame/` — project sources
  - `main.cpp` — game entrypoint

## Prerequisites

- Linux: `g++` (supports C++17) or `clang++`
- Windows: Visual Studio (open `ZombieGame.sln`)

## Build & Run (Linux)

From the repository root:

```bash
# compile the single-source demo
g++ -std=c++17 -O2 -IZombieGame -o ZombieGame ZombieGame/main.cpp

# run
./ZombieGame
```

If your project grows, consider adding a `Makefile` or `CMake` configuration.

## Build & Run (Windows / Visual Studio)

Open `ZombieGame.sln` in Visual Studio and build the solution (Debug/Release). Run the produced executable from the IDE or Explorer.

## Contributing

Contributions and improvements are welcome. Please open issues or pull requests describing changes.

