/**
 * @file    hal_gpio.c
 * @author  Mohammed El Sisi
 * @brief   GPIO Driver Implementation for PIC18F4620
 * @version 2.0.0
 * @date    2026-10-11
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * This file contains the implementation of the GPIO driver for PIC18F4620.
 * It supports two API styles: Struct-based and Direct.
 *
 * @note
 * - Register arrays provide fast O(1) lookup for register addresses.
 * - LAT registers are used for writes to avoid RMW issues.
 * - PORT registers are used for reads to get actual pin states.
 *
 * @warning
 * - Do not modify register arrays from outside this file.
 * - Always validate parameters before use.
 */

#include "hal_gpio.h"

#if GPIO_MODULE_ENABLE == CONFIG_ENABLE

/* =========================================================================
 * Register Arrays (Internal)
 * ========================================================================= */

/**
 * @brief Arrays of pointers to TRIS registers (direction control).
 * @note  Index: 0=TRISA, 1=TRISB, 2=TRISC, 3=TRISD, 4=TRISE
 */
volatile uint8 *tris_registers[] = {&TRISA, &TRISB, &TRISC, &TRISD, &TRISE};

/**
 * @brief Arrays of pointers to LAT registers (output latch).
 * @note  Used for WRITE operations to avoid RMW issues.
 */
volatile uint8 *lat_registers[]  = {&LATA,  &LATB,  &LATC,  &LATD,  &LATE};

/**
 * @brief Arrays of pointers to PORT registers (pin status).
 * @note  Used for READ operations to get actual pin state.
 */
volatile uint8 *port_registers[] = {&PORTA, &PORTB, &PORTC, &PORTD, &PORTE};

/* =========================================================================
 * ==================== STRUCT-BASED API IMPLEMENTATION ====================
 * ========================================================================= */

#if GPIO_STRUCT_API_ENABLE == CONFIG_ENABLE

#if GPIO_PORT_PIN_CONFIGURATIONS == CONFIG_ENABLE

/**
 * @brief Initialize pin direction only.
 * @param _pin_config Pointer to pin configuration.
 * @return E_OK if successful, E_NOT_OK otherwise.
 */
Std_ReturnType gpio_pin_direction_initialize(const pin_config_t *_pin_config) {
    Std_ReturnType ret = E_OK;

    /* -------- Parameter Validation -------- */
    if (NULL == _pin_config) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(_pin_config->port) ||
             !IS_VALID_PORT_PIN(_pin_config->port, _pin_config->pin)  ||
             !IS_VALID_DIRECTION(_pin_config->direction)) {
        ret = E_NOT_OK;
    }
    else {
        /* -------- Configure Direction -------- */
        /* Use switch for clarity (compiler generates efficient code) */
        switch (_pin_config->direction) {
            case GPIO_DIRECTION_OUTPUT:
                /* Set TRIS bit to 0 (Output) */
                CLEAR_BIT(*(tris_registers[_pin_config->port]), _pin_config->pin);
                break;

            case GPIO_DIRECTION_INPUT:
                /* Set TRIS bit to 1 (Input) */
                SET_BIT(*(tris_registers[_pin_config->port]), _pin_config->pin);
                break;

            default:
                ret = E_NOT_OK;
                break;
        }
    }

    return ret;
}

/**
 * @brief Get current pin direction.
 */
Std_ReturnType gpio_pin_get_direction_status(const pin_config_t *_pin_config,
                                              direction_t *direction_status) {
    Std_ReturnType ret = E_OK;

    if (NULL == _pin_config || NULL == direction_status) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(_pin_config->port) || !IS_VALID_PORT_PIN(_pin_config->port, _pin_config->pin)) {
        ret = E_NOT_OK;
    }
    else {
        /* Read TRIS bit: 0=Output, 1=Input */
        if (READ_BIT(*(tris_registers[_pin_config->port]), _pin_config->pin) == 0) {
            *direction_status = GPIO_DIRECTION_OUTPUT;
        } else {
            *direction_status = GPIO_DIRECTION_INPUT;
        }
    }

    return ret;
}

/**
 * @brief Write a logic value to a pin.
 */
