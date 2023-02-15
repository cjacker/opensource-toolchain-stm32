# Opensource toolchain for stm32

STM32 is a family of 32-bit microcontroller integrated circuits by STMicroelectronics. The STM32 chips are grouped into related series that are based around the same 32-bit ARM processor core, such as the Cortex-M33F, Cortex-M7F, Cortex-M4F, Cortex-M3, Cortex-M0+, or Cortex-M0. Internally, each microcontroller consists of the processor core, static RAM, flash memory, debugging interface, and various peripherals.

The STM32 family consists of 17 series of microcontrollers: H7, F7, F4, F3, F2, F1, F0, G4, G0, L5, L4, L4+ L1, L0, U5, WL, WB. and each series includes a lot of models with minor differences.

For more information about STM32 family, please refer to https://en.wikipedia.org/wiki/STM32.

There are also a lot of STM32 clones, such as GD32 / CH32 / MM32 etc. Most of them keep compatible with STM32. The toolchain and utilities described in this tutorial may also be used with such parts.

# Hardware prerequisites

* A development board with STM32 MCU. In this tutorial, I will use STM32F1/F4/H7 and GD32, CH32. 
* ST-LINK / DAPLink for programming and debugging.
  - DAPLink is a cheap, opensource and standard way to program/debug any Cortex-M MCU.
  - If you need work with STM8, buy a ST-Link. If not, buy a DAPLink.
  - JLink can support SWD interface, but it's too expensive and not worth to buy for beginners.
* USB2TTL UART adapter
  - for ISP programming of stm32f103, it does not support USB-DFU.

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
* Programming tool: stm32flash / dfu-util and OpenOCD.


# Compiler

## GCC

STM32 and various xx32 use 'arm-none-eabi' GCC toolchain, since they all are based on Cortex-M. it's not neccesary to build the toolchain yourself, there are already a lot of well supported prebuilt release and already widely used by developers. 

### from XPack
[xpack-dev-tools](https://github.com/xpack-dev-tools) provde a prebuilt 'arm-none-eabi' toolchain. you can download it from [here](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v12.2.1-1.2/xpack-arm-none-eabi-gcc-12.2.1-1.2-linux-x64.tar.gz). 

After download:
```
sudo mkdir -p /opt/xpack-arm-none-eabi
sudo tar xf xpack-arm-none-eabi-gcc-12.2.1-1.2-linux-x64.tar.gz -C /opt/xpack-arm-none-eabi --strip-components=1
```
and add /opt/xpack-arm-none-eabi/bin to PATH env.

NOTE, the triplet of xpack prebuilt toolchain is 'arm-none-eabi'.

### from ARM

You can download the prebuilt toolchain for various host from https://developer.arm.com/downloads/-/gnu-rm. 

**Note:** There is prebuilt arm-gcc v12.2 and can be downloaded from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads, but the gdb debugger broken due to python issue, do not use the latest 12.2 version until python issue fixed.

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

You can also use OpenOCD with ST-LINK:

```
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program app.elf verify reset exit"
```

If you use 'app.bin' in OpenOcd command, the start addr must be append, it should be 'program app.bin 0x8000000'.

## DAPLink

DAPLink provides a standardized way to access the Coresight Debug Access Port (DAP) of an ARM Cortex microcontroller via USB. Usually it supports both SWD and serial ports. the connection method is as same as ST-Link.

```
3.3v  -> 3.3v
SWCLK -> SWCLK
SWDIO -> SWDIO
GND   -> GND
```

Still use stm32f411 as target, build the 'baremetal-stm32f4' demo in this repo and program it:

```
openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program app.elf verify reset exit"
```

## JLink

Segger J-Link support ARMs Serial Wire Debug (SWD), but the utilities is all close sourced, and it's too expensive, not worth to buy for beginners. how to use it will not covered by this tutorial.

Since I had a JLink, what I had to mentioned here for beginners is: 'do not forget to connect the VTRef pin to target device'.


# Debugging

Build the codes with debug infomation and connect the ST-Link or DAPLink as mentioned above, launch OpenOCD as:
```
openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg
```
If you use ST-Link, change 'cmsis-dap.cfg' to 'st-link.cfg'.

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
