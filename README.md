# Opensource toolchain for stm32

STM32 is a family of 32-bit microcontroller integrated circuits by STMicroelectronics. The STM32 chips are grouped into related series that are based around the same 32-bit ARM processor core, such as the Cortex-M33F, Cortex-M7F, Cortex-M4F, Cortex-M3, Cortex-M0+, or Cortex-M0. Internally, each microcontroller consists of the processor core, static RAM, flash memory, debugging interface, and various peripherals.

The STM32 family consists of 17 series of microcontrollers: H7, F7, F4, F3, F2, F1, F0, G4, G0, L5, L4, L4+ L1, L0, U5, WL, WB. and each series includes a lot of models with minor differences.

For more information about STM32 family, please refer to https://en.wikipedia.org/wiki/STM32.

There are also a lot of STM32 clones, such as GD32 / CH32 / MM32 etc. Most of them keep compatible with STM32. The toolchain and utilities described in this tutorial may also be used with such parts.

# Hardware prerequisites

* A development board with STM32 MCU. In this tutorial, I will use STM32F1/F4/H7 and GD32, CH32. 
* DAPLink or STLink or JLink.
  - Most of so called xxLink from various vendors is DAPLink with SWD interface support.

# Toolchain overview

As any other ARM based MCUs, the toolchain for STM32 consists of:

* Compiler: GCC / Rust
* Debugger: OpenOCD/gdb
* SDKs: Various
  - spl/stm32cube and libopencm3 in C
  - stm32-hal and stm32-rs in rust, gd32-rs (forked from stm32-hal) in rust for GD32
* Flashing tool: ISP and OpenOCD.


# Compiler

## GCC

STM32 and various XX32 use the 'arm-none-eabi' GCC toolchain, since they all are based on Cortex-M. it's not neccesary to build the toolchain yourself, there are already a lot of well supported prebuilt release and already widely used by developers. 

You can download the prebuilt toolchain for various host from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads. 

Download and extract the toolchain for x86_64 linux:

```
wget "https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel
1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz?rev=7bd049b7a3034e64885
fa1a71c12f91d&hash=732D909FA8F68C0E1D0D17D08E057619" -O arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz
sudo tar xf arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz -C /opt
```

And add `/opt/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi/bin` to PATH env of your shell.

**NOTE:** the toolchain's tripplet is 'arm-none-eabi'.

There are also a lot of prebuilt 'arm-none-eabi' toolchains from other vendors, you can use them, the installation process is almost same.

## Rust

These years, as the Rust language evolves, it can be used for MCU development.

