/**
 * @file    hal_gpio.h
 * @author  Mohammed El Sisi
 * @brief   GPIO Driver Interface for PIC18F4620
 * @version 2.1.0
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
 * Both APIs are fully independent: each can be enabled alone or together
 * from device_config.h.
 *
 * Extras: analog/digital pin selection (ADCON1 PCFG) and PORTB pull-ups.
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

/** @brief Bit mask of the physical pins on PORTE (RE0-RE2) */
#define PORTE_VALID_PIN_MASK        ((uint8)0x07)

/** @brief Marker for a pin that has no analog (ANx) function */
#define GPIO_NO_ANALOG_CHANNEL      ((uint8)0xFF)

/** @brief Highest analog channel on PIC18F4620 (AN12) */
#define GPIO_MAX_ANALOG_CHANNEL     12

/** @brief ADCON1 PCFG<3:0> value: all pins digital */
#define GPIO_PCFG_ALL_DIGITAL       ((uint8)0x0F)

/** @brief ADCON1 PCFG<3:0> field mask (VCFG bits are preserved) */
#define GPIO_PCFG_MASK              ((uint8)0x0F)

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
 * Section: Driver Initialization
 * ========================================================================= */

/**
 * @brief   Initialize the GPIO driver.
 * @details When GPIO_INIT_ALL_DIGITAL is enabled, all analog-capable pins
 *          are switched to digital (ADCON1 PCFG = 0x0F) and the internal
 *          analog-request table is cleared. Call once at startup.
 * @return  E_OK always.
 */
Std_ReturnType gpio_init(void);

/* =========================================================================
 * Section: Direct API (independent)
 * ========================================================================= */

#if GPIO_DIRECT_API_ENABLE == CONFIG_ENABLE

/**
 * @brief   Setup a pin completely (direction + initial logic) - PREFERRED.
 * @details The pin is automatically switched to digital mode, unless the
 *          ADC module is enabled and has claimed the pin as analog.
 * @param   port      Port index (@ref port_index_t).
 * @param   pin       Pin index (@ref pin_index_t).
 * @param   direction Pin direction (@ref direction_t).
 * @param   logic     Initial logic (applied to outputs only).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_setup(port_index_t port,
                               pin_index_t pin,
                               direction_t direction,
                               logic_t logic);

/**
 * @brief   Set pin direction only.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_set_direction(port_index_t port, pin_index_t pin, direction_t direction);

/**
 * @brief   Get pin direction (from TRIS).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_get_direction(port_index_t port, pin_index_t pin, direction_t *direction);

/**
 * @brief   Write logic value to a pin (LAT).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_write(port_index_t port, pin_index_t pin, logic_t logic);

/**
 * @brief   Read logic value from a pin (PORT).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_read(port_index_t port, pin_index_t pin, logic_t *logic);

/**
 * @brief   Read the output latch of a pin (LAT), i.e. the value last written.
 * @note    Useful for outputs whose real pin level may differ (heavy load).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_read_latch(port_index_t port, pin_index_t pin, logic_t *logic);

/**
 * @brief   Toggle a pin (LAT).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_toggle(port_index_t port, pin_index_t pin);

/**
 * @brief   Reset a pin to its default state (Input, LAT = LOW).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_deinit(port_index_t port, pin_index_t pin);

/**
 * @brief   Setup a port completely (direction + initial value).
 * @note    Bits of non-existent pins (e.g. RE3-RE7) are left untouched.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_setup(port_index_t port, uint8 direction_mask, uint8 initial_value);

/**
 * @brief   Set port direction (TRIS), only for pins that exist.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_set_direction(port_index_t port, uint8 direction_mask);

/**
 * @brief   Get port direction (TRIS), non-existent pins read as 0.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_get_direction(port_index_t port, uint8 *direction_mask);

/**
 * @brief   Write 8-bit value to a port (LAT), only for pins that exist.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_write(port_index_t port, uint8 logic_mask);

/**
 * @brief   Read 8-bit value from a port (PORT), non-existent pins read as 0.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_read(port_index_t port, uint8 *logic);

/**
 * @brief   Toggle all existing pins of a port (LAT).
 * @note    Toggling the LAT bit of an input pin has no effect on the pin
 *          until it becomes an output.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_port_toggle(port_index_t port);

#endif /* GPIO_DIRECT_API_ENABLE */

/* =========================================================================
 * Section: Struct-Based API (independent)
 * ========================================================================= */

#if GPIO_STRUCT_API_ENABLE == CONFIG_ENABLE

#if GPIO_PORT_PIN_CONFIGURATIONS == CONFIG_ENABLE

