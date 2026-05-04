/* SPDX-License-Identifier: GPL-3.0 */
#ifndef BIT_MACROS_H__
#define BIT_MACROS_H__

// Check bit on n position. Double negation transform any not null value to 1
// null still as is
#define CHECK_BIT(mask, n) (!!(mask) & BIT(n))

// Input mask
#define IS_SET_INPUT_RAW(m)	CHECK_BIT(m, 10) // or ASCII input
#define IS_SET_INPUT_BIT(m)	CHECK_BIT(m, 9)
#define IS_SET_INPUT_OCTAL(m)	CHECK_BIT(m, 8)
#define IS_SET_INPUT_DEC(m)	CHECK_BIT(m, 7)
#define IS_SET_INPUT_HEX(m)	CHECK_BIT(m, 6)

// Output mask
#define IS_SET_OUTPUT_RAW(m)	CHECK_BIT(m, 5)
#define IS_SET_OUTPUT_ASCII(m)	CHECK_BIT(m, 4)
#define IS_SET_OUTPUT_BIT(m)	CHECK_BIT(m, 3)
#define IS_SET_OUTPUT_OCTAL(m)	CHECK_BIT(m, 2)
#define IS_SET_OUTPUT_DEC(m)	CHECK_BIT(m, 1)
#define IS_SET_OUTPUT_HEX(m)	CHECK_BIT(m, 0)

#endif // BIT_MACROS_H__
