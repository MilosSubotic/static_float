/**
 * @file static_float.h
 * @date Sep 2, 2014
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

#ifndef STATIC_FLOAT_H_
#define STATIC_FLOAT_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <type_traits>
#include <iostream>
#include <limits>
#include <cmath>

#ifdef HAVE_GMP
#if __x86_64__
#include <gmp-x86_64.h>
#endif
#include <gmpxx.h>
#endif

// TODO Remove later, just for debug.
#include "debug.h"

#include "static_float_exceptions.h"

#include "return_type.h"

///////////////////////////////////////////////////////////////////////////////

namespace static_float {

	////////////////////////////////////

	namespace detail {

		struct float_fields {
			unsigned mantisa : 23;
			unsigned exponent : 8;
			unsigned sign : 1;
		};

		union float_union {
			float_fields fields;
			float f;
			uint32_t bits;
		};

		struct double_fields {
			unsigned long mantisa : 52;
			unsigned exponent : 11;
			unsigned sign : 1;
		};

		union double_union {
			double_fields fields;
			double d;
			uint64_t bits;
		};

		template<uint X, uint Y>
		struct lower_than {
			static constexpr bool value = X < Y;
		};

		template<uint X, uint Y>
		struct lower_equal {
			static constexpr bool value = X <= Y;
		};

		typedef __int128_t int128_t;
		typedef __uint128_t uint128_t;

#ifdef HAVE_GMP
		class int_big_t : public mpz_class {
		public:
			int_big_t(){}

			template<typename T>
			int_big_t(const T& t)
				: mpz_class(t) {
			}

			int_big_t(const int128_t& t)
				: mpz_class(0) {
				*this += int64_t(t >> 64);
				*this <<= 64;
				*this += uint64_t(t);
			}

			int_big_t(const uint128_t& t)
				: mpz_class(0) {
				*this += uint64_t(t >> 64);
				*this <<= 64;
				*this += uint64_t(t);
			}

			int_big_t(const int_big_t& t)
				: mpz_class(static_cast<const mpz_class&>(t)) {
			}

			explicit operator signed long int() const {
				return get_si();
			}

			explicit operator float() const {
				return get_d();
			}

			explicit operator double() const {
				return get_d();
			}
		};

		class uint_big_t : public mpz_class {
		public:
			uint_big_t(){}

			template<typename T>
			uint_big_t(const T& t)
				: mpz_class(t) {
			}

			uint_big_t(const uint128_t& t)
				: mpz_class(0) {
				*this += uint64_t(t >> 64);
				*this <<= 64;
				*this += uint64_t(t);
			}

			uint_big_t(const uint_big_t& t)
				: mpz_class(static_cast<const mpz_class&>(t)) {
			}

			explicit operator unsigned long int() const {
				return get_ui();
			}

			explicit operator float() const {
				return get_d();
			}

			explicit operator double() const {
				return get_d();
			}
		};
#endif

		/**
		 * If integer part of usf for inversesqrt() is odd then expand B for 1.
		 */
		template<uint B, int E>
		constexpr uint BB() {
			// If integer part is odd expand B for 1.
			return (int(B)+E) & 0x1
				// Odd I.
				? B + 1
				// Even I.
				: B;
		}

		/**
		 * ceil half of BB.
		 */
		template<uint B, int E>
		constexpr uint BBH() {
			// If integer part is odd expand B for 1.
			return BB<B, E>() & 0x1
				// Odd BB.
				? (BB<B, E>() + 1) >> 1
				// Even BB.
				: BB<B, E>() >> 1;
		}

	} // namespace detail

	////////////////////////////////////

	/**
	 * @class sf
	 * @brief Signed static float-point number.
	 * @param B bits of number without sign bit (relative precision)
	 * @param E exponent
	 */
	template<uint B, int E = 0>
	class sf {
	public:
		// TODO This goes to custom bits integer.
		typedef typename std::conditional<
			detail::lower_than<B, 8>::value,
			int8_t,
			typename std::conditional<
				detail::lower_than<B, 16>::value,
				int16_t,
				typename std::conditional<
					detail::lower_than<B, 32>::value,
					int32_t,
					typename std::conditional<
						detail::lower_than<B, 64>::value,
						int64_t,
						typename std::conditional<
							detail::lower_than<B, 128>::value,
							detail::int128_t,
#ifdef HAVE_GMP
							detail::int_big_t
#else
							void
#endif
						>::type
					>::type
				>::type
			>::type
		>::type num_type;

		num_type num;

		static constexpr uint b = B;
		static constexpr int e = E;

		////////////////////////////////

	public:
		sf() {
		}

		sf(const sf& s2)
			: num(s2.num) {
		}

		sf(int i) {
			// TODO Do STATIC_FLOAT_CHECKED_BUILD.
			constexpr int sh = -E;
			if(sh > 0){
				uint ush = sh;
				num = num_type(i) << ush;
			}else{
				uint ush = -sh;
				num = num_type(i) >> ush;
			}
		}

		sf(long l) {
			// TODO Do STATIC_FLOAT_CHECKED_BUILD.
			constexpr int sh = -E;
			if(sh > 0){
				uint ush = sh;
				num = num_type(l) << ush;
			}else{
				uint ush = -sh;
				num = num_type(l) >> ush;
			}
		}

		sf(float f) {
			if(f == 0){
				num = 0;
				return;
			}

			detail::float_union fu;
			fu.f = f;

#if STATIC_FLOAT_CHECKED_BUILD
			constexpr uint max_exponent = 0x7f + E + B - 1;

			if(fu.fields.exponent > max_exponent){
				throw exceptions::out_of_range_error()
					<< __PRETTY_FUNCTION__
					<< " Number " << f << " is to large for this "
					<< "static float!"
					<< exceptions::endl;
			}
#endif

			// Add hidden 1.
			uint32_t m = fu.fields.mantisa | (1 << 23);
			// normalized exponent + mantisa bits - sf exponent
			int sh = 0x7f - fu.fields.exponent + 23 + E;
			if(sh == 0){
				num = m;
			}else if(sh > 0){
				num = m >> sh;
			}else{
				num = num_type(m) << -sh;
			}
			if(fu.fields.sign){
				num = -num;
			}
		}

		sf(double d) {
			if(d == 0){
				num = 0;
				return;
			}

			detail::double_union du;
			du.d = d;

#if STATIC_FLOAT_CHECKED_BUILD
			constexpr uint max_exponent = 0x3ff + E + B - 1;

			if(du.fields.exponent > max_exponent){
				throw exceptions::out_of_range_error()
					<< __PRETTY_FUNCTION__
					<< " Number " << d << " is to large for this "
					<< "static float!"
					<< exceptions::endl;
			}
#endif

			// Add hidden 1.
			uint64_t m = du.fields.mantisa | (1l << 52);
			// normalized exponent + mantisa bits - sf exponent
			int sh = 0x3ff - du.fields.exponent + 52 + E;
			if(sh == 0){
				num = m;
			}else if(sh > 0){
				num = m >> sh;
			}else{
				num = num_type(m) << -sh;
			}
			if(du.fields.sign){
				num = -num;
			}
		}


		template<uint B2, int E2>
		explicit sf(const sf<B2, E2>& s2) {
			// WARNING: Could make overflows.
			// TODO Check overflows and print warning.
			// TODO Add masking to suppress high cut bits.
			constexpr int sh = E2 - E;
			if(sh == 0){
				num = s2.num;
			}else if(sh > 0){
				uint ush = sh;
				num = s2.num << ush;
			}else{uint ush = -sh;
				num = s2.num >> ush;
			}
		}

		enum num_constr_t { num_field };
		/**
		 * For use in constexpr expressions and functions.
		 * @param num field num value
		 * @param foo to make a difference from others conversion constructor.
		 */
		explicit sf(num_type num, num_constr_t foo)
			: num(num) {
			(void)foo;
		}

		////////////////////////////////

	public:
		sf& operator=(const sf& s2) {
			num = s2.num;
			return *this;
		}

		template<uint B2, int E2>
		sf& operator=(const sf<B2, E2>& s2) {
			// Always check for overflows.
			static_assert(int(B) + E >= int(B2) + E2, "Possible overflow!");
			constexpr int sh = E2 - E;
			if(sh == 0){
				num = num_type(s2.num);
			}else if(sh > 0){
				uint ush = sh;
				num = num_type(decltype(s2.num)(s2.num << ush));
			}else{
				uint ush = -sh;
				num = num_type(decltype(s2.num)(s2.num >> ush));
			}
			return *this;
		}

		////////////////////////////////

	public:
		// TODO Make explicit after adding ostream operators.
		operator float() const {
			if(E == 0){
				return float(num);
			}else{
				return float(num) * ::exp2f(E);
			}
		}

		////////////////////////////////

	public:
		template<uint B2, int E2>
		sf& operator+=(const sf<B2, E2>& s2) {
			num += sf<B, E>(s2).num;
			return *this;
		}

		////////////////////////////////

	};

	/**
	 * @class sf_f
	 * @brief Signed static float-point number in range [-2.0, 2.0).
	 * @param F fraction bits
	 */
	template<uint F>
	class sf_f : public sf<F+1, -int(F)> {
	public:
		/**
		 * Forwarding arguments to sf constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args>
		sf_f(Args&&... args)
			: sf<F+1, -int(F)>(std::forward<Args>(args)...) {
		};
	};

	/**
	 * @class sf_if
	 * @brief Signed static float-point with integer and fraction bits.
	 * @param I integer bits
	 * @param F fraction bits
	 */
	template<uint I, uint F = 0>
	class sf_if : public sf<I+F, -int(F)> {
	public:
		/**
		 * Forwarding arguments to sf constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args>
		sf_if(Args&&... args)
			: sf<I+F, -int(F)>(std::forward<Args>(args)...) {
		};
	};

	/**
	 * @class sf_ife
	 * @brief Integer and fraction bits and exponent.
	 * @param I integer bits
	 * @param F fraction bits
	 * @param E exponent
	 */
	template<uint I, uint F, int E = 0>
	class sf_ife : public sf<I+F, E-int(F)> {
	public:
		/**
		 * Forwarding arguments to sf constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args>
		sf_ife(Args&&... args)
			: sf<I+F, E-int(F)>(std::forward<Args>(args)...) {
		};
	};


	/**
	 * @class usf
	 * @brief Unsigned static float-point number.
	 * @param B bits of number without sign bit (relative precision)
	 * @param E exponent
	 */
	template<uint B, int E = 0>
	class usf {
	public:
		// TODO This goes to custom bits integer.
		typedef typename std::conditional<
			detail::lower_equal<B, 8>::value,
			uint8_t,
			typename std::conditional<
				detail::lower_equal<B, 16>::value,
				uint16_t,
				typename std::conditional<
					detail::lower_equal<B, 32>::value,
					uint32_t,
					typename std::conditional<
						detail::lower_equal<B, 64>::value,
						uint64_t,
						typename std::conditional<
							detail::lower_equal<B, 128>::value,
							detail::uint128_t,
#ifdef HAVE_GMP
							detail::uint_big_t
#else
							void
#endif
						>::type
					>::type
				>::type
			>::type
		>::type num_type;

		num_type num;

		static constexpr uint b = B;
		static constexpr int e = E;

		////////////////////////////////

	public:
		usf() {
		}

		usf(const usf& u2)
			: num(u2.num) {
		}

		usf(unsigned int i) {
			// TODO Do STATIC_FLOAT_CHECKED_BUILD.
			constexpr int sh = -E;
			if(sh > 0){
				uint ush = sh;
				num = num_type(i) << ush;
			}else{
				uint ush = -sh;
				num = num_type(i) >> ush;
			}
		}

		usf(unsigned long l) {
			// TODO Do STATIC_FLOAT_CHECKED_BUILD.
			constexpr int sh = -E;
			if(sh > 0){
				uint ush = sh;
				num = num_type(l) << ush;
			}else{
				uint ush = -sh;
				num = num_type(l) >> ush;
			}
		}

		usf(float f) {
			if(f == 0){
				num = 0;
				return;
			}

			detail::float_union fu;
			fu.f = f;

#if STATIC_FLOAT_CHECKED_BUILD
			if(f < 0){
				throw exceptions::sign_error()
					<< __PRETTY_FUNCTION__
					<< " Negative number " << f
					<< " cannot be converted to unsigned static float!"
					<< exceptions::endl;
			}

			constexpr uint max_exponent = 0x7f + E + B - 1;

			if(fu.fields.exponent > max_exponent){
				throw exceptions::out_of_range_error()
					<< __PRETTY_FUNCTION__
					<< " Number " << f << " is to large for this "
					<< "static float!"
					<< exceptions::endl;
			}
#endif

			// Add hidden 1.
			uint32_t m = fu.fields.mantisa | (1 << 23);
			// normalized exponent + mantisa bits - sf exponent
			int sh = 0x7f - fu.fields.exponent + 23 + E;
			if(sh == 0){
				num = m;
			}else if(sh > 0){
				num = m >> sh;
			}else{
				num = num_type(m) << -sh;
			}
		}

		usf(double d) {
			if(d == 0){
				num = 0;
				return;
			}

			detail::double_union du;
			du.d = d;

#if STATIC_FLOAT_CHECKED_BUILD
			if(d < 0){
				throw exceptions::sign_error()
					<< __PRETTY_FUNCTION__
					<< " Negative number " << d
					<< " cannot be converted to unsigned static float!"
					<< exceptions::endl;
			}

			constexpr uint max_exponent = 0x3ff + E + B - 1;

			if(du.fields.exponent > max_exponent){
				throw exceptions::out_of_range_error()
					<< __PRETTY_FUNCTION__
					<< " Number " << d << " is to large for this "
					<< "static float!"
					<< exceptions::endl;
			}
#endif

			// Add hidden 1.
			uint64_t m = du.fields.mantisa | (1l << 52);
			// normalized exponent + mantisa bits - sf exponent
			int sh = 0x3ff - du.fields.exponent + 52 + E;
			if(sh == 0){
				num = m;
			}else if(sh > 0){
				num = m >> sh;
			}else{
				num = num_type(m) << -sh;
			}
		}


		template<uint B2, int E2>
		explicit usf(const usf<B2, E2>& u2) {
			// WARNING: Could make overflows.
			// TODO Check overflows and print warning.
			// TODO Add masking to suppress high cut bits.
			constexpr int sh = E2 - E;
			if(sh == 0){
				num = u2.num;
			}else if(sh > 0){
				uint ush = sh;
				num = u2.num << ush;
			}else{uint ush = -sh;
				num = u2.num >> ush;
			}
		}

		template<uint B2>
		explicit usf(const usf<B2, E>& u2) {
			// WARNING: Could make overflows.
			// TODO Check overflows and print warning.
			// TODO Add masking to suppress high cut bits.
			num = u2.num;
		}

		template<uint B2, int E2>
		explicit usf(const sf<B2, E2>& s) {
#if STATIC_FLOAT_CHECKED_BUILD
			if(s.num < 0){
				throw exceptions::sign_error()
					<< __PRETTY_FUNCTION__
					<< " Negative number " << s
					<< " cannot be converted to unsigned static float!"
					<< exceptions::endl;
			}
#endif
			// WARNING: Could make overflows.
			// TODO Check overflows and print warning.
			constexpr int sh = E2 - E;
			if(sh == 0){
				num = s.num;
			}else if(sh > 0){
				uint ush = sh;
				num = s.num << ush;
			}else{
				uint ush = -sh;
				num = s.num >> ush;
			}
		}

		enum num_constr_t { num_field };
		/**
		 * For use in constexpr expressions and functions.
		 * @param num field num value
		 * @param foo to make a difference from others conversion constructor.
		 */
		explicit usf(num_type num, num_constr_t foo)
			: num(num) {
			(void)foo;
		}

		////////////////////////////////

	public:
		usf& operator=(const usf& u2) {
			num = u2.num;
			return *this;
		}

		template<uint B2, int E2>
		usf& operator=(const usf<B2, E2>& u2) {
			// Always check for overflows.
			static_assert(int(B) + E >= int(B2) + E2, "Possible overflow!");
			constexpr int sh = E2 - E;
			if(sh == 0){
				num = num_type(u2.num);
			}else if(sh > 0){
				uint ush = sh;
				num = num_type(decltype(u2.num)(u2.num << ush));
			}else{
				uint ush = -sh;
				num = num_type(decltype(u2.num)(u2.num >> ush));
			}
			return *this;
		}

		////////////////////////////////

	public:
		// TODO Make explicit after adding ostream operators.
		operator float() const {
			if(E == 0){
				return float(num);
			}else{
				return float(num) * ::exp2f(E);
			}
		}

		////////////////////////////////

	public:
		template<uint B2, int E2>
		usf& operator+=(const usf<B2, E2>& u2) {
			num += usf<B, E>(u2).num;
			return *this;
		}

		////////////////////////////////
	};


	/**
	 * @class usf_f
	 * @brief Unsigned static float-point number in range [0, 2.0).
	 * @param F fraction bits
	 */
	template<uint F>
	class usf_f : public usf<F+1, -int(F)> {
	public:
		/**
		 * Forwarding arguments to usf constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args>
		usf_f(Args&&... args)
			: usf<F+1, -int(F)>(std::forward<Args>(args)...) {
		};
	};

	/**
	 * @class usf_if
	 * @brief Unsigned static float-point with integer and fraction bits.
	 * @param I integer bits
	 * @param F fraction bits
	 */
	template<uint I, uint F = 0>
	class usf_if : public usf<I+F, -int(F)> {
	public:
		/**
		 * Forwarding arguments to usf constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args>
		usf_if(Args&&... args)
			: usf<I+F, -int(F)>(std::forward<Args>(args)...) {
		};
	};

	/**
	 * @class usf_ife
	 * @brief Integer and fraction bits and exponent.
	 * @param I integer bits
	 * @param F fraction bits
	 * @param E exponent
	 */
	template<uint I, uint F, int E = 0>
	class usf_ife : public usf<I+F, E-int(F)> {
	public:
		/**
		 * Forwarding arguments to usf constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args>
		usf_ife(Args&&... args)
			: usf<I+F, E-int(F)>(std::forward<Args>(args)...) {
		};
	};


	template<uint B, int E>
	usf<B, E>
	unsign(const sf<B, E>& s) {
		return usf<B, E>(s);
	}

	template<uint B, int E>
	usf<B*2, E*2> // TODO Maybe should be even smaller than B*2.
	sq(const sf<B, E>& s) {
		typedef usf<B*2, E*2> rt;
		typedef typename sf<B*2, E*2>::num_type ct;
		rt res;
		res.num = ct(s.num) * ct(s.num);
		return res;
	}

	template<uint B, int E>
	usf<
		detail::BBH<B, E>() + detail::BB<B, E>(),
		-int(detail::BB<B, E>()) -(int(detail::BB<B, E>())+E)/2
	>
	inversesqrt(const usf<B, E>& x) {
		// For B greater than 128 bits use difference between current and
		// previous result as algorithm stop criteria.
		// TODO For B less and equal to 128 bits use lookup table to get
		// pre-calculated number of iterations.

		constexpr uint BB = detail::BB<B, E>();
		constexpr uint BBH = detail::BBH<B, E>();

		typedef usf<BBH + BB, -int(BB)> y_t;

		y_t y;
		y.num = (typename y_t::num_type(2) << BB-1);
		y_t prev_y = y;

		usf<BB, -int(BB)-1> x_half;
		x_half.num = x.num;

		y_t three_halfs;
		three_halfs.num = (typename y_t::num_type(3) << BB-1);

		while(true){
			y = y_t(y * (three_halfs - (x_half * y * y)));

			if(y == prev_y){
				break;
			}
			prev_y = y;
		}

		usf<BBH + BB, -int(BB) -(int(BB)+E)/2> res;
		res.num = y.num;
		return res;
	}

	template<uint B, int E>
	usf<B, E>
	abs(const sf<B, E>& s){
		usf<B, E> r;
		r.num = s.num >= 0 ? s.num : -s.num;
		return r;
	}

	////////////////////////////////////

} // namespace static_float


namespace rt {

	/*
	 * rt - return type
	 * ct - calculation type
	 */

	constexpr int max(int x, int y) {
		return x > y ? x : y;
	}
	constexpr int min(int x, int y) {
		return x < y ? x : y;
	}


	template<uint B1, int E1, uint B2, int E2>
	struct add_sub<static_float::sf<B1, E1>, static_float::sf<B2, E2>> {
		constexpr static int _min_e = min(E1, E2);
		constexpr static int _max_bit = max(int(B1) + E1, int(B2) + E2);
		typedef static_float::sf<_max_bit - _min_e + 1, _min_e> rt;
		typedef typename rt::num_type ct;
		constexpr static uint SH1 = E1 - _min_e;
		constexpr static uint SH2 = E2 - _min_e;
	};

	template<uint B1, int E1, uint B2, int E2>
	struct mul<static_float::sf<B1, E1>, static_float::sf<B2, E2>> {
		typedef static_float::sf<B1 + B2, E1 + E2> rt;
		typedef typename rt::num_type ct;
	};

	template<uint B1, int E1, uint B2, int E2>
	struct div<static_float::sf<B1, E1>, static_float::sf<B2, E2>> {
		// quotient<B1, E1-E2>, remainder<B2, E1-E2-B2>
		typedef static_float::sf<B1 + B2, E1 - E2 - int(B2)> rt;
		constexpr static uint SH1 = B2;
		constexpr static uint SH2 = 0;
		typedef typename static_float::sf<B1 + SH1, E1>::num_type num_ct;
		typedef typename static_float::sf<B2 + SH2, E2>::num_type den_ct;
	};

	template<uint B1, int E1, uint B2, int E2>
	struct dot3<static_float::sf<B1, E1>, static_float::sf<B2, E2>> {
		typedef static_float::sf<B1 + B2 + 2, E1 + E2> rt;
	};

	template<uint B1, int E1, uint B2, int E2>
	struct cross<static_float::sf<B1, E1>, static_float::sf<B2, E2>> {
		typedef static_float::sf<B1 + B2 + 1, E1 + E2> rt;
	};

	template<uint B, int E>
	struct normalize<static_float::sf<B, E>> {
		typedef static_float::sf<B, 1 - int(B)> rt;
	};



	template<uint B1, int E1, uint B2, int E2>
	struct add_sub<static_float::usf<B1, E1>, static_float::usf<B2, E2>> {
		constexpr static int _min_e = min(E1, E2);
		constexpr static int _max_bit = max(int(B1) + E1, int(B2) + E2);
		typedef static_float::usf<_max_bit - _min_e + 1, _min_e> rt;
		typedef typename rt::num_type ct;
		constexpr static uint SH1 = E1 - _min_e;
		constexpr static uint SH2 = E2 - _min_e;
	};

	template<uint B1, int E1, uint B2, int E2>
	struct mul<static_float::usf<B1, E1>, static_float::usf<B2, E2>> {
		// TODO Maybe is less than B1 + B2, like int(B1 + B2) - 2.
		typedef static_float::usf<B1 + B2, E1 + E2> rt;
		typedef typename rt::num_type ct;
	};


	template<uint B1, int E1, uint B2, int E2>
	struct mul<static_float::sf<B1, E1>, static_float::usf<B2, E2>> {
		// TODO Maybe is less than B1 + B2, like int(B1 + B2) - 1.
		typedef static_float::sf<B1 + B2, E1 + E2> rt;
		typedef typename rt::num_type ct;
	};

} // namespace rt


namespace static_float {

	template<uint B1, int E1, uint B2, int E2>
	typename rt::add_sub<sf<B1, E1>, sf<B2, E2>>::rt
	operator+(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::add_sub<sf<B1, E1>, sf<B2, E2>> a;
		typedef typename a::ct ct;
		typename a::rt r;
		r.num = (ct(s1.num) << a::SH1) + (ct(s2.num) << a::SH2);
		return r;
	}

	template<uint B1, int E1, uint B2, int E2>
	typename rt::add_sub<sf<B1, E1>, sf<B2, E2>>::rt
	operator-(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::add_sub<sf<B1, E1>, sf<B2, E2>> a;
		typedef typename a::ct ct;
		typename a::rt r;
		r.num = (ct(s1.num) << a::SH1) - (ct(s2.num) << a::SH2);
		return r;
	}

	template<uint B1, int E1, uint B2, int E2>
	typename rt::mul<sf<B1, E1>, sf<B2, E2>>::rt
	operator*(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::mul<sf<B1, E1>, sf<B2, E2>> m;
		typedef typename m::ct ct;
		typename m::rt r;
		r.num = ct(s1.num) * ct(s2.num);
		return r;
	}

	template<uint B1, int E1, uint B2, int E2>
	typename rt::div<sf<B1, E1>, sf<B2, E2>>::rt
	operator/(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::div<sf<B1, E1>, sf<B2, E2>> m;
		typedef typename m::num_ct num_ct;
		typedef typename m::den_ct den_ct;
		typename m::rt r;
		r.num = (num_ct(s1.num) << m::SH1) / (den_ct(s2.num) << m::SH2);
		return r;
	}

	template<uint B1, int E1, uint B2, int E2>
	bool operator<(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::add_sub<sf<B1, E1>, sf<B2, E2>> a;
		typedef typename a::ct ct;
		return (ct(s1.num) << a::SH1) < (ct(s2.num) << a::SH2);
	}

	template<uint B1, int E1, uint B2, int E2>
	bool operator<=(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::add_sub<sf<B1, E1>, sf<B2, E2>> a;
		typedef typename a::ct ct;
		return (ct(s1.num) << a::SH1) <= (ct(s2.num) << a::SH2);
	}

	template<uint B1, int E1, uint B2, int E2>
	bool operator>(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::add_sub<sf<B1, E1>, sf<B2, E2>> a;
		typedef typename a::ct ct;
		return (ct(s1.num) << a::SH1) > (ct(s2.num) << a::SH2);
	}

	template<uint B1, int E1, uint B2, int E2>
	bool operator>=(const sf<B1, E1>& s1, const sf<B2, E2>& s2) {
		typedef typename rt::add_sub<sf<B1, E1>, sf<B2, E2>> a;
		typedef typename a::ct ct;
		return (ct(s1.num) << a::SH1) >= (ct(s2.num) << a::SH2);
	}

	namespace detail {
		template<typename T>
		struct is_sf {
			static constexpr bool value = false;
		};

		template<uint B, int E>
		struct is_sf<sf<B, E>> {
			static constexpr bool value = true;
		};

	} // namespace detail



	template<uint B1, int E1, uint B2, int E2>
	typename rt::add_sub<usf<B1, E1>, usf<B2, E2>>::rt
	operator+(const usf<B1, E1>& s1, const usf<B2, E2>& s2) {
		typedef typename rt::add_sub<usf<B1, E1>, usf<B2, E2>> a;
		typedef typename a::ct ct;
		typename a::rt r;
		r.num = (ct(s1.num) << a::SH1) + (ct(s2.num) << a::SH2);
		return r;
	}

	template<uint B1, int E1, uint B2, int E2>
	typename rt::add_sub<usf<B1, E1>, usf<B2, E2>>::rt
	operator-(const usf<B1, E1>& s1, const usf<B2, E2>& s2) {
		typedef typename rt::add_sub<usf<B1, E1>, usf<B2, E2>> a;
		typedef typename a::ct ct;
		typename a::rt r;
		r.num = (ct(s1.num) << a::SH1) - (ct(s2.num) << a::SH2);
		return r;
	}

	template<uint B1, int E1, uint B2, int E2>
	typename rt::mul<usf<B1, E1>, usf<B2, E2>>::rt
	operator*(const usf<B1, E1>& s1, const usf<B2, E2>& s2) {
		typedef typename rt::mul<usf<B1, E1>, usf<B2, E2>> m;
		typedef typename m::ct ct;
		typename m::rt r;
		r.num = ct(s1.num) * ct(s2.num);
		return r;
	}

	template<uint B, int E>
	usf<B, E>
	operator>>(const usf<B, E>& u, int sh) {
		usf<B, E> r = u;
		r.num >>= sh;
		return r;
	}


	template<uint B1, int E1, uint B2, int E2>
	typename rt::mul<sf<B1, E1>, usf<B2, E2>>::rt
	operator*(const sf<B1, E1>& s1, const usf<B2, E2>& s2) {
		typedef typename rt::mul<sf<B1, E1>, usf<B2, E2>> m;
		typedef typename m::ct ct;
		typename m::rt r;
		r.num = ct(s1.num) * ct(s2.num);
		return r;
	}

	////////////////////////////////////

} // namespace static_float

///////////////////////////////////////////////////////////////////////////////
// Numeric limits.

namespace std {

	template<uint B, int E>
	class numeric_limits<static_float::sf<B, E>> {
	private:
		typedef static_float::sf<B, E> type;

	public:
		static constexpr bool is_specialized = true;

		static constexpr type min() noexcept {
			return type(0, type::num_field);
		}

		static constexpr type max() noexcept {
			typedef typename type::num_type nt;
			return type((nt(1) << B) - 1, type::num_field);
		}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
		static constexpr type lowest() noexcept {
			typedef typename type::num_type nt;
			return type(nt(1) << (B), type::num_field);
		}
#endif

		// TODO Implement.
/*
		static constexpr int digits = __FLT_MANT_DIG__;
		static constexpr int digits10 = __FLT_DIG__;
#ifdef __GXX_EXPERIMENTAL_CXX0X__
		static constexpr int max_digits10 = __glibcxx_max_digits10(
				__FLT_MANT_DIG__);
#endif
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr int radix = __FLT_RADIX__;
*/
		static constexpr type epsilon() noexcept {
			return type(1, type::num_field);
		}

		// TODO Implement.
/*
		static constexpr type round_error() noexcept {
			return 0.5F;
		}

		static constexpr int min_exponent = __FLT_MIN_EXP__;
		static constexpr int min_exponent10 = __FLT_MIN_10_EXP__;
		static constexpr int max_exponent = __FLT_MAX_EXP__;
		static constexpr int max_exponent10 = __FLT_MAX_10_EXP__;

		static constexpr bool has_infinity = __FLT_HAS_INFINITY__;
		static constexpr bool has_quiet_NaN =
				__FLT_HAS_QUIET_NAN__;
		static constexpr bool has_signaling_NaN = has_quiet_NaN;
		static constexpr float_denorm_style has_denorm =
				bool(__FLT_HAS_DENORM__) ? denorm_present : denorm_absent;
		static constexpr bool has_denorm_loss =
				__glibcxx_float_has_denorm_loss;

		static constexpr float infinity() noexcept {
			return __builtin_huge_valf();
		}

		static constexpr float quiet_NaN() noexcept {
			return __builtin_nanf("");
		}

		static constexpr float signaling_NaN() noexcept {
			return __builtin_nansf("");
		}

		static constexpr float denorm_min() noexcept {
			return __FLT_DENORM_MIN__;
		}

		static constexpr bool is_iec559 = has_infinity
				&& has_quiet_NaN && has_denorm == denorm_present;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;

		static constexpr bool traps = __glibcxx_float_traps;
		static constexpr bool tinyness_before =
				__glibcxx_float_tinyness_before;
		static constexpr float_round_style round_style =
				round_to_nearest;
		*/
	};


	template<uint B, int E>
	class numeric_limits<static_float::usf<B, E>> {
	private:
		typedef static_float::usf<B, E> type;

	public:
		static constexpr bool is_specialized = true;

		static constexpr type min() noexcept {
			return type(0, type::num_field);
		}

		static constexpr type max() noexcept {
			typedef typename type::num_type nt;
			return type(
					nt(1) << (B - 1) | ((nt(1) << (B - 1)) - 1),
					type::num_field);
		}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
		static constexpr type lowest() noexcept {
			return type(0, type::num_field);
		}
#endif

		// TODO Implement.
/*
		static constexpr int digits = __FLT_MANT_DIG__;
		static constexpr int digits10 = __FLT_DIG__;
#ifdef __GXX_EXPERIMENTAL_CXX0X__
		static constexpr int max_digits10 = __glibcxx_max_digits10(
				__FLT_MANT_DIG__);
#endif
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr int radix = __FLT_RADIX__;
*/
		static constexpr type epsilon() noexcept {
			return type(1, type::num_field);
		}

		// TODO Implement.
/*
		static constexpr type round_error() noexcept {
			return 0.5F;
		}

		static constexpr int min_exponent = __FLT_MIN_EXP__;
		static constexpr int min_exponent10 = __FLT_MIN_10_EXP__;
		static constexpr int max_exponent = __FLT_MAX_EXP__;
		static constexpr int max_exponent10 = __FLT_MAX_10_EXP__;

		static constexpr bool has_infinity = __FLT_HAS_INFINITY__;
		static constexpr bool has_quiet_NaN =
				__FLT_HAS_QUIET_NAN__;
		static constexpr bool has_signaling_NaN = has_quiet_NaN;
		static constexpr float_denorm_style has_denorm =
				bool(__FLT_HAS_DENORM__) ? denorm_present : denorm_absent;
		static constexpr bool has_denorm_loss =
				__glibcxx_float_has_denorm_loss;

		static constexpr float infinity() noexcept {
			return __builtin_huge_valf();
		}

		static constexpr float quiet_NaN() noexcept {
			return __builtin_nanf("");
		}

		static constexpr float signaling_NaN() noexcept {
			return __builtin_nansf("");
		}

		static constexpr float denorm_min() noexcept {
			return __FLT_DENORM_MIN__;
		}

		static constexpr bool is_iec559 = has_infinity
				&& has_quiet_NaN && has_denorm == denorm_present;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;

		static constexpr bool traps = __glibcxx_float_traps;
		static constexpr bool tinyness_before =
				__glibcxx_float_tinyness_before;
		static constexpr float_round_style round_style =
				round_to_nearest;
		*/
	};
} // namespace std

///////////////////////////////////////////////////////////////////////////////

#endif // STATIC_FLOAT_H_
