/**
 * @file    std_macros.h
 * @author  Mohammed El Sisi
 * @brief   Standard Bit Manipulation and Register Access Macros
 * @version 2.0.0
 * @date    2026-10-11
 *
 * @details
 * This file provides safe macros for bit manipulation and hardware
 * register access. All macros use full parentheses to prevent
 * operator precedence issues.
 *
 * @warning
 * - Do NOT use these macros with expressions that have side effects
 *   (e.g., SET_BIT(x++, 0)) - undefined behavior may occur.
 *
 * @note
 * - All macros use volatile pointers for hardware registers.
 */

#ifndef STD_MACROS_H
#define STD_MACROS_H

#include "mcal_std_types.h"

/** @brief Single-bit mask value used by the bit macros (ONLY definition). */
#define BIT_MASK                    ((uint8)1)

/* ==================== Bit Manipulation Macros ==================== */

/**
 * @brief   Set a specific bit to 1.
 * @param   REG       Register (e.g., PORTB, TRISC)
 * @param   BIT_POSN  Bit position (0-7)
 * @note    Uses OR operation to set the bit.
 */
#define SET_BIT(REG, BIT_POSN)      ((REG) |=  ((BIT_MASK) << (BIT_POSN)))

/**
 * @brief   Clear a specific bit to 0.
 * @param   REG       Register
 * @param   BIT_POSN  Bit position (0-7)
 * @note    Uses AND with inverted mask.
 */
#define CLEAR_BIT(REG, BIT_POSN)    ((REG) &= ~((BIT_MASK) << (BIT_POSN)))

/**
 * @brief   Toggle a specific bit (0->1 or 1->0).
 * @param   REG       Register
 * @param   BIT_POSN  Bit position (0-7)
 */
#define TOGGLE_BIT(REG, BIT_POSN)   ((REG) ^=  ((BIT_MASK) << (BIT_POSN)))

/**
 * @brief   Read a specific bit (returns 0 or 1).
 * @param   REG       Register
 * @param   BIT_POSN  Bit position (0-7)
 * @return  Bit value (0 or 1)
 */
#define READ_BIT(REG, BIT_POSN)     (((REG) >> (BIT_POSN)) & (BIT_MASK))

/**
 * @brief   Check if a bit is set (returns 1 if set).
 */
#define IS_BIT_SET(REG, BIT_POSN)   (((REG) >> (BIT_POSN)) & (BIT_MASK))

/**
 * @brief   Check if a bit is clear (returns 1 if clear).
 */
#define IS_BIT_CLEAR(REG, BIT_POSN) (!(((REG) >> (BIT_POSN)) & (BIT_MASK)))

/* ==================== Register Access Macros ==================== */

/**
 * @brief   Access an 8-bit hardware register.
 * @param   _X  Memory address of the register
 * @note    Uses volatile to prevent compiler optimization issues.
 */
#define HWRREG8(_X)     (*((volatile uint8 *)(_X)))

/** @brief Access a 16-bit hardware register. */
#define HWRREG16(_X)    (*((volatile uint16 *)(_X)))

/** @brief Access a 32-bit hardware register. */
#define HWRREG32(_X)    (*((volatile uint32 *)(_X)))

/* ==================== Multi-Bit Macros ==================== */

/** @brief Set multiple bits using a mask. */
#define SET_MASK(REG, MASK)         ((REG) |= (MASK))

/** @brief Clear multiple bits using a mask. */
#define CLEAR_MASK(REG, MASK)       ((REG) &= ~(MASK))

/** @brief Toggle multiple bits using a mask. */
#define TOGGLE_MASK(REG, MASK)      ((REG) ^= (MASK))

/** @brief Read a bit field (mask + shift). */
#define GET_MASK(REG, MASK, SHIFT)  (((REG) & (MASK)) >> (SHIFT))

/* ==================== Utility Macros ==================== */

/** @brief Get minimum of two values. */
#define MIN(a, b)                   ((a) < (b) ? (a) : (b))

/** @brief Get maximum of two values. */
#define MAX(a, b)                   ((a) > (b) ? (a) : (b))

/** @brief Swap two variables. @note Uses typeof (GCC/XC8 extension). */
#define SWAP(a, b)                  do { typeof(a) _t = (a); (a) = (b); (b) = _t; } while(0)

/** @brief Create a single-bit mask. */
#define BIT_VALUE(BIT)              (1UL << (BIT))  /* renamed from BIT_MASK(BIT) to avoid clashing with BIT_MASK */

#endif /* STD_MACROS_H */
