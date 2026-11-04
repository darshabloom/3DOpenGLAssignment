# 3D OpenGL Assignment — Underwater Scene

A university computer graphics assignment by **Darsha Waterhouse**, implemented in C using **OpenGL** and **FreeGLUT**.

## Overview

The application renders an interactive underwater environment centred on a controllable octopus. The scene contains a heightmap-generated seabed, textured terrain, coral, fish, bubbles, underwater lighting and animated caustic light patterns. A treasure chest can be opened when the octopus approaches it.

Features visible in the source include:
- Octopus movement, turning and a camera that can orbit while the octopus is stationary.
- A toggle between simplified and detailed octopus models.
- Procedural or animated underwater scene elements, including bubbles and fish.
- Heightmap-based seabed geometry, sand/rock textures and 16 animated caustic frames.
- Lighting effects, including an octopus-related spotlight.
- Optional coordinate display, wireframe rendering and an in-app keybind overlay.

## Controls

| Key | Action |
| --- | --- |
| **W** | Toggle forward swimming |
| **S** | Move backwards (hold) |
| **A / D** | Move left / right |
| **↑ / ↓** | Move up / down |
| **← / →** | Turn while swimming; orbit the camera while stationary |
| **B** | Release a burst of bubbles |
| **E** | Open the treasure chest when close enough |
| **M** | Toggle octopus model |
| **L** | Toggle filled / wireframe rendering |
| **C** | Toggle coordinate display |
| **Esc** | Toggle the keybind overlay |
| **Q** | Quit |

## Project structure

```text
.
├── .github/workflows/build.yml  # Windows cloud build
├── 3DAssignment02_DarshaWaterhouse.c
├── assets/
│   ├── seabed_heightmap.ppm
│   └── caust_001.bmp ... caust_016.bmp
├── freeglut.h
├── freeglut_std.h
├── freeglut_ext.h
├── glut.h
├── freeglut.lib
├── freeglut.dll
└── README.md
```

**Additional large textures:** `sand.bmp` and `rock.bmp` (approximately 50 MB each) are required when running the programme. They are in the original assignment project but are **not yet included in this GitHub repository**.

## Building without installing Visual Studio

The GitHub Actions workflow uses a Microsoft C/C++ toolchain on a hosted Windows runner. It produces a **32-bit Release executable** and statically links the C runtime using `/MT`, avoiding the Visual Studio Debug runtime requirement.

1. Open the repository's **Actions** tab.
2. Select **Build portable Windows OpenGL app**.
3. Choose **Run workflow**.
4. After a successful run, download the **PortableOpenGL-Win32** artifact.
5. Extract the ZIP. The executable, FreeGLUT DLL and the assets currently stored in `assets/` should be in the same directory.
6. Copy `sand.bmp` and `rock.bmp` from the original assignment's `AllWeeks` folder alongside the executable.
7. Run `Assignment01_DarshaWaterhouse.exe` on a Windows PC with OpenGL support.

The executable loads image files relative to its current working directory. If it opens and immediately closes, verify that all texture files are beside the executable and launch it from a Command Prompt to inspect any error.

## Technologies

- C
- OpenGL (fixed-function rendering)
- FreeGLUT
- Microsoft C compiler via GitHub Actions

## About this repository

This repository contains the source and a portable build workflow. It was reorganised to keep scene assets together. This README describes functionality observable in the source; the formal assessment brief and report have not been reviewed for an authoritative statement of requirements or design decisions.
