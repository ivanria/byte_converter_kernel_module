#ifndef CONV_HELPERS_H__
#define CONV_HELPERS_H__

size_t print_conv_bin(char *outbuf, char *inbuf, size_t inbuf_size);

size_t print_conv_octal(char *outbuf, char *inbuf, size_t inbuf_size);

size_t print_conv_dec(char *outbuf, char *inbuf, size_t inbuf_size);

size_t print_conv_hex(char *outbuf, char *inbuf, size_t inbuf_size);

size_t print_conv_raw(char *outbuf, char *inbuf, size_t inbuf_size);

size_t print_conv_ascii(char *outbuf, char *inbuf, size_t inbuf_size);

#endif // CONV_HELPERS_H__
