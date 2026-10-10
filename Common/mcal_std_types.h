/**
 * @file    mcal_std_types.h
 * @author  Mohammed El Sisi
 * @brief   Standard Data Types and Common Definitions
 * @version 2.0.0
 * @date    2026-10-11
 *
 * @details
 * This file provides platform-independent fixed-width data types,
 * standard return types (following AUTOSAR Std_ReturnType convention),
 * and common macros used throughout the project.
 *
 * @note
 * - All types are guaranteed to have consistent sizes across platforms.
 * - E_OK and E_NOT_OK values follow the AUTOSAR standard.
 *
 * @see https://www.autosar.org/
 */

#ifndef MCAL_STD_TYPES_H
#define MCAL_STD_TYPES_H

/* ==================== Includes ==================== */
#include "std_libraries.h"
#include "compiler.h"

/* ==================== Fixed-Width Data Types ==================== */

/** @brief Unsigned 8-bit integer (0 to 255) */
typedef unsigned char       uint8;

/** @brief Signed 8-bit integer (-128 to 127) */
typedef signed char         sint8;

/** @brief Unsigned 16-bit integer (0 to 65535) */
typedef unsigned short      uint16;

/** @brief Signed 16-bit integer (-32768 to 32767) */
typedef signed short        sint16;

/** @brief Unsigned 32-bit integer (0 to 4294967295) */
typedef unsigned long       uint32;

/** @brief Signed 32-bit integer (-2147483648 to 2147483647) */
typedef signed long         sint32;

/* ==================== AUTOSAR Standard Return Type ==================== */

/**
 * @brief Standard Return Type (AUTOSAR)
 * @note  0x00 = E_NOT_OK, 0x01 = E_OK
 */
typedef uint8 Std_ReturnType;

/* ==================== Boolean Type ==================== */

/**
 * @brief Boolean type definition
 */
typedef enum {
    FALSE = 0,      /**< Logical FALSE */
    TRUE  = 1       /**< Logical TRUE  */
} boolean_t;

/* ==================== Standard Logic Levels ==================== */

#define STD_HIGH        0x01    /**< Logic HIGH (5V) */
#define STD_LOW         0x00    /**< Logic LOW (0V)  */

#define STD_ON          0x01    /**< State ON  */
#define STD_OFF         0x00    /**< State OFF */

#define STD_ACTIVE      0x01    /**< State ACTIVE */
#define STD_IDLE        0x00    /**< State IDLE   */

/* ==================== Configuration Macros ==================== */

#ifndef CONFIG_ENABLE
#define CONFIG_ENABLE   0x01    /**< Configuration ENABLED  */
#endif
#ifndef CONFIG_DISABLE
#define CONFIG_DISABLE  0x00    /**< Configuration DISABLED */
#endif

/* ==================== AUTOSAR Return Values ==================== */

/** @brief Operation successful */
#define E_OK            (Std_ReturnType)0x01

/** @brief Operation failed */
#define E_NOT_OK        (Std_ReturnType)0x00

/* ==================== Utility Macros ==================== */

/** @brief Mark parameter as unused to suppress warnings */
#define UNUSED(x)       ((void)(x))

/** @brief Calculate array size at compile time */
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))

#endif /* MCAL_STD_TYPES_H */
