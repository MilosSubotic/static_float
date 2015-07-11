/**
 * @file type.h
 * @date Jul 25, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license MIT
 *
 * @brief Getting type of variable.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef TYPE_H_
#define TYPE_H_

///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <typeinfo>

///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__

#include <cstdlib>
#include <cxxabi.h>

inline std::string demangle(const char* name) {

	int status;

	char* demangled = abi::__cxa_demangle(name, NULL, NULL, &status);

	if(!status){
		std::string ret(demangled);
		std::free(demangled);
		return ret;
	}else{
		return name;
	}
}

#else

// Does nothing if not g++
inline std::string demangle(const char* name) {
	return name;
}

#endif

template <class T>
inline std::string type(const T& t) {
	return demangle(typeid(t).name());
}

///////////////////////////////////////////////////////////////////////////////

#endif // TYPE_H_
