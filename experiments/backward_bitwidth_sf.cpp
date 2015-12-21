/**
 * @file main.cpp
 * @date Sep 5, 2015
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

#include <stdlib.h>
#include <stdint.h>

#include <iostream>
using namespace std;

#include <vector>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////

#define DEBUG(var) \
	do{ \
		cout << #var << " = " << (var) << endl; \
	}while(0)

#define DEBUG_HEX(var) \
	do{ \
		cout << hex << #var << " = 0x" << (var) << dec << endl; \
	}while(0)

template<typename X>
class usf_expr {
public:
	operator X&() {
		return static_cast<X&>(*this);
	}
	operator X const&() const {
		return static_cast<const X&>(*this);
	}
};

template<uint B>
class usf : public usf_expr<usf<B> > {
public:
	uint64_t num;

	usf(uint64_t n)
			: num(n) {
	}

	// Construct from any usf_expr
	template<typename X>
	usf(usf_expr<X> const& vec) {
		X const& v = vec;
		num = v.eval<B>();
	}

	template<uint B1>
	uint64_t eval() const {
		return num >> B1;
	}

};

template<typename X1, typename X2>
class usf_mul : public usf_expr<usf_mul<X1, X2> > {
	X1 const& _u;
	X2 const& _v;
public:

	usf_mul(usf_expr<X1> const& u, usf_expr<X2> const& v)
			: _u(u), _v(v) {
	}

	template<uint B>
	uint64_t eval() const {
		return _u.eval<B/2>() * _v.eval<B/2>();
	}
};

template<typename X1, typename X2>
usf_mul<X1, X2> const operator*(
		usf_expr<X1> const& u,
		usf_expr<X2> const& v) {
	return usf_mul<X1, X2>(u, v);
}

int main() {
	usf<32> u(0x00018000);
	usf<32> v(0x00010000);
	usf<32> x = u * v;

	DEBUG_HEX(x.num);

	cout << "End." << endl;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
