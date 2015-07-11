/**
 * @file ast_test.cpp
 * @date Sep 29, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license MIT
 *
 * @brief
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "static_float.h"

using namespace static_float;

///////////////////////////////////////////////////////////////////////////////

extern "C" int16_t test(sf<4> x, sf<4, 1> y, sf<4, 0> z) {
	
	auto x_add_y = x + y;
	auto x_add_z = x + z;
	auto asdf = x_add_y * x_add_z;

	return asdf.num;
}

///////////////////////////////////////////////////////////////////////////////
