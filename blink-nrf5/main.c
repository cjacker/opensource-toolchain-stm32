#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define led 3 

int main(void)
{
    nrf_gpio_cfg_output(led); //configure led pin as output

    while (true)
    {
        nrf_gpio_pin_set(led);  // Turn LED OFF
        nrf_delay_ms(1000);
        nrf_gpio_pin_clear(led);// Turn LED ON
        nrf_delay_ms(1000);
    }
}
