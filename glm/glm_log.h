/**
 * @file glm_log.h
 * @date Jul 31, 2012
 *
 * @brief Ostream operators for glm data types.
 * @version 3.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license MIT
 *
 */

#ifndef GLM_LOG_H_
#define GLM_LOG_H_

///////////////////////////////////////////////////////////////////////////////

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ostream>

///////////////////////////////////////////////////////////////////////////////

namespace glm {

	namespace detail {
		template<typename T>
		T cfp(const T& t) {
			return t;
		}

		inline int cfp(char t) {
			return int(t);
		}

		inline unsigned int cfp(unsigned char t) {
			return (unsigned int) t;
		}
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tvec2<T>& v) {
		os << "vec2("
				<< detail::cfp(v.x) << ", "
				<< detail::cfp(v.y) << ')';
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tvec3<T>& v) {
		os << "vec3("
				<< detail::cfp(v.x) << ", "
				<< detail::cfp(v.y) << ", "
				<< detail::cfp(v.z) << ')';
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tvec4<T>& v) {
		os << "vec4("
				<< detail::cfp(v.x) << ", "
				<< detail::cfp(v.y) << ", "
				<< detail::cfp(v.z) << ", "
				<< detail::cfp(v.w)	<< ')';
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tmat2x2<T>& m) {
		os << "mat2(\n";
		for(unsigned c = 0; c < 2; c++){
			os << detail::cfp(m[c][0]) << ", "
					<< detail::cfp(m[c][1]);
			os << '\n';
		}
		os << ')';
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tmat3x3<T>& m) {
		os << "mat3(\n";
		for(unsigned c = 0; c < 3; c++){
			os << detail::cfp(m[c][0]) << ", "
					<< detail::cfp(m[c][1]) << ", "
					<< detail::cfp(m[c][2]);
			os << '\n';
		}
		os << ')';
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tmat4x4<T>& m) {
		os << "mat4(\n";
		for(int c = 0; c < 4; c++){
			os << detail::cfp(m[c][0]) << ", "
					<< detail::cfp(m[c][1]) << ", "
					<< detail::cfp(m[c][2]) << ", "
					<< detail::cfp(m[c][3]);
			os << '\n';
		}
		os << ')';
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const detail::tquat<T>& q) {
		os << "quat("
				<< detail::cfp(q.w) << ", "
				<< detail::cfp(q.x) << ", "
				<< detail::cfp(q.y) << ", "
				<< detail::cfp(q.z)
				<< ')';
		return os;
	}


} // namespace glm

///////////////////////////////////////////////////////////////////////////////

#endif // GLM_LOG_H_

