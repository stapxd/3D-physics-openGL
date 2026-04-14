### About the engine
A 3D physics engine built using C++17, OpenGL, GLFW, and GLEW. This project uses Premake5 for build configuration and project generation.

Prerequisites
- Visual Studio 2019 or 2022
- Windows OS (Current configuration targets Windows x86)

### Installation & Generation

1. Clone the repository

```
git clone https://github.com/stapxd/3D-physics-openGL.git
cd 3D-physics-openGL
```

2. Generate Visual Studio Files

Navigate to the scripts folder and run the batch file:

> If you want to use .bat file ensure that you have premake5.exe in vendor\bin\premake folder

```
# Runs Premake5 and creates the .sln file
GenerateProjects.bat
```

3. Build the Project

- Open OpenGLPhysicsEngine3D.sln in Visual Studio.
- Set your configuration (e.g., Debug or Release).
- Hit Build
