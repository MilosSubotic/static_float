/**
 * @file debug.h
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

#ifndef DEBUG_H_
#define DEBUG_H_

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdio>

#include "type.h"

///////////////////////////////////////////////////////////////////////////////

#define DEBUG(var) \
	do{ \
		std::cout << #var << " = " << var << std::endl; \
	}while(0)

#define PRINT_TYPE(var) \
	do{ \
		std::cout << "Type of variable \"" << #var \
			<< "\" is \"" << type(var)  << "\"" << std::endl; \
	}while(0)

#define DEBUG_HEX(var) \
	do{ \
		printf("%s = 0x%08x\n", #var, var); \
		fflush(stdout); \
	}while(0)

#define NEW_LINE() \
	do{ \
		std::cout << endl; \
	}while(0)

#define DEBUG_ALL(sf) \
	do{ \
		DEBUG(sf); \
		DEBUG_HEX(sf.num); \
		DEBUG(sf.b); \
		DEBUG(sf.e); \
	}while(0)

///////////////////////////////////////////////////////////////////////////////

#endif // DEBUG_H_
