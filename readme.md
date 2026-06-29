# Northern Kingdom

A real-time 3D rendering demo built from scratch in C++ and OpenGL 4.5, developed as part of the Computer Graphics course at TU Wien (2025). The scene depicts a medieval village with animated characters, a height-map-based terrain, dynamic lighting and shadows.

<img width="720" height="450" alt="Screenshot-NorthernKingdom" src="https://github.com/user-attachments/assets/af4a58d3-4934-4c98-aad5-a8be1e64ed89" />

## Scene

The scene contains multiple 3D-asset-loaded models (houses, a medieval tower, a castle guard with idle animation, a peasant girl with a walk animation, a well, a hanging sign, a water plane, street lights, and a backpack) placed on a procedurally displaced terrain generated from a PNG height map.

## Rendering Features

**Terrain**
- Height-map-driven terrain generated from a PNG grayscale image
- Adaptive tessellation via a hardware Tessellation Control/Evaluation shader (TCS/TES): tessellation level ranges from 4 to 64 depending on camera distance (min 20, max 800 units)
- Triplanar texture blending across three terrain layers: rock, grass, and snow, each with a diffuse and normal map (DDS format)
- Terrain-aware camera collision: camera stays above terrain surface at a configurable height offset

**Skeletal Animation**
- Bone hierarchy loaded from DAE (Collada) files via Assimp
- Linear blend skinning computed on the CPU; bone transformation matrices uploaded to the vertex shader as uniforms
- Two animated characters: castle guard (idle animation) and peasant girl (catwalk animation), each driven by a separate Animator instance

**Shadows**
- Directional shadow map: orthographic light projection rendered to a depth framebuffer, sampled with manual 3x3 PCF (Percentage Closer Filtering) and slope-based bias to reduce acne
- Omnidirectional shadow map: depth cubemap rendered using a geometry shader that emits 6 faces in one pass; sampled with 20-sample PCF for soft point light shadows

**Lighting**
- Blinn-Phong illumination model: ambient, diffuse, and specular contributions from one directional light and one point light
- Normal mapping: tangent-space normals computed in the vertex shader using a manually derived TBN matrix; toggleable at runtime (N key)
- Point light rendering: a lit geometry cube rendered at the light source position; point light can be toggled on/off at runtime (L key)
- Emission texture support

**Skybox**
- Cubemap skybox rendered with a dedicated shader pass; depth clamped to far plane to always appear behind geometry

**Model Loading**
- All 3D assets loaded via Assimp supporting OBJ and DAE formats
- Per-model diffuse, specular, and normal texture overrides
- Geometry class wraps VAO/VBO/EBO setup

## Controls

| Key / Input | Action |
|---|---|
| W A S D | Move camera |
| Mouse | Look around |
| Scroll | Zoom (FOV) |
| Arrow keys | Translate nearest object |
| K / M | Move nearest object up / down |
| TAB | Toggle between translate and rotate mode |
| L | Toggle point light on/off |
| N | Toggle normal mapping on/off |
| F | Toggle wireframe mode |
| ESC | Quit |

On startup, a pre-recorded camera path (`recordedCameraPath.txt`) is loaded and played back automatically if the file is present.

## Configuration

Window resolution, refresh rate, FOV, near/far planes and other settings are read from `assets/settings.ini` at startup — no recompilation needed.

## Tech Stack

| | |
|---|---|
| Language | C++20 |
| Graphics API | OpenGL 4.5 (Core Profile) |
| Windowing | GLFW |
| Extension loading | GLEW |
| Math | GLM |
| Model/animation loading | Assimp |
| Texture loading | stb_image, GLI (DDS) |
| Build system | CMake 3.30+ with vcpkg |

## Build Instructions

**Requirements:**
- CMake 3.30+
- C++20 compiler (MSVC, GCC, or Clang)
- GPU with OpenGL 4.5 support (tested on NVIDIA GeForce RTX 4070 Ti SUPER)
- vcpkg (included as a submodule)

**Windows — Visual Studio**

```bat
GENERATE_SLN.bat
```

This script runs CMake with the vcpkg toolchain and generates a `.sln` file. Open it in Visual Studio and build.

**Manual CMake**

```bash
git clone --recurse-submodules https://github.com/melli27/cg25-NorthernKingdom.git
cd cg25-NorthernKingdom
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## Project Structure

```text
assets/
  models/               # 3D model files (OBJ / DAE) and textures
  textures/             # Terrain textures (rock, grass, snow — DDS)
  hm2.png               # Height map used for terrain generation
  settings.ini          # Runtime configuration (resolution, FOV, ...)

src/
  NorthernKingdom.cpp   # Entry point: window, context, main loop
  Scene.cpp/h           # Scene setup: loads models, lights, terrain
  RenderManager.cpp/h   # Render calls: shaded, animated, depth passes
  InputManager.cpp/h    # Keyboard and mouse input handling
  Camera.cpp/h          # Free-fly camera with terrain collision
  Shader.cpp/h          # GLSL shader program wrapper
  Depthmap.cpp/h        # Shadow map framebuffer setup
  Animation.cpp/h       # Keyframe animation data (from Assimp)
  Animator.cpp/h        # Skeletal animation state and interpolation
  Bone.cpp/h            # Bone transform hierarchy
  Lights/               # DirectionalLight, PointLight, LightManager
  Model/                # Mesh, Model, Geometry, Texture classes
  Shader/               # All GLSL shader source files
  Skybox/               # Cubemap skybox rendering
  Terrain/              # Tessellated height-map terrain
  Utils/                # INIReader, stb_image, math utilities
```
