/**
 * @file return_type.h
 * @date Oct 5, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license MIT
 *
 * @brief Resolve return type of operations in compile time.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef RETURN_TYPE_H_
#define RETURN_TYPE_H_

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

namespace rt {

	// Primary template.
	template<typename T1, typename T2>
	struct add_sub {
		typedef void rt;
	};

	template<>
	struct add_sub<float, float> {
		typedef float rt;
	};


	// Primary template.
	template<typename T1, typename T2>
	struct mul {
		typedef void rt;
	};

	template<>
	struct mul<float, float> {
		typedef float rt;
	};


	template<typename T1, typename T2>
	struct div {
		typedef void rt;
	};

	template<>
	struct div<float, float> {
		typedef float rt;
	};


	template<typename T1, typename T2>
	struct dot2 {
		typedef void rt;
	};

	template<>
	struct dot2<float, float> {
		typedef float rt;
	};


	template<typename T1, typename T2>
	struct dot3 {
		typedef void rt;
	};

	template<>
	struct dot3<float, float> {
		typedef float rt;
	};


	template<typename T1, typename T2>
	struct cross {
		typedef void rt;
	};

	template<>
	struct cross<float, float> {
		typedef float rt;
	};


	template<typename T>
	struct normalize {
		typedef void tr;
	};

	template<>
	struct normalize<float> {
		typedef float rt;
	};

} // namespace rt

///////////////////////////////////////////////////////////////////////////////

#endif // RETURN_TYPE_H_
