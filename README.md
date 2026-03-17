# OS-base
The base for making an OS

This is a **minimal, standalone kernel written in C**.  
It’s a base for building your own operating system or experimenting with kernel development.

---

## Features

- Pure C kernel (no OS required)
- Prints a message directly to the screen (bare-metal)
- Structured to allow **future modules** or plugins
- Can run in an emulator like **QEMU**

---

## Files

- `kernel.c` — main kernel logic  
- `boot.asm` — minimal bootloader (loads the kernel)

---

## Getting Started

### Requirements

- `NASM` (for assembling bootloader)
- `i686-elf-gcc` (or equivalent cross-compiler)
- `QEMU` (to run the kernel safely)

### Build and Run

```bash
make
qemu-system-i386 -fda os-image.bin
