/**
 * @file    main.c
 * @author  Mohammed El Sisi
 * @brief   Morse Code Transmitter - table-driven A-Z / 0-9 on an LED
 * @version 1.0.0
 * @date    2026-10-11
 * @target  PIC18F4620 @ 4 MHz (HS)
 * @compiler XC8
 *
 * @details
 * Sends any string as International Morse code on an LED (and optional
 * buzzer on the same pin). Symbols come from a lookup table, encoded as
 * strings of '.' and '-'. All timing derives from one UNIT constant:
 *   dot = 1, dash = 3, intra-symbol gap = 1, letter gap = 3, word gap = 7.
 * The demo sends "SOS" then "MOHAMMED" forever.
 *
 * @par Hardware:
 * - RD0: LED through 330R to GND (active high), optional buzzer via transistor
 *
 * @note    Build ONE project per MPLAB X configuration: add this main.c plus
 *          the driver sources (hal_gpio.c, ecu_led.c if used). Do NOT add the
 *          root application.c (it also defines main()).
 *
 * @copyright Copyright (c) 2026 Mohammed El Sisi. MIT License.
 */
#include <xc.h>
#include "../../Config/device_config.h"          /* _XTAL_FREQ, feature toggles */
#include "../../MCAL_Layer/GPIO/hal_gpio.h"      /* GPIO driver                 */
#include "../../ECU_Layer/LED/ecu_led.h"         /* LED driver                  */

/* =========================================================================
 * Configuration Bits (PIC18F4620, 4 MHz HS crystal)
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
 * Timing (PARIS standard: UNIT_MS = 1200 / WPM)
 * ========================================================================= */
#define UNIT_MS             150         /**< 1 unit = 150 ms (~8 WPM)   */
#define DOT_UNITS           1
#define DASH_UNITS          3
#define SYMBOL_GAP_UNITS    1           /**< between dots/dashes        */
#define LETTER_GAP_UNITS    3           /**< between letters            */
#define WORD_GAP_UNITS      7           /**< between words              */

/* =========================================================================
 * Morse Table
 * ========================================================================= */
static const char * const morse_letters[26] = {
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",  "....", "..",   /* A-I */
    ".---", "-.-",  ".-..", "--",   "-.",   "---",  ".--.", "--.-", ".-.",  /* J-R */
    "...",  "-",    "..-",  "...-", ".--",  "-..-", "-.--", "--.."          /* S-Z */
};

static const char * const morse_digits[10] = {
    "-----", ".----", "..---", "...--", "....-",                            /* 0-4 */
    ".....", "-....", "--...", "---..", "----."                             /* 5-9 */
};

static const led_config_t morse_led = {
    .port     = PORTD_INDEX,
    .pin      = GPIO_PIN0,
    .active   = LED_ACTIVE_HIGH_MODE,
    .reserved = 0
};

/* =========================================================================
 * Helpers
 * ========================================================================= */

/** @brief Wait N Morse units. */
static void wait_units(uint8 units) {
    while (units--) {
        __delay_ms(UNIT_MS);
    }
}

/** @brief Look up a character; returns NULL for unsupported characters. */
static const char *morse_lookup(char c) {
    const char *code = NULL;

    if ((c >= 'a') && (c <= 'z')) {
        c = (char)(c - 'a' + 'A');
    }
    if ((c >= 'A') && (c <= 'Z')) {
        code = morse_letters[c - 'A'];
    } else if ((c >= '0') && (c <= '9')) {
        code = morse_digits[c - '0'];
    }

    return code;
}

/** @brief Send one character (dots/dashes with 1-unit gaps). */
static void morse_send_char(char c) {
    const char *code = morse_lookup(c);

    if (NULL != code) {
        while (*code) {
            (void)led_on(&morse_led);
            wait_units((*code == '-') ? DASH_UNITS : DOT_UNITS);
            (void)led_off(&morse_led);
            code++;
            if (*code) {
                wait_units(SYMBOL_GAP_UNITS);
            }
        }
    }
}

/**
 * @brief Send a whole string. Spaces produce a word gap; unknown
 *        characters are skipped.
 */
static void morse_send_string(const char *text) {
    while (*text) {
        if (*text == ' ') {
            wait_units(WORD_GAP_UNITS - LETTER_GAP_UNITS);  /* letter gap already done */
        } else {
            morse_send_char(*text);
            wait_units(LETTER_GAP_UNITS);
        }
        text++;
    }
}

/* =========================================================================
 * Main
 * ========================================================================= */
void main(void) {
    (void)gpio_init();
    (void)led_initialize(&morse_led);

    while (1) {
        morse_send_string("SOS");
        wait_units(WORD_GAP_UNITS);
        morse_send_string("MOHAMMED");
        wait_units(WORD_GAP_UNITS);
    }
}
