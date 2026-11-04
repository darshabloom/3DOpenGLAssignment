# 3D OpenGL Assignment — Underwater Scene

A university **Computer Graphics and Programming** project by **Darsha Waterhouse**, implemented in **procedural C** using **OpenGL** and **FreeGLUT**. The project follows the *Octopus Scene* assignment specification and develops an interactive 3D underwater animation.

## Overview

The application renders an interactive underwater environment centred on a controllable octopus. The scene contains a heightmap-generated seabed, textured terrain, coral, fish, bubbles, underwater lighting and animated caustic light patterns. A treasure chest can be opened when the octopus approaches it.

Features visible in the source include:
- Octopus movement, turning and a camera that can orbit while the octopus is stationary.
- A toggle between simplified and detailed octopus models.
- Procedural or animated underwater scene elements, including bubbles and fish.
- Heightmap-based seabed geometry, sand/rock textures and 16 animated caustic frames.
- Lighting effects, including an octopus-related spotlight.
- Optional coordinate display, wireframe rendering and an in-app keybind overlay.

## Assignment objectives

The assignment calls for a scaled 3D environment, a hierarchical animated octopus model, a track-and-follow camera, smooth simultaneous keyboard controls, and effective use of fixed-function OpenGL lighting, materials, texture mapping, display lists and fog. The octopus is designed to move in surge (forward/backwards), sway (sideways), heave (vertical movement) and yaw (turning), with animated feet and a propeller.

The assessment also asks for an engaging complete scene with additional objects and animation effects, plus a development logbook explaining design decisions and progress. These are **assignment requirements**; the overview above describes features present in the repository's source code.

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

This repository contains the source code, FreeGLUT dependencies, scene assets and a portable Windows build workflow. Its structure was reorganised to keep animated texture frames and the heightmap together. The project description uses the *Computer Graphics and Programming — Octopus Scene* assignment specification alongside features observed in the source. The separate developer logbook and complete assessment submission are not included here.
