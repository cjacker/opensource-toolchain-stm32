// GD32F303CCT6
// 256K ROM / 48K RAM
// LED to PB2

#include <stdint.h>

// register address
#define RCC_BASE      0x40021000
#define GPIOB_BASE    0x40010C00 

#define RCC_APB2ENR   *(volatile uint32_t *)(RCC_BASE   + 0x18)

#define GPIOB_CRL     *(volatile uint32_t *)(GPIOB_BASE + 0x00)
#define GPIOB_CRH     *(volatile uint32_t *)(GPIOB_BASE + 0x04)

#define GPIOB_ODR     *(volatile uint32_t *)(GPIOB_BASE + 0x0C)

// bit fields
#define RCC_IOPBEN   (1<<3)

#define GPIOB2      (1UL<<2)

void main(void)
{
    RCC_APB2ENR |= RCC_IOPBEN;
    GPIOB_CRL   &= 0xFFFFF0FF;
    GPIOB_CRL   |= 0x00000200;

    while(1)
    {
        GPIOB_ODR |=  GPIOB2;
        for (int i = 0; i < 500000; i++); // arbitrary delay
        GPIOB_ODR &= ~GPIOB2;
        for (int i = 0; i < 500000; i++); // arbitrary delay
    }
}
