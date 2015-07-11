/**
 * @file type_collector.h
 * @date Sep 20, 2014
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

#ifndef TYPE_COLLECTOR_H_
#define TYPE_COLLECTOR_H_

///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////

class type_collector {
public:
	type_collector(const char* file_name, bool collect) {
		_collect = collect;
		if(collect){
			_ofs = new std::ofstream(file_name);
		}else{
			_ofs = nullptr;
		}
	}

	~type_collector() {
		if(_collect){
			_ofs->close();
			delete _ofs;
		}
	}

	bool collect() const {
		return _collect;
	}
	std::ostream& ofs() {
		return *_ofs;
	}

private:
	bool _collect;
	std::ofstream* _ofs;
};

#define DEFINE_TC(file_name, collect) \
	type_collector tc(file_name, collect)

#define TC(var) \
	do{ \
		if(tc.collect()){ \
			tc.ofs() << __FILE__ << " @ " << __LINE__ \
				<< ": Type of variable \"" << #var \
				<< "\" is \"" << type(var)  << "\"" << endl;\
		} \
	}while(0)


///////////////////////////////////////////////////////////////////////////////

#endif // TYPE_COLLECTOR_H_
