/**
 * @file    hal_gpio.c
 * @author  Mohammed El Sisi
 * @brief   GPIO Driver Implementation for PIC18F4620
 * @version 2.1.0
 * @date    2026-10-11
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * This file contains the implementation of the GPIO driver for PIC18F4620.
 * It supports two API styles: Struct-based and Direct. All validation and
 * register access lives in the Direct API; the Struct API only unpacks
 * pin_config_t and forwards the call (single source of truth).
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
 * Register Arrays (Internal, file-local, read-only pointers)
 * ========================================================================= */

/**
 * @brief Pointers to TRIS registers (direction control).
 * @note  Index: 0=TRISA, 1=TRISB, 2=TRISC, 3=TRISD, 4=TRISE
 */
static volatile uint8 * const tris_registers[PORT_MAX_NUMBER] = {&TRISA, &TRISB, &TRISC, &TRISD, &TRISE};

/**
 * @brief Pointers to LAT registers (output latch).
 * @note  Used for WRITE operations to avoid RMW issues.
 */
static volatile uint8 * const lat_registers[PORT_MAX_NUMBER]  = {&LATA,  &LATB,  &LATC,  &LATD,  &LATE};

/**
 * @brief Pointers to PORT registers (pin status).
 * @note  Used for READ operations to get actual pin state.
 */
static volatile uint8 * const port_registers[PORT_MAX_NUMBER] = {&PORTA, &PORTB, &PORTC, &PORTD, &PORTE};

/**
 * @brief Valid (physically present) pin mask per port.
 * @note  PORTE only has RE0-RE2 as I/O (RE3 = MCLR, input only).
 *        Port-wide operations never touch bits outside this mask.
 */
static const uint8 port_valid_pin_mask[PORT_MAX_NUMBER] = {
    0xFF,                   /* PORTA: RA0-RA7 (RA6/RA7 are OSC pins in HS mode) */
    0xFF,                   /* PORTB: RB0-RB7 */
    0xFF,                   /* PORTC: RC0-RC7 */
    0xFF,                   /* PORTD: RD0-RD7 */
    PORTE_VALID_PIN_MASK    /* PORTE: RE0-RE2 */
};

/* =========================================================================
 * Analog / Digital Configuration (Internal)
 * ========================================================================= */

#if GPIO_ANALOG_CONFIG_ENABLE == CONFIG_ENABLE

/**
 * @brief Pin -> analog channel (ANx) lookup table for PIC18F4620.
 * @note  GPIO_NO_ANALOG_CHANNEL marks pins without an analog function.
 *        AN0=RA0 AN1=RA1 AN2=RA2 AN3=RA3 AN4=RA5 AN5=RE0 AN6=RE1 AN7=RE2
 *        AN8=RB2 AN9=RB3 AN10=RB1 AN11=RB4 AN12=RB0
 */
#define NA GPIO_NO_ANALOG_CHANNEL
static const uint8 analog_channel_map[PORT_MAX_NUMBER][PORT_PIN_MAX_NUMBER] = {
    /*            0    1    2    3    4    5    6    7  */
    /* PORTA */ { 0,   1,   2,   3,   NA,  4,   NA,  NA },
    /* PORTB */ { 12,  10,  8,   9,   11,  NA,  NA,  NA },
    /* PORTC */ { NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA },
    /* PORTD */ { NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA },
    /* PORTE */ { 5,   6,   7,   NA,  NA,  NA,  NA,  NA }
};
#undef NA

/**
 * @brief Bit n set = channel ANn has been requested analog by the user.
 */
static uint16 analog_request_mask = 0;

/**
 * @brief Program ADCON1<PCFG3:0> with the minimal threshold that makes
 *        every requested channel analog.
 * @details PCFG = 0x0F -> all digital; PCFG = 0x0E - n -> AN0..ANn analog.
 */
static void gpio_analog_apply(void) {
    uint8 pcfg    = GPIO_PCFG_ALL_DIGITAL;
    sint8 channel;

    /* Find the highest requested channel */
    for (channel = GPIO_MAX_ANALOG_CHANNEL; channel >= 0; channel--) {
        if (analog_request_mask & (uint16)BIT_VALUE(channel)) {
            pcfg = (uint8)(0x0E - (uint8)channel);
            break;
        }
    }

    /* Preserve VCFG bits, update PCFG only */
    ADCON1 = (uint8)((ADCON1 & (uint8)~GPIO_PCFG_MASK) | pcfg);
}

/**
 * @brief Number of the highest channel currently analog in hardware, or -1.
 */
