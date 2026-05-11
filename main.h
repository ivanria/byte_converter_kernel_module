/* SPDX-License-Identifier: GPL-3.0 */
#ifndef MAIN_H__
#define MAIN_H__

// Definitions for buffers size
#define MAIN_UBUF_SIZE			4096

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
	CONV_RAW	= 4.
};


#endif // MAIN_H__
