# Flux
 ![RepoSize](https://img.shields.io/github/repo-size/Shsl4/Flux)

A C++ Audio and UI playground
## Building
This project depends on Skia, GLFW, RtMidi, RtAudio, Vulkan-Headers and Nucleus, which must be compiled locally. A python script is provided to automate this process.

> ⚠️ This commit only supports macOS arm64 compilation. Windows and Linux have not been tested and will probably fail.

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