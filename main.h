#ifndef MAIN_H__
#define MAIN_H__


// Definitions for buffers size
#define MAIN_UBUF_SIZE 4096
#define TOTAL_LINES (MAIN_UBUF_SIZE / 4) // 4 bytes per line
#define CHAR_PER_LINE (((4 + (4 * 3)) + 1) + 32) // (1 space btw nibbles * 4) +
						 // (4 spaces btw bytes * 3) +
						 // (8 bits per byte * 4) +
						 // 1 new_line
#define UBUF_REPRESENT_SIZE (CHAR_PER_LINE * TOTAL_LINES)
// the broadest representation BITS representation

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




#endif // MAIN_H__
