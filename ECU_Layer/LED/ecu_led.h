/**
 * @file    ecu_led.h
 * @author  Mohammed El Sisi
 * @brief   LED Driver Interface
 * @version 1.0.0
 * @date    2026-10-11
 *
 * @details
 * This driver provides an abstraction for LED control. It uses the
 * Direct GPIO API internally. Supports both Active-High and Active-Low
 * LED configurations.
 *
 * @note
 * - LED pin must be configured as OUTPUT.
 * - Active-Low LEDs are supported via the active field.
 */

#ifndef ECU_LED_H
#define ECU_LED_H

/* ==================== Includes ==================== */
#include "../../MCAL_Layer/GPIO/hal_gpio.h"
#include "../../Config/device_config.h"

#if LED_MODULE_ENABLE == CONFIG_ENABLE

/* ==================== Data Type Declarations ==================== */

/**
 * @brief LED Active Mode enumeration.
 */
typedef enum {
    LED_ACTIVE_HIGH_MODE = 0,   /**< LED ON when pin = HIGH */
    LED_ACTIVE_LOW_MODE  = 1    /**< LED ON when pin = LOW  */
} led_active_t;

/**
 * @brief LED state enumeration.
 */
typedef enum {
    LED_OFF = 0,    /**< LED is OFF */
    LED_ON  = 1     /**< LED is ON  */
} led_state_t;

/**
 * @brief LED Configuration Structure.
 */
typedef struct {
    uint8 port      : 3;    /**< Port index (@ref port_index_t) */
    uint8 pin       : 3;    /**< Pin index (@ref pin_index_t)  */
    uint8 active    : 1;    /**< Active mode (@ref led_active_t) */
    uint8 reserved  : 1;    /**< Reserved for future use */
} led_config_t;

/* ==================== Function Declarations ==================== */

/**
 * @brief   Initialize an LED pin.
 * @param   led  Pointer to LED configuration structure.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType led_initialize(const led_config_t *led);

/**
 * @brief   Turn LED ON.
 * @param   led  Pointer to LED configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType led_on(const led_config_t *led);

/**
 * @brief   Turn LED OFF.
 * @param   led  Pointer to LED configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType led_off(const led_config_t *led);

/**
 * @brief   Toggle LED state.
 * @param   led  Pointer to LED configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType led_toggle(const led_config_t *led);

/**
 * @brief   Get the current LED state (reads the output latch, LAT).
 * @details Accounts for the active mode: an active-low LED is ON when
 *          its pin is LOW.
 * @param   led    Pointer to LED configuration.
 * @param   state  Pointer to store the state (@ref led_state_t).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType led_get_state(const led_config_t *led, led_state_t *state);

#endif /* LED_MODULE_ENABLE */
#endif /* ECU_LED_H */
