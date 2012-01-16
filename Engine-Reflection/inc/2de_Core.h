#ifndef _2DE_CORE_H_
#define _2DE_CORE_H_

#ifdef _MSC_VER

// DAMNIT, move it somewhere and delete unnecessary ones, please.. // I have already commented it out, so some time will pass and I'll check most of warnings.

//	#pragma warning (disable	:	4312)   //
// 	#pragma warning (disable	:	4311)	//	'type cast' : pointer truncation from 'void *' to
// 	#pragma warning (disable	:	4267)	//	conversion from 'size_t' to 'int', possible loss of data
// 	#pragma warning (disable	:	4305)	//	'initializing' : truncation from 'int' to 'scalar'
// 	#pragma warning (disable	:	4244)	//	 conversion from 'int' to 'scalar', possible loss of data
// 	#pragma warning (disable	:	4996)	
// 	#pragma warning (disable	:	4172)	//	returning address of local variable or temporary (!!!)
// 	#pragma warning (disable	:	4996)	//	rare
// 	#pragma warning (disable	:	4312)	//	conversion from 'int' to 'void *' of greater size (!!)
// 	#pragma warning (disable	:	4800)	//	forcing value to bool 'true' or 'false' (performance warning)
// 	#pragma warning (disable	:	4018)	//	signed/unsigned mismatch (!)
// 	#pragma warning (disable	:	4715)	//	not all control paths return a value (!!)
// 	#pragma warning (disable	:	4291)	//	void *operator new(unsigned,const char *,int)' : no matching operator delete found; memory will not be freed if initialization throws an exception
//after w4
// 	#pragma warning (disable	:	4706)	//	assignment within conditional expression (!!!)
// 	#pragma warning (disable	:	4701)	//	potentially uninitialized local variable 'origin_const' used
// 	 #pragma warning (disable	:	4201)	//	nonstandard extension used : nameless struct/union (!!!)
// 	#pragma warning (disable	:	4100)	//	unreferenced formal parameter
// 	 #pragma warning (disable	:	4239)	//	nonstandard extension used : 'return' : conversion from 'Matrix3' to 'Matrix3 &' (!!!)
// 	#pragma warning (disable	:	4189)	//	local variable is initialized but not referenced
// 	 #pragma warning (disable	:	4238)	//	nonstandard extension used : class rvalue used as lvalue (!!!)
// 	#pragma warning (disable	:	4389)	//	signed/unsigned mismatch
// 	#pragma warning (disable	:	4702)	//	unreachable code ^^"
// 	#pragma warning (disable	:	4611)	//	interaction between '_setjmp' and C++ object destruction is non-portable (???)
// 	#pragma warning (disable	:	4005)	// Macro redefinition
// 	#pragma warning (disable	:	4714)	// __forceinline not inlined
// 	#pragma warning (disable	:	4005)	// Macro redefinition
// 	#pragma warning (disable	:	4355)	// this in initializer list
// 	#pragma warning (disable	:	4505)	// for dirent: unreferenced function has been removed

#define VC_LEANMEAN
#define _CRT_SECURE_NO_DEPRECATE
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

