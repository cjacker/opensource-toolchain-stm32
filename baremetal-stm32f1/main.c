// STM32F103C8T6
// 64K ROM / 20K RAM
// LED to PC13 (for classic f103 bluepill )
// LED to PB2 (for WeAct f103 bluepill-plus)

#include <stdint.h>

// register address
#define RCC_BASE      0x40021000
#define GPIOB_BASE    0x40010c00
#define GPIOC_BASE    0x40011000

#define RCC_APB2ENR   *(volatile uint32_t *)(RCC_BASE   + 0x18)
#define GPIOB_CRH     *(volatile uint32_t *)(GPIOB_BASE + 0x04)
#define GPIOB_ODR     *(volatile uint32_t *)(GPIOB_BASE + 0x0C)
#define GPIOC_CRH     *(volatile uint32_t *)(GPIOC_BASE + 0x04)
#define GPIOC_ODR     *(volatile uint32_t *)(GPIOC_BASE + 0x0C)

// bit fields
#define RCC_IOPCEN   (1<<4)
#define GPIOB2       (1UL<<2)
#define GPIOC13      (1UL<<13)

void main(void)
{
    RCC_APB2ENR |= RCC_IOPCEN;
    // for PC13
    GPIOC_CRH   &= 0xFF0FFFFF;
    GPIOC_CRH   |= 0x00200000;

    // for PB2
    GPIOB_CRH   &= 0xFFFFFFCF;
    GPIOB_CRH   |= 0x00000010;

    while(1)
    {
        GPIOC_ODR |=  GPIOC13;
        GPIOB_ODR |=  GPIOB2;
        for (int i = 0; i < 100000; i++); // arbitrary delay
        GPIOC_ODR &= ~GPIOC13;
        GPIOB_ODR &= ~GPIOB2;
        for (int i = 0; i < 100000; i++); // arbitrary delay
    }
}
