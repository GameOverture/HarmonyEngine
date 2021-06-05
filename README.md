    
# Harmony Engine

Cross-platform game engine and editor for making expansive 2D or 2.5D games.

**Disclaimer:** This is a very much work in progress and not ready for public consumption.

## Technologies Utilized

**Engine:** OpenGL, SDL2 (or GLFW), Box2d, glad, glm, stb, soil2, rapidjson

**Editor:** Qt, freetype2, freetype-gl, libogg/vorbis, QtWaitingSpinner

**Build:** CMake, Emscripten

  
## Engine Features

- Compile and run the same game code in Windows, Mac, Linux environments. As well as target web and mobile environments by transpiling to wasm via Emscripten.
- Robust and simple programming API, learn once, apply to every type of game object
- First-class support for Spine Animation
- TBD
  
## Editor Features
Setup and design sprite animations, fonts, sound cues, particles, and more. Or by using any combination of them, create complex entity hierarchies, apply physics, and setup bounding volumes

![App Screenshot](https://via.placeholder.com/468x300?text=App+Screenshot+Here)
- Quickly generate and setup new code projects and builds with all dependencies taken care of
- Handle the entire asset pipeline from importing, previewing, and replacing assets
- Efficient texture atlas management, packing, and compression
- Stylized bitmap font creation
- Audio designer
- TBD

## Installation 

1. Clone this repository
2. Download and execute the [CMake Installer](https://cmake.org/download/). Make sure to set the PATH variable during installation
3. Run a pre-built Harmony Editor or compile it from source

### Compiling the Editor
- Compiling the editor **requires at least Qt Version 5.13.0**, open-source version
- Using CMake, choose one of the two methods below:
```
-- GUI TOOL ---------------------------------------------------------------------------------------------
Open CMake-gui tool

"Where is the source code:" should be:    <path to repo>
"Where to build the binaries:" should be: <path to repo>\build

Press "Configure" button and confirm creating the build directory
In the next dialog, choose which IDE to generate in first combo box

Fill in the value for variable Qt5_DIR. Specify cmake Qt5 directory:
-Example-  C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5
  
Press "Configure" again until all red options disappear
Press "Generate" once
Press "Open Project" to open the generated solution (.sln) or navigate to the specified build directory

-- COMMAND LINE ----------------------------------------------------------------------------------------
  
cmake -S "<path to repo>" -B "<path to repo>\build" -DQt5_DIR=<path cmake Qt5> -G "Visual Studio 16 2019"

Fill in the value for variable Qt5_DIR. Specify cmake Qt5 directory:
-Example-  C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5

This will use default settings to generate a build. If on Windows, CMake will detect which version of 
Visual Studio you have installed to generate if you omit the -G argument. If Visual Studio 2019 is chosen 
it will default to generating 64bit, while older versions of Visual Studio will generate 32bit. You can
explicitly set the archetecture yourself by including the -G and -A arguments. Examples below:
... -G "Visual Studio 16 2019" -A Win32
... -G "Visual Studio 15 2017" -A x64
... -G "Visual Studio 14 2015" -A x64

The solution (.sln) will be placed in the build directory (-B argument above)
```
  
## Usage/Examples

```C++
HySprite2d hero("Level1", "Hero");
hero.Load();
hero.pos.Set(0.0f, 0.0f);

// Move right and grow larger
hero.pos.Tween(100.0f, 0.0f, 2.5f);
hero.scale.Tween(1.5f, 1.5f, 2.5f);

// TODO: actual useful examples
```
  
## Documentation

lol

  
## License

**Engine:** [Modified zlib](https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE/)

**Editor:** [GPL/LGPL](https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE/)
