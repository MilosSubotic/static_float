/**
 * @file static_float_test.cpp
 * @date Jul 25, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license MIT
 *
 * @brief static_flaot test program.
 *
 * @version 2.0
 * Changelog:
 * 1.0 - Initial version.
 * 2.0 - Renamed to static_float_test.cpp.
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <vector>

using namespace std;

#include "type.h"

#include "debug.h"

#define STATIC_FLOAT_CHECKED_BUILD 1
#include "static_float.h"
#define GLM_SWIZZLE
#include "static_float_vector_math.h"

#include "type_collector.h"

#include "frac.h"

#include <glm/glm_log.h>

///////////////////////////////////////////////////////////////////////////////

#if 0

template<typename InputType>
vector<float> algorithm() {
	InputType x = 1.0f;
	InputType y = 0.5f;
	vector<float> res;

	PRINT_TYPE(x);
	DEBUG(x);
	DEBUG(float(x));
	res.emplace_back(x);
	PRINT_TYPE(y);
	DEBUG(y);
	DEBUG(float(y));
	res.emplace_back(y);

	auto x_add_y = x + y;
	PRINT_TYPE(x_add_y);
	DEBUG(x_add_y);
	DEBUG(float(x_add_y));
	res.emplace_back(x_add_y);

	auto x_add_y_add_y = x_add_y + y;
	PRINT_TYPE(x_add_y_add_y);
	DEBUG(x_add_y_add_y);
	DEBUG(float(x_add_y_add_y));
	res.emplace_back(x_add_y_add_y);

	auto x_mul_y = x * y;
	PRINT_TYPE(x_mul_y);
	DEBUG(x_mul_y);
	DEBUG(float(x_mul_y));
	res.emplace_back(x_mul_y);

	auto x_mul_y_add_x = x_mul_y + x;
	PRINT_TYPE(x_mul_y_add_x);
	DEBUG(x_mul_y_add_x);
	DEBUG(float(x_mul_y_add_x));
	res.emplace_back(x_mul_y_add_x);

	auto x_add_y_add_x_mul_y = x_add_y + x_mul_y;
	PRINT_TYPE(x_add_y_add_x_mul_y);
	DEBUG(x_add_y_add_x_mul_y);
	DEBUG(float(x_add_y_add_x_mul_y));
	res.emplace_back(x_add_y_add_x_mul_y);

	auto x_sub_y = x - y;
	PRINT_TYPE(x_sub_y);
	res.emplace_back(x_sub_y);

	cout << endl;

	return res;
}

void diff_res(const vector<float>& r0, const vector<float>& r1) {
	assert(r0.size() == r1.size());
	for(uint i = 0; i < r0.size(); i++){
		float diff = abs(float(r0[i] - r1[i]));
		if(diff > 0.0000001f){
			cout << "Result differs!" << endl;
			assert(0 && "Result differs!");
			break;
		}
	}
}

#endif

#define sf_float_assert(sf, f) \
	do{ \
		if(float(sf) != f){ \
			std::cerr << __FILE__ << ":" << __LINE__ << ": " \
				<< __PRETTY_FUNCTION__ << ": " \
				<< "\nStatic float assert:" \
				<< "\n " << #sf << " = " << sf \
				<< "\n " << #f << " = " << f \
				<< std::endl; \
			exit(134); \
		} \
	}while(0)

#define sf_float_delta_assert(sf, f) \
	do{ \
		float epsilon = std::numeric_limits<decltype(sf)>::epsilon(); \
		float error = ::abs(float(sf) - f); \
		if(error > epsilon){ \
			std::cerr << __FILE__ << ":" << __LINE__ << ": " \
				<< __PRETTY_FUNCTION__ << ": " \
				<< "\nStatic float delta assert:" \
				<< "\n epsilon = " << epsilon \
				<< "\n error = " << error \
				<< "\n " << #sf << " = " << sf \
				<< "\n " << #f << " = " << f \
				<< std::endl; \
			exit(134); \
		} \
	}while(0)

#define try_for_except \
	ok = false; \
	try

#define catch_except(except) \
	catch(except&){ \
		ok = true; \
	} \
	assert(ok);


template<uint B, int E>
void test_inversesqrt(){
	using namespace static_float;
	using glm::inversesqrt;

	typedef usf<B, E> type;

	auto x = std::numeric_limits<type>::min();
	auto end = std::numeric_limits<type>::max();
	auto eps = std::numeric_limits<type>::epsilon();

	while(true){
		x += eps;

		auto y = inversesqrt(x);
		float fx = float(x);
		float fy = inversesqrt(fx);

		// FIXME Bad precision. Should use epsilon of y.
		assert(abs(float(y) - fy) <= float(eps));
		//sf_float_delta_assert(y, fy);

		if(x == end){
			break;
		}
	}

}

template<uint B, int E>
void test_normalize(){
	using namespace static_float;
	using namespace glm;
	using namespace vector_math;

	typedef sf<B, E> type;

	auto eps = std::numeric_limits<type>::epsilon();
	auto start = std::numeric_limits<type>::min() + 1;
	auto end = std::numeric_limits<type>::max();

	auto x = start;
	auto x_end = end;
	while(true){
		auto y = x;
		auto y_end = end;
		while(true){
			auto z = y;
			auto z_end = end;
			while(true){

				tvec3<type> v = tvec3<type>(x, y, z);
				tvec3<float> vf = tvec3<float>(v);

				auto n = normalize(v);
				tvec3<float> nf = normalize(vf);

				sf_float_delta_assert(n.x, nf.x);
				sf_float_delta_assert(n.y, nf.y);
				sf_float_delta_assert(n.z, nf.z);

				if(z == z_end){
					break;
				}
				z += eps;
			}

			if(y == y_end){
				break;
			}
			y += eps;
		}

		if(x == x_end){
			break;
		}
		x += eps;
	}
}

template<uint B1, int E1, uint B2, int E2>
static_float::sf<B1, E1>
dot(const glm::detail::tvec3<static_float::sf<B1, E1>>& x, const glm::detail::tvec3<static_float::sf<B2, E2>>& y) {
	return x.x;
}

int main() {

	using namespace static_float;
	using namespace static_float::exceptions;
	using namespace frac_ns;

	bool ok;

	////////////////////////////////////

	DEFINE_TC("collected_types.txt", true);

	////////////////////////////////////
	// num_type.
/*
	sf<7> s7;
	assert(sizeof(s7.num) == 1);

	sf<8> s8;
	assert(sizeof(s8.num) == 2);

	////////////////////////////////////
	// sf(float f)

	sf<1, 0> s10_1 = 1.0;
	assert(s10_1.num == 1);

	// TODO Change all std::exception to appropriate static_float error.

	try_for_except{
		sf<1, 0> s10_2 = 2.0;
	}catch_except(std::exception)

	sf<1, 1> s11_2 = 2.0;
	assert(s11_2.num == 1);

	try_for_except{
		sf<1, 1> s11_4 = 4.0;
	}catch_except(std::exception)

	sf<1, 0> s10_m1 = -1.0;
	assert(s10_m1.num == -1);

	try_for_except{
		sf<1, 0> s10_4 = -2.0;
	}catch_except(std::exception)

	sf<1, 1> s11_m2 = -2.0;
	assert(s11_m2.num == -1);

	try_for_except{
		sf<1, 1> s11_m4 = -4.0;
	}catch_except(std::exception)


	sf<1, -1> s1m1_0_5 = 0.5;
	assert(s1m1_0_5.num == 1);

	try_for_except{
		sf<1, -1> s1m1_1 = 1.0;
	}catch_except(std::exception)

	////////////////////////////////////
	// sf(const sf<I2, E2>& s2)

	sf<9, -8> s9m8_1 = 1;
	auto s7m8_0 = sf<7, -8>(s9m8_1);
	sf_float_assert(s7m8_0, 0);

	////////////////////////////////////
	// operator=(const sf<I2, E2>& s2)

	sf<3, 0> s30_3 = 3;
	sf_float_assert(s30_3, 3);
	sf<3, 1> s31_2;
	s31_2 = s30_3;
	sf_float_assert(s31_2, 2);

	// static_assert
#if 0
	sf<1, 0> s10_2;
	s10_2 = s11_2;
#endif

	////////////////////////////////////
	// operator float() const

	sf_float_assert(s10_1, 1);
	sf_float_assert(s11_2, 2);
	sf_float_assert(s1m1_0_5, 0.5);

	////////////////////////////////////
	// operator+(const sf<I1, E1>& s1, const sf<I2, E2>& s2)

	sf<3, 2> s32_12 = 12;
	sf_float_assert(s32_12, 12);
	sf<3, 1> s31_10 = 10;
	sf_float_assert(s31_10, 10);
	sf<5, 1> s51_24 = 24;
	sf_float_assert(s51_24, 24);

	auto add_22 = s32_12 + s31_10;
	sf_float_assert(add_22, 22);
	assert(add_22.b == 5);
	assert(add_22.e == 1);

	auto add_36 = s32_12 + s51_24;
	sf_float_assert(add_36, 36);
	assert(add_36.b == 6);
	assert(add_36.e == 1);

	////////////////////////////////////
	// operator*(const sf<I1, E1>& s1, const sf<I2, E2>& s2)

	auto mul_120 = s32_12 * s31_10;
	sf_float_assert(mul_120, 120);
	assert(mul_120.b == 6);
	assert(mul_120.e == 3);

	////////////////////////////////////
	// operator/(const sf<I1, E1>& s1, const sf<I2, E2>& s2)

	sf<4, 0> s40_10 = 10;
	sf<2, 1> s21_2 = 2;

	auto div_5 = s40_10 / s21_2;
	sf_float_assert(div_5, 5);
	assert(div_5.b == 6);
	assert(div_5.e == -3);

	auto back_10 = div_5 * s21_2;
	sf_float_assert(back_10, 10);
	assert(back_10.b == 8);
	assert(back_10.e == -2);

	auto div_2_4 = s51_24 / s31_10;
	sf_float_delta_assert(div_2_4, 2.4);
	assert(div_2_4.b == 8);
	assert(div_2_4.e == -3);

	auto div_2 = s51_24 / s32_12;
	sf_float_assert(div_2, 2);
	//sf_float_delta_assert(div_2, 2);
	assert(div_2.b == 8);
	assert(div_2.e == -4);

	auto div_6 = s32_12 / s11_2;
	sf_float_assert(div_6, 6);
	//sf_float_delta_assert(div_6, 6);
	assert(div_6.b == 4);
	assert(div_6.e == 0);

	////////////////////////////////////
	// sf& operator+=(const sf<I2, E2>& s2)

	constexpr uint acc_iters = 4;
	constexpr uint acc_I = 3;
	assert(acc_I == ceil(log2(acc_iters)) + 1);
	sf<s10_1.b + acc_I, s10_1.e> acc = 0;
	for(uint i = 0; i < acc_iters; i++){
		acc += s10_1;
	}
	sf_float_assert(acc, s10_1 * acc_iters);

	////////////////////////////////////
*/
	using glm::detail::tvec3;
	using ::dot;
	tvec3<sf<2, 0>> v3_s20_0_1_2(0, 1, 2);
	tvec3<sf<3, 0>> v3_s30_3_4_5(3, 4, 5);
	tvec3<sf<2, 1>> v3_s21_2_4_6(2, 4, 6);

	auto dot_14 = dot(v3_s20_0_1_2, v3_s30_3_4_5);/*
	sf_float_assert(dot_14, 14);
	assert(dot_14.b == 7);
	assert(dot_14.e == 0);

	auto dot_16 = dot(v3_s20_0_1_2, v3_s21_2_4_6);
	sf_float_assert(dot_16, 16);
	assert(dot_16.b == 6);
	assert(dot_16.e == 1);

	auto cross_m3_6_m3 = cross(v3_s20_0_1_2, v3_s30_3_4_5);
	sf_float_assert(cross_m3_6_m3.x, -3);
	sf_float_assert(cross_m3_6_m3.y, 6);
	sf_float_assert(cross_m3_6_m3.z, -3);
	assert(cross_m3_6_m3.x.b == 6);
	assert(cross_m3_6_m3.x.e == 0);

	auto cross_m2_4_m2 = cross(v3_s20_0_1_2, v3_s21_2_4_6);
	sf_float_assert(cross_m2_4_m2.x, -2);
	sf_float_assert(cross_m2_4_m2.y, 4);
	sf_float_assert(cross_m2_4_m2.z, -2);
	assert(cross_m2_4_m2.x.b == 5);
	assert(cross_m2_4_m2.x.e == 1);

	////////////////////////////////////
	// usf()

	// TODO

	////////////////////////////////////
	// unsign()

	assert(float(unsign(s10_1)) == float(s10_1));

	try_for_except{
		unsign(sf<1, 0>(-1.0));
	}catch_except(std::exception)

	////////////////////////////////////
	// sq()

	assert(sq(sf<1, 0>(3)) == unsign(sf<1, 0>(3) * sf<1, 0>(3)));
	assert(sq(sf<1, 0>(-3)) == unsign(sf<1, 0>(-3) * sf<1, 0>(-3)));

	////////////////////////////////////
	// inversesqrt()

	test_inversesqrt<4, -4>();
	test_inversesqrt<5, -4>();
	test_inversesqrt<6, -4>();
	test_inversesqrt<5, -5>();
	test_inversesqrt<6, -5>();
	test_inversesqrt<7, -5>();
	//test_inversesqrt<22, 0>(); // Max B.

	////////////////////////////////////
*/
#if 0
	{
		constexpr uint B = 6;
		constexpr int E = 0;

		using glm::inversesqrt;

		typedef usf<B, E> type;

		auto x = std::numeric_limits<type>::min();
		auto end = std::numeric_limits<type>::max();
		auto eps = std::numeric_limits<type>::epsilon();

		while(true){
			x += eps;

			auto y = inversesqrt(x);
			float fx = float(x);
			float fy = inversesqrt(fx);
			y.num += 1;

			DEBUG_ALL(x);
			DEBUG_ALL(y);
			NEW_LINE();

			assert(abs(float(y) - fy) <= float(eps));

			if(x == 16){
				break;
			}
		}

		for(uint i = 2; i <= 16; i++){
			float f = i;
			cout << i << ": " << inversesqrt(f-1) - inversesqrt(f) << endl;
		}

	}
