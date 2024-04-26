# PS4 Input

This application allows you to test your PS4 gamepads (or your input system implementation if you are making an PS4 emulator). It's based on OpenOrbis Input Sample, original project can be found [here](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/tree/master/samples/input).

## New features
- Left and right stick position tracking
- Haptic feedback on `L2`/`R2` trigger press
- Switching the lightbar color by pressing `Triangle`
- Mulitple users support (Press `Options` button on the gamepad to switch to it)
- TouchPad reading

## Libraries used

- libc
- libkernel
- libc++
- libSceVideoOut
- libScePad
- libSceUserService


## Building

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
