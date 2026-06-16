/* SPDX-License-Identifier: GPL-3.0 */
#ifndef BIT_MACROS_H__
#define BIT_MACROS_H__

// Check bit on n position. Double negation transform any not null value to 1
// null still as is
#define CHECK_BIT(mask, n)		(((mask) >> (n)) & 1)

// Mask manage bit
#define IS_SET_MODE_INCREMENTAL(m)	CHECK_BIT(m, 31)

// Input mask
#define IS_SET_INPUT_RAW(m)		CHECK_BIT(m, 14) // or ASCII input
#define IS_SET_INPUT_BIN(m)		CHECK_BIT(m, 13)
#define IS_SET_INPUT_OCTAL(m)		CHECK_BIT(m, 12)
#define IS_SET_INPUT_DEC(m)		CHECK_BIT(m, 11)
#define IS_SET_INPUT_HEX(m)		CHECK_BIT(m, 10)

// Output groupped by word size (position 8-9, if count from zero)
// ( May be only makes sense if the Little Endian mode enabled)
#define IS_SET_GROUP_1(m)		((((m) >> 8) & 0x3) == 0)
#define IS_SET_GROUP_2(m)		((((m) >> 8) & 0x3) == 1)
#define IS_SET_GROUP_4(m)		((((m) >> 8) & 0x3) == 2)
#define IS_SET_GROUP_8(m)		((((m) >> 8) & 0x3) == 3)

// Output Endianness control bit 0 - Big, 1 - Little
#define IS_SET_OUTPUT_LETTLE(m)		CHECK_BIT(m, 7)
#define IS_SET_OUTPUT_BIG(m)		(!(CHECK_BIT(m, 7)))

// Output add banner and footer
#define IS_SET_OUTPUT_ADDINFO(m)	CHECK_BIT(m, 6)

// Output mask
#define IS_SET_OUTPUT_RAW(m)		CHECK_BIT(m, 5)
#define IS_SET_OUTPUT_ASCII(m)		CHECK_BIT(m, 4)
#define IS_SET_OUTPUT_BIN(m)		CHECK_BIT(m, 3)
#define IS_SET_OUTPUT_OCTAL(m)		CHECK_BIT(m, 2)
#define IS_SET_OUTPUT_DEC(m)		CHECK_BIT(m, 1)
#define IS_SET_OUTPUT_HEX(m)		CHECK_BIT(m, 0)

#endif // BIT_MACROS_H__
