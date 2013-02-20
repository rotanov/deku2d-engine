#ifndef _2DE_CORE_H_
#define _2DE_CORE_H_

#ifdef _MSC_VER
	#define VC_LEANMEAN
	#define _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_WARNINGS
	#define NOMINMAX
	#undef min
	#undef max
	#undef GetClassName
#endif // _MSC_VER

#define _SECURE_SCL 0

#include <SDL/SDL.h>
//#undef main

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <memory.h>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>
#include <iterator>
#include <limits>

#include <sys/stat.h>

#ifdef _WIN32
#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define Get  Get
#include <tchar.h>
#include "dirent/dirent.h"
#else
#include <unistd.h>
#include <dirent.h>
#define MAX_PATH 260
#endif //_WIN32


// FIXME: bad.. 2de_Core shouldn't include anything from engine
#include "2de_RTTI.h"


namespace Deku2D
{
	using std::string;
	using std::vector;
	using std::map;
	using std::stack;
	using std::ostream;
	using std::multimap;
	using std::stringstream;
	using std::list;
	using std::queue;
	using std::pair;
	using std::set;
	using std::ostringstream;
	using std::istringstream;
	using std::swap;
	using std::istream_iterator;
	using std::back_inserter;
	using std::max;
	using std::copy;
	using std::endl;
	using std::cout;
	using std::min;
	using std::ofstream;
	using std::ios_base;
	using std::cerr;
	using std::numeric_limits;

	//#define USE_SDL_OPENGL
	//#define USING_OPENGL
	#define USE_GLEW

	#if defined(_MSC_VER)
		#define __INLINE __forceinline
	#elif __GNUC__ >= 4
		#define __INLINE __inline __attribute__ ((always_inline))
	#else
		#define __INLINE inline
	#endif

	#ifdef _MSC_VER
		#define snprintf _snprintf
	#endif //_MSC_VER

	typedef unsigned char byte;

	template<typename T>
	__INLINE void SAFE_DELETE(T*& a)
	{
		delete a;
		a = NULL;
	}

	template<typename T>
	__INLINE void SAFE_DELETE_ARRAY(T*& a)
	{
		delete [] a;
		a = NULL;
	}

	#define DISABLE_DEBUG_BOXES
	//#undef DISABLE_DEBUG_BOXES

	template<typename T>
	struct identity
	{
		typedef T type;
	};

	/**
	* VariantConvert - contains implementations of functions which convert strings to arbitrary plain type, supported by stringstream.
	*/

	namespace VariantConvert
	{
		template<typename T>
		__INLINE T from_string_impl(const string &s, identity<T>)
		{
			T result;
			stringstream ss;
			ss >> std::noskipws;
			ss.str(s);
			ss >> result;
			return result;
		}

		__INLINE bool from_string_impl(const string &s, identity<bool>)
		{
			// ios::boolalpha - have you looked for it, while trying to reinvent the wheel?
			// it doesn't suit our needs.. i don't remember why, though..
			if (s == "false" || s.empty())
				return false;

			long double result;
			stringstream ss;
			ss.str(s);
			ss >> result;

			if (ss.fail())
				return true;

			return ((int) result != 0);
		}

		__INLINE string from_string_impl(const string &s, identity<string>)
		{
			return s;
		}

		template <typename T>
		__INLINE string to_string_impl(const T& t)
		{
				ostringstream s;
				s << t;
				return s.str();
		}

		template <>
		__INLINE string to_string_impl(const bool& t)
		{
			return t ? "true" : "false";
		}

	}	//	namespace VariantConvert

	template<typename T>
	T from_string(const string &s)
	{
		return VariantConvert::from_string_impl(s, identity<T>());
	}

	template<typename T>
	string to_string(const T& t)
	{
		return VariantConvert::to_string_impl(t);
	}


	/**
	* Environment - contains functions and probably some constants that are related to environment.
	*
	* Ideally in future all (or not all) platform-dependent code should be here.
	*/

