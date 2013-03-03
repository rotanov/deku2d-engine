#pragma once

#define D2D_RUNTIME_ERROR(MESSAGE)\
	throw std::runtime_error(std::string(MESSAGE) + " At file: \"" + __FILE__ + "\", line: " + Deku2D::to_string(__LINE__) );\

#define D2D_ASSERT(EXPRESSION)\
	if (!(EXPRESSION))\
		throw std::runtime_error("Assertion failed. Expression: \"" + std::string(#EXPRESSION) + "\" At file: \"" + __FILE__ + "\", line: " + Deku2D::to_string(__LINE__) );\