#endif
/*

	////////////////////////////////////
	// normalize()

	using namespace glm;
	using namespace vector_math;

	{
		auto v = tvec3<sf<6, -5>>(1.978475, 1.0, 1.0);
		tvec3<float> vf = tvec3<float>(v);

		auto n = normalize(v);
		tvec3<float> nf = normalize(vf);

		sf_float_delta_assert(n.x, nf.x);
		sf_float_delta_assert(n.y, nf.y);
		sf_float_delta_assert(n.z, nf.z);
	}

	{
		// Max B.
		auto v = tvec3<sf<30, 0>>(1.0, 1.0, 1.0);
		tvec3<float> vf = tvec3<float>(v);

		auto n = normalize(v);
		tvec3<float> nf = normalize(vf);

		sf_float_delta_assert(n.x, nf.x);
		sf_float_delta_assert(n.y, nf.y);
		sf_float_delta_assert(n.z, nf.z);
	}

#if 0
	test_normalize<6, -2>();
#endif

	{
		auto v = tvec3<sf<30, -21>>(-3.8147e-05, -0.00025034, -0.000166893);
		tvec3<float> vf = tvec3<float>(v);

		auto n = normalize(v);
		tvec3<float> nf = normalize(vf);

		sf_float_delta_assert(n.x, nf.x);
		sf_float_delta_assert(n.y, nf.y);
		sf_float_delta_assert(n.z, nf.z);
	}

	////////////////////////////////////
	// int_big_t and uint_big_t.

	{
		using namespace static_float::detail;

		uint_big_t x = 1e40; // Need more than 128 bits.
		assert(double(x) == 1e40);
		int_big_t y = 1e40; // Need more than 128 bits.
		assert(double(y) == 1e40);

		uint128_t u_128 = 1e20 + 1e15;
		uint_big_t u_big = 0;
		u_big += uint64_t(u_128 >> 64);
		u_big <<= 64;
		u_big += uint64_t(u_128);
		assert(double(u_big) == double(u_128));

		int128_t i_128 = -1e20 - 1e15;
		int_big_t i_big = 0;
		i_big += int64_t(i_128 >> 64);
		i_big <<= 64;
		i_big += uint64_t(i_128);
		assert(double(i_big) == double(i_128));
	}
	{
		using namespace static_float::detail;

		uint128_t u_128 = 1e20 + 1e15;
		uint_big_t u_big = u_128;
		assert(double(u_big) == double(u_128));

		int128_t i_128 = -1e20 - 1e15;
		int_big_t i_big = i_128;
		assert(double(i_big) == double(i_128));
	}

	{
		using namespace static_float::detail;

		//auto v = tvec3<int_big_t>(-3.8147e-05, -0.00025034, -0.000166893);
	}
*/
	////////////////////////////////////

