# Opensource toolchain for stm32

STM32 is a family of 32-bit microcontroller integrated circuits by STMicroelectronics. The STM32 chips are grouped into related series that are based around the same 32-bit ARM processor core, such as the Cortex-M33F, Cortex-M7F, Cortex-M4F, Cortex-M3, Cortex-M0+, or Cortex-M0. Internally, each microcontroller consists of the processor core, static RAM, flash memory, debugging interface, and various peripherals.

The STM32 family consists of 17 series of microcontrollers: H7, F7, F4, F3, F2, F1, F0, G4, G0, L5, L4, L4+ L1, L0, U5, WL, WB. and each series includes a lot of models with minor differences.

For more information about STM32 family, please refer to https://en.wikipedia.org/wiki/STM32.

There is also a lot of STM32 clone, such as GD32, CH32, etc, most of them are compatible with STM32. The toolchain described in this tutorial may also be used with such products.

# Hardware prerequisites

* A development board with STM32 MCU. for example, in this tutorial, I will use STM32F1/F4/H7 and GD32F303.
* A stlink adapter or A CMSIS DAP SWD adapter.

# Toolchain overview

As other ARM based MCUs, the toolchain for STM32 consists of:

* Compiler, ARM GNU toolchain
* Debugger, OpenOCD/gdb
* SDKs
* Flashing tool, OpenOCD or USB storage mode.


# ARM GNU toolchain

As same as various ARM MCUs, STM32 use the 'arm-none-eabi' GNU toolchain. it's not neccesary to build the toolchain yourself, since there are already a lot of well supported prebuilt release and already widely used by developers. If you insist to build it yourself, you can refer to linaro project.

You can download the toolchain for various ARCH from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads.

Download and extract the toolchain (here is x86_64 linux)

```
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
sudo tar xf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /opt
```

And add `/opt/gcc-arm-none-eabi-10.3-2021.10/bin` to PATH env according to the shell you used.

NOTE the toolchain's tripplet is 'arm-none-eabi'.

There are also a lot of prebuilt 'arm-none-eabi' toolchains from other vendors, you can also use them as you like.

# SDKs
