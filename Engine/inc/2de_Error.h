#ifndef _2DE_ERROR_H_
#define _2DE_ERROR_H_

#include <sstream>

#define D2D_RUNTIME_ERROR(MESSAGE)\
	{\
		std::ostringstream ss;\
		ss << __LINE__;\
		throw std::runtime_error(std::string(MESSAGE) + " At file: \"" + __FILE__ + "\", line: " + ss.str());\
	}\

#define D2D_ASSERT(EXPRESSION)\
	{\
		std::ostringstream ss;\
		ss << __LINE__;\
		if (!(EXPRESSION))\
			throw std::runtime_error("Assertion failed. Expression: \"" + std::string(#EXPRESSION) + "\" At file: \"" + __FILE__ + "\", line: " + ss.str());\
	}\

#endif // _2DE_ERROR_H_
