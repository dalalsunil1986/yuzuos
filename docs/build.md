# Build instructions

## Prerequisites

### Linux

#### Debian family

```console
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo cmake qemu-system-x86 ninja-build wget
```

## Build

Create a directory called `build` at the root of the project, then run cmake
```console
$ mkdir build
$ cd build
$ cmake .. -G Ninja
```

Now start compiling the toolchain
```console
$ ninja toolchain
```
This will compile the GCC and binutils and install in the `build/tools/cross` directory

Once the toolkit has been built, we can build the project
```console
ninja all
```
This command will compile the entire `YuzuOS`, using the `ninja` command will automatically build as many jobs in parallel as it detects processors

## Run

Now we can test the operating system using the `qemu-system-x86` emulator
```console
ninja run
```
Note that the above command also starts automatic compilation, discarding the use of the `ninja all` command
