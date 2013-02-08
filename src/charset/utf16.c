/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		utf16.c
 * @ingroup 	charset
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream/stream.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_charset_utf16_get(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t* ch);
tb_long_t tb_charset_utf16_get(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t* ch)
{
	// init
	tb_byte_t const* 	p = tb_bstream_pos(bst);
	tb_byte_t const* 	q = p;
	tb_size_t 			n = tb_bstream_left(bst);

	// not enough? break it
	tb_check_return_val(n > 1, -1);

	// the first character
	tb_uint32_t c = be? tb_bits_get_u16_be(p) : tb_bits_get_u16_le(p);
	p += 2;

	// large?
	if (c >= 0xd800 && c <= 0xdbff) 
	{
		// not enough? break it
		tb_check_return_val(n > 3, -1);

		// the next character
		tb_uint32_t c2 = be? tb_bits_get_u16_be(p) : tb_bits_get_u16_le(p);
		if (c2 >= 0xdc00 && c2 <= 0xdfff)
		{
			c = ((c - 0xd800) << 10) + (c2 - 0xdc00) + 0x0010000;
			p += 2;
		};
	};

	// next
	if (p > q) tb_bstream_skip(bst, p - q);

	// set character
	*ch = c;

	// ok?
	return p > q? 1 : 0;
}

tb_long_t tb_charset_utf16_set(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t ch);
tb_long_t tb_charset_utf16_set(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t ch)
{
	// init
	tb_byte_t* 	p = tb_bstream_pos(bst);
	tb_byte_t* 	q = p;
	tb_size_t 	n = tb_bstream_left(bst);

	if (ch <= 0x0000ffff) 
	{
		// not enough? break it
		tb_check_return_val(n > 1, -1);

		// set character
		if (be) tb_bstream_set_u16_be(bst, ch);
		else tb_bstream_set_u16_le(bst, ch);
	}
	else if (ch > 0x0010ffff)
	{
		// not enough? break it
		tb_check_return_val(n > 1, -1);

		// set character
		if (be) tb_bstream_set_u16_be(bst, 0x0000fffd);
		else tb_bstream_set_u16_le(bst, 0x0000fffd);
	}
	else
	{
		// not enough? break it
		tb_check_return_val(n > 3, -1);

		// set character
		ch -= 0x0010000;
		if (be)
		{
			tb_bstream_set_u16_be(bst, (ch >> 10) + 0xd800);
			tb_bstream_set_u16_be(bst, (ch & 0x3ff) + 0xdc00);
		}
		else 
		{
			tb_bstream_set_u16_le(bst, (ch >> 10) + 0xd800);
			tb_bstream_set_u16_le(bst, (ch & 0x3ff) + 0xdc00);
		}
	};

	// ok
	return 1;
}