#include "2de_Object.h"

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

	//	#define DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS


	typedef unsigned char byte;

	//	"Forever" instead of "for(;;)", anyway, just kidding.
	//#define forever for(;;)

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

	#define DEAD_BEEF 0xdeadbeef
	#define DEAD_FOOD 0xdeadf00d

	#define DISABLE_DEBUG_BOXES
	//#undef DISABLE_DEBUG_BOXES

	namespace _details {	// Stolen from http://aka-rider.livejournal.com/4949.html 
							// if it won't come in handy, I'll remove it.
		namespace typetag_private {

		/**
		*  address of this structure is used as typetag
		*/
		template <typename _Ty> struct typetag_holder {};

		template <typename _Ty>
		inline const void* GenTypeTag()
		{
			static typetag_holder<_Ty> typetagInst;
			return reinterpret_cast<void*>(&typetagInst);
		}

		} // namespace typetag_private

		template <typename C>
		inline const void * typetag()
		{
			return _details::typetag_private::GenTypeTag<C>();
		}

		/** 
		*  remove modifier removes const, volatile and others modifiers
		*/
		template <typename _Ty>
		struct remove_modifier
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<const _Ty>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<volatile _Ty>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<_Ty&>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<const _Ty&>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<_Ty*>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<const _Ty*>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<const _Ty* const>
		{
			typedef _Ty type;
		};

		template <typename _Ty>
		struct remove_modifier<volatile _Ty*>
		{
			typedef _Ty type;
		};


		// А вот это уже из александреску
		// Счетчик значений
		template <class C>
		class CType2ValCounter
		{
		protected:
			static C Counter;
		};

		template<class C> C CType2ValCounter<C>::Counter;

		// Генератор значений
		template<class T, class C>
		class CType2ValGenerator : public CType2ValCounter<C>
		{
		public:
			C ID;
			CType2ValGenerator()
			{
				ID = CType2ValCounter<C>::Counter;
				++CType2ValCounter<C>::Counter;
			}
		};


	} // namespace _details

	template <typename C>
	inline const void * typetag()
	{
		return _details::typetag_private::
		GenTypeTag<typename _details::remove_modifier<C>::type>();
	}

	// Шаблонная функция получения идентификатора типа
	template <class T, class C>
	C Type2Val()
	{
		static _details::CType2ValGenerator<T, C> ValueGenerator;

		return ValueGenerator.ID;
	}

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
			// ios::boolalpha - have you looked for it, while trying to reinvent the wheel? // it doesn't suit our needs.. i don't remember why, though..
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

	}	//	namespace VariantConvert

	template<typename T>
	T from_string(const string &s)
	{
		return VariantConvert::from_string_impl(s, identity<T>());
	}

	// Template class for some manager
	template <typename C>	// C - container type
	class CCommonManager : public virtual CObject // is it necessary?
	{
	public:
		typedef C ManagerContainer;
		typedef typename C::value_type T;
		ManagerContainer Objects;

	public:		
		typedef typename ManagerContainer::iterator ManagerIterator;
		typedef typename ManagerContainer::const_iterator ManagerConstIterator;

		T Get(const string &AName)	// I think, we're better to avoid search by object's name. Search by ID or address instead.
		{					// 	hashmap, anyone?...
			T result = NULL;

			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
			{
				if ((*i)->isDestroyed())
				{
					continue;
				}
				if ((*i)->GetName() == AName)
				{
					result = *i;
					break;
				}
			}

			return result;
		}

		virtual void Add(const T &AObject)
		{
			Objects.push_back(AObject);
		}

		void Remove(const T &AObject)
		{
			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
			{
				if (*i == AObject)
				{
					Objects.erase(i);
					return;
				}
			}
		}

		void Remove(size_t AID)
		{
			T temp = NULL;
			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
				if ((*i)->GetID() == AID)			
				{
					temp = *i;
					break;
				}
			if (temp == NULL)
				return;
			Objects.remove(temp);
		}

		void Remove(const string &AName)
		{
			T temp = NULL;
			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
				if ((*i)->GetName() == AName)
				{
					temp = *i;
					break;
				}
			if (temp == NULL)
				return;
			Objects.remove(temp);
		}

		CCommonManager()
		{
			ClassName = "CommonManager";
		}

		virtual ~CCommonManager()
		{
			Objects.clear();
		}
	};


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
	 * CSingletonManager - a class that is responsible for destroying singletones.
	 */

	class CSingletonManager
	{
	public:
		static CSingletonManager* Instance();
		static void Init();
		void Add(CObject *AObject);
		void Clear();
		static void Finalize();

	private:
		CSingletonManager()
		{
		}
		static CSingletonManager *_instance;
		stack<CObject*> Singletones;
	};


	/**
	 * CTSingleton - singleton template class with automatic deletion using CSingletonManager. Acts like a singleton holder.
	 *
	 * To use it just create a static instance of this class as follows:
	 * 
	 * 	class CSomeClass
	 * 	{
	 * 	public:
	 * 		<...>
	 * 	private:
	 * 		<...>
	 *	protected:
	 *		friend class CTSingleton<CSomeClass>;	// <-- don't forget it
	 *		CSomeClass() { }
	 * 	};
	 *
	 * 	static CTSingleton<CSomeClass> SomeClass;
	 *
	 * Now you can acces your singletoned class with -> operator, like this:
	 * 	SomeClass->SomeMethod();
	 *
	 *
	 * @todo: более описательное имя, а то разных реализаций синглтона всё-таки бывает много.
	 *	@todo: переделать всё это, как_у_александреску.
	 */

	template <typename T>
	class CTSingleton
	{
	public:
		static T* Instance();

		T* operator->()
		{
			return Instance();
		}

	private:
		static T * _instance;

	#ifdef _DEBUG
		static set<const std::type_info *> UnderConstruction;
	#endif // _DEBUG
	};

	template <typename T>
	T* CTSingleton<T>::Instance()
	{
	#ifdef _DEBUG
		if (UnderConstruction.count(&typeid(T)) == 1)
		{
			// we can't use CLog here (because it's singleton too), but still able to log to stdout
			Environment::LogToStdOut("ERROR", "Recursive singleton constructor call has been discovered, typeid: '%s'", typeid(T).name());

			throw std::logic_error(string("Recursive singleton constructor call has been discovered, typeid: ") + typeid(T).name());
		}
	#endif // _DEBUG

		if (!_instance)
		{
	#ifdef _DEBUG
			UnderConstruction.insert(&typeid(T));
	#endif // _DEBUG

			_instance = new T;
			CSingletonManager::Instance()->Add(_instance);

	#ifdef _DEBUG
			UnderConstruction.erase(&typeid(T));
	#endif // _DEBUG
		}
		return _instance;
	}

	template <typename T>
	T* CTSingleton<T>::_instance = 0;

	#ifdef _DEBUG
	template <typename T>
	set<const std::type_info *> CTSingleton<T>::UnderConstruction;
	#endif // _DEBUG

	/**
	 * Let there be Visitor
	 * Taken from http://www.everfall.com/paste/id.php?bqa1eibc559f
	 * http://www.gamedev.ru/community/oo_design/articles/?id=431
	 * Cause it's Hierarchical visitor there will be
	 * VisitOnEnter
	 * VisitOnLeave
	 */

	class IVisitorBase
	{
	public:	
		virtual ~IVisitorBase() {};
	};

	template <typename T, typename R = void>
	class IVisitor : virtual public IVisitorBase
	{
	public:
		typedef R CReturnType;
		virtual CReturnType VisitOnEnter(T&) = 0;
		virtual CReturnType VisitOnLeave(T&) = 0;
	};

	template <typename R = void>
	class IVisitableBase
	{
	public:
		typedef R CReturnType;
		virtual ~IVisitableBase() {}
		virtual CReturnType AcceptOnEnter(IVisitorBase&) = 0;
		virtual CReturnType AcceptOnLeave(IVisitorBase&) = 0;

	protected:
		template <typename T>
		static CReturnType ConcreteAcceptOnEnter(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnEnter(visited);
			}
			return CReturnType();
		}
		template <typename T>
		static CReturnType ConcreteAcceptOnLeave(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnLeave(visited);
			}
			return CReturnType();
		}
	};

	template <typename R = void>
	class IVisitableObject : public CObject
	{
		D2D_DECLARE_RTTI(IVisitableObject, CObject)

	public:
		typedef R CReturnType;
		virtual ~IVisitableObject() {}
		virtual CReturnType AcceptOnEnter(IVisitorBase&) = 0;
		virtual CReturnType AcceptOnLeave(IVisitorBase&) = 0;

	protected:
		template <typename T> 
		static CReturnType ConcreteAcceptOnEnter(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnEnter(visited);
			}
			return CReturnType();
		}
		template <typename T> 
		static CReturnType ConcreteAcceptOnLeave(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnLeave(visited);
			}
			return CReturnType();
		}
	};

	#define D2D_DECLARE_VISITABLE()	\
		virtual CReturnType AcceptOnEnter(IVisitorBase& visitor)	\
		{ return ConcreteAcceptOnEnter(*this, visitor); }	\
		virtual CReturnType AcceptOnLeave(IVisitorBase& visitor)	\
		{ return ConcreteAcceptOnLeave(*this, visitor); }

	/**
	* CStorage - base class for CFile and CMemory classes, that describes their interface.
	*/

	class CStorage
	{
	public:
		enum EOpenMode
		{
			OPEN_MODE_READ,
			OPEN_MODE_WRITE,
		};
		
		enum ESeekOrigin
		{
			SEEK_ORIGIN_BEGINNING,
			SEEK_ORIGIN_CURRENT,
			SEEK_ORIGIN_END,
		};

		CStorage();

		virtual bool Close() = 0;

		virtual bool Read(void *Buffer, size_t BytesCount, size_t ElementsCount = 1) = 0;
		virtual bool Write(const void *Data, size_t BytesCount, size_t ElementsCount = 1) = 0;
		
		// kinda-type-and-size-safe versions of Read and Write.. don't work with arrays, though..
		template<typename T>
		bool Read(T *Buffer)
		{
			return Read(Buffer, sizeof(T));
		}

		template<typename T>
		bool Write(const T *Data)
		{
			return Write(Data, sizeof(T));
		}

		bool ReadByte(byte *Buffer);
		bool WriteByte(byte *Data);
		bool WriteByte(byte Data);

		// i think, that CFile should not provide posibility to read 0-terminated strings from files..
		// formats should be developed in such way, that does NOT include reading variable-length strings from binary files.. it's bad design of data structures to do so..
		// BUT even if someone ever need to store string in binary file, they should develop structure for storing string size too, together with string.. and then deal with it in higher level of abstraction..
		// YES OFCOURSE YOU"RE FUCKIING RIGHT> LET"S DO IT

		bool ReadString(char *Buffer, int ASize);
		bool ReadString(string &Buffer);
		bool WriteString(const char *Data);
		bool WriteString(const string &Data);

		bool WriteText(const char *Data);
		bool WriteText(const string &Data);

		virtual bool ReadLine(char *Buffer, int ASize) = 0;
		virtual bool ReadLineS(string &Buffer);
		bool WriteLine(const char *Data);
		bool WriteLine(const string &Data);

		virtual string GetContent() = 0;
		bool SetContent(const string &Data);

		virtual bool Seek(long Offset, ESeekOrigin Origin) = 0;
		bool Rewind();
		virtual bool Flush();

		virtual bool Eof() const = 0;
		virtual bool Good() const = 0;
		virtual long Size() const = 0;

	protected:
		bool WriteLine(const char *Data, size_t Size);

		EOpenMode Mode;

		const int READ_BUFFER_DEFAULT_SIZE;

	private:
		// CStorage can't be copied or assigned.
		CStorage(const CStorage &Source);
		CStorage& operator=(const CStorage &Source);
	};

	/**
	* CFile - a class that provides interface to reading and writing of files.
	*
	* Each instance of the class represents a single file, implementing RAII principle.
	* This class is a front-end for stdio. Also extends it with some lazy-functions, like get file contents into a string.
	* Can't be copied or assigned.
	*/

	class CFile : public CStorage
	{
	public:
		CFile();
		CFile(const string &AFilename, EOpenMode AMode);
		~CFile();

		bool Open(const string &AFilename, EOpenMode AMode);
		bool Close();

		bool Read(void *Buffer, size_t BytesCount, size_t ElementsCount = 1);
		bool Write(const void *Data, size_t BytesCount, size_t ElementsCount = 1);
		
		bool ReadLine(char *Buffer, int ASize);

		string GetContent();

		bool Seek(long Offset, ESeekOrigin Origin);
		bool Flush();

		bool Eof() const;
		bool Good() const;
		long Size() const;

	private:
		FILE *File;
		string Filename;
	};

	class CMemory : public CStorage
	{
	public:
		CMemory();
		CMemory(byte *ABeginningPointer, long ALength, EOpenMode AMode);
		~CMemory();

		bool Open(byte *ABeginningPointer, long ALength, EOpenMode AMode);
		bool Close();

		bool Read(void *Buffer, size_t BytesCount, size_t ElementsCount = 1);
		bool Write(const void *Data, size_t BytesCount, size_t ElementsCount = 1);

		bool ReadLine(char *Buffer, int ASize);

		string GetContent();

		bool Seek(long Offset, ESeekOrigin Origin);

		bool Eof() const;
		bool Good() const;
		long Size() const;

	private:
		byte *BeginningPointer;
		byte *CurrentPointer;
		long Length;
	};

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

	template <typename T>
	__INLINE string to_string(const T& t)
	{
		ostringstream s;
		s << t;
		return s.str();
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


	/**
	*	Memory leaks catching.
	*	Some issues are fixed but there's still a room for improvement.
	*	Taken from http://www.flipcode.com/archives/How_To_Find_Memory_Leaks.shtml by Dion Picco (23 May 2000)
	*	@todo: Implement new[] and delete []
	*	@todo: Remove inline from AddTrack and RemoveTrack and others.
	*/

	#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

	#include <list>

	struct ALLOC_INFO
	{
		unsigned long address;
		unsigned long size;
		char file[MAX_PATH];//bad but anyway... probably needs replacement with string type
		unsigned long line;
	};

	typedef list<ALLOC_INFO*> AllocList;

	extern AllocList *allocList;

	inline void AddTrack(unsigned long addr,  unsigned long asize,  const char *fname, unsigned long lnum)
	{
		ALLOC_INFO *info;

		if (allocList == NULL)
			allocList = new(AllocList);

		info = new(ALLOC_INFO);
		info->address = addr;
		strncpy(info->file, fname, MAX_PATH);
		info->file[MAX_PATH] = 0;
		info->line = lnum;
		info->size = asize;
		allocList->insert(allocList->begin(), info);
	};

	void RemoveTrack(unsigned long addr);
	void DumpUnfreed();

	inline void * operator new(unsigned size, const char *file, int line)
	{
		void *ptr = (void *)malloc(size);
		AddTrack((unsigned long)ptr, size, file, line);
		return(ptr);
	};

	inline void operator delete(void *p)
	{
		RemoveTrack((unsigned long)p);
		free(p);
	};

	inline void operator delete[](void *p)
	{
		RemoveTrack((unsigned long)p);
		free(p);
	};

	#ifdef _DEBUG
		#define DEBUG_NEW new(__FILE__, __LINE__)
	#else
		#define DEBUG_NEW new
	#endif
		#define new DEBUG_NEW

	#endif // defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

}	//	namespace Deku2D

#endif // _2DE_CORE_H_
