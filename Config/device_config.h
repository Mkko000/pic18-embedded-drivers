/**
 * @file    device_config.h
 * @author  Mohammed El Sisi
 * @brief   Single Point of Configuration for the Entire Project
 * @version 2.0.0
 * @date    2026-10-11
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * This file serves as the SINGLE POINT of configuration for the entire
 * project. All features can be enabled or disabled from this file without
 * modifying any other source file.
 *
 * The pattern used is called "Feature Toggle" or "Configuration Pattern".
 *
 * @note
 * - Change CONFIG_ENABLE / CONFIG_DISABLE values only.
 * - Do not modify other files to enable/disable features.
 *
 * @warning
 * - Some features depend on others (e.g., LED depends on GPIO).
 * - Verify dependencies before disabling any module.
 */

#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include "../Common/mcal_std_types.h"

/* =========================================================================
 * SECTION 1: COMMON CONFIGURATION
 * ========================================================================= */

/**
 * @brief Feature Toggle Values
 */
/* CONFIG_ENABLE / CONFIG_DISABLE are defined ONLY in Common/mcal_std_types.h */

/**
 * @brief Platform Selection
 */
#define PLATFORM_PIC18F     1
#define PLATFORM_DSPIC      2
#define PLATFORM_PIC32      3

#define PLATFORM            PLATFORM_PIC18F

/**
 * @brief Clock Frequency in Hz
 * @note  Must match the crystal oscillator used.
 */
#define _XTAL_FREQ          4000000UL

/**
 * @brief Debug Mode Configuration
 * @note  Enables debug logging and printf statements.
 */
#define DEBUG_MODE          CONFIG_DISABLE

/* =========================================================================
 * SECTION 2: MCAL LAYER CONFIGURATION
 * ========================================================================= */

/* -------- GPIO Module -------- */
#define GPIO_MODULE_ENABLE              CONFIG_ENABLE
#define GPIO_STRUCT_API_ENABLE          CONFIG_ENABLE
#define GPIO_DIRECT_API_ENABLE          CONFIG_ENABLE
#define GPIO_PORT_PIN_CONFIGURATIONS    CONFIG_ENABLE
#define GPIO_PORT_CONFIGURATIONS        CONFIG_ENABLE

/* -------- ADC Module -------- */
#define ADC_MODULE_ENABLE               CONFIG_DISABLE

/* -------- Timer Module -------- */
#define TIMER0_MODULE_ENABLE            CONFIG_DISABLE
#define TIMER1_MODULE_ENABLE            CONFIG_DISABLE
#define TIMER2_MODULE_ENABLE            CONFIG_DISABLE
#define TIMER3_MODULE_ENABLE            CONFIG_DISABLE

/* -------- UART Module -------- */
#define UART_MODULE_ENABLE              CONFIG_DISABLE

/* -------- SPI Module -------- */
#define SPI_MODULE_ENABLE               CONFIG_DISABLE

/* -------- I2C Module -------- */
#define I2C_MODULE_ENABLE               CONFIG_DISABLE

/* =========================================================================
 * SECTION 3: ECU LAYER CONFIGURATION
 * ========================================================================= */

/* -------- LED Module -------- */
#define LED_MODULE_ENABLE               CONFIG_ENABLE
#define LED_ACTIVE_HIGH                 CONFIG_ENABLE
#define LED_ACTIVE_LOW                  CONFIG_DISABLE

/* -------- Button Module -------- */
#define BUTTON_MODULE_ENABLE            CONFIG_DISABLE

/* -------- LCD Module -------- */
#define LCD_MODULE_ENABLE               CONFIG_DISABLE

/* -------- Motor Module -------- */
#define MOTOR_MODULE_ENABLE             CONFIG_DISABLE

/* -------- Sensor Module -------- */
#define SENSOR_MODULE_ENABLE            CONFIG_DISABLE

/* =========================================================================
 * SECTION 4: LIBRARY CONFIGURATION
 * ========================================================================= */

#define MATH_UTILS_MODULE_ENABLE        CONFIG_DISABLE
#define CRC_MODULE_ENABLE               CONFIG_DISABLE

/* =========================================================================
 * SECTION 5: APPLICATION LAYER CONFIGURATION
 * ========================================================================= */

#define APP_MODE_NORMAL                 CONFIG_ENABLE
#define APP_MODE_TEST                   CONFIG_DISABLE
#define APP_MODE_DEBUG                  CONFIG_DISABLE

/* =========================================================================
 * DEPENDENCY CHECKS
 * ========================================================================= */

#if (LED_MODULE_ENABLE == CONFIG_ENABLE) && (GPIO_MODULE_ENABLE != CONFIG_ENABLE)
    #error "LED_MODULE requires GPIO_MODULE to be enabled!"
#endif

#if (BUTTON_MODULE_ENABLE == CONFIG_ENABLE) && (GPIO_MODULE_ENABLE != CONFIG_ENABLE)
    #error "BUTTON_MODULE requires GPIO_MODULE to be enabled!"
#endif

#if (LED_MODULE_ENABLE == CONFIG_ENABLE) && (GPIO_DIRECT_API_ENABLE != CONFIG_ENABLE)
    #error "LED_MODULE requires GPIO_DIRECT_API to be enabled!"
#endif

#endif /* DEVICE_CONFIG_H */
