// STM32H743VIT6
// 2048K ROM / 1024K RAM
// LED to PE3

#include <stdint.h>

// register address
#define RCC_BASE 0x58024400U
#define GPIOE_BASE 0x58021000U

#define RCC_AHB4ENR   *(volatile uint32_t *)(RCC_BASE + 0xE0)

#define GPIOE_MODER   *(volatile uint32_t *)(GPIOE_BASE + 0x00)
#define GPIOE_ODR     *(volatile uint32_t *)(GPIOE_BASE + 0x14)

// bit fields
#define RCC_AHB4ENR_GPIOEEN (1<<4)

#define GPIOE3 (1UL<<3)

void main(void)
{
    /* Enable GPIOE clock (AHB1ENR: bit 3) */
    // AHB1ENR: XXXX XXXX XXXX XXXX XXXX XXXX XXX1 XXXX
    RCC_AHB4ENR |= 0x00000010;

    // Note: |= means read the contents of the left operand, or it with
    //   the right operand and write the result back to the left operand

    // Another way to write a 1 to a bit location is to shift it that much
    // Meaning shift number 1, 4 times to the left. Which would result in
    // 0x10
    // RCC_AHB4ENR |= (1U << 4);

    // We can also use the predefined directives
    // RCC->AHB4ENR |= RCC_AHB1ENR_GPIODEN;
    // RCC_AHB4ENR_GPIOEEN here will expand to (1 << 4)

    /* Make Pin 3 output (MODER: bits 7:6) */
    // Each pin is represented with two bits on the MODER register
    // 00 - input (reset state)
    // 01 - output
    // 10 - alternate function
    // 11 - analog mode

    // We can leave the pin at default value to make it an input pin.
    // In order to make a pin output, we need to write 01 to the relevant
    // section in MODER register
    // We first need to AND it to reset them, then OR it to set them.
    //                     bit31                                         bit0
    // MODER register bits : xx xx xx xx XX XX XX XX XX XX XX XX 01 XX XX XX
    //                      p15                                  p3       p0

    GPIOE_MODER &= 0xFFFFFF3F; // 1111 1111 1111 1111 1111 1111 0011 1111
    GPIOE_MODER |= 0x00000040; // 0000 0000 0000 0000 0000 0000 0100 0000

    while(1)
    {
        GPIOE_ODR |=  GPIOE3;
        for (int i = 0; i < 1000000; i++); // arbitrary delay
        GPIOE_ODR &= ~GPIOE3;
        for (int i = 0; i < 1000000; i++); // arbitrary delay
    }
}
