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
> Neither batch nor make script will successfully build this project with the latest OpenOrbis release (v0.5.2), you should apply libScePad header patches from the `padpatch` directory!

To build this project, the developer will need clang, which is provided in the toolchain. The `OO_PS4_TOOLCHAIN` environment variable will also need to be set to the root directory of the SDK installation.

__Windows__
Run the batch file from command prompt or powershell with the following command:
```
.\build.bat .\x64\Debug
```

__Linux__
Run the makefile.
```
make
```


## Author(s)

- Specter
- igor725 (improved it a bit)
