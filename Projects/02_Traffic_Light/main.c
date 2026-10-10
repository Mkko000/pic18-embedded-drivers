/**
 * @file    main.c
 * @author  Mohammed El Sisi
 * @brief   4-Way Traffic Light - table-driven state machine with Emergency mode
 * @version 1.0.0
 * @date    2026-10-11
 * @target  PIC18F4620 @ 4 MHz (HS)
 * @compiler XC8
 *
 * @details
 * Twelve LEDs model a 4-way junction (N, S, E, W heads; R/Y/G each).
 * North+South heads move together, East+West heads move together.
 * The controller is a 6-state machine whose outputs and durations live in
 * one constant table. Time advances in 50 ms ticks, so the Emergency button
 * is polled continuously (no long blocking delays).
 *
 * Emergency (RB0, active low, internal pull-up): while held, all heads go
 * RED and the yellow lamps flash. On release the cycle restarts from
 * "all red" for safety.
 *
 * @par Hardware:
 * - PORTC: RC0 N_R, RC1 N_Y, RC2 N_G, RC3 S_R, RC4 S_Y, RC5 S_G
 * - PORTD: RD0 E_R, RD1 E_Y, RD2 E_G, RD3 W_R, RD4 W_Y, RD5 W_G
 * - RB0: push button to GND (internal weak pull-up enabled)
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
 * Lamp Encoding (one 3-bit group per head: bit0=R, bit1=Y, bit2=G)
 * ========================================================================= */
#define LAMP_R          0x01
#define LAMP_Y          0x02
#define LAMP_G          0x04

/** @brief Same lamp on both heads of an axis (head A bits 0-2, head B bits 3-5). */
#define AXIS(lamp)      ((uint8)((lamp) | ((lamp) << 3)))

#define NS_PORT         PORTC_INDEX     /**< North + South heads */
#define EW_PORT         PORTD_INDEX     /**< East + West heads   */

#define EMERGENCY_PORT  PORTB_INDEX
#define EMERGENCY_PIN   GPIO_PIN0

#define TICK_MS         50              /**< Scheduler tick       */
#define SEC(s)          ((uint16)((s) * (1000 / TICK_MS)))
#define FLASH_TICKS     10              /**< 500 ms yellow flash  */

/* =========================================================================
 * State Table
 * ========================================================================= */

/** @brief Traffic controller states. */
typedef enum {
    ST_NS_GREEN = 0,    /**< NS green,  EW red  */
    ST_NS_YELLOW,       /**< NS yellow, EW red  */
    ST_ALL_RED_1,       /**< Clearance          */
    ST_EW_GREEN,        /**< EW green,  NS red  */
    ST_EW_YELLOW,       /**< EW yellow, NS red  */
    ST_ALL_RED_2,       /**< Clearance          */
    ST_COUNT
} tl_state_t;

/** @brief Outputs and duration of one state. */
typedef struct {
    uint8  ns;          /**< PORTC pattern       */
    uint8  ew;          /**< PORTD pattern       */
    uint16 ticks;       /**< Duration in ticks   */
} tl_step_t;

/** @brief The whole controller behaviour in one table. */
static const tl_step_t tl_table[ST_COUNT] = {
    /* ns               ew               duration */
    { AXIS(LAMP_G),     AXIS(LAMP_R),    SEC(10) },   /* ST_NS_GREEN  */
    { AXIS(LAMP_Y),     AXIS(LAMP_R),    SEC(3)  },   /* ST_NS_YELLOW */
    { AXIS(LAMP_R),     AXIS(LAMP_R),    SEC(2)  },   /* ST_ALL_RED_1 */
    { AXIS(LAMP_R),     AXIS(LAMP_G),    SEC(10) },   /* ST_EW_GREEN  */
    { AXIS(LAMP_R),     AXIS(LAMP_Y),    SEC(3)  },   /* ST_EW_YELLOW */
    { AXIS(LAMP_R),     AXIS(LAMP_R),    SEC(2)  },   /* ST_ALL_RED_2 */
};

/* =========================================================================
 * Helpers
 * ========================================================================= */

/** @brief Drive both axes. */
static void set_lamps(uint8 ns, uint8 ew) {
    (void)gpio_port_write(NS_PORT, ns);
    (void)gpio_port_write(EW_PORT, ew);
}

/** @brief TRUE while the emergency button is held (active low). */
static boolean_t emergency_active(void) {
    logic_t level = GPIO_HIGH;
    (void)gpio_pin_read(EMERGENCY_PORT, EMERGENCY_PIN, &level);
    return (GPIO_LOW == level) ? TRUE : FALSE;
}

/* =========================================================================
 * Main
 * ========================================================================= */
void main(void) {
    tl_state_t state       = ST_ALL_RED_2;   /* start safe: next = NS green */
    uint16     ticks_left  = 0;
    uint8      flash_timer = 0;
    boolean_t  flash_on    = FALSE;
    boolean_t  in_emergency = FALSE;

    (void)gpio_init();
    (void)gpio_port_setup(NS_PORT, 0xC0, 0x00);     /* RC0-RC5 outputs */
    (void)gpio_port_setup(EW_PORT, 0xC0, 0x00);     /* RD0-RD5 outputs */
    (void)gpio_portb_pullup_enable();
    (void)gpio_pin_setup(EMERGENCY_PORT, EMERGENCY_PIN, GPIO_DIRECTION_INPUT, GPIO_LOW);

    while (1) {
        if (emergency_active()) {
            /* -------- Emergency: all red + flashing yellow -------- */
            in_emergency = TRUE;
            if (0 == flash_timer) {
                flash_timer = FLASH_TICKS;
                flash_on    = (flash_on == TRUE) ? FALSE : TRUE;
            }
            flash_timer--;
            set_lamps((uint8)(AXIS(LAMP_R) | (flash_on ? AXIS(LAMP_Y) : 0)),
                      (uint8)(AXIS(LAMP_R) | (flash_on ? AXIS(LAMP_Y) : 0)));
        }
        else {
            if (TRUE == in_emergency) {
                /* Released: restart from clearance (all red) for safety */
                in_emergency = FALSE;
                state        = ST_ALL_RED_2;
                ticks_left   = tl_table[state].ticks;
                set_lamps(tl_table[state].ns, tl_table[state].ew);
            }

            /* -------- Normal state machine -------- */
            if (0 == ticks_left) {
                state      = (tl_state_t)((state + 1) % ST_COUNT);
                ticks_left = tl_table[state].ticks;
                set_lamps(tl_table[state].ns, tl_table[state].ew);
            }
            ticks_left--;
        }

        __delay_ms(TICK_MS);
    }
}
