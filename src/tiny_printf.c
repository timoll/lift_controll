/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @addtogroup	USART
 * @{
 *
 * @file		printf.c
 *
 * @brief		Atollic TrueSTUDIO Minimal System calls file\n
 *				For more information about which c-functions need which of
 *				these lowlevel functions please consult the Newlib
 *				libc-manual.
 *
 *****************************************************************************
 * @copyright
 * @{
 *
 * The file is distributed as is, without any warranty of any kind.
 *
 * &copy; Copyright Atollic AB.
 * You may use this file as-is or modify it according to the needs of your
 * project. This file may only be built (assembled or compiled and linked)
 * using the Atollic TrueSTUDIO(R) product. The use of this file together
 * with other tools than Atollic TrueSTUDIO(R) is not permitted.
 *
 * @}
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
__attribute__ ((alias("iprintf"))) int printf(const char *fmt, ...);
__attribute__ ((alias("fiprintf"))) int fprintf(FILE* fp, const char *fmt,
                                                  ...);
__attribute__ ((alias("siprintf"))) int sprintf(char* str, const char *fmt,
                                                  ...);
extern int _write(int fd, char *str, int len);
void ts_itoa(char **buf, uint32_t d, uint8_t base);
uint32_t ts_formatstring(char *buf, const char *fmt, va_list va);
uint32_t ts_formatlength(const char *fmt, va_list va);

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		Convert integer to ascii
 * @param[in]	buf
 * @param[in]	d
 * @param[in]	base
 * @return		void
 */
void ts_itoa(char **buf, uint32_t d, uint8_t base) {

	uint32_t div = 1;

	while (d / div >= base) {
		div *= base;
	}

	while (div != 0) {
		uint8_t num = d / div;
		d = d % div;
		div /= base;
		if (num > 9) {
			*((*buf)++) = (num - 10) + 'A';
		}
		else {
			*((*buf)++) = num + '0';
		}
	}
}

/**
 * @brief		Writes arguments va to buffer buf according to format fmt
 * @param[out]	buf		Buffer to store the string in it
 * @param[in]	fmt		Format string
 * @param[in]	va		Arguments for the format string
 * @return		Length of string
 */
uint32_t ts_formatstring(char *buf, const char *fmt, va_list va) {

	char *start_buf = buf;

	while (*fmt) {
		/* Character needs formating? */
		if (*fmt == '%') {
			switch (*(++fmt)) {
			case 'c':
				*buf++ = va_arg(va, int);
				break;
			case 'd':
			case 'i': {
				int32_t val = va_arg(va, signed int);
				if (val < 0) {
					val *= -1;
					*buf++ = '-';
				}
				ts_itoa(&buf, val, 10);
			}
				break;
			case 's': {
				char * arg = va_arg(va, char *);
				while (*arg) {
					*buf++ = *arg++;
				}
			}
				break;
			case 'u':
				ts_itoa(&buf, va_arg(va, unsigned int), 10);
				break;
			case 'x':
			case 'X':
				ts_itoa(&buf, va_arg(va, int), 16);
				break;
			case '%':
				*buf++ = '%';
				break;
			}
			fmt++;
		}
		/* Else just copy */
		else {
			*buf++ = *fmt++;
		}
	}
	*buf = 0;

	return (uint32_t) (buf - start_buf);
}

/**
 * @brief		Calculate maximum length of the resulting string from the
 *				format string and va_list va.
 * @param[in]	fmt		Format string
 * @param[in]	va		Arguments for the format string
 * @return		Maximum length
 */
uint32_t ts_formatlength(const char *fmt, va_list va) {

	uint32_t length = 0;

	while (*fmt) {
		if (*fmt == '%') {
			++fmt;
			switch (*fmt) {
			case 'c':
				va_arg(va, int);
				++length;
				break;
			case 'd':
			case 'i':
			case 'u':
				/* 32 bits integer is max 11 characters with minus sign */
				length += 11;
				va_arg(va, int);
				break;
			case 's': {
				char * str = va_arg(va, char *);
				while (*str++)
					++length;
			}
				break;
			case 'x':
			case 'X':
				/* 32 bits integer as hex is max 8 characters */
				length += 8;
				va_arg(va, unsigned int);
				break;
			default:
				++length;
				break;
			}
		}
		else {
			++length;
		}
		++fmt;
	}

	return length;
}

/**
 * @brief		Loads data from the given locations and writes them to the
 *				given character string according to the format parameter.
 * @param[out]	buf		Buffer to store the string in it
 * @param[in]	fmt		Format string
 * @param[in]	...		Additional parameters
 * @return		Number of bytes written
 */
int siprintf(char *buf, const char *fmt, ...) {

	uint32_t length;
	va_list va;

	va_start(va, fmt);
	length = ts_formatstring(buf, fmt, va);
	va_end(va);

	return length;
}

/**
 * @brief		Loads data from the given locations and writes them to the
 *				given file stream according to the format parameter.
 * @param[out]	stream	The output stream
 * @param[in]	fmt		Format string
 * @param[in]	...		Additional parameters
 * @return		Number of bytes written
 */
int fiprintf(FILE * stream, const char *fmt, ...) {

	uint32_t length = 0;
	va_list va;

	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);

	{
		char buf[length];

		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);
		length = _write(stream->_file, buf, length);
		va_end(va);
	}

	return length;
}

/**
 * @brief		Loads data from the given locations and writes them to the
 *				standard output according to the format parameter.
 * @param[in]	fmt		Format string
 * @param[in]	...		Additional parameters
 * @return		Number of bytes written
 */
int iprintf(const char *fmt, ...) {

	uint32_t length = 0;
	va_list va;

	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);

	{
		char buf[length];

		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);
		length = _write(1, buf, length);
		va_end(va);
	}

	return length;
}

/**
 * @brief		fputs writes the string at s (but without the trailing null)
 *				to the file or stream identified by fp.
 * @param[in]	s		The string to write
 * @param[in]	fp		Pointer to the file or stream
 * @return		If successful, the result is 0; otherwise, the result is EOF.
 */
int fputs(const char *s, FILE *fp) {

	uint32_t length = strlen(s);
	uint32_t wlen = 0;
	int8_t res;

	wlen = _write((fp->_file), (char*) s, length);
	wlen += _write((fp->_file), "\n", 1);

	if (wlen == (length + 1)) {
		res = 0;
	}
	else {
		res = EOF;
	}

	return res;
}

/**
 * @brief		puts writes the string at s (followed by a newline, instead
 *				of the trailing null) to the standard output stream.
 * @param[in]	s		The string to write
 * @return		If successful, the result is a nonnegative integer; otherwise,
 *				the result is EOF.
 */
int puts(const char *s) {

	uint32_t length = strlen(s);
	uint32_t numbytes = 0;
	int8_t res;

	numbytes = _write(1, (char*) s, length);
	numbytes += _write(1, "\n", 1);

	if (numbytes == (length + 1)) {
		res = 0;
	}
	else {
		res = EOF;
	}

	return res;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
