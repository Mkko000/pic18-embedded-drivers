/**
 * @file    config_bits.c
 * @author  Mohammed El Sisi
 * @brief   PIC18F4620 Configuration Bits (fuses) - single source for every build
 * @version 1.0.0
 * @date    2026-10-11
 * @target  PIC18F4620
 * @compiler XC8
 *
 * @details
 * XC8 accepts `#pragma config` in any ONE C file of a project. Keeping them
 * here means application.c and every Projects/<NN>/main.c share the exact
 * same fuses. The oscillator is selected with OSC_MODE in device_config.h
 * and checked against _XTAL_FREQ at compile time.
 *
 * @warning Add this file to Source Files EXACTLY ONCE per MPLAB X project.
 *          Adding it twice (or keeping pragmas elsewhere) causes
 *          "multiple definition" / conflicting configuration errors.
 *
 * @copyright Copyright (c) 2026 Mohammed El Sisi. MIT License.
 */

#include <xc.h>
#include "device_config.h"

/* =========================================================================
 * Oscillator Sanity Checks
 * ========================================================================= */
#if !defined(OSC_MODE)
    #error "OSC_MODE is not defined in device_config.h"
#elif OSC_MODE == OSC_MODE_XT
    #if (_XTAL_FREQ > 4000000UL)
        #error "XT mode supports crystals up to 4 MHz - use OSC_MODE_HS"
    #endif
#elif OSC_MODE == OSC_MODE_HS
    #if (_XTAL_FREQ < 4000000UL) || (_XTAL_FREQ > 25000000UL)
        #error "HS mode needs a 4 - 25 MHz crystal (_XTAL_FREQ)"
    #endif
#elif OSC_MODE == OSC_MODE_HSPLL
    #if (_XTAL_FREQ < 16000000UL) || (_XTAL_FREQ > 40000000UL)
        #error "HSPLL: _XTAL_FREQ = 4 x crystal (4 - 10 MHz) -> must be 16 - 40 MHz"
    #endif
#elif OSC_MODE == OSC_MODE_INTIO67
    #if (_XTAL_FREQ != 31000UL)   && (_XTAL_FREQ != 125000UL)  && \
        (_XTAL_FREQ != 250000UL)  && (_XTAL_FREQ != 500000UL)  && \
        (_XTAL_FREQ != 1000000UL) && (_XTAL_FREQ != 2000000UL) && \
        (_XTAL_FREQ != 4000000UL) && (_XTAL_FREQ != 8000000UL)
        #error "INTIO67: _XTAL_FREQ must be 31k, 125k, 250k, 500k, 1M, 2M, 4M or 8M"
    #endif
#else
    #error "Unknown OSC_MODE - use OSC_MODE_XT / HS / HSPLL / INTIO67"
#endif

/* =========================================================================
 * CONFIG1H - Oscillator
 * ========================================================================= */
/** @brief Oscillator source, selected by OSC_MODE in device_config.h. */
#if   OSC_MODE == OSC_MODE_XT
#pragma config OSC = XT         /* XT crystal (<= 4 MHz)                    */
#elif OSC_MODE == OSC_MODE_HS
#pragma config OSC = HS         /* HS crystal (4 - 25 MHz)                  */
#elif OSC_MODE == OSC_MODE_HSPLL
#pragma config OSC = HSPLL      /* HS crystal with 4x PLL                   */
#else
#pragma config OSC = INTIO67    /* Internal oscillator, RA6/RA7 as I/O      */
#endif
/** @brief Fail-Safe Clock Monitor: switch to internal clock if crystal fails. */
#pragma config FCMEN = OFF
/** @brief Internal/External oscillator switchover (two-speed start-up). */
#pragma config IESO = OFF

/* =========================================================================
 * CONFIG2L / CONFIG2H - Power-up, Brown-out, Watchdog
 * ========================================================================= */
/** @brief Power-up Timer: ~65 ms delay after power-on for VDD to settle. */
#pragma config PWRT = OFF
/** @brief Brown-out Reset: reset when VDD drops below BORV. */
#pragma config BOREN = OFF
/** @brief Brown-out voltage level (3 = minimum setting). */
#pragma config BORV = 3
/** @brief Watchdog Timer: OFF (enable + clear with CLRWDT() for robustness). */
#pragma config WDT = OFF
/** @brief Watchdog postscaler (1:32768, only used when WDT = ON). */
#pragma config WDTPS = 32768

/* =========================================================================
 * CONFIG3H - Peripherals and MCLR
 * ========================================================================= */
/** @brief CCP2 output multiplexed on RC1. */
#pragma config CCP2MX = PORTC
/** @brief PORTB<4:0> are DIGITAL I/O on reset (not analog). */
#pragma config PBADEN = OFF
/** @brief Timer1 oscillator in normal (higher power) mode. */
#pragma config LPT1OSC = OFF
/** @brief MCLR pin enabled (RE3 = reset input, needs 10k pull-up). */
#pragma config MCLRE = ON

/* =========================================================================
 * CONFIG4L - Debug, LVP, Stack
 * ========================================================================= */
/** @brief Reset on stack overflow/underflow. */
#pragma config STVREN = ON
/** @brief Low-Voltage Programming OFF: RB5 is a normal I/O pin. */
#pragma config LVP = OFF
/** @brief Extended instruction set OFF (required by XC8). */
#pragma config XINST = OFF
/** @brief Background debugger OFF (RB6/RB7 are normal I/O). */
#pragma config DEBUG = OFF

/* =========================================================================
 * CONFIG5L / CONFIG5H - Code Protection (read-out protection)
 * ========================================================================= */
/** @brief Code protection for blocks 0-3, boot block and data EEPROM: OFF. */
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config CPB = OFF
#pragma config CPD = OFF

/* =========================================================================
 * CONFIG6L / CONFIG6H - Write Protection
 * ========================================================================= */
/** @brief Write protection for blocks 0-3, config regs, boot block, EEPROM: OFF. */
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF

/* =========================================================================
 * CONFIG7L / CONFIG7H - Table Read Protection
 * ========================================================================= */
/** @brief Table-read protection (reads from other blocks) for blocks 0-3 and boot: OFF. */
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF
#pragma config EBTRB = OFF
