#include "msglen.h"
#include "bswap.h"

#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// see http://wiki.msgpack.org/display/MSGPACK/Format+specification

static void
msgpackclen_msg_buf_invalid_type(char **error, unsigned char type)
{
	if (error == NULL) {
		return;
	}

	int len = asprintf(error, "invalid type: %x", (int)type);
	assert(len >= 0);
}

static size_t
msgpackclen_msg_buf_read_elem (void *buf, size_t off, size_t size, char **error);

static size_t
msgpackclen_msg_buf_read_elems (void *buf, size_t off, size_t size, uint32_t n, char **error)
{
	uint32_t i;
	size_t r, total = 0;

	for (i = 0; i < n; i++)
	{
		r = msgpackclen_msg_buf_read_elem (buf, off, size, error);
		if (r == 0)
			return 0;
		off += r;
		total += r;
	}

	return total;
}

static size_t
msgpackclen_msg_buf_read_elem (void *buf, size_t off, size_t size, char **error)
{
	unsigned char type;
	uint32_t n;
	size_t r;

	if (off >= size)
	{
		return 0;
	}

	type = *((unsigned char *)buf + off);
	off++;

	// Positive FixNum 	0xxxxxxx 	0x00 - 0x7f
	if (type <= 0x7f)
	{
		return 1;
	}
	// FixMap 	1000xxxx 	0x80 - 0x8f
	if (type >= 0x80 && type <= 0x8f)
	{
		n = type ^ 0x80;
		if (n == 0)
			return 1;
		r = msgpackclen_msg_buf_read_elems(buf, off, size, n * 2, error);
		return r > 0 ? r + 1 : 0;
	}
	// FixArray 	1001xxxx 	0x90 - 0x9f
	if (type >= 0x90 && type <= 0x9f)
	{
		n = type ^ 0x90;
		if (n == 0)
			return 1;
		r = msgpackclen_msg_buf_read_elems(buf, off, size, n, error);
		return r > 0 ? r + 1 : 0;
	}
	// FixRaw 	101xxxxx 	0xa0 - 0xbf
	if (type >= 0xa0 && type <= 0xbf)
	{
		n = type ^ 0xa0;
		return n + 1;
	}
	// nil 	11000000 	0xc0
	if (type == 0xc0)
	{
		return 1;
	}
	// false 	11000010 	0xc2
	if (type == 0xc2)
	{
		return 1;
	}
	// true 	11000011 	0xc3
	if (type == 0xc3)
	{
		return 1;
	}
	// float 	11001010 	0xca
	if (type == 0xca)
	{
		return 5;
	}
	// double 	11001011 	0xcb
	if (type == 0xcb)
	{
		return 9;
	}
	// uint 8 	11001100 	0xcc
	if (type == 0xcc)
	{
		return 2;
	}
	// uint 16 	11001101 	0xcd
	if (type == 0xcd)
	{
		return 3;
	}
	// uint 32 	11001110 	0xce
	if (type == 0xce)
	{
		return 5;
	}
	// uint 64 	11001111 	0xcf
	if (type == 0xcf)
	{
		return 9;
	}
	// int 8 	11010000 	0xd0
	if (type == 0xd0)
	{
		return 2;
	}
	// int 16 	11010001 	0xd1
	if (type == 0xd1)
	{
		return 3;
	}
	// int 32 	11010010 	0xd2
	if (type == 0xd2)
	{
		return 5;
	}
	// int 64 	11010011 	0xd3
	if (type == 0xd3)
	{
		return 9;
	}
	// raw 16 	11011010 	0xda
	if (type == 0xda)
	{
		if (off + 2 > size)
		{
			return 0;
		}
		n = bswap16 (*(uint16_t *)((char *)buf + off));
		return n + 3;
	}
	// raw 32 	11011011 	0xdb
	if (type == 0xdb)
	{
		if (off + 4 > size)
		{
			return 0;
		}
		n = bswap32 (*(uint32_t *)((char *)buf + off));
		return n + 5;
	}
	// array 16 	11011100 	0xdc
	if (type == 0xdc)
	{
		if (off + 2 > size)
		{
			return 0;
		}
		n = bswap16 (*(uint16_t *)((char *)buf + off));
		if (n == 0)
			return 3;
		r = msgpackclen_msg_buf_read_elems(buf, off + 2, size, n, error);
		return r > 0 ? r + 3 : 0;
	}
	// array 32 	11011101 	0xdd
	if (type == 0xdd)
	{
		if (off + 4 > size)
		{
			return 0;
		}
		n = bswap32 (*(uint32_t *)((char *)buf + off));
		if (n == 0)
			return 5;
		r = msgpackclen_msg_buf_read_elems(buf, off + 2, size, n, error);
		return r > 0 ? r + 5 : 0;
	}
	// map 16 	11011110 	0xde
	if (type == 0xde)
	{
		if (off + 2 > size)
		{
			return 0;
		}
		n = bswap16 (*(uint16_t *)((char *)buf + off));
		if (n == 0)
			return 3;
		r = msgpackclen_msg_buf_read_elems(buf, off + 2, size, n * 2, error);
		return r > 0 ? r + 3 : 0;
	}
	// map 32 	11011111 	0xdf
	if (type == 0xdf)
	{
		if (off + 4 > size)
		{
			return 0;
		}
		n = bswap32 (*(uint32_t *)((char *)buf + off));
		if (n == 0)
			return 5;
		r = msgpackclen_msg_buf_read_elems(buf, off + 2, size, n * 2, error);
		return r > 0 ? r + 5 : 0;
	}
	// Negative FixNum 	111xxxxx 	0xe0 - 0xff
	if (type >= 0xe0 /* && type <= 0xff */)
	//                  ^^^^^^^^^^^^^^^
	// comparison is always true due to limited range of data type
	{
		return 1;
	}

	msgpackclen_msg_buf_invalid_type(error, type);
	return 0;
}

size_t
msgpackclen_buf_read (void *buf, size_t size, char **error)
{
	size_t r;

	r = msgpackclen_msg_buf_read_elem(buf, 0, size, error);

	if (r > size)
		return 0;
	return r;
}
