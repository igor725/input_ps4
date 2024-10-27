# PS4 Input

This application allows you to test your PS4 gamepads (or your input system implementation if you are making an PS4 emulator). It's based on OpenOrbis Input Sample, original project can be found [here](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/tree/master/samples/input).

## New features
- Left and right stick position tracking
- Mulitple users support (Press `Options` button on the gamepad to activate it)
- Haptic feedback on `L2`/`R2` trigger press for active gamepad
- Switching the lightbar color by pressing `Triangle` for active gamepad
- Pad speaker playing some sound by pressing `Cross` for active gamepad
- TouchPad reading for active gamepad

## Libraries used

- libc
- libkernel
- libc++
- libSceVideoOut
- libSceAudioOut
- libScePad
- libSceUserService


## Building

> [!WARNING]
> This project won't build with the latest atm OpenOrbis release (v0.5.2), you should use my [OpenOrbis fork](https://github.com/igor725/OpenOrbis-PS4-Toolchain) for it!

To build this project, the developer will need clang, which is provided in the toolchain. The `OO_PS4_TOOLCHAIN` environment variable will also need to be set to the root directory of the SDK installation.

This project uses my [OpenOrbis template](https://github.com/igor725/oo_template), to build the project use following CMake:
```
cmake -B./build/ -S./ -DCMAKE_TOOLCHAIN_FILE=OpenOrbis-tc.cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build ./build/ -j4
cmake --install ./build/
```

You can use VS Code with recomended extensions aswell. CMake extension is already configured in project directory.


## Author(s)

- Specter
- igor725 (improved it a bit)
