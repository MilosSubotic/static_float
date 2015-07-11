/**
 * @file frac.h
 * @date Sep 28, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license MIT
 *
 * @brief Fraction number.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef FRAC_H_
#define FRAC_H_

///////////////////////////////////////////////////////////////////////////////

#include "return_type.h"

///////////////////////////////////////////////////////////////////////////////

namespace frac_ns {

	/**
	 * @class frac
	 * @brief Fraction number.
	 * @param N Numerator type.
	 * @param E Denominator type
	 */
	template<typename N, typename D>
	class frac {
	public:
		frac()
			: num(0), den(1) {
		}
		frac(N num)
			: num(num), den(1) {
		}
		frac(N num, D den)
			: num(num), den(den) {
		}

		template<typename N2, typename D2>
		frac(const frac<N2, D2>& f2)
			: num(f2.num), den(f2.den){
		}

		////////////////////////////////

	public:
		N num;
		D den;

		////////////////////////////////

	public:
		typedef N num_type;
		typedef D den_type;

		////////////////////////////////
	};


	////////////////////////////////////

	// TODO All usual operators.

	template<typename N1, typename D1, typename N2, typename D2>
	frac<typename rt::mul<N1, N2>::rt, typename rt::mul<D1, D2>::rt>
	operator*(const frac<N1, D1>& f1, const frac<N2, D2>& f2) {
		frac<typename rt::mul<N1, N2>::rt, typename rt::mul<D1, D2>::rt> fr;
		fr.num = f1.num * f2.num;
		fr.den = f1.den * f2.den;
		return fr;
	}

	template<typename N1, typename D1, typename T2>
	frac<typename rt::mul<N1, T2>::rt, D1>
	operator*(const frac<N1, D1>& f1, const T2& t2) {
		frac<typename rt::mul<N1, T2>::rt, D1> fr;
		fr.num = f1.num * t2;
		fr.den = f1.den;
		return fr;
	}

	template<typename N1, typename D1, typename N2, typename D2>
	bool operator>=(const frac<N1, D1>& f1, const frac<N2, D2>& f2) {
		return f1.num * f2.den >= f2.num * f1.den;
	}

	template<typename N1, typename D1, typename T2>
	bool operator>=(const frac<N1, D1>& f1, const T2& t2) {
		return f1.num >= t2 * f1.den;
	}

	template<typename T1, typename N2, typename D2>
	bool operator>=(const T1& t1, const frac<N2, D2>& f2) {
		return t1 * f2.den >= f2.num;
	}

	template<typename N, typename D>
	std::ostream& operator<<(std::ostream& os, const frac<N, D>& f) {
		os << "frac(" << f.num << ", " << f.den << ")";
		return os;
	}

	template<typename N, typename D>
	typename rt::div<N, D>::rt
	div(const frac<N, D>& f) {
		return f.num / f.den;
	}

	////////////////////////////////////

} // namespace frac_ns

///////////////////////////////////////////////////////////////////////////////

#endif // FRAC_H_
