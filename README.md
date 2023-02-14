# Opensource toolchain for stm32

STM32 is a family of 32-bit microcontroller integrated circuits by STMicroelectronics. The STM32 chips are grouped into related series that are based around the same 32-bit ARM processor core, such as the Cortex-M33F, Cortex-M7F, Cortex-M4F, Cortex-M3, Cortex-M0+, or Cortex-M0. Internally, each microcontroller consists of the processor core, static RAM, flash memory, debugging interface, and various peripherals.

The STM32 family consists of 17 series of microcontrollers: H7, F7, F4, F3, F2, F1, F0, G4, G0, L5, L4, L4+ L1, L0, U5, WL, WB. and each series includes a lot of models with minor differences.

For more information about STM32 family, please refer to https://en.wikipedia.org/wiki/STM32.

There are also a lot of STM32 clones, such as GD32 / CH32 / MM32 etc. Most of them keep compatible with STM32. The toolchain and utilities described in this tutorial may also be used with such parts.

# Hardware prerequisites

* A development board with STM32 MCU. In this tutorial, I will use STM32F1/F4/H7 and GD32, CH32. 
* STLink / JLink / Any CMSIS-DAP debugger
  - for programming and debugging.
* USB2TTL UART adapter
  - for ISP programming of stm32f103, it do not support USB-DFU.

# Toolchain overview

As any other ARM based MCUs, the toolchain for STM32 consists of:

* Compiler: GCC / Rust
* Debugger: OpenOCD/gdb
* SDKs: Various
  - official SPL in C
  - STM32 Cube/HAL in C
  - libopencm3 in C
  - stm32-hal in rust
  - stm32-rs in rust
* Programming tool: dfu-util and OpenOCD.


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

## Official SPL

**Note:** STM32 official SPLs was deprecated serveral years ago. it's recommend to use Cube/HAL instead of SPL.

Standard Peripherals library covers 3 abstraction levels, and includes:

* A complete register address mapping with all bits, bit fields and registers declared in C. This avoids a cumbersome task and more important, it brings the benefits of a bug free reference mapping file, speeding up the early project phase.

* A collection of routines and data structures which covers all peripheral functions (drivers with common API). It can directly be used as a reference framework, since it also includes macros for supporting core-related intrinsic features and common constants and data types definition.

* A set of examples covering all available peripherals with template projects for the most common development toolchains. With the appropriate hardware evaluation board, this allows to get started with a brand new micro within few hours.

Each driver consists of a set of functions covering all peripheral functionalities. The development of each driver is driven by a common API (application programming interface) which standardizes the driver structure, the functions and the parameter names. The driver source code is developed in ‘Strict ANSI-C’ (relaxed ANSI-C for projects and example files). It is fully documented and is MISRA-C 2004 compliant. Writing the whole library in ‘Strict ANSI-C’ makes it independent from the software toolchain. Only the start-up files depend on the toolchain.

All SPL packages can be downloaded from [here](https://www.st.com/en/embedded-software/stm32-standard-peripheral-libraries.html), you may need register an account and login first before download, available SPLs includes:

```
STSW-STM32048 	STM32F0xx standard peripherals library
STSW-STM32054 	STM32F10x standard peripheral library 
STSW-STM32115 	STM32F37x/F38x DSP and standard peripherals library
STSW-STM32062 	STM32F2xx standard peripherals library (UM1061) 
STSW-STM32065 	STM32F4 DSP and standard peripherals library
STSW-STM32077   STM32L1xx standard peripherals library
```

You could notice there is no SPL for such as G4 or H7 etc, since SPL was deprecated, for such models, you should use Cube/HAL.

The problem with SPL is all these libraries lack of 'Makefile' support, but you can found some forked repo which contains a 'Makefile'. 

A lot of STM32 clones such as CH32F / GD32F also have this issue, their firmware library or StdPeriph library or EVT packages almost have the same project structure and code organization as STM32 SPL, I provided a demo project for GD32F470ZGT6 (LiangShan Pi board from JLC) to blink four LEDs. You can take is as reference how to write a Makefile for such libraries.

As mentioned in bare metal programming section, you may also need to prepare a startup asm file (work with GCC) and a linker script. these files can be taken from libopencm3 or various other opensource projects. For stm32, you can also use stm32cubemx to generate the startup and linker script files, but it will not covered by this tutorial.

Use LiangShan Pi with GD32F470ZGT6 as example, there is a demo project in this repo, the 'GD32F4xx_Firmware_Library' directly comes from GD32 official Demo Suite without any modifications. What I added is a linker script and a startup asm file for gd32f470, the 'led' dir contains blink source codes and a 'Makefile':

```
git clone https://github.com/cjacker/opensource-toolchain-stm32
cd liangshan_pi_gd32f470zgt6_blink
make
```

The target elf/hex/bin files 'gd32f470zgt6.xxx' will be generated at `build` dir. 

## STM32 Cube/HAL

STM32CubeMX is a graphical tool that allows a very easy configuration of STM32 microcontrollers and microprocessors, as well as the generation of the corresponding initialization C code. 

STM32CubeMX can support generate 'Makefile' project for Linux and arm-gcc toolchain. It's very easy to use, just follow the wizard, select your mcu model, and choose 'Makefile' project, a skeleton of a new project will be generated and can use `make` to build.

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

# Programming

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

### ISP programming utilities

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

**NOTE 2:** Do NOT forget restore the jumper to start the device from flash.


#### USB-DFU 
After ISP mode activated, if the part support USB DFU, run `lsusb`, the output looks like:

```
Bus 001 Device 040: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
```




