//! This module contains hardware configuration setup, eg GPIO, DMA, and IO.

use hal::{
    dma::{self, DmaInput, DmaInterrupt},
    gpio::{OutputSpeed, Pin, PinMode, Port},
};

pub fn setup_pins() {
    // let mut can_rx = Pin::new(Port::A, 11, PinMode::Alt(9));
    // let mut can_tx = Pin::new(Port::B, 9, PinMode::Alt(9));
    //
    // can_tx.output_speed(OutputSpeed::VeryHigh);
    // can_rx.output_speed(OutputSpeed::VeryHigh);
}

pub fn setup_dma() {
    // dma::enable_mux1();
}
