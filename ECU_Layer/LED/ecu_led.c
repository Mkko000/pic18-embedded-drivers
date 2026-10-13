/**
 * @file    ecu_led.c
 * @author  Mohammed El Sisi
 * @brief   LED Driver Implementation
 * @version 1.0.0
 * @date    2026-10-11
 *
 * @details
 * Implementation of LED Driver using Direct GPIO API.
 */

#include "ecu_led.h"

#if LED_MODULE_ENABLE == CONFIG_ENABLE

/* =========================================================================
 * GPIO API Selection (prefer Direct API, fall back to Struct API)
 * ========================================================================= */
#if GPIO_DIRECT_API_ENABLE == CONFIG_ENABLE
    #define LED_USE_GPIO_DIRECT_API
#endif

#ifndef LED_USE_GPIO_DIRECT_API
/**
 * @brief Build a pin_config_t for the Struct API from an LED config.
 */
static inline pin_config_t led_to_pin_config(const led_config_t *led, logic_t logic) {
    pin_config_t cfg;
    cfg.port      = led->port;
    cfg.pin       = led->pin;
    cfg.direction = GPIO_DIRECTION_OUTPUT;
    cfg.logic     = (uint8)logic;
    return cfg;
}
#endif

/** @brief Logic level that turns the LED ON for its active mode. */
#define LED_ON_LOGIC(led)   (((led)->active == LED_ACTIVE_HIGH_MODE) ? GPIO_HIGH : GPIO_LOW)
/** @brief Logic level that turns the LED OFF for its active mode. */
#define LED_OFF_LOGIC(led)  (((led)->active == LED_ACTIVE_HIGH_MODE) ? GPIO_LOW : GPIO_HIGH)

/**
 * @brief Internal: write a logic level to the LED pin with the enabled API.
 */
static Std_ReturnType led_write(const led_config_t *led, logic_t logic) {
#ifdef LED_USE_GPIO_DIRECT_API
    return gpio_pin_write((port_index_t)led->port, (pin_index_t)led->pin, logic);
#else
    pin_config_t cfg = led_to_pin_config(led, logic);
    return gpio_pin_write_logic(&cfg, logic);
#endif
}

/**
 * @brief Initialize an LED pin (output, OFF).
 */
Std_ReturnType led_initialize(const led_config_t *led) {
    Std_ReturnType ret = E_OK;

    if (NULL == led) {
        ret = E_NOT_OK;
    }
    else {
#ifdef LED_USE_GPIO_DIRECT_API
        ret = gpio_pin_setup((port_index_t)led->port, (pin_index_t)led->pin,
                              GPIO_DIRECTION_OUTPUT, LED_OFF_LOGIC(led));
#else
        pin_config_t cfg = led_to_pin_config(led, LED_OFF_LOGIC(led));
        ret = gpio_pin_initialize(&cfg);
#endif
    }

    return ret;
}

/**
 * @brief Turn LED ON.
 */
Std_ReturnType led_on(const led_config_t *led) {
    return (NULL == led) ? E_NOT_OK : led_write(led, LED_ON_LOGIC(led));
}

/**
 * @brief Turn LED OFF.
 */
Std_ReturnType led_off(const led_config_t *led) {
    return (NULL == led) ? E_NOT_OK : led_write(led, LED_OFF_LOGIC(led));
}

/**
 * @brief Toggle LED state.
 */
Std_ReturnType led_toggle(const led_config_t *led) {
    Std_ReturnType ret = E_OK;

    if (NULL == led) {
        ret = E_NOT_OK;
    }
    else {
#ifdef LED_USE_GPIO_DIRECT_API
        ret = gpio_pin_toggle((port_index_t)led->port, (pin_index_t)led->pin);
#else
        pin_config_t cfg = led_to_pin_config(led, GPIO_LOW);
        ret = gpio_pin_toggle_logic(&cfg);
#endif
    }

    return ret;
}

/**
 * @brief Get LED state (from LAT, active-mode aware).
 */
Std_ReturnType led_get_state(const led_config_t *led, led_state_t *state) {
    Std_ReturnType ret = E_OK;

    if ((NULL == led) || (NULL == state)) {
        ret = E_NOT_OK;
    }
    else {
        logic_t latch = GPIO_LOW;

#ifdef LED_USE_GPIO_DIRECT_API
        ret = gpio_pin_read_latch((port_index_t)led->port, (pin_index_t)led->pin, &latch);
#else
        pin_config_t cfg = led_to_pin_config(led, GPIO_LOW);
        ret = gpio_pin_read_latch_logic(&cfg, &latch);
#endif
        if (E_OK == ret) {
            *state = (latch == LED_ON_LOGIC(led)) ? LED_ON : LED_OFF;
        }
    }

    return ret;
}

#endif /* LED_MODULE_ENABLE */
