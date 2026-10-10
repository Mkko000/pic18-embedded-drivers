/**
 * @file    hal_gpio.h
 * @author  Mohammed El Sisi
 * @brief   GPIO Driver Interface for PIC18F4620
 * @version 2.0.0
 * @date    2026-10-11
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * This driver provides a complete interface for controlling GPIO pins
 * and ports on the PIC18F4620 microcontroller. It supports two API styles:
 *
 * - **Struct-Based API**: Uses pin_config_t for organized pin management.
 * - **Direct API**: Uses simple parameter passing for quick operations.
 *
 * Both APIs can be enabled/disabled from device_config.h.
 *
 * @note
 * - The driver uses LAT registers for writing to avoid Read-Modify-Write issues.
 * - The driver uses PORT registers for reading actual pin states.
 * - All functions perform NULL pointer checks and range validation.
 *
 * @warning
 * - Ensure the correct clock frequency is set in device_config.h.
 * - Do not access TRIS/PORT/LAT registers directly from outside this driver.
 *
 * @see device_config.h
 * @see mcal_std_types.h
 * @see std_macros.h
 *
 * @copyright Copyright (c) 2026 Mohammed El Sisi. All rights reserved.
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

/* ==================== Section: Includes ==================== */
#include <xc.h>
#include "../../Common/mcal_std_types.h"
#include "../../Common/std_macros.h"
#include "../../Config/device_config.h"

#if GPIO_MODULE_ENABLE == CONFIG_ENABLE

/* ==================== Section: Macro Declarations ==================== */

/* BIT_MASK is defined in Common/std_macros.h - do not redefine here */

/** @brief Maximum number of pins per port */
#define PORT_PIN_MAX_NUMBER         8

/** @brief Maximum number of ports */
#define PORT_MAX_NUMBER             5

/* -------- Range Check Macros -------- */

/** @brief Check if port is valid (0-4) */
#define IS_VALID_PORT(port)         (((uint8)(port)) < PORT_MAX_NUMBER)

/** @brief Check if pin is valid (0-7) */
#define IS_VALID_PIN(pin)           (((uint8)(pin)) < PORT_PIN_MAX_NUMBER)

/** @brief Number of physical pins on PORTE (RE0-RE2; RE3 is MCLR, input only) */
#define PORTE_PIN_MAX_NUMBER        3

/** @brief Check port + pin together (rejects pins that do not exist on the chip, e.g. RE3-RE7) */
#define IS_VALID_PORT_PIN(port, pin) (IS_VALID_PORT(port) && IS_VALID_PIN(pin) && \
        ((((uint8)(port)) != PORTE_INDEX) || (((uint8)(pin)) < PORTE_PIN_MAX_NUMBER)))

/** @brief Check if direction is valid */
#define IS_VALID_DIRECTION(dir)     (((uint8)(dir)) <= GPIO_DIRECTION_INPUT)

/** @brief Check if logic is valid */
#define IS_VALID_LOGIC(logic)       (((uint8)(logic)) <= GPIO_HIGH)

/* ==================== Section: Data Type Declarations ==================== */

/**
 * @brief   Logic level enumeration.
 *
 * Defines the two possible logic states for a GPIO pin.
 */
typedef enum {
    GPIO_LOW  = 0,      /**< Logic LOW (0V)  */
    GPIO_HIGH = 1       /**< Logic HIGH (5V) */
} logic_t;

/**
 * @brief   Pin direction enumeration.
 */
typedef enum {
    GPIO_DIRECTION_OUTPUT = 0,  /**< Configure pin as output */
    GPIO_DIRECTION_INPUT  = 1   /**< Configure pin as input  */
} direction_t;

/**
 * @brief   Pin index enumeration (0 to 7).
 */
typedef enum {
    GPIO_PIN0 = 0,      /**< Pin 0 */
    GPIO_PIN1,          /**< Pin 1 */
    GPIO_PIN2,          /**< Pin 2 */
    GPIO_PIN3,          /**< Pin 3 */
    GPIO_PIN4,          /**< Pin 4 */
    GPIO_PIN5,          /**< Pin 5 */
    GPIO_PIN6,          /**< Pin 6 */
    GPIO_PIN7           /**< Pin 7 */
} pin_index_t;

/**
 * @brief   Port index enumeration.
 */
typedef enum {
    PORTA_INDEX = 0,    /**< Port A */
    PORTB_INDEX,        /**< Port B */
    PORTC_INDEX,        /**< Port C */
    PORTD_INDEX,        /**< Port D */
    PORTE_INDEX         /**< Port E */
} port_index_t;

/**
 * @brief   Pin configuration structure (8 bits total).
 *
 * @details
 * This structure contains all information needed to configure a single
 * GPIO pin. Uses bitfields to pack all fields into 1 byte of RAM.
 *
 * @note    Total size = 8 bits = 1 byte.
 */
typedef struct {
    uint8 port      : 3;   /**< @brief Port index (@ref port_index_t) */
    uint8 pin       : 3;   /**< @brief Pin index (@ref pin_index_t) */
    uint8 direction : 1;   /**< @brief Direction (@ref direction_t) */
    uint8 logic     : 1;   /**< @brief Logic level (@ref logic_t) */
} pin_config_t;

/* =========================================================================
 * Section: Struct-Based API
 * ========================================================================= */
#if GPIO_STRUCT_API_ENABLE == CONFIG_ENABLE

