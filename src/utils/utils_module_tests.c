/*
 * utils module tests
 * Copyright (c) 2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/bitfield.h"


struct printf_test_data {
	u8 *data;
	size_t len;
	char *encoded;
};

static const struct printf_test_data printf_tests[] = {
	{ (u8 *) "abcde", 5, "abcde" },
	{ (u8 *) "a\0b\nc\ed\re\tf\"\\", 13, "a\\0b\\nc\\ed\\re\\tf\\\"\\\\" },
	{ (u8 *) "\x00\x31\x00\x32\x00\x39", 6, "\\x001\\0002\\09" },
	{ (u8 *) "\n\n\n", 3, "\n\12\x0a" },
	{ (u8 *) "\303\245\303\244\303\266\303\205\303\204\303\226", 12,
	  "\\xc3\\xa5\xc3\\xa4\\xc3\\xb6\\xc3\\x85\\xc3\\x84\\xc3\\x96" },
	{ (u8 *) "\303\245\303\244\303\266\303\205\303\204\303\226", 12,
	  "\\303\\245\\303\\244\\303\\266\\303\\205\\303\\204\\303\\226" },
	{ (u8 *) "\xe5\xe4\xf6\xc5\xc4\xd6", 6,
	  "\\xe5\\xe4\\xf6\\xc5\\xc4\\xd6" },
	{ NULL, 0, NULL }
};


static int printf_encode_decode_tests(void)
{
	int i;
	size_t binlen;
	char buf[100];
	u8 bin[100];
	int errors = 0;

	wpa_printf(MSG_INFO, "printf encode/decode tests");

	for (i = 0; printf_tests[i].data; i++) {
		const struct printf_test_data *test = &printf_tests[i];
		printf_encode(buf, sizeof(buf), test->data, test->len);
		wpa_printf(MSG_INFO, "%d: -> \"%s\"", i, buf);

		binlen = printf_decode(bin, sizeof(bin), buf);
		if (binlen != test->len ||
		    os_memcmp(bin, test->data, binlen) != 0) {
			wpa_hexdump(MSG_ERROR, "Error in decoding#1",
				    bin, binlen);
			errors++;
		}

		binlen = printf_decode(bin, sizeof(bin), test->encoded);
		if (binlen != test->len ||
		    os_memcmp(bin, test->data, binlen) != 0) {
			wpa_hexdump(MSG_ERROR, "Error in decoding#2",
				    bin, binlen);
			errors++;
		}
	}

	if (errors) {
		wpa_printf(MSG_ERROR, "%d printf test(s) failed", errors);
		return -1;
	}

	return 0;
}


static int bitfield_tests(void)
{
	struct bitfield *bf;
	int i;
	int errors = 0;

	wpa_printf(MSG_INFO, "bitfield tests");

	bf = bitfield_alloc(123);
	if (bf == NULL)
		return -1;

	for (i = 0; i < 123; i++) {
		if (bitfield_is_set(bf, i) || bitfield_is_set(bf, i + 1))
			errors++;
		if (i > 0 && bitfield_is_set(bf, i - 1))
			errors++;
		bitfield_set(bf, i);
		if (!bitfield_is_set(bf, i))
			errors++;
		bitfield_clear(bf, i);
		if (bitfield_is_set(bf, i))
			errors++;
	}

	for (i = 123; i < 200; i++) {
		if (bitfield_is_set(bf, i) || bitfield_is_set(bf, i + 1))
			errors++;
		if (i > 0 && bitfield_is_set(bf, i - 1))
			errors++;
		bitfield_set(bf, i);
		if (bitfield_is_set(bf, i))
			errors++;
		bitfield_clear(bf, i);
		if (bitfield_is_set(bf, i))
			errors++;
	}

	for (i = 0; i < 123; i++) {
		if (bitfield_is_set(bf, i) || bitfield_is_set(bf, i + 1))
			errors++;
		bitfield_set(bf, i);
		if (!bitfield_is_set(bf, i))
			errors++;
	}

	for (i = 0; i < 123; i++) {
		if (!bitfield_is_set(bf, i))
			errors++;
		bitfield_clear(bf, i);
		if (bitfield_is_set(bf, i))
			errors++;
	}

	for (i = 0; i < 123; i++) {
		if (bitfield_get_first_zero(bf) != i)
			errors++;
		bitfield_set(bf, i);
	}
	if (bitfield_get_first_zero(bf) != -1)
		errors++;
	for (i = 0; i < 123; i++) {
		if (!bitfield_is_set(bf, i))
			errors++;
		bitfield_clear(bf, i);
		if (bitfield_get_first_zero(bf) != i)
			errors++;
		bitfield_set(bf, i);
	}
	if (bitfield_get_first_zero(bf) != -1)
		errors++;

	bitfield_free(bf);

	if (errors) {
		wpa_printf(MSG_ERROR, "%d bitfield test(s) failed", errors);
		return -1;
	}

	return 0;
}


int utils_module_tests(void)
{
	int ret = 0;

	wpa_printf(MSG_INFO, "utils module tests");

	if (printf_encode_decode_tests() < 0)
		ret = -1;
	if (bitfield_tests() < 0)
		ret = -1;

	return ret;
}
