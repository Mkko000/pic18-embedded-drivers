/**
 * @file    main.c
 * @author  Mohammed El Sisi
 * @brief   8-bit Binary Counter (0-255) with Up / Down / Reset buttons
 * @version 1.0.0
 * @date    2026-10-11
 * @target  PIC18F4620 @ 4 MHz (HS)
 * @compiler XC8
 *
 * @details
 * PORTC shows an 8-bit counter in binary. Three buttons on PORTB use the
 * internal weak pull-ups (pressed = LOW). Each button is debounced in
 * software (state must be stable for DEBOUNCE_TICKS samples) and acts on
 * the falling edge only, so holding a button counts once.
 *
 * Counter wraps 255 -> 0 (Up) and 0 -> 255 (Down).
 *
 * @note    Classic bug avoided: `for (uint8 i = 0; i < 256; i++)` never ends
 *          because a uint8 is always < 256. This project keeps a uint8 value
 *          and relies on defined modulo-256 wrap-around instead.
 *
 * @par Hardware:
 * - RC0..RC7: 8 LEDs (LSB on RC0), 330R to GND
 * - RB0: UP, RB1: DOWN, RB2: RESET - push buttons to GND
 *
 * @note    Build ONE project per MPLAB X configuration: add this main.c plus
 *          the driver sources (hal_gpio.c, ecu_led.c if used). Do NOT add the
 *          root application.c (it also defines main()).
 *
 * @copyright Copyright (c) 2026 Mohammed El Sisi. MIT License.
 */
#include <xc.h>
#include "../../Config/device_config.h"          /* _XTAL_FREQ, feature toggles */
#include "../../MCAL_Layer/GPIO/hal_gpio.h"      /* GPIO driver (Direct API)    */

#if GPIO_DIRECT_API_ENABLE != CONFIG_ENABLE
    #error "This program uses the GPIO Direct API: set GPIO_DIRECT_API_ENABLE to CONFIG_ENABLE in Config/device_config.h"
#endif

/* =========================================================================
 * Configuration
 * ========================================================================= */
#define DISPLAY_PORT        PORTC_INDEX
#define BUTTON_PORT         PORTB_INDEX
#define SAMPLE_MS           5           /**< Button sampling period     */
#define DEBOUNCE_TICKS      4           /**< 4 x 5 ms = 20 ms stable    */

/** @brief Button identifiers (also index into the tables below). */
typedef enum {
    BTN_UP = 0,
    BTN_DOWN,
    BTN_RESET,
    BTN_COUNT
} button_id_t;

/** @brief Debounce state of one button. */
typedef struct {
    pin_index_t pin;        /**< PORTB pin                          */
    logic_t     stable;     /**< Debounced level                    */
    uint8       counter;    /**< Consecutive samples differing      */
} button_t;

static button_t buttons[BTN_COUNT] = {
    { GPIO_PIN0, GPIO_HIGH, 0 },    /* UP    */
    { GPIO_PIN1, GPIO_HIGH, 0 },    /* DOWN  */
    { GPIO_PIN2, GPIO_HIGH, 0 },    /* RESET */
};

/* =========================================================================
 * Helpers
 * ========================================================================= */

/**
 * @brief  Sample one button, debounce it and detect a press edge.
 * @return TRUE exactly once per press (HIGH -> LOW transition).
 */
static boolean_t button_pressed(button_t *btn) {
    boolean_t pressed = FALSE;
    logic_t   raw     = GPIO_HIGH;

    (void)gpio_pin_read(BUTTON_PORT, btn->pin, &raw);

    if (raw != btn->stable) {
        if (++btn->counter >= DEBOUNCE_TICKS) {
            btn->stable  = raw;
            btn->counter = 0;
            if (GPIO_LOW == raw) {
                pressed = TRUE;             /* falling edge = press */
            }
        }
    }
    else {
        btn->counter = 0;                   /* bounce: restart      */
    }

    return pressed;
}

/* =========================================================================
 * Main
 * ========================================================================= */
void main(void) {
    uint8 count = 0;
    uint8 i;

    (void)gpio_init();
    (void)gpio_port_setup(DISPLAY_PORT, GPIO_PORT_ALL_OUTPUT, GPIO_PORT_ALL_LOW);
    (void)gpio_portb_pullup_enable();
    for (i = 0; i < BTN_COUNT; i++) {
        (void)gpio_pin_setup(BUTTON_PORT, buttons[i].pin, GPIO_DIRECTION_INPUT, GPIO_LOW);
    }

    while (1) {
        if (button_pressed(&buttons[BTN_UP]))    { count++; }   /* 255 -> 0 */
        if (button_pressed(&buttons[BTN_DOWN]))  { count--; }   /* 0 -> 255 */
        if (button_pressed(&buttons[BTN_RESET])) { count = 0; }

        (void)gpio_port_write(DISPLAY_PORT, count);
        __delay_ms(SAMPLE_MS);
    }
}
