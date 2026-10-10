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
 * Configuration Bits for PIC18F4620
 * ========================================================================= */
#pragma config OSC = HS
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRT = OFF
#pragma config BOREN = OFF
#pragma config BORV = 3
#pragma config WDT = OFF
#pragma config WDTPS = 32768
#pragma config CCP2MX = PORTC
#pragma config PBADEN = OFF
#pragma config LPT1OSC = OFF
#pragma config MCLRE = ON
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config XINST = OFF
#pragma config DEBUG = OFF
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config CPB = OFF
#pragma config CPD = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF
#pragma config EBTRB = OFF

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

    return;
}
