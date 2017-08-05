# ChaOS [![Build Status](https://travis-ci.org/Arignir/chaos.svg?branch=master)](https://travis-ci.org/Arignir/chaos)

ChaOS is a fun project that i'm doing to entertain myself during 2017's summer vacations.

It's a unix-like featureless kernel, that only supports x86 (Intel 32 bits). My goal is to end-up having a basic shell that can handle simple userland commands like `ls`, `cat` and `echo`.

# Build Dependencies
* `nasm`
* `make`
* `gcc` or `clang`
* `grub-mkrescue` and `xorriso` (generally packed with other binaries as `grub`)
* `qemu` (cpu emulator) *optional*

If you are using `apt-get` as your package manager (`Debian`, `Ubuntu` etc.), you can use this command to install all dependencies:
```bash
apt-get install qemu grub-pc-bin xorriso nasm
```

If you are using `pacman` as your package manager (`ArchLinux`, `Manjaro` etc.), you can use this command instead:
```bash
pacman -Sy qemu grub libisoburn nasm
```

If you are using an other package manager, well... Good luck! :p

# Building an iso

To build the kernel, run
```bash
make kernel
```

To build a complete iso with grub installed (suitable for USB flash drives or virtual machines), run
```bash
make iso
```

# Running with QEMU

If you want to run ChaOS through QEMU even if it's boring & useless right now, run
```bash
make run
```

# Roadmap

These are all the features that i'd like to implement by the end of 2017's summer vacations. I probably won't have the time to implement everything, but eh, challenge accepted! :D

- [X] Kernel architecture
- [X] High-address Kernel
- [X] Boot
  - [X] Multiboot
  - [X] Paging setup
  - [X] GDT setup
  - [X] IDT setup
- [ ] Pc drivers
  - [X] VGA
  - [X] Serial
  - [ ] Date / time
- [X] Memory
  - [X] PMM
  - [X] VMM
  - [X] Kernel heap
  - [X] User heap
  - [X] User stacks / mmap
- [ ] Multi process / threads
  - [X] Scheduling
  - [X] Kernel threads
  - [ ] Processes (`fork()` and `exit()`)
- [X] Syscall interface
- [ ] Userspace (ring3)
- [ ] Filesystem (`open()`, `read()`, `write()`, `close()`, `unlink`, `opendir()`, `readdir()`, `closedir()`, `pipe()`, `dup()`, `dup2()`)
- [ ] ELF loading
- [ ] User space programs (init, shell, basic binaries such as `echo`, `ls`, `rm` etc.)

# :rocket: Wanna participate?

Fork me!