static sint8 gpio_analog_highest_hw_channel(void) {
    uint8 pcfg = (uint8)(ADCON1 & GPIO_PCFG_MASK);
    sint8 highest;

    if (pcfg >= GPIO_PCFG_ALL_DIGITAL) {
        highest = -1;
    } else if (pcfg <= 0x02) {
        highest = GPIO_MAX_ANALOG_CHANNEL;      /* 0x00-0x02: AN0-AN12 analog */
    } else {
        highest = (sint8)(0x0E - pcfg);
    }

    return highest;
}

/**
 * @brief Internal: release a channel and recompute PCFG (no validation).
 * @return E_OK if the pin ends up digital, E_NOT_OK otherwise.
 */
static Std_ReturnType gpio_pin_make_digital(port_index_t port, pin_index_t pin) {
    Std_ReturnType ret     = E_OK;
    uint8          channel = analog_channel_map[port][pin];

    if (GPIO_NO_ANALOG_CHANNEL != channel) {
        analog_request_mask &= (uint16)~BIT_VALUE(channel);
        gpio_analog_apply();

        /* Threshold limitation: still analog if a higher channel is analog */
        if ((sint8)channel <= gpio_analog_highest_hw_channel()) {
            ret = E_NOT_OK;
        }
    }

    return ret;
}

#endif /* GPIO_ANALOG_CONFIG_ENABLE */

/* =========================================================================
 * Driver Initialization
 * ========================================================================= */

Std_ReturnType gpio_init(void) {
#if GPIO_INIT_ALL_DIGITAL == CONFIG_ENABLE
    #if GPIO_ANALOG_CONFIG_ENABLE == CONFIG_ENABLE
    analog_request_mask = 0;
    #endif
    /* All analog-capable pins -> digital (PCFG = 0x0F), keep VCFG bits */
    ADCON1 = (uint8)((ADCON1 & (uint8)~GPIO_PCFG_MASK) | GPIO_PCFG_ALL_DIGITAL);
#endif
    return E_OK;
}

/* =========================================================================
 * ======================== DIRECT API IMPLEMENTATION =====================
 * ========================================================================= */

#if GPIO_DIRECT_API_ENABLE == CONFIG_ENABLE

/**
 * @brief Setup a pin completely (digital mode + direction + initial logic).
 */
Std_ReturnType gpio_pin_setup(port_index_t port,
                               pin_index_t pin,
                               direction_t direction,
                               logic_t logic) {
    Std_ReturnType ret = E_OK;

    /* -------- Range Check -------- */
    if (!IS_VALID_PORT_PIN(port, pin) ||
        !IS_VALID_DIRECTION(direction) ||
        !IS_VALID_LOGIC(logic)) {
        ret = E_NOT_OK;
    }
    else {
#if GPIO_ANALOG_CONFIG_ENABLE == CONFIG_ENABLE
        /* -------- Step 0: Digital mode (unless ADC claimed the pin) -------- */
        uint8 channel = analog_channel_map[port][pin];
    #if ADC_MODULE_ENABLE == CONFIG_ENABLE
        if ((GPIO_NO_ANALOG_CHANNEL == channel) ||
            !(analog_request_mask & (uint16)BIT_VALUE(channel)))
    #endif
        {
            if (GPIO_NO_ANALOG_CHANNEL != channel) {
                (void)gpio_pin_make_digital(port, pin);   /* best effort */
            }
        }
#endif
        /* -------- Step 1: Initial Logic first (glitch-free output) -------- */
        if (GPIO_DIRECTION_OUTPUT == direction) {
            if (GPIO_HIGH == logic) {
                SET_BIT(*(lat_registers[port]), pin);
            } else {
                CLEAR_BIT(*(lat_registers[port]), pin);
            }
            CLEAR_BIT(*(tris_registers[port]), pin);    /* 0 = Output */
        }
        else {
            SET_BIT(*(tris_registers[port]), pin);      /* 1 = Input */
        }
    }

    return ret;
}

/**
 * @brief Set pin direction only.
 */
Std_ReturnType gpio_pin_set_direction(port_index_t port, pin_index_t pin, direction_t direction) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT_PIN(port, pin) || !IS_VALID_DIRECTION(direction)) {
        ret = E_NOT_OK;
    }
    else if (GPIO_DIRECTION_OUTPUT == direction) {
        CLEAR_BIT(*(tris_registers[port]), pin);
    }
    else {
        SET_BIT(*(tris_registers[port]), pin);
    }

    return ret;
}

/**
 * @brief Get pin direction.
 */
Std_ReturnType gpio_pin_get_direction(port_index_t port, pin_index_t pin, direction_t *direction) {
    Std_ReturnType ret = E_OK;

    if (NULL == direction) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        *direction = (READ_BIT(*(tris_registers[port]), pin) == 0) ?
                     GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT;
    }

    return ret;
}

/**
 * @brief Write logic value to a pin directly.
 */