#if GPIO_PORT_PIN_CONFIGURATIONS == CONFIG_ENABLE

/**
 * @brief   Initialize pin direction only.
 * @param   _pin_config  Pointer to pin configuration structure.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 * @note    Use gpio_pin_setup() for complete initialization.
 */
Std_ReturnType gpio_pin_direction_initialize(const pin_config_t *_pin_config);

/**
 * @brief   Get current pin direction.
 * @param   _pin_config       Pointer to pin configuration.
 * @param   direction_status  Pointer to store the direction.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_get_direction_status(const pin_config_t *_pin_config,
                                              direction_t *direction_status);

/**
 * @brief   Write a logic value to a pin.
 * @param   _pin_config  Pointer to pin configuration.
 * @param   logic        Logic value (GPIO_LOW or GPIO_HIGH).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_write_logic(const pin_config_t *_pin_config, logic_t logic);

/**
 * @brief   Read a logic value from a pin.
 * @param   _pin_config  Pointer to pin configuration.
 * @param   logic        Pointer to store the read logic value.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_read_logic(const pin_config_t *_pin_config, logic_t *logic);

/**
 * @brief   Toggle the state of a pin.
 * @param   _pin_config  Pointer to pin configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_toggle_logic(const pin_config_t *_pin_config);

/**
 * @brief   Full pin initialization (direction + initial logic).
 * @param   _pin_config  Pointer to pin configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_initialize(const pin_config_t *_pin_config);

/**
 * @brief   Reset pin to default state (Input, LOW).
 * @param   _pin_config  Pointer to pin configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_deinitialize(const pin_config_t *_pin_config);

#endif /* GPIO_PORT_PIN_CONFIGURATIONS */

#if GPIO_PORT_CONFIGURATIONS == CONFIG_ENABLE

/**
 * @brief   Initialize port direction mask.
 * @param   port           Port index.
 * @param   direction_mask 8-bit direction mask (0=Output, 1=Input).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_direction_initialize(port_index_t port, uint8 direction_mask);

/**
 * @brief   Get port direction status.
 * @param   port              Port index.
 * @param   direction_status  Pointer to store the direction mask.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_get_direction_status(port_index_t port, uint8 *direction_status);

/**
 * @brief   Write 8-bit value to a port.
 * @param   port        Port index.
 * @param   logic_mask  8-bit value to write.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_write_logic(port_index_t port, uint8 logic_mask);

/**
 * @brief   Read 8-bit value from a port.
 * @param   port   Port index.
 * @param   logic  Pointer to store the value.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_read_logic(port_index_t port, uint8 *logic);

/**
 * @brief   Toggle all pins of a port.
 * @param   port  Port index.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_toggle_logic(port_index_t port);

/**
 * @brief   Full port initialization (direction mask + initial value).
 * @param   port            Port index.
 * @param   direction_mask  Direction mask.
 * @param   initial_value   Initial value.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_initialize(port_index_t port, uint8 direction_mask, uint8 initial_value);

#endif /* GPIO_PORT_CONFIGURATIONS */

#endif /* GPIO_STRUCT_API_ENABLE */

/* =========================================================================
 * Section: Direct API (Simple, Fast, No Struct)
 * ========================================================================= */
#if GPIO_DIRECT_API_ENABLE == CONFIG_ENABLE

/* ---------- Pin Direct APIs ---------- */

/**
 * @brief   Setup a GPIO pin completely (PREFERRED).
 * @param   port       Port index (@ref port_index_t).
 * @param   pin        Pin index (@ref pin_index_t).
 * @param   direction  Direction (@ref direction_t).
 * @param   logic      Initial logic (@ref logic_t) - only if OUTPUT.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 *
 * @par Example:
 * @code
 *   gpio_pin_setup(PORTC_INDEX, GPIO_PIN0,
 *                  GPIO_DIRECTION_OUTPUT, GPIO_LOW);
 * @endcode
 */
Std_ReturnType gpio_pin_setup(port_index_t port,
                               pin_index_t pin,
                               direction_t direction,
                               logic_t logic);

/**
 * @brief   Write logic value to a pin directly.
 */
Std_ReturnType gpio_pin_write(port_index_t port, pin_index_t pin, logic_t logic);

/**
 * @brief   Read logic value from a pin directly.
 */
Std_ReturnType gpio_pin_read(port_index_t port, pin_index_t pin, logic_t *logic);

/**
 * @brief   Toggle a pin directly.
 */
Std_ReturnType gpio_pin_toggle(port_index_t port, pin_index_t pin);

/* ---------- Port Direct APIs ---------- */

/**
 * @brief   Setup a port completely.
 */
Std_ReturnType gpio_port_setup(port_index_t port, uint8 direction_mask, uint8 initial_value);

/**
 * @brief   Write 8-bit value to a port directly.
 */
Std_ReturnType gpio_port_write(port_index_t port, uint8 logic_mask);

/**
 * @brief   Read 8-bit value from a port directly.
 */
Std_ReturnType gpio_port_read(port_index_t port, uint8 *logic);

/**
 * @brief   Toggle all pins of a port directly.
 */
Std_ReturnType gpio_port_toggle(port_index_t port);

#endif /* GPIO_DIRECT_API_ENABLE */

#endif /* GPIO_MODULE_ENABLE */
#endif /* HAL_GPIO_H */
