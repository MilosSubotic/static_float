/**
 * @file static_float_exceptions.h
 * @date Sep 27, 2014
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

#ifndef STATIC_FLOAT_EXCEPTIONS_H_
#define STATIC_FLOAT_EXCEPTIONS_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <sstream>
#include <execinfo.h>

///////////////////////////////////////////////////////////////////////////////

namespace static_float {

	namespace exceptions {
		// Forward declarations.
		class exception;
		inline exception& endl(exception& e);

		/**
		 * @class Exception
		 * @brief Exception with ability to concatenate with ostream operator.
		 */
		class exception : public std::exception {
			friend inline exception& endl(exception& e);
		public:
			explicit exception() {
				grab_backtrace();
			}
		    virtual ~exception() noexcept {}

		    ///////////////////////////////////

		public:
			exception(const exception& e) {
				_oss << e._oss.str();
			}
			exception& operator=(const exception& e) {
				if(this != &e){
					_oss << e._oss.str();
				}
				return *this;
			}

		    ///////////////////////////////////

		public:
		    /**
		     * Concatenate new message on exception message.
		     * @param e exception
		     * @param t Data to be concatenated.
		     * @return argument e
		     */
		    template<typename Type>
		    exception& operator<<(const Type& t){
		    	_oss << t;
		    	return *this;
		    }

		    /**
		     * Ostream function for manipulators.
		     * @param pf Manipulator.
		     * @return this.
		     */
		    exception& operator<<(exception& (*pf)(exception&)){
		    	return pf(*this);
		    }

		    ///////////////////////////////////

		public:
		    /**
		     * @return Description of error.
		     */
		    virtual const char* what() const noexcept {
		    	_what = _oss.str();
		    	return _what.c_str();
		    }

		    ///////////////////////////////////

		protected:
		    std::ostringstream _oss;
		    mutable std::string _what;

		    void grab_backtrace() {
		        void* callstack[256];
				int frames = ::backtrace(
						callstack,
						sizeof(callstack) / sizeof(callstack[0]));
		        char** symbols = backtrace_symbols(callstack, frames);
		        try {
		        	_oss << "Stack Trace:";
		            for (int i = 0; i < frames; ++i) {
		            	_oss << "\n\t" << symbols[i];
		            }
		            _oss << std::endl;
		        } catch(const std::exception &) {
		            std::free(symbols);
		            throw;
		        }
		        std::free(symbols);
		    }

		};

		inline exception& endl(exception& e){
			e._oss << std::endl;
			return e;
		}

		class runtime_error : public exception {
		public:
			explicit runtime_error() {}
			virtual ~runtime_error() noexcept {}
		};

		class overflow_error : public runtime_error {
		public:
			explicit overflow_error() {}
			virtual ~overflow_error() noexcept {}
		};

		class out_of_range_error : public runtime_error {
		public:
			explicit out_of_range_error() {}
			virtual ~out_of_range_error() noexcept {}
		};

		class sign_error : public runtime_error {
		public:
			explicit sign_error() {}
			virtual ~sign_error() noexcept {}
		};

	} // namespace exceptions

} // namespace static_float

///////////////////////////////////////////////////////////////////////////////

#endif // STATIC_FLOAT_EXCEPTIONS_H_
