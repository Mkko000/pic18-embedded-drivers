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

/**
 * @brief Initialize an LED pin.
 */
Std_ReturnType led_initialize(const led_config_t *led) {
    Std_ReturnType ret = E_OK;

    if (NULL == led) {
        ret = E_NOT_OK;
    }
    else {
        /* Determine initial logic based on active mode */
        logic_t initial_logic = (led->active == LED_ACTIVE_HIGH_MODE) ?
                                 GPIO_LOW : GPIO_HIGH;

        /* Use Direct API - one call */
        ret = gpio_pin_setup((port_index_t)led->port,
                              (pin_index_t)led->pin,
                              GPIO_DIRECTION_OUTPUT,
                              initial_logic);
    }

    return ret;
}

/**
 * @brief Turn LED ON.
 */
Std_ReturnType led_on(const led_config_t *led) {
    Std_ReturnType ret = E_OK;

    if (NULL == led) {
        ret = E_NOT_OK;
    }
    else {
        logic_t logic = (led->active == LED_ACTIVE_HIGH_MODE) ?
                         GPIO_HIGH : GPIO_LOW;

        ret = gpio_pin_write((port_index_t)led->port,
                              (pin_index_t)led->pin,
                              logic);
    }

    return ret;
}

/**
 * @brief Turn LED OFF.
 */
Std_ReturnType led_off(const led_config_t *led) {
    Std_ReturnType ret = E_OK;

    if (NULL == led) {
        ret = E_NOT_OK;
    }
    else {
        logic_t logic = (led->active == LED_ACTIVE_HIGH_MODE) ?
                         GPIO_LOW : GPIO_HIGH;

        ret = gpio_pin_write((port_index_t)led->port,
                              (pin_index_t)led->pin,
                              logic);
    }

    return ret;
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
        ret = gpio_pin_toggle((port_index_t)led->port,
                               (pin_index_t)led->pin);
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

        ret = gpio_pin_read_latch((port_index_t)led->port, (pin_index_t)led->pin, &latch);
        if (E_OK == ret) {
            *state = (latch == ((led->active == LED_ACTIVE_HIGH_MODE) ? GPIO_HIGH : GPIO_LOW)) ?
                     LED_ON : LED_OFF;
        }
    }

    return ret;
}

#endif /* LED_MODULE_ENABLE */
