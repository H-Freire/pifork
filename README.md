# PiFork

## Overview

A Raspberry Pi 2 implementation of a simple operational system with UNIX standard system calls for process handling

## Project Structure

```
.
├── include/      # Public header files
├── src/          # Source code
├── rpi-firmware/ # Base firmware and output directory if image
├── examples/     # Use cases
└── Makefile      # Build instructions
```
## Prerequisites (toolchain)

- **Compiler**: GCC (arm-none-eabi-gcc)
- **Debugger**: GDB (gdb-multiarch)
- **Emulator**: QEMU (qemu-system-arm)
- **Build Tool**: Make

Ensure the required tools are installed:

```
sudo apt-get update
sudo apt-get install binutils-arm-none-eabi gcc-arm-none-eabi gcb-multiarch qemu-system-arm make
```

## Building the Project

To fully compile the project, use the following command:

```
make all CCVERSION=XX.Y.Z EXAMPLE=program
```

Where XX.Y.Z represents your installed GCC version, which may be checked with `arm-none-eabi-gcc -v` and program refer to one of the provided examples (wait, waitpid, permissions).

This command will compile all source files, reproducing the `src/` directory structure with object files in a `obj/` directory. The generated image of the kernel can be located at the `rpi-firmware/` directory.

## Debugging

To debug the project, you will need a J-Tag connected to the RPi and OpenOCD installed. With those requirements in order, run:

```
make debug
```

This command will start the GDB debugger and load the `.elf` executable on the board for inspection and step-through execution.

## Emulation

To run the project on the `raspi2b` machine, use the command:

```
make qemu
```

This will start QEMU and emulate the `.elf` executable on the target architecture.
It should be warned that the emulation capability for this particular project is limited due to lack of support for certain features on the mentioned machine, such as the Core Timer. This means that the task/process switching and dispatching won't work on its on with the emulator, requiring manual branches on the code execution to verify its functionality.

## Cleanup

To clean the build artifacts and stop running applications, use:

```
make clean
```
or
```
make rebuild
```
Both will remove the compiled executable, image and object files, while the second command will also recompile those artifacts. The considerations noted on the [build instructions](#building-the-project) should be taken into consideration when running the latter.

