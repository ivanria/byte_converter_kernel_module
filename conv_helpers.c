#include <linux/types.h>
#include <linux/sprintf.h>

#include "main.h"


static size_t print_conv_bin(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte;
	char *p = outbuf;
	ssize_t i, shift;

	for (i = 0; i < inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 4 == 0)
			p += scnprintf(p, OFFSET_STR_BUF, "0x%04x ",
					(unsigned int)i);
		for (shift = 7; shift >= 0; shift--) {
			*p++ = (byte & (1 << shift)) ? '1' : '0';
			if (shift == 4) *p++ = ' ';
		}

		if ((i + 1) % 4 == 0) {
			*p++ = '\n';
		} else if (i != (ssize_t)inbuf_size - 1) {
			*p++ = ' '; *p++ = ' '; *p++ = ' '; *p++ = ' ';
		}
	}
	if (i % 4 != 0)
		*p++ = '\0'; // Have rest
	else
		*(p - 1) = '\0'; // Have not rest, replace '\n' to '\0'
	return p - outbuf;
}

static size_t print_conv_octal(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte;
	char *p = outbuf;
	ssize_t shift, i;
	for (i = 0; i < (ssize_t)inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 8 == 0)
			p += scnprintf(p, OFFSET_STR_BUF, "0x%04x ",
					(unsigned int)i);
		*p++ = 'o';
		for (shift = 6; shift >= 0; shift -= 3)
			*p++ = ((byte >> shift) & 7) + '0';
		if ((i + 1) % 8 == 0) {
			*p++ = '\n';
		} else if (i != (ssize_t)inbuf_size - 1) {
			*p++ = ' '; *p++ = ' ';
		}
	}
	if (i % 8 != 0)
		*p++ = '\0'; // Have rest
	else
		*(p - 1) = '\0'; // Have not rest, replace '\n' to '\0'
	return p - outbuf;
}


