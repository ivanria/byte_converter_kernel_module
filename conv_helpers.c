#include <linux/types.h>
#include <linux/sprintf.h>

#include "main.h"


static void print_conv_bin(char *outbuf, char *inbuf, size_t inbuf_size)
{
	uint8_t byte;
	char *p = outbuf;
	int bit, i;

	for (i = 0; i < inbuf_size; i++) {
		byte = inbuf[i];
		if (i % 4 == 0)
			p += scnprintf(p, OFFSET_STR_BUF, "0x%04x ", i);
		for (bit = 7; bit >= 0; bit--) {
			*p++ = (byte & (1 << bit)) ? '1' : '0';
			if (bit == 4) *p++ = ' ';
		}

		if ((i + 1) % 4 == 0) {
			*p++ = '\n';
		} else if (i != inbuf_size - 1) {
			*p++ = ' '; *p++ = ' '; *p++ = ' '; *p++ = ' ';
		}
	}
	if (i % 4 != 0)
		*p = '\0';
	else
		*(p - 1) = '\0';
}