Std_ReturnType gpio_pin_write_logic(const pin_config_t *_pin_config, logic_t logic) {
    Std_ReturnType ret = E_OK;

    if (NULL == _pin_config) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(_pin_config->port) ||
             !IS_VALID_PORT_PIN(_pin_config->port, _pin_config->pin)  ||
             !IS_VALID_LOGIC(logic)) {
        ret = E_NOT_OK;
    }
    else {
        /* Use LAT register (not PORT) to avoid RMW issue */
        switch (logic) {
            case GPIO_LOW:
                CLEAR_BIT(*(lat_registers[_pin_config->port]), _pin_config->pin);
                break;
            case GPIO_HIGH:
                SET_BIT(*(lat_registers[_pin_config->port]), _pin_config->pin);
                break;
            default:
                ret = E_NOT_OK;
                break;
        }
    }

    return ret;
}

/**
 * @brief Read a logic value from a pin.
 */
Std_ReturnType gpio_pin_read_logic(const pin_config_t *_pin_config, logic_t *logic) {
    Std_ReturnType ret = E_OK;

    if (NULL == _pin_config || NULL == logic) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(_pin_config->port) || !IS_VALID_PORT_PIN(_pin_config->port, _pin_config->pin)) {
        ret = E_NOT_OK;
    }
    else {
        /* Read from PORT (actual pin state) */
        if (READ_BIT(*(port_registers[_pin_config->port]), _pin_config->pin) == 1) {
            *logic = GPIO_HIGH;
        } else {
            *logic = GPIO_LOW;
        }
    }

    return ret;
}

/**
 * @brief Toggle the state of a pin.
 */
Std_ReturnType gpio_pin_toggle_logic(const pin_config_t *_pin_config) {
    Std_ReturnType ret = E_OK;

    if (NULL == _pin_config) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(_pin_config->port) || !IS_VALID_PORT_PIN(_pin_config->port, _pin_config->pin)) {
        ret = E_NOT_OK;
    }
    else {
        TOGGLE_BIT(*(lat_registers[_pin_config->port]), _pin_config->pin);
    }

    return ret;
}

/**
 * @brief Full pin initialization.
 */
Std_ReturnType gpio_pin_initialize(const pin_config_t *_pin_config) {
    Std_ReturnType ret = E_OK;

    if (NULL == _pin_config) {
        ret = E_NOT_OK;
    }
    else {
        /* Step 1: Set direction */
        ret = gpio_pin_direction_initialize(_pin_config);

        /* Step 2: Set initial logic (if direction was set) */
        if (E_OK == ret) {
            ret = gpio_pin_write_logic(_pin_config, _pin_config->logic);
        }
    }

    return ret;
}

/**
 * @brief Reset pin to default state (Input, LOW).
 */
Std_ReturnType gpio_pin_deinitialize(const pin_config_t *_pin_config) {
    Std_ReturnType ret = E_OK;

    if (NULL == _pin_config) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(_pin_config->port) || !IS_VALID_PORT_PIN(_pin_config->port, _pin_config->pin)) {
        ret = E_NOT_OK;
    }
    else {
        /* Set as Input */
        SET_BIT(*(tris_registers[_pin_config->port]), _pin_config->pin);
        /* Set value to LOW */
        CLEAR_BIT(*(lat_registers[_pin_config->port]), _pin_config->pin);
    }

    return ret;
}

#endif /* GPIO_PORT_PIN_CONFIGURATIONS */

#if GPIO_PORT_CONFIGURATIONS == CONFIG_ENABLE

/**
 * @brief Initialize port direction mask.
 */
Std_ReturnType gpio_port_direction_initialize(port_index_t port, uint8 direction_mask) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        /* Write direction mask to TRIS register */
        *(tris_registers[port]) = direction_mask;
    }

    return ret;
}

/**
 * @brief Get port direction status.
 */
Std_ReturnType gpio_port_get_direction_status(port_index_t port, uint8 *direction_status) {
    Std_ReturnType ret = E_OK;

    if (NULL == direction_status) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *direction_status = *(tris_registers[port]);
    }

    return ret;
}

/**
 * @brief Write 8-bit value to a port.
 */
Std_ReturnType gpio_port_write_logic(port_index_t port, uint8 logic_mask) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *(lat_registers[port]) = logic_mask;
    }

    return ret;
}

/**
 * @brief Read 8-bit value from a port.
 */
Std_ReturnType gpio_port_read_logic(port_index_t port, uint8 *logic) {
    Std_ReturnType ret = E_OK;

    if (NULL == logic) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *logic = *(port_registers[port]);
    }

    return ret;
}

/**
 * @brief Toggle all pins of a port.
 */
Std_ReturnType gpio_port_toggle_logic(port_index_t port) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *(lat_registers[port]) = ~(*(lat_registers[port]));
    }

    return ret;
}