Building rust is not a easy task for beginners, the easiest way to install rust toolchain is [rustup](https://rust-lang.github.io/rustup/).

Install rustup:

```
# install rustup
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
# install stable toolchain, it will install various components, such as rustc, rust-std, cargo ...
rustup default stable
```

After rustup and default stable toolchain installed, you can install some targets as need,
```
# For Cortex-M4F and Cortex-M7F (eg F, L4, H7):
rustup target add thumbv7em-none-eabihf

# For Cortex-M0 and Cortex-M0+ (eg G)
rustup target add thumbv6m-none-eabi

# For Cortex-M33F and Cortex-M35F (eg L5, U5):
rustup target add thumbv8m.main-none-eabihf
```


# SDKs

STM32 has a large, high-quality development ecosystem, there are various opensource tools and SDKs for STM32 development. 

You can always write firmware which directly runs on the hardware without using any underlying software abstraction (So called bare metal programming). For ARM GCC, except the official SPL/STM32Cube libraries, there is [libopencm3](https://github.com/libopencm3/) which support STM32 very well. For Rust, there is [stm32-hal](https://github.com/David-OConnor/stm32-hal) or [stm32-rs](https://github.com/stm32-rs) which support a lot of STM32 models, 

## Bare Metal

For simple tasks such as blink a led, you can write some bare metal codes without depending on any libraries, usually a bare metal project of STM32 consists of: 

- **A Linker script** which define the memory layout
- **A startup file** in c or asm (typically written in assembly) which:
  + initialize the stack pointer
  + initialize the non-zero read/write-data in RAM
  + initialize the zero read/write-data in RAM
  + define the interrupt vector table
  + jump to the main function
- **A main function**

There are some baremetal demos in this repo for STMf1/f4/h7, you can take these demos as reference.

## SPL

Standard Peripherals library covers 3 abstraction levels, and includes:

* A complete register address mapping with all bits, bit fields and registers declared in C. This avoids a cumbersome task and more important, it brings the benefits of a bug free reference mapping file, speeding up the early project phase.

* A collection of routines and data structures which covers all peripheral functions (drivers with common API). It can directly be used as a reference framework, since it also includes macros for supporting core-related intrinsic features and common constants and data types definition.

* A set of examples covering all available peripherals with template projects for the most common development toolchains. With the appropriate hardware evaluation board, this allows to get started with a brand new micro within few hours.

Each driver consists of a set of functions covering all peripheral functionalities. The development of each driver is driven by a common API (application programming interface) which standardizes the driver structure, the functions and the parameter names. The driver source code is developed in ‘Strict ANSI-C’ (relaxed ANSI-C for projects and example files). It is fully documented and is MISRA-C 2004 compliant. Writing the whole library in ‘Strict ANSI-C’ makes it independent from the software toolchain. Only the start-up files depend on the toolchain.

All SPL packages can be downloaded from https://www.st.com/en/embedded-software/stm32-standard-peripheral-libraries.html. includes:

```
STSW-STM32048 	STM32F0xx standard peripherals library
STSW-STM32054 	STM32F10x standard peripheral library 
STSW-STM32115 	STM32F37x/F38x DSP and standard peripherals library
STSW-STM32062 	STM32F2xx standard peripherals library (UM1061) 
STSW-STM32065 	STM32F4 DSP and standard peripherals library
STSW-STM32077   STM32L1xx standard peripherals library
```

## stm32-hal

[stm32-hal](https://github.com/David-OConnor/stm32-hal) rust library provides high-level access to STM32 peripherals.

- Provide high-level access to most STM32 peripherals
- Support these STM32 families: F3, F4, L4, L5, G, H, U, and W
- Allow switching MCUs with minimal code change
- Provide a consistent API across peripheral modules
- Support both DMA and non-DMA interfaces
- Be suitable for commercial projects
- Provide a clear, concise API
- Provide source code readable by anyone cross-checking a reference manual (RM)

Before using this library, you need have rust toolchain and some utilities installed. please refer to above Compiler section for toolchain and desire target installation. besides the toolchain, you also need to install `flip-link` and `probe-run` to use stm32-hal:

```
cargo install flip-link
cargo install probe-run
```

Please make sure the `~/.cargo/bin` is in your PATH env.

After everything ready, clone this repo and find the `rust-stm32-hal-stm32h7` example, this example is modified from [stm32-hal quickstart repo](https://github.com/David-OConnor/stm32-hal-quickstart) to blink LED of [WeAct MiniH7XX board](https://github.com/WeActStudio/MiniSTM32H7xx). 

You may need:
- modify the below line of `Cargo.toml` to match your MCU:
```
stm32-hal2 = { version = "^1.5.0", features = ["l4x3", "l4rt"]}
```
For stm32h743vit6:
```
stm32-hal2 = { version = "^1.5.5", features = ["h743v", "h7rt"]}
```

Please refer to [`Cargo.toml`](https://github.com/David-OConnor/stm32-hal/blob/main/Cargo.toml) of stm32-hal project to learn how to specify features for your MCUs.

- modify the `runner` and `target` line of `.cargo/config.toml`
For stm32h743vit6, it should changed from:
```
runner = "probe-run --chip STM32L443CCTx"
```
to:
```
runner = "probe-run --chip STM32H743VITx"
```

and set `target` to:
```
target = "thumbv7em-none-eabihf"
```
- change `memory.x`(the linker script for rust) according to your MCU.
If you really do NOT know how to change this `memory.x` file, you can refer to a corresponding linker script of SPL or libopencm3.

- Then build the example:
```
cargo build --release
```
or run it on target device directly:
```
cargo run --release
```

The target elf file will be generated as `target/thumbv7em-none-eabihf/release/stm32h743vit6`, it can be programmed to target device later.

## stm32-rs
[stm32-rs](https://github.com/stm32-rs) is a series of community Rust support projects for STM32 microcontrollers. there are different crate for different series of STM32:
- [stm32g0xx-hal](https://github.com/stm32-rs/stm32g0xx-hal) for STM32G0 family.
- [stm32c0xx-hal](https://github.com/stm32-rs/stm32c0xx-hal) for STM32C0 family.
- [stm32l0xx-hal](https://github.com/stm32-rs/stm32l0xx-hal) for STM32L0 family.
- [stm32l1xx-hal](https://github.com/stm32-rs/stm32l1xx-hal) for STM32L1 family.
- [stm32f0xx-hal](https://github.com/stm32-rs/stm32f0xx-hal) for STM32F0 family.
- [stm32f1xx-hal](https://github.com/stm32-rs/stm32f1xx-hal) for STM32F1 family.
- [stm32f3xx-hal](https://github.com/stm32-rs/stm32f3xx-hal) for STM32F3 family.
- [stm32f4xx-hal](https://github.com/stm32-rs/stm32f4xx-hal) for STM32F4 family.
- [stm32f7xx-hal](https://github.com/stm32-rs/stm32f7xx-hal) for STM32F7 family.
- [stm32g4xx-hal](https://github.com/stm32-rs/stm32g4xx-hal) for STM32G4 family (work in progress).
- [stm32h7xx-hal](https://github.com/stm32-rs/stm32h7xx-hal) for STM32H7 family.
- [stm32wlxx-hal](https://github.com/stm32-rs/stm32wlxx-hal) for STM32WL family.

Compare to [stm32-hal](https://github.com/David-OConnor/stm32-hal), although these crates impl traits defined in embedded-hal project, the impl detail and other API beyond embedded-hal differs a lot.

Here still use WeAct MiniH7xx board (stm32h743vit6) as example:

```
git clone https://github.com/stm32-rs/stm32h7xx-hal
```

The LED control port is PE3, edit `examples/blinky.rs`, change from:
```
let mut led = gpioe.pe1.into_push_pull_output();
```
to:
```
let mut led = gpioe.pe3.into_push_pull_output();
```
And build it as:
```
cargo build --features=stm32h743v,rt --example blinky --release
```
the target elf file will be generated at `./target/thumbv7em-none-eabihf/release/examples/blinky`, it can be used to program to target device later.





