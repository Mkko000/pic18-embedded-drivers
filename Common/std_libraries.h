/**
 * @file    std_libraries.h
 * @author  Mohammed El Sisi
 * @brief   Standard C Libraries Aggregation
 * @version 2.0.0
 * @date    2026-10-11
 *
 * @details
 * This file aggregates all standard C library includes in one place.
 * Only what the drivers actually use is included by default.
 * This ensures consistency across the project and makes it easy to
 * add or remove libraries.
 *
 * @warning
 * - Some standard C libraries consume significant Flash/RAM.
 * - Use with caution on resource-constrained MCUs.
 */

#ifndef STD_LIBRARIES_H
#define STD_LIBRARIES_H

/* -------- Required by the drivers -------- */
#include <stddef.h>   /**< NULL, size_t */

/* -------- Optional (uncomment only when needed: they cost Flash/RAM) -------- */
/* #include <stdint.h> */   /**< uint8_t, uint16_t ... */
/* #include <string.h> */   /**< memset, memcpy, strlen */
/* #include <stdio.h>  */   /**< printf, sprintf (heavy on PIC18!) */
/* #include <stdlib.h> */   /**< atoi, abs */
/* #include <math.h>   */   /**< sqrt, pow (heavy on PIC18!) */

#endif /* STD_LIBRARIES_H */
