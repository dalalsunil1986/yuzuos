# Build instructions

## Prerequisites

### Linux

#### Debian family

```console
$ sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo cmake qemu-system-x86 ninja-build wget
```

## Build

Create a directory called `build` at the root of the project, then run cmake
```console
$ mkdir build
$ cd build
$ cmake .. -G Ninja
```
### Toolchain

Now start compiling the toolchain
```console
$ ninja toolchain
```
This will compile the `GCC` and `binutils` and install in the `build/tools/cross` directory

Once the toolkit has been built, we can build the project
```console
$ ninja install
```
This command will compile the entire `YuzuOS` and install the built files into `build/sysroot`, using the `ninja` command will automatically build as many jobs in parallel as it detects processors

Now build a disk image from the `build/sysroot` directory
```console
$ ninja image
```
This will create a disk image called `qemu_img` using the file system `ext2` and save it in the `build` directory

### Emulator

We use the `QEMU` emulator to run` YuzuOS`, to compile it run the following command:
```console
$ ninja qemu
```
This will compile the `QEMU` and install in the `build/tools/cross` directory

## Run

Now we can test the operating system using the `qemu-system-x86` emulator
```console
$ ninja run
```
Note that the above command also starts automatic compilation, discarding the use of the `ninja all` command