/**
 * @brief   Initialize pin direction only.
 * @param   _pin_config Pointer to pin configuration.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_direction_initialize(const pin_config_t *_pin_config);

/**
 * @brief   Get current pin direction.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_get_direction_status(const pin_config_t *_pin_config,
                                              direction_t *direction_status);

/**
 * @brief   Write a logic value to a pin.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_write_logic(const pin_config_t *_pin_config, logic_t logic);

/**
 * @brief   Read a logic value from a pin.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_read_logic(const pin_config_t *_pin_config, logic_t *logic);

/**
 * @brief   Read the output latch (LAT) of a pin.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_read_latch_logic(const pin_config_t *_pin_config, logic_t *logic);

/**
 * @brief   Toggle the state of a pin.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_toggle_logic(const pin_config_t *_pin_config);

/**
 * @brief   Full pin initialization (digital mode + direction + logic).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_initialize(const pin_config_t *_pin_config);

/**
 * @brief   Reset pin to default state (Input, LOW).
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_deinitialize(const pin_config_t *_pin_config);

#endif /* GPIO_PORT_PIN_CONFIGURATIONS */

#if GPIO_PORT_CONFIGURATIONS == CONFIG_ENABLE

/** @brief Initialize port direction mask. */
Std_ReturnType gpio_port_direction_initialize(port_index_t port, uint8 direction_mask);

/** @brief Get port direction status. */
Std_ReturnType gpio_port_get_direction_status(port_index_t port, uint8 *direction_status);

/** @brief Write 8-bit value to a port. */
Std_ReturnType gpio_port_write_logic(port_index_t port, uint8 logic_mask);

/** @brief Read 8-bit value from a port. */
Std_ReturnType gpio_port_read_logic(port_index_t port, uint8 *logic);

/**
 * @brief Toggle all existing pins of a port (LAT).
 * @note  Toggling the LAT bit of an input pin has no effect on the pin
 *        until it becomes an output.
 */
Std_ReturnType gpio_port_toggle_logic(port_index_t port);

/** @brief Full port initialization. */
Std_ReturnType gpio_port_initialize(port_index_t port, uint8 direction_mask, uint8 initial_value);

#endif /* GPIO_PORT_CONFIGURATIONS */

#endif /* GPIO_STRUCT_API_ENABLE */

/* =========================================================================
 * Section: Analog / Digital Pin Configuration
 * ========================================================================= */

#if GPIO_ANALOG_CONFIG_ENABLE == CONFIG_ENABLE

/**
 * @brief   Request a pin to be analog (ANx input).
 *
 * @details PIC18F4620 selects analog pins with ADCON1<PCFG3:0>, which is a
 *          THRESHOLD scheme (ANn..AN0 analog), not per-pin. The driver keeps
 *          track of every requested analog channel and programs the minimal
 *          PCFG value that makes all of them analog.
 *
 * @warning Hardware limitation: every channel BELOW the highest requested one
 *          becomes analog too (e.g. requesting AN4/RA5 also makes AN0-AN3
 *          analog). Plan pin assignments so analog inputs use low channels.
 *
 * @param   port Port index.
 * @param   pin  Pin index.
 * @return  E_OK if successful, E_NOT_OK if the pin has no ANx function.
 */
Std_ReturnType gpio_pin_set_analog(port_index_t port, pin_index_t pin);

/**
 * @brief   Release a pin back to digital mode and recompute PCFG.
 * @return  E_OK if the pin is now digital (or has no analog function),
 *          E_NOT_OK if invalid or it must stay analog because a higher
 *          channel is still requested analog (threshold limitation).
 */
Std_ReturnType gpio_pin_set_digital(port_index_t port, pin_index_t pin);

/**
 * @brief   Check whether a pin is currently analog in hardware.
 * @return  E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_is_analog(port_index_t port, pin_index_t pin, boolean_t *is_analog);

#endif /* GPIO_ANALOG_CONFIG_ENABLE */

/* =========================================================================
 * Section: PORTB Internal Weak Pull-ups
 * ========================================================================= */

#if GPIO_PORTB_PULLUP_ENABLE == CONFIG_ENABLE

/**
 * @brief   Enable PORTB internal weak pull-ups (INTCON2.RBPU = 0, active low).
 * @note    Pull-ups are applied automatically only to PORTB pins configured
 *          as inputs; outputs are not affected.
 * @return  E_OK always.
 */
Std_ReturnType gpio_portb_pullup_enable(void);

/**
 * @brief   Disable PORTB internal weak pull-ups (INTCON2.RBPU = 1).
 * @return  E_OK always.
 */
Std_ReturnType gpio_portb_pullup_disable(void);

#endif /* GPIO_PORTB_PULLUP_ENABLE */

#endif /* GPIO_MODULE_ENABLE */
#endif /* HAL_GPIO_H */
