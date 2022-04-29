// STM32F411CEU6
// 512K ROM / 128K RAM
// LED to PC13

#include <stdint.h>

// register address
#define PERIPH_BASE   (0x40000000U)
#define PERIPH_BASE_AHB1  (PERIPH_BASE + 0x20000)
#define RCC_BASE      (PERIPH_BASE_AHB1 + 0x3800)

#define GPIOC_BASE    (PERIPH_BASE_AHB1 + 0x0800)

#define RCC_AHB1ENR   *(volatile uint32_t *)(RCC_BASE + 0x30)
#define GPIOC_MODER   *(volatile uint32_t *)(GPIOC_BASE + 0x00)
#define GPIOC_ODR     *(volatile uint32_t *)(GPIOC_BASE + 0x14)

// bit fields
#define RCC_AHB1ENR_GPIODEN (1<<2)
#define GPIOC13 (1UL<<13)

void main(void)
{
    /* Enable GPIOD clock (AHB1ENR: bit 3) */
    // AHB1ENR: XXXX XXXX XXXX XXXX XXXX XXXX XXXX X1XX
    RCC_AHB1ENR |= 0x00000006;

    // Note: |= means read the contents of the left operand, or it with
    //   the right operand and write the result back to the left operand

    // Another way to write a 1 to a bit location is to shift it that much
    // Meaning shift number 1, 2 times to the left. Which would result in
    // 0b0100 or 0x6
    // RCC_AHB1ENR |= (1U << 2);

    // We can also use the predefined directives
    // RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // RCC_AHB1ENR_GPIODEN here will expand to (1 << 3)

    /* Make Pin 13 output (MODER: bits 27:26) */
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
    // MODER register bits : xx xx 01 xx XX XX XX XX XX XX XX XX XX XX XX XX
    //                      p15   p13                                     p0

    GPIOC_MODER &= 0xF3FFFFFF; // 1111 0011 1111 1111 1111 1111 1111 1111
    GPIOC_MODER |= 0x04000000; // 0000 0100 0000 ...

    while(1)
    {
        GPIOC_ODR |=  GPIOC13;
        for (int i = 0; i < 1000000; i++); // arbitrary delay
        GPIOC_ODR &= ~GPIOC13;
        for (int i = 0; i < 1000000; i++); // arbitrary delay
    }
}