#if 0
	////////////////////////////////////

	typedef sf<6> int_m128_127;

	NEW_LINE();
	int_m128_127 s0_0 = 0.0f;
	PRINT_TYPE(s0_0);
	DEBUG((int)s0_0.num);
	DEBUG(s0_0);
	DEBUG(float(s0_0));
	sf_float_delta_assert(s0_0, 0.0f);

	NEW_LINE();
	int_m128_127 s1_0 = 1.0f;
	PRINT_TYPE(s1_0);
	DEBUG((int)s1_0.num);
	DEBUG(s1_0);
	DEBUG(float(s1_0));
	sf_float_delta_assert(s1_0, 1.0f);

	NEW_LINE();
	int_m128_127 s0_5 = 0.5f;
	PRINT_TYPE(s0_5);
	DEBUG((int)s0_5.num);
	DEBUG(s0_5);
	DEBUG(float(s0_5));
	sf_float_delta_assert(s0_5, 0.5f);

	NEW_LINE();
	int_m128_127 s0_75 = 0.75f;
	PRINT_TYPE(s0_75);
	DEBUG((int)s0_75.num);
	DEBUG(s0_75);
	DEBUG(float(s0_75));
	sf_float_delta_assert(s0_75, 0.75f);

	NEW_LINE();
	int_m128_127 s0_25 = 0.25f;
	PRINT_TYPE(s0_25);
	DEBUG((int)s0_25.num);
	DEBUG(s0_25);
	DEBUG(float(s0_25));
	sf_float_delta_assert(s0_25, 0.25f);

	NEW_LINE();
	int_m128_127 s0_33 = 0.33f;
	PRINT_TYPE(s0_33);
	DEBUG((int)s0_33.num);
	DEBUG(s0_33);
	DEBUG(float(s0_33));
	sf_float_delta_assert(s0_33, 0.33f);

	NEW_LINE();
	int_m128_127 sm1_0 = -1.0f;
	PRINT_TYPE(sm1_0);
	DEBUG((int)sm1_0.num);
	DEBUG(sm1_0);
	DEBUG(float(sm1_0));
	sf_float_delta_assert(sm1_0, -1.0f);

	NEW_LINE();
	int_m128_127 sm0_5 = -0.5f;
	PRINT_TYPE(sm0_5);
	DEBUG((int)sm0_5.num);
	DEBUG(sm0_5);
	DEBUG(float(sm0_5));
	sf_float_delta_assert(sm0_5, -0.5f);

	vector<float> ref = algorithm<float>();

	auto r0 = algorithm<int_m128_127>();

	diff_res(ref, r0);

	TC(sm1_0);



	////////////////////////////////////

	NEW_LINE();
	typedef sf<5, 1> fix_vertex_t;
	fix_vertex_t s2_0 = 2.0f;
	PRINT_TYPE(s2_0);
	DEBUG((int)s2_0.num);
	DEBUG(s2_0);
	DEBUG(float(s2_0));
	assert(float(s2_0) == 2.0f);

	////////////////////////////////////

	TC(s2_0);

	////////////////////////////////////

	try{
		NEW_LINE();
		fix_vertex_t s2_1 = 2.1f;
	}catch(std::exception& e){
		cout << e.what() << endl;
		cout << "Passed overflow check!" << endl;
	}
	NEW_LINE();

	////////////////////////////////////

	static_assert(glm::detail::type<fix_vertex_t>::is_float,
			"Need float type for glm operations.");

	NEW_LINE();

	////////////////////////////////////

	/*
	using glm::detail::tvec2;
	tvec2<sf<6, 0>> vx2_1;
	tvec2<sf<6, 0>> vy2_1;
	auto d2_1 = dot(vx2_1, vy2_1);
	PRINT_TYPE(d2_1);
	tvec2<sf<6, 0>> vx2_2;
	tvec2<sf<5, 1>> vy2_2;
	auto d2_2 = dot(vx2_2, vy2_2);
	PRINT_TYPE(d2_2);
	*/

	using static_float::tvec3;
	tvec3<sf<6, 0>> vx3_1;
	tvec3<sf<6, 0>> vy3_1;
	auto d3_1 = dot(vx3_1, vy3_1);
	PRINT_TYPE(d3_1);
	tvec3<sf<6, 0>> vx3_2;
	tvec3<sf<5, 1>> vy3_2;
	auto d3_2 = dot(vx3_2, vy3_2);
	PRINT_TYPE(d3_2);

	glm::vec3 vx3_1_glm(tvec3<float>(vx3_1));

	////////////////////////////////////

	NEW_LINE();
	sf<13, 3> x = 2;
	sf<26, 6> y = 1.92188;
	auto fxy = float(x) * float(y);
	auto sfxy = x * y;
	sf_float_delta_assert(sfxy, fxy);

	////////////////////////////////////

	NEW_LINE();
	sf<10, 2> s2 = 2.256;
	DEBUG(s2);
	DEBUG_HEX(s2.num);
	auto s0 = sf<14, 3>(s2);
	DEBUG(s0);
	DEBUG_HEX(s0.num);
	sf_float_delta_assert(s0, float(s2));

	auto s3 = sf<9, 2>(s2);
	DEBUG(s3);
	DEBUG_HEX(s3.num);
	sf_float_delta_assert(s3, float(s2));

	////////////////////////////////////


	{
		NEW_LINE();
		sf<2, 0> a = 1.0;
		PRINT_TYPE(a);
		auto acc0 = a;
		PRINT_TYPE(acc0);
		auto acc1 = acc0 + a;
		PRINT_TYPE(acc1);
		auto acc2 = acc1 + a;
		PRINT_TYPE(acc2);
		auto acc3 = acc2 + a;
		PRINT_TYPE(acc3);
		DEBUG(acc3);
		DEBUG_HEX((int)acc3.num);
	}

	{
		NEW_LINE();
		sf<2, 0> a = 1.0;
		PRINT_TYPE(a);
		auto a0_0 = a + a;
		PRINT_TYPE(a0_0);
		auto a0_1 = a + a;
		PRINT_TYPE(a0_1);
		auto a1_0 = a0_0 + a0_1;
		PRINT_TYPE(a1_0);
		DEBUG(a1_0);
		DEBUG_HEX((int)a1_0.num);

	}

	{
		NEW_LINE();
		sf<2, 0> a = 1.0;
		constexpr uint acc_count = 4;
		constexpr uint acc_I = ceil(log2(acc_count)) + 1;
		DEBUG(acc_I);
		sf<2, 0, acc_I> acc = 0;
		for(uint i = 0; i < acc_count; i++){
			acc += a;
		}
		DEBUG_HEX(acc.num);
		PRINT_TYPE(acc);
		auto acc_norm = norm(acc);
		PRINT_TYPE(acc_norm);
		DEBUG(acc_norm);
		DEBUG_HEX(acc_norm.num);

	}

	////////////////////////////////////

	rt::mul<float, float>::rt a;
	PRINT_TYPE(a);
	rt::mul<sf<2, 0>, sf<3, 1>>::rt b;
	PRINT_TYPE(b);

	////////////////////////////////////
	// frac test.

	using namespace frac_ns;

	frac<float, float> f0(2, 1);
	frac<float, float> f1(3, 1);
	auto f2 = f0 * f1;
	PRINT_TYPE(f2);
	DEBUG(f2);

	typedef sf<3, 3> sf_t;
	frac<sf_t, sf_t> sf0(2, 1);
	frac<sf_t, sf_t> sf1(3, 1);
	auto sf2 = sf0 * sf1;
	PRINT_TYPE(sf2);
	DEBUG(sf2);

	////////////////////////////////////
	// Division test.

	sf<0, 0> num0 = 1;
	DEBUG(num0);
	DEBUG_HEX(num0);
	sf<1, 1> den0 = 2;
	DEBUG(den0);
	DEBUG_HEX(den0);
	sf<0, -1, 2> exp_div0;// = 0.5;
	exp_div0.num = 1;
	DEBUG(exp_div0);
	DEBUG_HEX(exp_div0);
	auto div0 = num0 / den0;
	PRINT_TYPE(div0);
	DEBUG(div0);
	DEBUG_HEX(div0);

	////////////////////////////////////
	// int128_t test.

