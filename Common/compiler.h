/**
 * @file    compiler.h
 * @author  Mohammed El Sisi
 * @brief   Compiler Specific Definitions and Platform Abstraction
 * @version 2.0.0
 * @date    2026-10-11
 *
 * @details
 * This file provides compiler-specific definitions to make the code
 * portable across different compilers (XC8, XC16, XC32, GCC, etc.).
 *
 * @note
 * - Add new compiler support here as needed.
 * - Do not include this file directly; include mcal_std_types.h instead.
 */

#ifndef COMPILER_H
#define COMPILER_H

/* ==================== Compiler Detection ==================== */
#if defined(__XC8__)
    #define COMPILER_XC8
    #define COMPILER_NAME       "XC8"
#elif defined(__XC16__)
    #define COMPILER_XC16
    #define COMPILER_NAME       "XC16"
#elif defined(__XC32__)
    #define COMPILER_XC32
    #define COMPILER_NAME       "XC32"
#elif defined(__GNUC__)
    #define COMPILER_GCC
    #define COMPILER_NAME       "GCC"
#else
    #define COMPILER_UNKNOWN
    #define COMPILER_NAME       "Unknown"
#endif

/* ==================== Inline Functions ==================== */
#if defined(COMPILER_XC8) || defined(COMPILER_GCC)
    #define INLINE              static inline
#else
    #define INLINE              static
#endif

/* ==================== Force Inline (Critical Functions) ==================== */
#if defined(COMPILER_XC8) || defined(COMPILER_GCC)
    #define ALWAYS_INLINE       __attribute__((always_inline)) static inline
#else
    #define ALWAYS_INLINE       INLINE
#endif

/* ==================== Interrupt Handling ==================== */
#if defined(COMPILER_XC8)
    #define INTERRUPT_HANDLER   void __interrupt()
    #define ISR_ATTRIBUTE       __interrupt
#elif defined(COMPILER_GCC)
    #define INTERRUPT_HANDLER   void __attribute__((interrupt))
    #define ISR_ATTRIBUTE       __attribute__((interrupt))
#else
    #define INTERRUPT_HANDLER   void
    #define ISR_ATTRIBUTE
#endif

/* ==================== NOP Instruction ==================== */
#if defined(COMPILER_XC8)
    #define NOP()               __asm("NOP")
#elif defined(COMPILER_GCC)
    #define NOP()               __asm__ __volatile__("nop")
#else
    #define NOP()
#endif

#endif /* COMPILER_H */