/**
 * @brief Full port initialization.
 */
Std_ReturnType gpio_port_initialize(port_index_t port, uint8 direction_mask, uint8 initial_value) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *(tris_registers[port]) = direction_mask;
        *(lat_registers[port])  = initial_value;
    }

    return ret;
}

#endif /* GPIO_PORT_CONFIGURATIONS */

#endif /* GPIO_STRUCT_API_ENABLE */

/* =========================================================================
 * ======================== DIRECT API IMPLEMENTATION =====================
 * ========================================================================= */

#if GPIO_DIRECT_API_ENABLE == CONFIG_ENABLE

/**
 * @brief Setup a pin completely (direction + initial logic) - PREFERRED.
 */
Std_ReturnType gpio_pin_setup(port_index_t port,
                               pin_index_t pin,
                               direction_t direction,
                               logic_t logic) {
    Std_ReturnType ret = E_OK;

    /* -------- Range Check -------- */
    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_DIRECTION(direction)) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_LOGIC(logic)) {
        ret = E_NOT_OK;
    }
    else {
        /* -------- Step 1: Set Direction -------- */
        switch (direction) {
            case GPIO_DIRECTION_OUTPUT:
                CLEAR_BIT(*(tris_registers[port]), pin);    /* 0 = Output */
                break;
            case GPIO_DIRECTION_INPUT:
                SET_BIT(*(tris_registers[port]), pin);      /* 1 = Input */
                break;
            default:
                ret = E_NOT_OK;
                break;
        }

        /* -------- Step 2: Set Initial Logic (only for OUTPUT) -------- */
        if ((E_OK == ret) && (GPIO_DIRECTION_OUTPUT == direction)) {
            switch (logic) {
                case GPIO_LOW:
                    CLEAR_BIT(*(lat_registers[port]), pin);
                    break;
                case GPIO_HIGH:
                    SET_BIT(*(lat_registers[port]), pin);
                    break;
                default:
                    ret = E_NOT_OK;
                    break;
            }
        }
    }

    return ret;
}

/**
 * @brief Write logic value to a pin directly.
 */
Std_ReturnType gpio_pin_write(port_index_t port, pin_index_t pin, logic_t logic) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port) || !IS_VALID_PORT_PIN(port, pin) || !IS_VALID_LOGIC(logic)) {
        ret = E_NOT_OK;
    }
    else {
        switch (logic) {
            case GPIO_LOW:
                CLEAR_BIT(*(lat_registers[port]), pin);
                break;
            case GPIO_HIGH:
                SET_BIT(*(lat_registers[port]), pin);
                break;
            default:
                ret = E_NOT_OK;
                break;
        }
    }

    return ret;
}

/**
 * @brief Read logic value from a pin directly.
 */
Std_ReturnType gpio_pin_read(port_index_t port, pin_index_t pin, logic_t *logic) {
    Std_ReturnType ret = E_OK;

    if (NULL == logic) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(port) || !IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        *logic = (READ_BIT(*(port_registers[port]), pin) == 1) ? GPIO_HIGH : GPIO_LOW;
    }

    return ret;
}

/**
 * @brief Toggle a pin directly.
 */
Std_ReturnType gpio_pin_toggle(port_index_t port, pin_index_t pin) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port) || !IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        TOGGLE_BIT(*(lat_registers[port]), pin);
    }

    return ret;
}

/**
 * @brief Setup a port completely.
 */
Std_ReturnType gpio_port_setup(port_index_t port, uint8 direction_mask, uint8 initial_value) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *(tris_registers[port]) = direction_mask;
        *(lat_registers[port])  = initial_value;
    }

    return ret;
}

/**
 * @brief Write 8-bit value to a port directly.
 */
Std_ReturnType gpio_port_write(port_index_t port, uint8 logic_mask) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *(lat_registers[port]) = logic_mask;
    }

    return ret;
}

/**
 * @brief Read 8-bit value from a port directly.
 */
Std_ReturnType gpio_port_read(port_index_t port, uint8 *logic) {
    Std_ReturnType ret = E_OK;

    if (NULL == logic) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *logic = *(port_registers[port]);
    }

    return ret;
}

/**
 * @brief Toggle all pins of a port directly.
 */
Std_ReturnType gpio_port_toggle(port_index_t port) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *(lat_registers[port]) = ~(*(lat_registers[port]));
    }

    return ret;
}

#endif /* GPIO_DIRECT_API_ENABLE */

#endif /* GPIO_MODULE_ENABLE */