Std_ReturnType gpio_pin_write(port_index_t port, pin_index_t pin, logic_t logic) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT_PIN(port, pin) || !IS_VALID_LOGIC(logic)) {
        ret = E_NOT_OK;
    }
    else if (GPIO_HIGH == logic) {
        SET_BIT(*(lat_registers[port]), pin);
    }
    else {
        CLEAR_BIT(*(lat_registers[port]), pin);
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
    else if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        *logic = (READ_BIT(*(port_registers[port]), pin) == 1) ? GPIO_HIGH : GPIO_LOW;
    }

    return ret;
}

/**
 * @brief Read the output latch (LAT) of a pin.
 */
Std_ReturnType gpio_pin_read_latch(port_index_t port, pin_index_t pin, logic_t *logic) {
    Std_ReturnType ret = E_OK;

    if (NULL == logic) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        *logic = (READ_BIT(*(lat_registers[port]), pin) == 1) ? GPIO_HIGH : GPIO_LOW;
    }

    return ret;
}

/**
 * @brief Toggle a pin directly.
 */
Std_ReturnType gpio_pin_toggle(port_index_t port, pin_index_t pin) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        TOGGLE_BIT(*(lat_registers[port]), pin);
    }

    return ret;
}

/**
 * @brief Reset a pin to default state (Input, LOW).
 */
Std_ReturnType gpio_pin_deinit(port_index_t port, pin_index_t pin) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        SET_BIT(*(tris_registers[port]), pin);      /* Input */
        CLEAR_BIT(*(lat_registers[port]), pin);     /* LOW   */
    }

    return ret;
}

/**
 * @brief Setup a port completely (valid pins only).
 */
Std_ReturnType gpio_port_setup(port_index_t port, uint8 direction_mask, uint8 initial_value) {
    Std_ReturnType ret = gpio_port_write(port, initial_value);   /* latch first */

    if (E_OK == ret) {
        ret = gpio_port_set_direction(port, direction_mask);
    }

    return ret;
}

/**
 * @brief Set port direction (valid pins only).
 */
Std_ReturnType gpio_port_set_direction(port_index_t port, uint8 direction_mask) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        uint8 valid = port_valid_pin_mask[port];
        *(tris_registers[port]) = (uint8)((*(tris_registers[port]) & (uint8)~valid) |
                                          (direction_mask & valid));
    }

    return ret;
}

/**
 * @brief Get port direction (non-existent pins read as 0).
 */
Std_ReturnType gpio_port_get_direction(port_index_t port, uint8 *direction_mask) {
    Std_ReturnType ret = E_OK;

    if (NULL == direction_mask) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        *direction_mask = (uint8)(*(tris_registers[port]) & port_valid_pin_mask[port]);
    }

    return ret;
}

/**
 * @brief Write 8-bit value to a port (valid pins only).
 */
Std_ReturnType gpio_port_write(port_index_t port, uint8 logic_mask) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        uint8 valid = port_valid_pin_mask[port];
        *(lat_registers[port]) = (uint8)((*(lat_registers[port]) & (uint8)~valid) |
                                         (logic_mask & valid));
    }

    return ret;
}

/**
 * @brief Read 8-bit value from a port (non-existent pins read as 0).
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
        *logic = (uint8)(*(port_registers[port]) & port_valid_pin_mask[port]);
    }

    return ret;
}

/**
 * @brief Toggle only the OUTPUT pins of a port (mask with ~TRIS).
 */
Std_ReturnType gpio_port_toggle(port_index_t port) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT(port)) {
        ret = E_NOT_OK;
    }
    else {
        uint8 output_pins = (uint8)(~(*(tris_registers[port])) & port_valid_pin_mask[port]);
        *(lat_registers[port]) ^= output_pins;
    }

    return ret;
}

#endif /* GPIO_DIRECT_API_ENABLE */

/* =========================================================================
 * ============ STRUCT-BASED API (thin wrappers over Direct API) ===========
 * ========================================================================= */

#if GPIO_STRUCT_API_ENABLE == CONFIG_ENABLE

#if GPIO_PORT_PIN_CONFIGURATIONS == CONFIG_ENABLE

/** @brief Unpack port index from a pin_config_t */
#define CFG_PORT(cfg)   ((port_index_t)((cfg)->port))
/** @brief Unpack pin index from a pin_config_t */
#define CFG_PIN(cfg)    ((pin_index_t)((cfg)->pin))

Std_ReturnType gpio_pin_direction_initialize(const pin_config_t *_pin_config) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_set_direction(CFG_PORT(_pin_config), CFG_PIN(_pin_config),
                                  (direction_t)_pin_config->direction);
}

Std_ReturnType gpio_pin_get_direction_status(const pin_config_t *_pin_config,
                                              direction_t *direction_status) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_get_direction(CFG_PORT(_pin_config), CFG_PIN(_pin_config), direction_status);
}

