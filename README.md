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

* Compiler: ARM GCC toolchain
* Debugger: OpenOCD/gdb
* SDKs
* Flashing tool: ISP and OpenOCD.


# ARM GCC toolchain

STM32 use the 'arm-none-eabi' GCC toolchain. it's not neccesary to build the toolchain yourself, since there are already a lot of well supported prebuilt release and already widely used by developers. 

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

# SDKs

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


