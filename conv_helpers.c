#include <linux/types.h>
#include <linux/sprintf.h>

#include "main.h"

static size_t print_conv_bin(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte;
	char *p = outbuf;
	ssize_t bit, i;

	for (i = 0; i < (ssize_t)inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 4 == 0)
			p += scnprintf(p, OFFSET_STR_BUF, "0x%04x ",
					(unsigned int)i);
		for (bit = 7; bit >= 0; bit--) {
			*p++ = (byte & (1 << bit)) ? '1' : '0';
			if (bit == 4) *p++ = ' ';
		}

		if ((i + 1) % 4 == 0) {
			*p++ = '\n';
		} else if (i != (ssize_t)inbuf_size - 1) {
			*p++ = ' '; *p++ = ' '; *p++ = ' '; *p++ = ' ';
		}
	}

	if ((i % 4) == 0) {
		*p++ = '\0'; // Have not remains
	} else {
	*p++ = '\n'; *p++ = '\0'; // Have remains
	}
	return p - outbuf;
}

static size_t print_conv_octal(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte;
	char *p = outbuf;
	ssize_t i;
	for (i = 0; i < (ssize_t)inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 8 == 0)
			p += scnprintf(p, OFFSET_STR_BUF, "0x%04x ",
					(unsigned int)i);
		*p++ = 'o';
		*p++ = ((byte >> 6) & 7) + '0';
		*p++ = ((byte >> 3) & 7) + '0';
		*p++ = (byte & 7) + '0';

		if ((i + 1) % 8 == 0) {
			*p++ = '\n';
		} else if (i != (ssize_t)inbuf_size - 1) {
			*p++ = ' '; *p++ = ' ';
		}
	}

	if ((i % 8) == 0) {
		*p++ = '\0'; // Have not remains
	} else {
	*p++ = '\n'; *p++ = '\0'; // Have remains
	}
	return p - outbuf;
}

static size_t print_conv_dec(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte;
	char *p = outbuf;
	ssize_t i;
	for (i = 0; i < (ssize_t)inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 10 == 0)
			p += scnprintf(p, OFFSET_STR_BUF, "0x%04x ",
					(unsigned int)i);

		*p++ = (byte / 100) + '0';
		*p++ = ((byte / 10) % 10) + '0';
		*p++ = (byte % 10) + '0';

		if ((i + 1) % 10 == 0) {
			*p++ = '\n';
		} else if (i != (ssize_t)inbuf_size - 1) {
			*p++ = ' '; *p++ = ' ';
		}
	}

	if ((i % 10) == 0) {
		*p++ = '\0'; // Have not remains
	} else {
	*p++ = '\n'; *p++ = '\0'; // Have remains
	}
	return p - outbuf;
}

static size_t print_conv_hex(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte, nibble;
	char *p = outbuf;
	ssize_t i;
	for (i = 0; i < (ssize_t)inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 16 == 0)
			p += scnprintf(p, OFFSET_STR_BUF_H, "0x%04x   ",
					(unsigned int)i);

		nibble = ((byte >> 4) & 15);
		*p++ = (nibble < 10) ? (nibble += '0') : (nibble - 10 + 'A');

		nibble = (byte & 15);
		*p++ = (nibble < 10) ? (nibble += '0') : (nibble - 10 + 'A');

		if ((i + 1) % 16 == 0) {
			*p++ = '\n';
		} else if (i != (ssize_t)inbuf_size - 1) {
			if ((i + 1) % 4 == 0) {
				*p++ = ' '; *p++ = ' ';
			} else {
				*p++ = ' ';
			}
		}
	}

	if ((i % 16) == 0) {
		*p++ = '\0'; // Have not remains
	} else {
	*p++ = '\n'; *p++ = '\0'; // Have remains
	}
	return p - outbuf;
}


