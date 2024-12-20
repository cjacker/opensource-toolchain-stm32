# Opensource toolchain for ARM Cortex-M (For example, STM32)

The ARM Cortex-M is a group of 32-bit RISC ARM processor cores licensed by Arm Holdings. These cores are optimized for low-cost and energy-efficient integrated circuits, which have been embedded in tens of billions of consumer devices. Though they are most often the main component of microcontroller chips, sometimes they are embedded inside other types of chips too. The Cortex-M family consists of Cortex-M0, Cortex-M0+, Cortex-M1, Cortex-M3, Cortex-M4, Cortex-M7, Cortex-M23, Cortex-M33, Cortex-M35P, Cortex-M55. The Cortex-M4 / M7 / M33 / M35P / M55 cores have an FPU silicon option, and when included in the silicon these cores are sometimes known as "Cortex-Mx with FPU" or "Cortex-MxF", where 'x' is the core variant. 

Arm Holdings neither manufactures nor sells CPU devices based on its own designs, but rather licenses the processor architecture to interested parties. Arm offers a variety of licensing terms, varying in cost and deliverables. To all licensees, Arm provides an integratable hardware description of the ARM core, as well as complete software development toolset and the right to sell manufactured silicon containing the ARM CPU. 

STM32 is a family of 32-bit ARM Cortex-M based microcontroller integrated circuits by STMicroelectronics. The STM32 chips are grouped into related series that are based around the same 32-bit ARM processor core, such as the Cortex-M33F, Cortex-M7F, Cortex-M4F, Cortex-M3, Cortex-M0+, or Cortex-M0. Internally, each microcontroller consists of the processor core, static RAM, flash memory, debugging interface, and various peripherals.

The STM32 family consists of 17 series of microcontrollers: H7, F7, F4, F3, F2, F1, F0, G4, G0, L5, L4, L4+ L1, L0, U5, WL, WB. and each series includes a lot of models with minor differences.

For more information about STM32 family, please refer to https://en.wikipedia.org/wiki/STM32.

There are also a lot of STM32 clones, such as GD32 / CH32 / AT32 / MM32 etc. Most of them try to keep compatible with STM32, but they may also have their own SDKs / firmware libraries. The toolchains and utilities described in this tutorial could be used with such parts from different vendors.

