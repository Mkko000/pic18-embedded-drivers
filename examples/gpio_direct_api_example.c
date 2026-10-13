/**
 * @file    gpio_direct_api_example.c
 * @author  Mohammed El Sisi
 * @brief   Example: blink an LED on RC0 using the GPIO Direct API
 * @version 1.0.0
 * @date    2026-10-13
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * Requires GPIO_DIRECT_API_ENABLE = CONFIG_ENABLE in Config/device_config.h.
 * Build it instead of application.c (one main() per MPLAB X project) and add
 * Config/config_bits.c and MCAL_Layer/GPIO/hal_gpio.c to the project.
 */

#include <xc.h>
#include "../Config/device_config.h"
#include "../MCAL_Layer/GPIO/hal_gpio.h"

#if GPIO_DIRECT_API_ENABLE != CONFIG_ENABLE
    #error "This program uses the GPIO Direct API: set GPIO_DIRECT_API_ENABLE to CONFIG_ENABLE in Config/device_config.h"
#endif

void main(void)
{
    (void)gpio_init();
    (void)gpio_pin_setup(PORTC_INDEX, GPIO_PIN0, GPIO_DIRECTION_OUTPUT, GPIO_LOW);

    while (1) {
        (void)gpio_pin_toggle(PORTC_INDEX, GPIO_PIN0);
        __delay_ms(500);
    }
}
