//! This module contains initialization code, run once at program start.

use hal::{
    clocks::Clocks,
    dma::{self, Dma},
    gpio::{Pin, PinMode, Port},
    iwdg, pac,
};

use crate::setup;

pub fn run() {
    // pub fn run(cx: app::init::Context) -> (Shared, Local) {
    // Set up ARM Cortex-M peripherals. These are common to many MCUs, including all STM32 ones.
    let cp = cortex_m::Peripherals::take().unwrap();
    // let cp = cx.core;
    // Set up peripherals specific to the microcontroller you're using.
    let dp = pac::Peripherals::take().unwrap();

    // Create an initial clock configuration that uses the MCU's internal oscillator (HSI),
    // sets the MCU to its maximum system clock speed.
    let clock_cfg = Clocks::default();

    clock_cfg.setup().unwrap();

    let _dma = Dma::new(dp.DMA1);

    setup::setup_pins();
    setup::setup_dma();

    // Enable the watchdog with a 0.1s timeout.
    iwdg::setup(0.1);

    // (
    //     Shared {},
    //     Local {},
    // )
}
