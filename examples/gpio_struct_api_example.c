/**
 * @file    gpio_struct_api_example.c
 * @author  Mohammed El Sisi
 * @brief   Example: blink an LED on RC0 using the GPIO Struct API
 * @version 1.0.0
 * @date    2026-10-13
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * Requires GPIO_STRUCT_API_ENABLE = CONFIG_ENABLE in Config/device_config.h.
 * Build it instead of application.c (one main() per MPLAB X project) and add
 * Config/config_bits.c and MCAL_Layer/GPIO/hal_gpio.c to the project.
 */

#include <xc.h>
#include "../Config/device_config.h"
#include "../MCAL_Layer/GPIO/hal_gpio.h"

#if (GPIO_STRUCT_API_ENABLE != CONFIG_ENABLE) || (GPIO_PORT_PIN_CONFIGURATIONS != CONFIG_ENABLE)
    #error "This program uses the GPIO Struct API: set GPIO_STRUCT_API_ENABLE and GPIO_PORT_PIN_CONFIGURATIONS to CONFIG_ENABLE in Config/device_config.h"
#endif

/** @brief RC0 as output, starting LOW. */
static const pin_config_t led_pin = {
    .port      = PORTC_INDEX,
    .pin       = GPIO_PIN0,
    .direction = GPIO_DIRECTION_OUTPUT,
    .logic     = GPIO_LOW
};

void main(void)
{
    (void)gpio_init();
    (void)gpio_pin_initialize(&led_pin);

    while (1) {
        (void)gpio_pin_toggle_logic(&led_pin);
        __delay_ms(500);
    }
}
