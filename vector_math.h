/**
 * @file vector_math.h
 * @date Oct 12, 2014
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

#ifndef VECTOR_MATH_H_
#define VECTOR_MATH_H_

///////////////////////////////////////////////////////////////////////////////

#include <glm/glm.hpp>

#include "return_type.h"

///////////////////////////////////////////////////////////////////////////////

namespace vector_math {

	template<typename T>
	class tvec3 : public glm::detail::tvec3<T> {
	public:
		typedef T value_type;

		//////////////////////////////////////
		// Implicit basic constructors

		tvec3()
				: glm::detail::tvec3<T>() {
		}
		template <typename U>
		tvec3(tvec3<U> const & v)
				: glm::detail::tvec3<T>(v.x, v.y, v.z) {
		}
		template <typename U>
		tvec3(glm::detail::tvec3<U> const & v)
				: glm::detail::tvec3<T>(v) {
		}

		//////////////////////////////////////
		// Explicit basic constructors

		template <typename U>
		explicit tvec3(U const & s)
				: glm::detail::tvec3<T>(s) {
		}
		template <typename U, typename V, typename W>
		explicit tvec3(
				U const & x,
				V const & y,
				W const & z)
				: glm::detail::tvec3<T>(x, y, z) {
		}

/*
		tvec3(const tvec3& v)
			: glm::detail::tvec3<T>(v.x, v.y, v.z){
		}

		~tvec3(){}
*/

		//////////////////////////////////////

		tvec3<T> & operator=(tvec3<T> const & v){
			glm::detail::tvec3<T>::operator=(v);
			return *this;
		}
		tvec3<T> & operator=(glm::detail::tvec3<T> const & v){
			return glm::detail::tvec3<T>::operator=(v);
		}
		template <typename U>
		tvec3<T> & operator=(glm::detail::tvec3<U> const & v){
			glm::detail::tvec3<T>::operator=(v);
			return *this;
		}

		////////////////////////////////
	};

	////////////////////////////////////

	template<typename T1, typename T2>
	typename rt::dot3<T1, T2>::rt
	dot(const tvec3<T1>& x, const tvec3<T2>& y) {
		return x.x * y.x + x.y * y.y + x.z * y.z;
	}

	template<typename T1, typename T2>
	tvec3<typename rt::cross<T1, T2>::rt>
	cross(const tvec3<T1>& x, const tvec3<T2>& y) {
		return tvec3<typename rt::cross<T1, T2>::rt>(
			x.y * y.z - y.y * x.z,
			x.z * y.x - y.z * x.x,
			x.x * y.y - y.x * x.y
        );
	}


	template<typename T1, typename T2>
	tvec3<typename rt::add_sub<T1, T2>::rt>
	operator+(const tvec3<T1>& x, const tvec3<T2>& y) {
		return tvec3<typename rt::add_sub<T1, T2>::rt>(
			x.x + y.x,
			x.y + y.y,
			x.z + y.z
		);
	}

	template<typename T1, typename T2>
	tvec3<typename rt::add_sub<T1, T2>::rt>
	operator-(const tvec3<T1>& x, const tvec3<T2>& y) {
		return tvec3<typename rt::add_sub<T1, T2>::rt>(
			x.x - y.x,
			x.y - y.y,
			x.z - y.z
		);
	}

	template<typename T1, typename T2>
	tvec3<typename rt::mul<T1, T2>::rt>
	operator*(const tvec3<T1>& x, const T2& y) {
		return tvec3<typename rt::mul<T1, T2>::rt>(
			x.x * y,
			x.y * y,
			x.z * y
		);
	}

	////////////////////////////////////

	template<typename T>
	tvec3<typename rt::normalize<T>::rt>
	normalize(const tvec3<T>& x) {
		auto sqr = x.x * x.x + x.y * x.y + x.z * x.z;
	    return x * glm::inversesqrt(sqr);
	}

	////////////////////////////////////

} // namespace vector_math


///////////////////////////////////////////////////////////////////////////////

#endif // VECTOR_MATH_H_
