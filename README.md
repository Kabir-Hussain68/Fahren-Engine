# Fahren-Engine

A hobbyist game engine made in OpenGL for learning purposes, currently in development. 

## Getting Started

VS Code is the recommended editor.

**1. Cloning the repository:**

Start by cloning the repository with `git clone --recurse-submodules https://github.com/Kabir-Hussain68/Fahren-Engine.git`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

**2. Building:**

```bash
cmake -B build
cmake --build build
./build/Sandbox/Sandbox
```

## Requirements
- CMake 3.22+
- GCC / Clang or MSVC
