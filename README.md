# Rubik's Cube Simulator

An interactive 3D Rubik's Cube simulator built with C++ and OpenGL (FreeGLUT) as part of a Computer Graphics Sessional project.

| | |
|---|---|
| ![alt text](/Images/1.png) | ![alt text](/Images/2.png) |

## Overview

This project renders a full 3x3x3 Rubik's Cube using individual cubies and supports smooth animated slice rotations. It includes mouse-based camera control, keyboard-driven moves, lighting, and a gradient background to improve visual clarity.

## Features

- Real-time 3D rendering with OpenGL
- 27-cubie cube representation (3x3x3)
- Animated face and middle-layer rotations
- Mouse drag camera orbit control
- Mouse/keyboard zoom control
- Basic lighting and colored faces
- Frame-updated animation timer (~60 FPS)

## Controls

### Mouse

- Left-click + drag: Rotate camera
- Right-click + drag: Zoom in/out
- Scroll wheel: Zoom in/out

### Keyboard

- `R` / `r`: Right face (CW / CCW)
- `L` / `l`: Left face (CW / CCW)
- `U` / `u`: Up face (CW / CCW)
- `D` / `d`: Down face (CW / CCW)
- `F` / `f`: Front face (CW / CCW)
- `B` / `b`: Back face (CW / CCW)
- `V` / `v`: Middle layer (vertical axis)
- `H` / `h`: Middle layer (horizontal axis)
- `+` / `-`: Zoom in/out
- `Esc`: Exit

## Tech Stack

- Language: C++
- Graphics: OpenGL (fixed-function pipeline)
- Windowing/Input: FreeGLUT
- Build environment: Code::Blocks + MinGW (Windows)

## Project Structure

- `main.cpp`: Application logic, rendering, animation, input handling
- `Rubik's Cube Simulator.cbp`: Code::Blocks project file
- `bin/`: Compiled binaries
- `obj/`: Build objects

## Build and Run (Windows, Code::Blocks)

1. Install Code::Blocks with MinGW.
2. Ensure FreeGLUT is available in your MinGW include/lib paths.
3. Open `Rubik's Cube Simulator.cbp` in Code::Blocks.
4. Build and run the `Debug` or `Release` target.

The project links against:

- `freeglut`
- `opengl32`
- `glu32`
- `winmm`
- `gdi32`

## Build from Command Line (MinGW, optional)

If your environment is configured with FreeGLUT headers/libraries:

```bash
g++ main.cpp -o rubiks_simulator -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32
./rubiks_simulator
```

## Academic Context

Developed as part of a Computer Graphics Sessional assignment.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