Std_ReturnType gpio_pin_write_logic(const pin_config_t *_pin_config, logic_t logic) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_write(CFG_PORT(_pin_config), CFG_PIN(_pin_config), logic);
}

Std_ReturnType gpio_pin_read_logic(const pin_config_t *_pin_config, logic_t *logic) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_read(CFG_PORT(_pin_config), CFG_PIN(_pin_config), logic);
}

Std_ReturnType gpio_pin_toggle_logic(const pin_config_t *_pin_config) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_toggle(CFG_PORT(_pin_config), CFG_PIN(_pin_config));
}

Std_ReturnType gpio_pin_initialize(const pin_config_t *_pin_config) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_setup(CFG_PORT(_pin_config), CFG_PIN(_pin_config),
                          (direction_t)_pin_config->direction,
                          (logic_t)_pin_config->logic);
}

Std_ReturnType gpio_pin_deinitialize(const pin_config_t *_pin_config) {
    return (NULL == _pin_config) ? E_NOT_OK :
           gpio_pin_deinit(CFG_PORT(_pin_config), CFG_PIN(_pin_config));
}

#endif /* GPIO_PORT_PIN_CONFIGURATIONS */

#if GPIO_PORT_CONFIGURATIONS == CONFIG_ENABLE

Std_ReturnType gpio_port_direction_initialize(port_index_t port, uint8 direction_mask) {
    return gpio_port_set_direction(port, direction_mask);
}

Std_ReturnType gpio_port_get_direction_status(port_index_t port, uint8 *direction_status) {
    return gpio_port_get_direction(port, direction_status);
}

Std_ReturnType gpio_port_write_logic(port_index_t port, uint8 logic_mask) {
    return gpio_port_write(port, logic_mask);
}

Std_ReturnType gpio_port_read_logic(port_index_t port, uint8 *logic) {
    return gpio_port_read(port, logic);
}

Std_ReturnType gpio_port_toggle_logic(port_index_t port) {
    return gpio_port_toggle(port);
}

Std_ReturnType gpio_port_initialize(port_index_t port, uint8 direction_mask, uint8 initial_value) {
    return gpio_port_setup(port, direction_mask, initial_value);
}

#endif /* GPIO_PORT_CONFIGURATIONS */

#endif /* GPIO_STRUCT_API_ENABLE */

/* =========================================================================
 * Analog / Digital Public API
 * ========================================================================= */

#if GPIO_ANALOG_CONFIG_ENABLE == CONFIG_ENABLE

Std_ReturnType gpio_pin_set_analog(port_index_t port, pin_index_t pin) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT_PIN(port, pin) ||
        (GPIO_NO_ANALOG_CHANNEL == analog_channel_map[port][pin])) {
        ret = E_NOT_OK;
    }
    else {
        analog_request_mask |= (uint16)BIT_VALUE(analog_channel_map[port][pin]);
        gpio_analog_apply();
        SET_BIT(*(tris_registers[port]), pin);  /* analog pins must be inputs */
    }

    return ret;
}

Std_ReturnType gpio_pin_set_digital(port_index_t port, pin_index_t pin) {
    Std_ReturnType ret = E_OK;

    if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        ret = gpio_pin_make_digital(port, pin);
    }

    return ret;
}

Std_ReturnType gpio_pin_is_analog(port_index_t port, pin_index_t pin, boolean_t *is_analog) {
    Std_ReturnType ret = E_OK;

    if (NULL == is_analog) {
        ret = E_NOT_OK;
    }
    else if (!IS_VALID_PORT_PIN(port, pin)) {
        ret = E_NOT_OK;
    }
    else {
        uint8 channel = analog_channel_map[port][pin];
        *is_analog = ((GPIO_NO_ANALOG_CHANNEL != channel) &&
                      ((sint8)channel <= gpio_analog_highest_hw_channel())) ? TRUE : FALSE;
    }

    return ret;
}

#endif /* GPIO_ANALOG_CONFIG_ENABLE */

/* =========================================================================
 * PORTB Internal Weak Pull-ups
 * ========================================================================= */

#if GPIO_PORTB_PULLUP_ENABLE == CONFIG_ENABLE

Std_ReturnType gpio_portb_pullup_enable(void) {
    INTCON2bits.RBPU = 0;   /* active low: 0 = pull-ups enabled */
    return E_OK;
}

Std_ReturnType gpio_portb_pullup_disable(void) {
    INTCON2bits.RBPU = 1;   /* 1 = pull-ups disabled */
    return E_OK;
}

#endif /* GPIO_PORTB_PULLUP_ENABLE */

#endif /* GPIO_MODULE_ENABLE */
