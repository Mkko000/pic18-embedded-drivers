/**
 * @file    main.c
 * @author  Mohammed El Sisi
 * @brief   LED Chaser - Knight Rider + pattern-table effects on PORTC
 * @version 1.0.0
 * @date    2026-10-11
 * @target  PIC18F4620 @ 4 MHz (HS)
 * @compiler XC8
 *
 * @details
 * Eight LEDs on PORTC play a sequence of effects stored in a constant
 * pattern table (Flash-resident). Each effect is a list of frames with its
 * own frame time and repeat count, and the effects cycle forever:
 *   1. Knight Rider (bounce)   2. Fill / Empty
 *   3. Center-out / in         4. Alternate (0x55 / 0xAA)
 *   5. Running pair
 *
 * @par Hardware:
 * - LED0..LED7 on RC0..RC7, each through 330R to GND (active high)
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

/* =========================================================================
 * Effect Definitions
 * ========================================================================= */

/** @brief One visual effect: a frame list played @ref repeat times. */
typedef struct {
    const uint8 *frames;    /**< Pointer to PORTC frames          */
    uint8        count;     /**< Number of frames                 */
    uint8        step_ms;   /**< Frame time (x 10 ms)             */
    uint8        repeat;    /**< How many times the effect plays  */
} effect_t;

/** @brief Knight Rider: single LED bouncing left/right. */
static const uint8 fx_bounce[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x40, 0x20, 0x10, 0x08, 0x04, 0x02
};

/** @brief Fill from right then empty from left. */
static const uint8 fx_fill[] = {
    0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF,
    0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01
};

/** @brief Center-out then center-in. */
static const uint8 fx_center[] = { 0x18, 0x24, 0x42, 0x81, 0x42, 0x24 };

/** @brief Alternate odd/even LEDs. */
static const uint8 fx_alternate[] = { 0x55, 0xAA };

/** @brief Two adjacent LEDs running across. */
static const uint8 fx_pair[] = { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x81 };

/** @brief Number of elements in a table. */
#define COUNT_OF(a)     ((uint8)(sizeof(a) / sizeof((a)[0])))

/** @brief Effect playlist (edit freely - main loop needs no change). */
static const effect_t effects[] = {
    { fx_bounce,    COUNT_OF(fx_bounce),    6,  3 },   /* 60 ms  */
    { fx_fill,      COUNT_OF(fx_fill),      8,  2 },   /* 80 ms  */
    { fx_center,    COUNT_OF(fx_center),    12, 4 },   /* 120 ms */
    { fx_alternate, COUNT_OF(fx_alternate), 30, 6 },   /* 300 ms */
    { fx_pair,      COUNT_OF(fx_pair),      7,  3 },   /* 70 ms  */
};

/* =========================================================================
 * Helpers
 * ========================================================================= */

/** @brief Delay N x 10 ms (__delay_ms needs a constant argument). */
static void delay_10ms(uint8 n) {
    while (n--) {
        __delay_ms(10);
    }
}

/** @brief Play one effect on PORTC. */
static void play_effect(const effect_t *fx) {
    uint8 r, i;
    for (r = 0; r < fx->repeat; r++) {
        for (i = 0; i < fx->count; i++) {
            (void)gpio_port_write(PORTC_INDEX, fx->frames[i]);
            delay_10ms(fx->step_ms);
        }
    }
}

/* =========================================================================
 * Main
 * ========================================================================= */
void main(void) {
    uint8 e;

    (void)gpio_init();                                      /* all digital  */
    (void)gpio_port_setup(PORTC_INDEX, GPIO_PORT_ALL_OUTPUT, GPIO_PORT_ALL_LOW);  /* PORTC output */

    while (1) {
        for (e = 0; e < COUNT_OF(effects); e++) {
            play_effect(&effects[e]);
        }
    }
}
