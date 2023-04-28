# Flux
 ![RepoSize](https://img.shields.io/github/repo-size/Shsl4/Flux)

A C++ Audio and UI playground
## Building
This project depends on Skia, GLFW, RtMidi, RtAudio, Vulkan-Headers and Nucleus, which must be compiled locally. A python script is provided to automate this process.
### macOS
Install the required build tools:
```sh
brew install cmake ninja python3
```
Clone the project and launch the setup script:
```sh
git clone https://github.com/Shsl4/Flux.git --recursive
cd Flux
python3 setup.py
```
Open the generated Xcode project:
```sh
open Flux.xcodeproj
```
### Windows
Install the required build tools:
```sh
winget install Kitware.CMake 
winget install Ninja-build.Ninja 
winget install python3
```
Clone the project and launch the setup script:
```sh
git clone https://github.com/Shsl4/Flux.git --recursive
cd Flux
python3 setup.py
```
Open the generated Visual Studio project:
```sh
open Flux.lnk
```
### Linux (Debian)
Install the required build tools:
```sh
sudo apt install cmake ninja-build python3
```
Make sure required linux libraries are installed:
```sh
sudo apt install pulseaudio alsa libfreetype-dev libasound2-dev
```
Clone the project and launch the setup script:
```sh
git clone https://github.com/Shsl4/Flux.git --recursive
cd Flux
python3 setup.py
```
Open the project with a CMake compatible IDE such as CLion or use the makefile build:
```sh
cd .cmake
make
```