	namespace Environment
	{
		namespace DateTime
		{
			tm* GetLocalTimeAndDate();
			string GetFormattedTime(tm *TimeStruct, const char *Format);
		}	//	namespace DateTime

		namespace Paths
		{
			string GetExecutablePath();

			string GetWorkingDirectory();
			void SetWorkingDirectory(const string &AWorkingDirectory);

			string GetConfigPath();
			void SetConfigPath(const string &AConfigPath);

			string GetLogPath();
			void SetLogPath(const string &ALogPath);

			string GetUniversalDirectory();
		}	//	namespace Paths

		namespace Variables
		{
			string Get(const string &AName);
			void Set(const string &AName, const string &AValue);
		}	//	namespace Variables

		void LogToStdOut(const char *Event, const char *Format, ...);
		string GetLineTerminator();
	}	//	namespace Environment




	/**
	* FileSystem - a namespace that contains functions for working with file system in a cross-platform way.
	*/

	namespace FileSystem
	{
		bool Exists(const string &APath);
		
		// TODO: extend, add more functions..

	}	//	namespace FileSystem


	// TODO: move this function somewhere.. btw, it's used only in one place: CEnvironment::Paths::GetExecutablePath()
	void DelFNameFromFPath(char *src); // POSIXes have dirname, that does exactly the same, but windows doesn't have it..

	__INLINE string itos(int i)
	{
		ostringstream s;
		s << i;
		return s.str();
	}

	__INLINE int stoi(const string &src)
	{
		stringstream s;
		s << src;
		int i;
		s >> i;
		return i;
	}

	__INLINE vector<float> GetNFloatTokensFromString( const string &source, unsigned n)
	{
		istringstream iss(source);
		vector<string> tokens;
		copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
		if (tokens.size() != n)
			throw "pizedc";
		vector<float> result;
		result.resize(n);
		for (unsigned i = 0; i < n; i++)
			result[i] = from_string<float>(tokens[i]);
		return result;
	}

	/**
	* CCaseInsensitiveComparison - functor-class, that compares strings in case-insensitive manner. Used by STL containers, etc.
	*/

	class CCaseInsensitiveComparison
	{
	public:
		bool operator()(const string &lhs, const string &rhs) const
		{
			return (SDL_strcasecmp(lhs.c_str(), rhs.c_str()) < 0);
		}
	};

	template <typename From, typename To>
	class IsConvertible
	{
	public:
		enum { result = 0, };
	};

	#define	D2D_DECLARE_CONVERTIBLE(FROM, TO)	\
		template <>	\
		class IsConvertible<FROM, TO>	\
		{	\
		public:	\
			enum { result = 1, };	\
		};	\

	D2D_DECLARE_CONVERTIBLE(string, string)
	D2D_DECLARE_CONVERTIBLE(string, bool)
	D2D_DECLARE_CONVERTIBLE(string, int)
	D2D_DECLARE_CONVERTIBLE(string, float)
	D2D_DECLARE_CONVERTIBLE(bool, string)
	D2D_DECLARE_CONVERTIBLE(int, string)
	D2D_DECLARE_CONVERTIBLE(float, string)

	// Conversion from Type to Type for the case, when exception should be thrown at runtime when there is no implementation for a conversion.
	template<typename T, int A = IsIntegral<T>::result>
	class Convert
	{
	public:
		static T FromString(const string &s)
		{
			return VariantConvert::from_string_impl(s, identity<T>());
		}

		static string ToString(const T &value)
		{
			return VariantConvert::to_string_impl(value);
		}

	};

	template<typename T>
	class Convert<T, 0>
	{
	public:
		static T FromString(const string &s)
		{
			throw "Conversion from string to the type %s is not implemented."; // TODO substitute typename if possible
		}

		static string ToString(const T &value)
		{
			throw "Conversion to string from type %s is not implemented."; // TODO substitute typename if possible
		}

	};


}	//	namespace Deku2D

#endif // _2DE_CORE_H_
