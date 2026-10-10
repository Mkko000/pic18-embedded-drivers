/**
 * @file    application.c
 * @author  Mohammed El Sisi
 * @brief   Main Application - GPIO Driver Demo
 * @version 2.0.0
 * @date    2026-10-11
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * This is the main application that demonstrates the usage of the
 * GPIO driver. It toggles 3 LEDs connected to PORTC (RC0, RC1, RC2)
 * every 500 ms.
 *
 * @par Hardware Setup:
 * - Crystal: 4 MHz connected to OSC1/OSC2 with 22pF caps
 * - MCLR: Pull-up 10K to VDD
 * - LEDs: RC0, RC1, RC2 with 330Ω resistors to GND
 * - VDD: +5V, VSS: GND
 */

#include <xc.h>
#include "Config/device_config.h"   /* _XTAL_FREQ for __delay_ms() */
#include "ECU_Layer/LED/ecu_led.h"

/* =========================================================================
 * LED Configurations
 * ========================================================================= */
#if LED_MODULE_ENABLE == CONFIG_ENABLE

/** @brief Red LED on RC0, Active High */
led_config_t led_red = {
    .port   = PORTC_INDEX,
    .pin    = GPIO_PIN0,
    .active = LED_ACTIVE_HIGH_MODE,
    .reserved = 0
};

/** @brief Green LED on RC1, Active High */
led_config_t led_green = {
    .port   = PORTC_INDEX,
    .pin    = GPIO_PIN1,
    .active = LED_ACTIVE_HIGH_MODE,
    .reserved = 0
};

/** @brief Blue LED on RC2, Active High */
led_config_t led_blue = {
    .port   = PORTC_INDEX,
    .pin    = GPIO_PIN2,
    .active = LED_ACTIVE_HIGH_MODE,
    .reserved = 0
};

#endif /* LED_MODULE_ENABLE */

/* =========================================================================
 * Main Function
 * ========================================================================= */
void main(void) {

    /* -------- Initialization -------- */
    (void)gpio_init();      /* all pins digital (ADCON1 PCFG = 0x0F) */

    #if LED_MODULE_ENABLE == CONFIG_ENABLE
    led_initialize(&led_red);
    led_initialize(&led_green);
    led_initialize(&led_blue);
    #endif

    /* -------- Main Loop -------- */
    while(1) {

        #if LED_MODULE_ENABLE == CONFIG_ENABLE

        /* -------- State 1: Red ON, others OFF -------- */
        led_on(&led_red);
        led_off(&led_green);
        led_off(&led_blue);
        __delay_ms(500);

        /* -------- State 2: Green ON, others OFF -------- */
        led_off(&led_red);
        led_on(&led_green);
        led_off(&led_blue);
        __delay_ms(500);

        /* -------- State 3: Blue ON, others OFF -------- */
        led_off(&led_red);
        led_off(&led_green);
        led_on(&led_blue);
        __delay_ms(500);

        #endif
    }

}
