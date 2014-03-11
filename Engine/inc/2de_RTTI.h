#ifndef _2DE_RTTI_H_
#define _2DE_RTTI_H_

#define _SECURE_SCL 0
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <typeinfo>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <stdexcept>

#include "2de_Property.h"
#include "2de_Error.h"

/**
 *	TODO:
 *	Right now contents of this module have no relation to it's name.
 *	This should be resolved.
 */

namespace Deku2D
{
	using std::string;
	using std::map;
	using std::vector;

	template <typename T>
	class IsPointer
	{
	public:
		enum { result = false, };
	};

	template <typename T>
	class IsPointer<T*>
	{
	public:
		enum { result = true, };
	};

	template <typename T>
	class IsIntegral
	{
	public:
		enum { result = false, };
	};

	#define D2D_DECLARE_INTEGRAL_TYPE(T)	\
		template <>	\
		class IsIntegral<T>	\
		{	\
		public:	\
			enum { result = true, };	\
		};	\

	D2D_DECLARE_INTEGRAL_TYPE(char)
	D2D_DECLARE_INTEGRAL_TYPE(int)
	D2D_DECLARE_INTEGRAL_TYPE(float)
	D2D_DECLARE_INTEGRAL_TYPE(bool)
	D2D_DECLARE_INTEGRAL_TYPE(std::string)

	template<typename T>
	class IsAbstract
	{
		// todo: ensure sizeof(char) == 1 and sizeof(short) == 2
		template<class U> static char test( U (*)[1] );
		template<class U> static short test( ... );

	public:
		enum { result = sizeof( test<T>( 0 ) ) - 1 };
	};

	template<typename T, int A = IsAbstract<T>::result>
	class Make
	{
	public:
		static T* New()
		{
			return new T();
		}
	};

	template<typename T>
	class Make<T, 1>
	{
	public:
		static T* New()
		{
			D2D_RUNTIME_ERROR("Deku2D::Make can't instantiate an abstract class.")
		}
	};

}	//	namespace Deku2D

#endif	// _2DE_RTTI_H_
