/* SPDX-License-Identifier: GPL-3.0 */
#ifndef MAIN_H__
#define MAIN_H__

// Definitions for buffers size
#define MAIN_UBUF_SIZE		4096

#define OFFSET_STR_LEN		7
#define OFFSET_STR_BUF		(OFFSET_STR_LEN + 1)

// Bit representation calculate buffer size macros
#define RESTB(x)		((x) % 4)

// 9 char per byte, 4 spaces btw bytes, 4 bytes print on string
#define OUT_BUF_SIZE_BIN(bc) \
	(((bc) / 4) * (9 * 4 + 4 * 3 + OFFSET_STR_LEN + 1) + REST_SIZE_BIN(bc))

// Octal representation calculate buffer size macros
#define RESTO(x)		((x) % 8)

#define REST_SIZE_OCTAL(x) \
	((RESTO(x) * 4) + ((RESTO(x) == 0) ? (0) : \
		((RESTO(x) - 1) * 2 + OFFSET_STR_LEN + 1)))

// 4 char per byte, 2 space btw bytes, 8 bytes print on string
#define OUT_BUF_SIZE_OCTAL(bc) \
	(((bc) / 8) * (4 * 8 + 2 * 7 + OFFSET_STR_LEN + 1) + \
	 REST_SIZE_OCTAL(bc))

// Dec representation calculate buffer size macros
#define RESTD(x)		((x) % 10)

#define REST_SIZE_DEC(x) \
	((RESTD(x) * 3) + ((RESTD(x) == 0) ? (0) : \
		((RESTO(x) - 1) * 2 + OFFSET_STR_LEN + 1)))

// 3 char per byte, 2 space btw bytes, 10 bytes print on string
#define OUT_BUF_SIZE_DEC(bc) \
	(((bc) / 10) * (3 * 10 + 2 * 9 + OFFSET_STR_LEN + 1) + \
	 REST_SIZE_DEC(bc))

// Hex representation calculate buffer size macros
#define RESTH(x)                ((x) % 16)
#define RH(x)			RESTH(x)

#define REST_SIZE_HEX(x) \
        ((RH(x) * 2) + ((RH(x) == 0) ? (0) : ((RH(x) > 0 && RH(x) < 5) ? \
			((RH(x) - 1) + OFFSET_STR_LEN_H + 1) : \
			((RH(x) > 4 && RH(x) < 9) ? \
			 (RH(x) + OFFSET_STR_LEN_H + 1) : \
			 ((RH(x) > 8 && RH(x) < 13) ? \
			  (RH(x) + 1 + OFFSET_STR_LEN_H + 1) : \
			  (RH(x) + 2 + OFFSET_STR_LEN_H + 1))))))
// 2 char per byte, 1 space btw 4 bytes, 2 space btw group of 4 bytes,
// 16 bytes print on string
#define OUT_BUF_SIZE_HEX(bc) \
        (((bc) / 16) * (2 * 16 + 3 * 4 + 2 * 3 + OFFSET_STR_LEN_H + 1) + \
         REST_SIZE_HEX(bc))

// Debug macros
#ifdef BYTE_CONV_DEBUG
	#define _BYTE_CONV_DEBUG_MODE 1
#else
	#define _BYTE_CONV_DEBUG_MODE 0
#endif

#define BC_PR_DEBUG(fmt, ...) \
	do { \
		if (_BYTE_CONV_DEBUG_MODE) { \
			pr_info("[BYTE_CONV_DEB] [%s:%d]: " fmt, \
					__func__, __LINE__, ##__VA_ARGS__); \
		} \
	} while (0)

enum byte_conv_base {
	CONV_HEX	= 0,
	CONV_DEC	= 1,
	CONV_OCTAL	= 2,
	CONV_BIN	= 3,
	CONV_RAW	= 4,
};

#endif // MAIN_H__
