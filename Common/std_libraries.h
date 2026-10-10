/**
 * @file    std_libraries.h
 * @author  Mohammed El Sisi
 * @brief   Standard C Libraries Aggregation
 * @version 2.0.0
 * @date    2026-10-11
 *
 * @details
 * This file aggregates all standard C library includes in one place.
 * This ensures consistency across the project and makes it easy to
 * add or remove libraries.
 *
 * @warning
 * - Some standard C libraries consume significant Flash/RAM.
 * - Use with caution on resource-constrained MCUs.
 */

#ifndef STD_LIBRARIES_H
#define STD_LIBRARIES_H

#include <stdio.h>    /**< Standard I/O (printf, sprintf) */
#include <string.h>   /**< String operations (strcpy, strlen) */
#include <math.h>     /**< Math functions (sqrt, pow) */
#include <stdlib.h>   /**< General utilities (atoi, malloc) */

#endif /* STD_LIBRARIES_H */