# Table of contents
  + [Hardware prerequist](#hardware-prerequist)
  + [Toolchain overview](#toolchain-overview)
  + [Compiler](#compiler)
    - [GCC](#gcc)
      + [from XPack](#from-xpack)
      + [from ARM](#from-arm)
    - [Rust](#rust)
  + [SDKs](#sdks)
    - [Bare metal programming](#bare-metal-programming)
    - [Official Firmware library](#official-firmware-library)
    - [STM32 Cube/HAL](#stm32-cubehal)
    - [libopencm3](#libopencm3)
    - [Rust stm32-hal](#stm32-hal)
    - [Rust stm32-rs](#stm32-rs)
    - [nRF5 SDK](#nrf5-sdk)
  + [Programming](#programming)
    - [ISP](#isp)
      + [to activate ISP mode](#to-activate-isp-mode)
      + [ISP programming](#isp-programming)
        - [UART](#uart-isp)
        - [USB-DFU](#usb-dfu)
    - [ST-Link](#st-link)
    - [DAPLink](#daplink)
      + [OpenOCD](#openocd)
      + [pyOCD](#pyocd)
    - [JLink](#jlink)
    - [Special Case 1: Synwit SWM](#special-case-1-synwit-swm)
    - [Special Case 2: Luat Core-AIR105 devboard](#special-case-2-luat-core-air105-devboard)
  + [Debugging](#debugging)
    - [OpenOCD](#openocd-1)
    - [pyOCD](#pyocd-1)
    - [Debugging with gdb](#debugging-with-gdb)
  + [Project templates](#project-templates)
  
# Hardware prerequist

* A development board with STM32 MCU. 
  - In this tutorial, I use below devboards. all resources such as firmware libraries will be provided:
    +  STM32G0 / F1 / F4 / H7
    +  GD32F1 / F3 / F4
    +  CH32F1
    +  AT32F403A
    +  APM32F1
    +  HC32L110
    +  MM32F3270
    +  Luat AIR105 (MH1903S)
    +  SynWit SWM181[CBT6]
 
* DAPLink / ST-LINK / JLink adapter for programming and debugging.
  - DAPLink is highly recommended. it's a cheap, opensource and standard solution to program/debug any Cortex-M MCU.
  - If you also need work with STM8, it's neccesary to have a ST-Link (it also support SWIM interface used by STM8). If not, buy a DAPLink.
  - JLink can support SWD interface, but it's too expensive and not worth to buy for beginners.

* USB2TTL UART adapter
  - for ISP programming of stm32f103, it doesn't support USB-DFU.

# Toolchain overview
* Compiler: GCC / Rust
* SDKs: Various
  - official SPL in C
  - STM32 Cube/HAL in C
  - libopencm3 in C
  - stm32-hal in rust
  - stm32-rs in rust
* Programming tool: 
  - stm32flash / dfu-util for ISP programming
  - OpenOCD / pyOCD for DAP programming and debugging
* Debugging: OpenOCD and pyOCD / gdb


# Compiler

## GCC

STM32 and various xx32 use 'arm-none-eabi' GCC toolchain, since they are all based on ARM Cortex-M. it's not neccesary to build the toolchain yourself, there are already a lot of well supported prebuilt release and already widely used by developers. 

### from XPack

[xpack-dev-tools](https://github.com/xpack-dev-tools) provde a prebuilt 'arm-none-eabi' toolchain. you can download it from [here](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack). 

After download:
```
sudo mkdir -p /opt/xpack-arm-none-eabi
sudo tar xf xpack-arm-none-eabi-gcc-13.3.1-1.1-linux-x64.tar.gz -C /opt/xpack-arm-none-eabi --strip-components=1
```
and add /opt/xpack-arm-none-eabi/bin to PATH env.

NOTE, the triplet of xpack prebuilt toolchain is 'arm-none-eabi'.

### from ARM

You can download the prebuilt toolchain for various host from [arm website](https://developer.arm.com/downloads/-/gnu-rm). 

**Note:** There is prebuilt arm-gcc v12.2 and can be downloaded from [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads), but currently the gdb debugger broken due to python issue, do NOT use the latest 12.2 version until this gdb issue fixed.

Download and extract the toolchain for x86_64 linux:

```
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
sudo tar xf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /opt
```

And add `/opt/gcc-arm-none-eabi-10.3-2021.10/bin` to PATH env of your shell.

**NOTE:** the toolchain's tripplet is 'arm-none-eabi'.

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

## Bare Metal Programming

For simple tasks such as blink a led, you can write bare metal codes without using any libraries, usually a bare metal project of ARM Cortex-M consists of:

- **A Linker script** to define the memory layout.
- **A startup file** typically written in assembly which:
  + initialize the stack pointer
  + initialize the non-zero read/write-data in RAM
  + initialize the zero read/write-data in RAM
  + define the interrupt vector table
  + jump to the main function
- **A main function** to do the task.

These components are also the essiential minimum components to work with GNU Toolchain.

There are some baremetal demos in this repo for stm32f1/f4/h7 (what I have when written this tutorial), you can take these demos as reference.

## Official Firmware library

Cortex-M MCU vendors usually provide firmware library for each part or part family. ST STM32 call it as 'SPL' (standard peripherals library), WCH CH32F ship it as 'EVT' and GigaDevice GD32F call it as 'firmware library', etc. They all have the similar project structure and file organization, and all are very similar to original STM32 'SPL'.

**Note:** stm32 'SPL' was deprecated serveral years ago by stm32cube. it's recommend to use Cube/HAL with stm32 instead of 'SPL'.

Usually, a firmware library contains:
- A complete register address mapping with all bits, bit fields and registers declared in C.
- A collection of routines and data structures which covers all peripheral functions
- A set of examples covering all available peripherals with template projects for the most common development toolchains

All stm32 'SPL' packages can be downloaded from [here](https://www.st.com/en/embedded-software/stm32-standard-peripheral-libraries.html), you may need register an account and login first before download, Available SPLs includes:

```
STSW-STM32048 	STM32F0xx standard peripherals library
STSW-STM32054 	STM32F10x standard peripheral library 
STSW-STM32115 	STM32F37x/F38x DSP and standard peripherals library
STSW-STM32062 	STM32F2xx standard peripherals library (UM1061) 
STSW-STM32065 	STM32F4 DSP and standard peripherals library
STSW-STM32077   STM32L1xx standard peripherals library
```

You could already notice there is no SPL for such as stm32 G4 or H7, since SPL was deprecated, you should use Cube/HAL for such models.

The problem of these SPLs is all of them lack 'Makefile' support and maybe also lack gcc support, most of them are designed for Keil MDK or other commercial IDE. but you may found some forked repo which port to GCC and have a Makefile.

A lot of firmware libraries of STM32 clones (such as CH32F / GD32F / AT32, etc) also have this issue : lack of Makefile support, even gcc support. Their firmware libraries usually can be downloaded from their official website, For AT32F, ArteryTek officially maintain all its' firmware libraries at : https://github.com/ArteryTek, you can clone them if you work with AT32F.

I make '[ch32f evt convertor](https://github.com/cjacker/ch32f_evt_makefile_gcc_project_template)' and '[gd32 fwlib convertor](https://github.com/cjacker/gd32_fwlib_convertor)' to help developers convert CH32F and GD32F official firmware library package, make it support GCC and Makefile. All firmware library from CH32 and GD32 had been tested, and here are some pre-converted firmware libraries:

- [CH32F103EVT](https://github.com/cjacker/ch32f103evt_gcc_makefile)
- [CH32F20xEVT](https://github.com/cjacker/ch32f20xevt_gcc_makefile)
- [GD32F10x firmware library](https://github.com/cjacker/gd32f10x_firmware_library_gcc_makefile)
- [GD32F30x firmware library](https://github.com/cjacker/gd32f30x_firmware_library_gcc_makefile)
- [GD32F4xx firmware library](https://github.com/cjacker/gd32f4xx_firmware_library_gcc_makefile)

For more CH32F and GD32F firmware libraries, you can use the convert tools to convert it your self.

If you want to convert other XX32 firmware library, you may need write a linker script and a startup asm file for it. The startup file can be converted from 'ARM' startup file shipped in vendor's package with [startupfile_generator.py](./startupfile_generator.py), this tool is taken and modified from 'platform-gd32'. and there is also a [Linker script template](./ldscript.template.ld) provided, you can modify it according to your MCU, the most important job is to set FLASH SIZE and RAM SIZE.

Use [GD32F4xx firmware library](https://github.com/cjacker/gd32f4xx_firmware_library_gcc_makefile) as example, the default part set to gd32f470zgt6 and the default 'User' codes is to blink four LEDs on LiangShan Pi dev board from JLC.

```
git clone https://github.com/cjacker/gd32f4xx_firmware_library_gcc_makefile
make
```

The target files 'gd32f470zgt6.elf / bin / hex' will be generated in `build` dir. 

## STM32 Cube/HAL

STM32CubeMX is a graphical tool that allows a very easy configuration of STM32 microcontrollers and microprocessors, as well as the generation of the corresponding initialization C code. 

STM32CubeMX can support generate 'Makefile' project for Linux and arm-gcc toolchain. It's very easy to use, just follow the wizard, select your mcu model, tune some configurations, and choose 'Makefile' project, a skeleton of a new project will be generated and can use `make` to build directly.

You may encounter some warnings such as: 

```
warning: _write is not implemented and will always fail
warning: _read is not implemented and will always fail
```

And notice that uart printf by implementing and redirecting 'fputc' doesn't work anymore.

This issue is gcc toolchain specifc, instead of `fputc` and `fgetc`, you should implement `_write` and `_read` for gcc toolchain, etc. 

An easy way to solve this issue is find `syscalls.c` from https://github.com/STMicroelectronics/ for your parts, it defined `_write` and `_read` as:

```
__attribute__((weak)) int _read(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    *ptr++ = __io_getchar();
  }
return len;
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}
```

What you need to do is implement `__io_putchar` and `__io_getchar`, for example:

```
// these codes works with MDK and GCC.
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE* f)
#endif


PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
  return (ch);
}

GETCHAR_PROTOTYPE
{
  HAL_StatusTypeDef status = HAL_BUSY;
  uint8_t data;
  while(status != HAL_OK)
    status = HAL_UART_Receive(&huart1, &data, 1, HAL_MAX_DELAY);
  return (data);
}
```

## libopencm3

The libopencm3 project aims to create an open-source firmware library for various ARM Cortex-M microcontrollers. It not only support STM32 but also support a lot of MCU models from other vendors.

Currently (at least partly) supported microcontrollers:

- ST STM32 F0xx/F1xx/F2xx/F30x/F37x/F4xx/F7xx/H7xx series
- ST STM32 G0xx G4xx L0xx L1xx L4xx series
- Atmel SAM3A/3N/3S/3U/3X series, as well as SAMDxx and friends
- NXP LPC1311/13/17/42/43
- Stellaris LM3S series (discontinued, without replacement)
- TI (Tiva) LM4F series (continuing as TM4F, pin and peripheral compatible)
- EFM32 Gecko series (only core support)
- Freescale Vybrid VF6xx
- Qorvo (formerly ActiveSemi) PAC55XX
- Synwit SWM050
- Nordic NRF51x and NRF52x


The library is written completely from scratch based on the vendor datasheets, programming manuals, and application notes. The code is meant to be used with a GCC toolchain for ARM (arm-elf or arm-none-eabi), flashing of the code to a microcontroller can be done using the OpenOCD ARM JTAG software.

Here use WeAct MiniH7xx board (stm32h743vit6) as example, a LED is controled by PE3.

```
git clone https://github.com/libopencm3/libopencm3-miniblink.git
cd libopencm3-miniblink
# checkout the libopencm3 submodule
git submodule update --init --progress
```

Modify `boards.stm32.mk`, add a line:
```
$(eval $(call stm32h7board,weact-studio-minih7xx,GPIOE,GPIO3))
```

Then build it as:
```
make
```

The target elf file will be generated at `bin/stm32/weact-studio-minih7xx.elf`, it can be programmed to target device later.

You may also found there are some other board files live in the top dir of libopencm3-miniblink, as mentioned above, libopencm3 can support a lot of other MCU models not limited to STM32, acctually, this library can also be used with EFM32 from silicon labs, NRF52 from Nordic, etc.

libopencm3-miniblink is only a demo project to demo how to use this library, it's a good start to learn how to build a project with libopencm3.


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

Before using this library, you need have rust toolchain and some utilities installed. please refer to above Compiler section for toolchain and desire target installation. besides the toolchain, you also need to install `flip-link` and `probe-rs` to use stm32-hal:

```
cargo install flip-link
cargo install probe-rs --features cli
```

Please make sure the `~/.cargo/bin` is in your PATH env.

After everything ready, clone this repo and find the `rust-stm32-hal-stm32h7` example, this example is modified from [stm32-hal quickstart repo](https://github.com/David-OConnor/stm32-hal-quickstart1) to blink LED of [WeAct MiniH7XX board](https://github.com/WeActStudio/MiniSTM32H7xx). 

You may need:
- modify the below line of `Cargo.toml` to match your MCU:
```
hal = { package = "stm32-hal2", version = "^1.6.4", features = ["l4x3", "l4rt"]}
```
For stm32h743vit6(WeAct MiniH7XX board):
```
hal = { package = "stm32-hal2", version = "^1.6.4", features = ["h743v", "h7rt"]}
```

Please refer to [`Cargo.toml`](https://github.com/David-OConnor/stm32-hal/blob/main/Cargo.toml) of stm32-hal project to learn how to specify features for your MCUs.

- modify the `runner` and `target` line of `.cargo/config.toml`
For stm32h743vit6, it should changed from:
```
runner = "probe-rs run --chip STM32H723ZGTx" # to list chips, run `probe-rs chip list.`
```
to:
```
runner = "probe-rs run --chip STM32H743VITx"
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

or run it on target device directly if it connect to host via CMSIS-DAP:
```
cargo run --release
```

The running result looks like:
```
    Finished `release` profile [optimized + debuginfo] target(s) in 0.05s
     Running `probe-rs run --chip STM32H743VITx target/thumbv7em-none-eabihf/release/rust-stm32-hal-stm32h7`
      Erasing ✔ [00:00:01] [###############################] 128.00 KiB/128.00 KiB @ 64.85 KiB/s (eta 0s )
  Programming ✔ [00:00:02] [##################################] 16.00 KiB/16.00 KiB @ 7.93 KiB/s (eta 0s )    Finished in 4.147s
<lvl> Hello, world!
└─ rust_stm32_hal_stm32h7::__cortex_m_rt_main @ src/main.rs:31
```

If `cargo run --release` report some errors, please run `probe-rs list` to verify the target device:
```
The following debug probes were found:
[0]: DAPLink CMSIS-DAP -- 0d28:0204:070000814a46bc321431313030313736a5a5a5a597969908 (CMSIS-DAP)
```

The target elf file will be generated as `target/thumbv7em-none-eabihf/release/rust-stm32-hal-stm32h7`, it can be programmed to target device later (for example, use OpenOCD).

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

## nRF5 SDK
This section is for Nordic nRF5 only.

Nordic nRF5 series MCUs are based on ARM cortex-m, it's no doubt it support SWD protocol, and you can use toolchains, debuggers and utils suite for cortex-m with nRF5.

Like different firmware libraries for various cortex-m MCU, nRF5 has it's own SDK, you can download it from https://developer.nordicsemi.com/nRF5_SDK/. the latest version is 'nRF5_SDK_v17.x.x' when this tutorial written.

```
wget https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v17.x.x/nRF5_SDK_17.1.0_ddde560.zip
mkdir -p $HOME/nrf
unzip nRF5_SDK_17.1.0_ddde560.zip -d $HOME/nrf
```

After SDK extracted, since the 'gcc-none-eabi' GNU toolchain already added to PATH env, and it can be used system wide, you need edit `components/toolchain/gcc/Makefile.posix`, change from
```
GNU_INSTALL_ROOT ?= /usr/local/gcc-arm-none-eabi-9-2020-q2-update/bin/
```
to 
```
GNU_INSTALL_ROOT ?= 
```

Then use `blink-nrf5` demo in this repo
```
cd blink-nrf5
make SDK_ROOT=$HOME/mcu/nRF5_SDK_17.1.0_ddde560
```

If you didn't chane `components/toolchain/gcc/Makefile.posix`, you may encounter an error as :
```
Cannot find: '/usr/local/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gcc'.
Please set values in: "/home/cjacker/mcu/nRF5_SDK_17.1.0_ddde560/components/toolchain/gcc/Makefile.posix"
according to the actual configuration of your system.
```

After built successfully, `nrf52832_xxaa.hex` and `nrf52832_xxaa.bin` will be generated at `_build` dir, it can be programmed by OpenOCD as
```
openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/nrf52.cfg -c "program _build/nrf52832_xxaa.bin verify reset exit"

```
Please refer to next section about how to install and use OpenOCD.

If you have a JLink debugger, you can also use JLink and `nrfjprog` to program and debug, the `nrfjprog` can be downloaded from https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools/download.


# Programming

There are various ways to program a Cortex-M part, all Cortex-M parts should support SWD program and debug interface.

- UART or USB ISP if it had a factory bootloader, please check datasheet.
- ST-Link if it's a STM32 part.
- CMSIS-DAP / DAP-Link
  + OpenOCD if your part already supported by OpenOCD
  + pyOCD if vendor provide DFP Pack
- JLink if vendor provide JFlash support

The advantage of pyOCD is : it can use the flash algo pack for Keil MDK directly.

## ISP

Almost every STM32 MCU has a bootloader, the bootloader is stored in the internal boot ROM (system memory) of STM32 devices, and is programmed by ST during production. Its main task is to download the application program to the internal Flash memory through one of the available serial peripherals (such as USART, CAN, USB, I2C, SPI).

you can refer to [AN2606](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) to find out more information about the bootloader and parts list.

**Note:** Not all parts support USB DFU modes, for example, The embedded bootloader of STM32F103 does not provide DFU functionality as you can find in [AN2606](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf), table 3, it support UART ISP mode.

### to activate ISP mode

Usually, there is always a 'BOOT0' or/and 'BOOT1' buttons or jumpers on stm32 dev board. 

If the part support USB-DFU, to activate the bootloader (aka ISP mode), you need hold the 'BOOT0' button down and plug in USB port. if the target dev board already plugged in, you can hold the 'BOOT0' button down, press and release 'RESET' button, then release 'BOOT0' button.

If the part do not support USB-DFU such as STM32F103. you need to close the 'BOOT0' jumper to 3.3v and connect a USB2TTL adapter as:
```
USB2TTL : STM32F103
3.3v   ->  3.3v
TX     ->  RX(PA10)
RX     ->  TX(PA9)
GND    ->  GND
``` 

### ISP programming

#### UART ISP

For stm32f103, the bootloader only support UART ISP, you need to use [stm32flash](https://sourceforge.net/p/stm32flash/wiki/Home/) to program the part.

Download, build and install it:

```
wget "https://sourceforge.net/projects/stm32flash/files/stm32flash-0.7.tar.gz/download" -O stm32flash-0.7.tar.gz
tar xf stm32flash-0.7.tar.gz
cd stm32flash-0.7
./configure --prefix=/usr
make
sudo make install
```

After installed, run `stm32flash` directly for usage:
```
Usage: stm32flash [-bvngfhc] [-[rw] filename] [tty_device | i2c_device]
	-a bus_address	Bus address (e.g. for I2C port)
	-b rate		Baud rate (default 57600)
	-m mode		Serial port mode (default 8e1)
	-r filename	Read flash to file (or - stdout)
	-w filename	Write flash from file (or - stdout)
	-C		Compute CRC of flash content
	-u		Disable the flash write-protection
	-j		Enable the flash read-protection
	-k		Disable the flash read-protection
	-o		Erase only
	-e n		Only erase n pages before writing the flash
	-v		Verify writes
	-n count	Retry failed writes up to count times (default 10)
	-g address	Start execution at specified address (0 = flash start)
	-S address[:length]	Specify start address and optionally length for
	                   	read/write/erase operations
	-F RX_length[:TX_length]  Specify the max length of RX and TX frame
	-s start_page	Flash at specified page (0 = flash start)
	-f		Force binary parser
	-h		Show this help
	-c		Resume the connection (don't send initial INIT)
			*Baud rate must be kept the same as the first init*
			This is useful if the reset fails
	-R		Reset device at exit.
	-i GPIO_string	GPIO sequence to enter/exit bootloader mode
			GPIO_string=[entry_seq][:[exit_seq]]
			sequence=[[-]signal]&|,[sequence]

GPIO sequence:
	The following signals can appear in a sequence:
	  Integer number representing GPIO pin
	  'dtr', 'rts' or 'brk' representing serial port signal
	The sequence can use the following delimiters:
	  ',' adds 100 ms delay between signals
	  '&' adds no delay between signals
	The following modifiers can be prepended to a signal:
	  '-' reset signal (low) instead of setting it (high)

Examples:
	Get device information:
		stm32flash /dev/ttyS0
	  or:
		stm32flash /dev/i2c-0

	Write with verify and then start execution:
		stm32flash -w filename -v -g 0x0 /dev/ttyS0

	Read flash to file:
		stm32flash -r filename /dev/ttyS0

	Read 100 bytes of flash from 0x1000 to stdout:
		stm32flash -r - -S 0x1000:100 /dev/ttyS0

	Start execution:
		stm32flash -g 0x0 /dev/ttyS0

	GPIO sequence:
	- entry sequence: GPIO_3=low, GPIO_2=low, 100ms delay, GPIO_2=high
	- exit sequence: GPIO_3=high, GPIO_2=low, 300ms delay, GPIO_2=high
		stm32flash -i '-3&-2,2:3&-2,,,2' /dev/ttyS0
	GPIO sequence adding delay after port opening:
	- entry sequence: delay 500ms
	- exit sequence: rts=high, dtr=low, 300ms delay, GPIO_2=high
		stm32flash -R -i ',,,,,:rts&-dtr,,,2' /dev/ttyS0
```

Refer to above section to find out how to activate the stm32f103 ISP MODE and how to connect to a USB2TTL UART adapter. then plug the adatper into USB port, and run:

```
stm32flash /dev/ttyUSB0
```

The output looks like:
```
Interface serial_posix: 57600 8E1
Version      : 0x22
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0410 (STM32F10xxx Medium-density)
- RAM        : Up to 20KiB  (512b reserved by bootloader)
- Flash      : Up to 128KiB (size first sector: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB
```

Using the 'baremetal-stm32f1' example in this repo, build the demo and run:

```
sudo stm32flash -w app.bin -v -g 0x0 /dev/ttyACM0
```

The output looks like:

```
Using Parser : Raw BINARY
Size         : 140
Interface serial_posix: 57600 8E1
Version      : 0x22
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0410 (STM32F10xxx Medium-density)
- RAM        : Up to 20KiB  (512b reserved by bootloader)
- Flash      : Up to 128KiB (size first sector: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB
Write to memory
Erasing memory
Wrote and verified address 0x0800008c (100.00%) Done.

Starting execution at address 0x08000000... done.
```

After command excuted successfully, the LED controled by PC13 will blink.

**NOTE 1:** If use `-g 0x0` to start excution, the target device will exit ISP mode, you need to replug it to activate the ISP mode again.

**NOTE 2:** Do NOT forget to restore the jumper to start the device from flash.


#### USB-DFU

Use a stm32f411ceu6 [board](https://github.com/WeActStudio/WeActStudio.MiniSTM32F4x1) as example, after ISP mode activated, run `lsusb`, the output looks like:

```
Bus 001 Device 040: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
```

You need use [dfu-util](https://dfu-util.sourceforge.net/) to program, download and install it as:
```
wget https://dfu-util.sourceforge.net/releases/dfu-util-0.11.tar.gz
tar xf dfu-util-0.11.tar.gz
cd dfu-util-0.11
./configure --prefix=/usr
make 
sudo make install
```

Run `dfu-util -h` to print the usage:
```
Usage: dfu-util [options] ...
  -h --help                     Print this help message
  -V --version                  Print the version number
  -v --verbose                  Print verbose debug statements
  -l --list                     List currently attached DFU capable devices
  -e --detach                   Detach currently attached DFU capable devices
  -E --detach-delay seconds     Time to wait before reopening a device after detach
  -d --device <vendor>:<product>[,<vendor_dfu>:<product_dfu>]
                                Specify Vendor/Product ID(s) of DFU device
  -n --devnum <dnum>            Match given device number (devnum from --list)
  -p --path <bus-port. ... .port>       Specify path to DFU device
  -c --cfg <config_nr>          Specify the Configuration of DFU device
  -i --intf <intf_nr>           Specify the DFU Interface number
  -S --serial <serial_string>[,<serial_string_dfu>]
                                Specify Serial String of DFU device
  -a --alt <alt>                Specify the Altsetting of the DFU Interface
                                by name or by number
  -t --transfer-size <size>     Specify the number of bytes per USB Transfer
  -U --upload <file>            Read firmware from device into <file>
  -Z --upload-size <bytes>      Specify the expected upload size in bytes
  -D --download <file>          Write firmware from <file> into device
  -R --reset                    Issue USB Reset signalling once we're finished
  -w --wait                     Wait for device to appear
  -s --dfuse-address address<:...>      ST DfuSe mode string, specifying target
                                address for raw file download or upload (not
                                applicable for DfuSe file (.dfu) downloads).
                                Add more DfuSe options separated with ':'
                leave           Leave DFU mode (jump to application)
                mass-erase      Erase the whole device (requires "force")
                unprotect       Erase read protected device (requires "force")
                will-reset      Expect device to reset (e.g. option bytes write)
                force           You really know what you are doing!
                <length>        Length of firmware to upload from device
```

Run `dfu-util -l` to found the target device:
```
dfu-util 0.11

Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
Copyright 2010-2021 Tormod Volden and Stefan Schmidt
This program is Free Software and has ABSOLUTELY NO WARRANTY
Please report bugs to http://sourceforge.net/p/dfu-util/tickets/

Found DFU: [0483:df11] ver=2200, devnum=51, cfg=1, intf=0, path="1-3", alt=3, name="@Device Feature/0xFFFF0000/01*004 e", serial="337238533430"
Found DFU: [0483:df11] ver=2200, devnum=51, cfg=1, intf=0, path="1-3", alt=2, name="@OTP Memory /0x1FFF7800/01*512 e,01*016 e", serial="337238533430"
Found DFU: [0483:df11] ver=2200, devnum=51, cfg=1, intf=0, path="1-3", alt=1, name="@Option Bytes  /0x1FFFC000/01*016 e", serial="337238533430"
Found DFU: [0483:df11] ver=2200, devnum=51, cfg=1, intf=0, path="1-3", alt=0, name="@Internal Flash  /0x08000000/04*016Kg,01*064Kg,03*128Kg", serial="337238533430"
```

Then build the 'baremetal-stm32f4' demo in this repo and program it:

```
sudo dfu-util -a 0 -s 0x8000000 -RD app.bin
```


## ST-LINK

You may noticed the ISP mode is not very convenient for daily use already. 

The ST-LINK is the official in-circuit debugger and programmer for the STM8 and STM32 MCU. It use a 2-wire interface named 'SWD' to connect with STM32 target.

```
3.3v  -> 3.3v
SWCLK -> SWCLK
SWDIO -> SWDIO
GND   -> GND
```

You need install the [stlink](https://github.com/stlink-org/stlink) tool to communicate with ST-LINK adatper, download and install it as:

```
git clone https://github.com/stlink-org/stlink.git
cd stlink
mkdir build
cd build
cmake -DSTLINK_UDEV_RULES_DIR=/etc/udev/rules.d -DSTLINK_STATIC_LIB=OFF ..
make
sudo make install
```

After installation, connect ST-LINK with target dev board (here use STM32F411) and plug it to PC USB port.

Run `st-info --probe`, the output looks like:

```
Found 1 stlink programmers
  version:    V2J27S6
  serial:     55FF6B068683575549200767
  flash:      524288 (pagesize: 16384)
  sram:       131072
  chipid:     0x0431
  descr:      stm32f411re
```

Then build the 'baremetal-stm32f4' demo in this repo and program it:

```
st-flash write app.bin 0x8000000
```

You can also use OpenOCD / pyOCD with ST-LINK:

```
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program app.elf verify reset exit"
```

If you use 'app.bin' in OpenOCD command, the start addr must be append, it should be 'program app.bin 0x8000000'.


## DAPLink

DAPLink provides a standardized way to access the Coresight Debug Access Port (DAP) of an ARM Cortex microcontroller via USB. Usually it supports both SWD and serial ports. It can be supported very well by OpenOCD and pyOCD. the pins connection is as same as ST-Link.

```
3.3v  -> 3.3v
SWCLK -> SWCLK
SWDIO -> SWDIO
GND   -> GND
```

### OpenOCD

**OpenOCD installation:**

For this tutorial, since AT32F and HC32L110 flash driver is not support by upstream OpenOCD, We will build a patched OpenOCD:
- AT32F patch is from https://github.com/ArteryTek/openocd
- HC32L110 patch is from https://github.com/Spritetm/openocd-hc32l110/

```
https://github.com/openocd-org/openocd.git
cd openocd
git submodule update --init --recursive --progress
git checkout 8a3723022689dd078c3e61268615616d5566fc94
 
# patch for ArteryTek AT32F
wget https://raw.githubusercontent.com/cjacker/opensource-toolchain-stm32/main/openocd-0.12-dev-add-at32.patch
cat openocd-0.12-dev-add-at32.patch|patch -p1
# patch for HuaDa HC32L110
wget https://raw.githubusercontent.com/cjacker/opensource-toolchain-stm32/main/openocd-0.12-dev-add-hc32l110.patch
cat openocd-0.12-dev-add-hc32l110.patch|patch -p1

# configure and built it
./bootstrap
./configure --prefix=/opt/openocd \
  --disable-werror \
  --enable-static \
  --disable-shared \
  --enable-dummy \
  --enable-ftdi \
  --enable-stlink \
  --enable-ti-icdi \
  --enable-ulink \
  --enable-usb-blaster-2 \
  --enable-ft232r \
  --enable-vsllink \
  --enable-xds110 \
  --enable-cmsis-dap-v2 \
  --enable-osbdm \
  --enable-opendous \
  --enable-aice \
  --enable-usbprog \
  --enable-rlink \
  --enable-armjtagew \
  --enable-cmsis-dap \
  --enable-nulink \
  --enable-kitprog \
  --enable-usb-blaster \
  --enable-presto \
  --enable-openjtag \
  --enable-jlink \
  --enable-parport \
  --enable-jtag_vpi \
  --enable-jtag_dpi \
  --enable-ioutil \
  --enable-amtjtagaccel \
  --enable-ep39xx \
  --enable-at91rm9200 \
  --enable-gw16012 \
  --enable-oocd_trace \
  --enable-buspirate \
  --enable-sysfsgpio \
  --enable-linuxgpiod \
  --enable-xlnx-pcie-xvc \
  --enable-remote-bitbang \
  --disable-internal-jimtcl \
  --disable-doxygen-html \
  CROSS=

make -j4
sudo make install
```

After OpenOCD installed, please add `/opt/openocd/bin` to PATH env.


**OpenOCD usage:**

Use stm32f411 as example, build the 'baremetal-stm32f4' demo in this repo and program it:

```
openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program app.elf verify reset exit"
```

### pyOCD 

**pyOCD installation:**

pyOCD installation is rather simpler than OpenOCD:

```
python -m pip install pyocd
```
After pyocd installed, please add `$HOME/.local/bin` to PATH env to find `pyocd` command.

**pyOCD usage:**

After pyOCD installed, you will have the 'pyocd' and 'pyocd-gdbserver' command in your PATH.

To list target pyOCD can support:
```
pyocd list --targets
```

To erase target device:
```
pyocd erase -c -t <target>
```

To program target device:
```
pyocd load <target hex file>.hex -t <target>
```

You may noticed: if OpenOCD doesn't support specific target device, you have to patch OpenOCD to support it.

For pyOCD, it will be simpler. It can directly use flash algos included in CMSIS Device Family Packs (DFPs) as Keil MDK does. You can always find MDK packs for your part from part vendors.

If your part is not supported by pyOCD by default, follow bellow steps to use vendor's 'device pack'.
- Find corresponding 'device pack' file from part vendor and put it at the top dir of your project.
- create `pyocd.yaml` at the top dir of your project, the contents looks like:

```
pack:
  - <relative dir of the pack file>
```

Then you can use pyocd like:

- to list targets:

```
pyocd list --targets
```

- to erase and program the part:
```
pyocd erase -c -t <target> --config pyocd.yaml
pyocd load <target hex file>.hex -t <target> --config pyocd.yaml
```

## JLink
Since all JLink utilities is close sourced, the usage of JLink will not covered by this tutorial.

What I have to mentioned here is : If you really can not find a opensource way to program / debug your parts, you would be able to use JFlash support pack from vendor to program / debug your device by JFlash and JLinkGDBServer.


## Special Case 1: Synwit SWM

Up to now, Synwit SWM MCU based on Cortex-M can not be programed/debugged with OpenOCD or pyOCD (the official DFP Pack not works as expected), we have to use `SWMProg` or JLink to program it.

I made a fork to enable linux support for SWMProg here: https://github.com/cjacker/SWMProg

```
git clone https://github.com/cjacker/SWMProg
cd SWMProg
git checkout linux
python ./SWMProg.py
```

Besides SWMProg, you could also be able to use JLink with [upstream JFlash pack](https://github.com/Synwit-Co-Ltd/JFlash) to program and debug SWM32 MCU.

## Special Case 2: Luat Core-AIR105 devboard

AIR105 and MHS1903s is identical, it no doubt can support SWD, but Luat Core-AIR105 devboard did not export SWD interface, If you do not want to modify the hardware, you have to use [air105-uploader](https://github.com/racerxdl/air105-uploader) to program this devboard.

```
git clone https://github.com/racerxdl/air105-uploader
cd air105-uploader
python upload.py /dev/ttyUSB0 <air105 / mh1903 bin file>
```

# Debugging

Build the codes in debug mode and connect the ST-Link or DAPLink or JLink as mentioned above.

## OpenOCD

If the target device not supported by OpenOCD, please refer to next section to use 'pyOCD' with 'device pack' or JLink.

Launch OpenOCD as:
```
openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg
```

If you use ST-Link, change 'cmsis-dap.cfg' to 'st-link.cfg', Or


The output looks like:
```
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : Using CMSIS-DAPv2 interface with VID:PID=0x1a86:0x8012, serial=C7AA8F064C85
Info : CMSIS-DAP: SWD supported
Info : CMSIS-DAP: FW Version = 2.0.0
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 0 TDO = 0 nTRST = 0 nRESET = 0
Info : CMSIS-DAP: Interface ready
Info : clock speed 2000 kHz
Info : SWD DPIDR 0x2ba01477
Info : [stm32f4x.cpu] Cortex-M4 r0p1 processor detected
Info : [stm32f4x.cpu] target has 6 breakpoints, 4 watchpoints
Info : starting gdb server for stm32f4x.cpu on 3333
Info : Listening on port 3333 for gdb connections
Info : accepting 'gdb' connection on tcp/3333
```

## pyOCD

Launch pyOCD as:
```
pyocd gdbserver -t <target> --config pyocd.xml
```

The output looks like:
```
0000454 W Board ID lckf is not recognized [mbed_board]
0000460 I Target type is hc32l110 [board]
0000501 I DP IDR = 0x2ba01477 (v1 rev2) [dap]
0000513 I AHB-AP#0 IDR = 0x24770011 (AHB-AP var1 rev2) [ap]
0000530 I AHB-AP#0 Class 0x1 ROM table #0 @ 0xe00ff000 (designer=751 part=c9e) [rom_table]
0000535 I [0]<e000e000:SCS v7-M class=14 designer=43b:Arm part=00c> [rom_table]
0000538 I [1]<e0001000:DWT v7-M class=14 designer=43b:Arm part=002> [rom_table]
0000541 I [2]<e0002000:FPB v7-M class=14 designer=43b:Arm part=003> [rom_table]
0000544 I [3]<e0000000:ITM v7-M class=14 designer=43b:Arm part=001> [rom_table]
0000549 I [4]<e0040000:TPIU M4 class=9 designer=43b:Arm part=9a1 devtype=11 archid=0000 devid=ca1:0:0> [rom_table]
0000553 W Invalid coresight component, cidr=0x0 [rom_table]
0000554 I [5]e0041000: cidr=0, pidr=0, component invalid> [rom_table]
0000558 I CPU core #0 is Cortex-M4 r0p1 [cortex_m]
0000565 I FPU present: FPv4-SP-D16-M [cortex_m]
0000574 I 4 hardware watchpoints [dwt]
0000578 I 6 hardware breakpoints, 4 literal comparators [fpb]
0000605 I Semihost server started on port 4444 (core 0) [server]
0000646 I GDB server started on port 3333 (core 0) [gdbserver]
```

## debugging with gdb
Start another terminal window, run:

```
arm-none-eabi-gdb ./app.elf
(gdb) target remote :3333
Remote debugging using :3333
0x0800006e in main () at main.c:63
63              for (int i = 0; i < 1000000; i++); // arbitrary delay
(gdb) load
Loading section .text, size 0x8c lma 0x8000000
Start address 0x08000000, load size 140
Transfer rate: 206 bytes/sec, 140 bytes/write.
(gdb) break main
Breakpoint 1 at 0x8000016: file main.c, line 26.
Note: automatically using hardware breakpoints for read-only addresses.
(gdb) continue
Continuing.

Breakpoint 1, main () at main.c:26
26          RCC_AHB1ENR |= 0x00000006;

```

# Project templates
Since there are a lot of different SDKs for STM32 and XX32, unified project templates is not exist, it depends on which programming language and library you use. 

Anyway, here list all libraries mentioned in this tutorial, you can take these examples/demo codes as reference to start your own project:

- [libopencm3-miniblink](https://github.com/libopencm3/libopencm3-miniblink) for libopencm3
- [stm32-hal-quickstart](https://github.com/David-OConnor/stm32-hal-quickstart) for rust stm32-hal.
- [various stm32-rs hal library](https://github.com/stm32-rs) for rust stm32-rs.
- [ch32f103evt](https://github.com/cjacker/ch32f103evt_gcc_makefile/) for CH32F103.
- [ch32f20xevt](https://github.com/cjacker/ch32f20xevt_gcc_makefile) for CH32F20x
- [gd32f10x firmware library](https://github.com/cjacker/gd32f10x_firmware_library_gcc_makefile) for GD32F10x and [WeAct GD32 Bluepill Plus](https://github.com/WeActStudio/WeActStudio.BluePill-Plus-GD32).
- [gd32f30x firmware library](https://github.com/cjacker/gd32f30x_firmware_library_gcc_makefile) for GD32F30x and [WeAct GD32 Bluepill Plus](https://github.com/WeActStudio/BluePill-Plus).
- [gd32f4xx firmware library](https://github.com/cjacker/gd32f4xx_firmware_library_gcc_makefile) for GD32F4xx and LiangShan Pi from JLC.
- [at32f403a_407 firmware library](https://github.com/cjacker/AT32F403A_407_Firmware_Library_gcc_makefile) for at32f403a / 407 and [WeAct BlackPill Board](https://github.com/WeActStudio/WeActStudio.BlackPill)
- [apm32f10x firmware library](https://github.com/cjacker/apm32f10x_firmware_library_gcc_makefile) for apm32f103cbt6 and [WeAct BluePill Board](https://github.com/WeActStudio/WeActStudio.BluePill-Plus-APM32)
- [hc32l110 firmware library](https://github.com/cjacker/hc32l110_firmware_library_gcc_makefile) for HuaDa Semiconductor HC32L110 series.
- [mm32f3270 firmware library](https://github.com/cjacker/mm32f3270_firmware_library_gcc_makefile) for Shanghai MindMotion mm32f3270 series.
- [air105 / mhs1903s firmware library](https://github.com/cjacker/mh1903_firmware_library_gcc_makefile) for Luat air105 / MegaHunt mh1903[s] 
- [swm181 firmware library](https://github.com/cjacker/swm181_firmware_library_gcc_makefile) for SynWit SWM181.