#if USE_INT128_T
	{
		typedef long double d_t;
		typedef int128_t i_t;

#define i_d_delta_assert(i, d) \
	assert(abs(d_t(i) - d) < \
			float(std::numeric_limits<long double>::epsilon()))

		i_t i0 = 2L << 50 + 2l << 20;
		d_t d0 = 2l << 50 + 2l << 20;
		i_t i1 = 2l << 49 + 2l << 21;
		d_t d1 = 2l << 49 + 2l << 21;

		i_t i_add = i0 + i1;
		d_t d_add = d0 + d1;
		i_d_delta_assert(i_add, d_add);

		i_t i_sub = i0 - i1;
		d_t d_sub = d0 - d1;
		i_d_delta_assert(i_sub, d_sub);

		i_t i_mul = i0 * i1;
		d_t d_mul = d0 * d1;
		i_d_delta_assert(i_mul, d_mul);

		i_t i_sl = i0 << 20;
		d_t d_sl = d0 * exp2(20);
		i_d_delta_assert(i_sl, d_sl);

		i_t i_rl = i_sl >> 20;
		d_t d_rl = d_sl / exp2(20);
		i_d_delta_assert(i_rl, d_rl);

		assert(i0 > i1 == d0 > d1);
		assert(i0 >= i1 == d0 >= d1);
		assert(i0 >= i0 == d0 >= d0);

		assert(-(-i0) == i0);

		auto fb = 0x10000000000000000_int128_t;
		PRINT_TYPE(fb);
		DEBUG(fb);
	}
#endif

	////////////////////////////////////
	// More than 64 bits test.

	NEW_LINE();

	sf<50> a_50_bits = 0.2;
	DEBUG(a_50_bits);
	sf<50> b_50_bits = 0.3;
	auto c_100_bits = a_50_bits * b_50_bits;
	PRINT_TYPE(c_100_bits);
	PRINT_TYPE(c_100_bits.num);
	DEBUG(c_100_bits.num);
	DEBUG(c_100_bits);
	sf<50> c_50_bits = c_100_bits;
	DEBUG(c_50_bits);

	detail::int_big_t bt = c_100_bits;
#endif
	////////////////////////////////////

	NEW_LINE();

	////////////////////////////////////

	cout << "End" << endl;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
