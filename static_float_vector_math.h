/**
 * @file static_float_vector_math.h
 * @date Dec 7, 2014
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

#ifndef STATIC_FLOAT_VECTOR_MATH_H_
#define STATIC_FLOAT_VECTOR_MATH_H_

///////////////////////////////////////////////////////////////////////////////

#include "static_float.h"
#include "vector_math.h"

///////////////////////////////////////////////////////////////////////////////

namespace vector_math {

	template<uint B, int E>
	tvec3<typename rt::normalize<static_float::sf<B, E>>::rt>
	normalize_inversesqrt(const tvec3<static_float::sf<B, E>>& v) {
		using namespace static_float;
		typedef tvec3<typename rt::normalize<static_float::sf<B, E>>::rt> rt;
		auto sq_sum = sq(v.x) + sq(v.y) + sq(v.z);
		auto is = inversesqrt(sq_sum);
		auto is_msbs_cutted = static_float::usf<1 - is.e - E - B, is.e>(is);
		auto normalized = v * is_msbs_cutted;
		auto rounded = rt(normalized);
		return rounded;
	}

	template<uint B, int E>
	tvec3<typename rt::normalize<static_float::sf<B, E>>::rt>
	normalize(const tvec3<static_float::sf<B, E>>& v) {
		if(v.x == 0 && v.y == 0 && v.z == 0){
			// Should return NaN but we do not support it for now.
			return v;
		}

		using namespace static_float;

		typedef tvec3<typename rt::normalize<static_float::sf<B, E>>::rt> rt;
		typedef sf<B+1, 1-int(B+1)> elem_t;
		typedef usf<B+1, 1-int(B+1)> elem_abs_t;
		typedef tvec3<elem_t> vec_t;
		// TODO Check how much precision scale need. Hint:
		// If type of v is sf<3, 0> and v.x.num is 0b111 and v.y and v.z are 0,
		// and type of n is sf<4, 0> and value after initial div is 0b0111
		// at the end n should be 0b1000.
		typedef usf<B+1, 1-int(B+1)> scale_t;

		vec_t n;
		// Some kind of initial division. Elements are [0, 1.0).
		n.x.num = v.x.num;
		n.y.num = v.y.num;
		n.z.num = v.z.num;

		// Rotate left (multiply by 2) until at least one element
		// is [0.5, 1.0), by checking if B bit is 1.

		elem_t one_half = 0.5;
		while(abs(n.x) < 0.5 && abs(n.y) < 0.5 && abs(n.z) < 0.5){
			n.x.num <<= 1;
			n.y.num <<= 1;
			n.z.num <<= 1;
		}

		auto len_sq = sq(n.x) + sq(n.y) + sq(n.z);

		// Function is len_sq*s^2-1.
		// Find root of len_sq*s^2-1 with bisection method for s in (0, 2).

		scale_t s_min = std::numeric_limits<scale_t>::min();
		scale_t s_max = std::numeric_limits<scale_t>::max();

		// Sign of f_min is always 1 and sign of f_max is 0.
		for(uint i = 0; i < B+1; i++){
			// New half.
			// TODO Maybe some better way for calculating half.
			scale_t s_half = scale_t((s_min + s_max) >> 1);

			// Calculate function.
			auto f_half	= s_half*s_half*len_sq - 1;
			// Check sign.
			if(f_half < 0){
				// Sign is 1, same as f_min.
				s_min = s_half;
			}else{
				// Sign is 0, same as f_max.
				s_max = s_half;
			}
		}
		scale_t scale = s_max;

		auto n2 = n * scale;

		return rt(n2);
	}

} // namespace vector_math

///////////////////////////////////////////////////////////////////////////////

#endif // STATIC_FLOAT_VECTOR_MATH_H_
