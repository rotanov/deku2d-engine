#ifndef _2DE_CORE_H_
#define _2DE_CORE_H_

#ifdef _MSC_VER
	// чёрт возьми, перепроверьте это уже кто-нибудь и удалите лишнее...
	// если уж у строгого GCC остался 1 варнинг (ну может 3, если считать и демки), то у похуистического мсвс ну никак не может быть 25..

	//#pragma warning (disable	:	4312)   //
	#pragma warning (disable	:	4311)	//	'type cast' : pointer truncation from 'void *' to
	#pragma warning (disable	:	4267)	//	conversion from 'size_t' to 'int', possible loss of data
	#pragma warning (disable	:	4305)	//	'initializing' : truncation from 'int' to 'scalar'
	#pragma warning (disable	:	4244)	//	 conversion from 'int' to 'scalar', possible loss of data
	#pragma warning (disable	:	4996)	
	#pragma warning (disable	:	4172)	//	returning address of local variable or temporary (!!!)
	#pragma warning (disable	:	4996)	//	rare
	#pragma warning (disable	:	4312)	//	conversion from 'int' to 'void *' of greater size (!!)
	#pragma warning (disable	:	4800)	//	forcing value to bool 'true' or 'false' (performance warning)
	#pragma warning (disable	:	4018)	//	signed/unsigned mismatch (!)
	#pragma warning (disable	:	4715)	//	not all control paths return a value (!!)
	#pragma warning (disable	:	4291)	//	void *operator new(unsigned int,const char *,int)' : no matching operator delete found; memory will not be freed if initialization throws an exception
	//after w4
	#pragma warning (disable	:	4706)	//	assignment within conditional expression (!!!)
	#pragma warning (disable	:	4701)	//	potentially uninitialized local variable 'origin_const' used
	 #pragma warning (disable	:	4201)	//	nonstandard extension used : nameless struct/union (!!!)
	#pragma warning (disable	:	4100)	//	unreferenced formal parameter
	 #pragma warning (disable	:	4239)	//	nonstandard extension used : 'return' : conversion from 'Matrix3' to 'Matrix3 &' (!!!)
	#pragma warning (disable	:	4189)	//	local variable is initialized but not referenced
	 #pragma warning (disable	:	4238)	//	nonstandard extension used : class rvalue used as lvalue (!!!)
	#pragma warning (disable	:	4389)	//	signed/unsigned mismatch
	#pragma warning (disable	:	4702)	//	unreachable code ^^"
	#pragma warning (disable	:	4611)	//	interaction between '_setjmp' and C++ object destruction is non-portable (???)
	#pragma warning (disable	:	4005)	// Macro redefinition
	#pragma warning (disable	:	4714)	// __forceinline not inlined
	#pragma warning (disable	:	4005)	// Macro redefinition

	#define VC_LEANMEAN
	#define _CRT_SECURE_NO_DEPRECATE
	#define NOMINMAX
#endif // _MSC_VER

#include <IL/il.h>
#include <SDL/SDL.h>

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
#include <memory.h>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

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
#endif //_WIN32


using namespace std;

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

#ifdef _WIN32
	#define CFILE_DEFAULT_MAXIMUM_PATH_LENGTH MAX_PATH
#else
	#define CFILE_DEFAULT_MAXIMUM_PATH_LENGTH 260
#endif //_WIN32

#ifdef _MSC_VER
	#define snprintf _snprintf
#endif //_MSC_VER

//#define DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS


typedef unsigned char		byte;

//	"Forever" instead of "for(;;)", anyway, just kidding.
#define Forever for(;;)

template<typename T>
__INLINE void SAFE_DELETE(T*& a)
{
	delete a, a = NULL;
}

template<typename T>
__INLINE void SAFE_DELETE_ARRAY(T*& a)
{
	delete [] a, a = NULL;
}

#define DEAD_BEEF 0xdeadbeef
#define DEAD_FOOD 0xdeadf00d

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
* CVariantConvert - содержит реализации функций конвертирования из строки в произвольный простой тип (из поддерживаемых stringstream).
*/

class CVariantConvert	// Looks more like a namespace
{
public:
	template<typename T>
	static T from_string_impl(const string &s, identity<T>)
	{
		T result;
		stringstream ss;
		ss >> noskipws;
		ss.str(s);
		ss >> result;
		return result;
	}

	static bool from_string_impl(const string &s, identity<bool>)
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

	static string from_string_impl(const string &s, identity<string>)
	{
		return s;
	}
};

template<typename T>
T from_string(const string &s)
{
	return CVariantConvert::from_string_impl(s, identity<T>());
}

class CObject; // forward declaration

class CEvent
{
public:
	CEvent();
	CEvent(const string &AName, CObject *ASender);

	template<typename T>
	T GetData(const string &AName) const;

	template<typename T>
	void SetData(const string &AName, const T &AValue);

	string GetName() const;
	void SetName(const string &AName);

	CObject* GetSender() const;
	void SetSender(CObject *ASender);

private:
	string Name;
	CObject *Sender;
	map<string, string> Data;
};

/**
* CObject - base class for many classes.
*/

class CObject
{
public:
	CObject();
	virtual ~CObject();

	void IncRefCount();
	static void DecRefCount(CObject* AObject);

	string GetName() const;
	virtual void SetName(const string &AObjectName);

	unsigned int GetID() const;

	bool isDestroyed() const;
	void SetDestroyed();

	bool isManaged() const;

	virtual void ProcessEvent(const CEvent &AEvent);

protected:
	bool Managed;
	bool Destroyed;
	unsigned int ID;
	string Name;

private:
	CObject(const CObject &AObject);
	CObject& operator=(const CObject &AObject);
	//size_t RefCount;

	static unsigned int CObjectCount;

	friend class CFactory;
};

//////////////////////////////////////////////////////////////////////////
// CGameObject

class CGameObject : public CObject
{
private:
	class traverse_iterator
	{
	protected:
		CGameObject *GameObject;

		bool IsEnd() const;
		bool IsValid() const;

	public:
		traverse_iterator(CGameObject &AGameObject);
		CGameObject& operator *();
		const CGameObject& operator *() const;
	};

public:	
	CGameObject *Parent;
	std::vector<CGameObject*> Children;

	class traverse_iterator_bfs : public traverse_iterator
	{
	private:
		std::queue<CGameObject*> Queue;
		unsigned int Index; // Index of current object in it's parent Children

	public:
		traverse_iterator_bfs(CGameObject &AGameObject);

		bool Ok();

		traverse_iterator_bfs& operator++();
		traverse_iterator_bfs& operator--();

		bool operator ==(const traverse_iterator_bfs &rhs) const;
		bool operator !=(const traverse_iterator_bfs &rhs) const;
	};

	class traverse_iterator_dfs : public traverse_iterator
	{
	private:
		std::vector<std::pair<CGameObject*, unsigned int> > Path;

	public:

	};

	CGameObject();
	virtual ~CGameObject();

	void Attach(CGameObject* AGameObject);

	void SetParent(CGameObject* AGameObject);

	template<typename TypeIterator>
	void Detach(const TypeIterator &Iterator)
	{
		(*Iterator)->Parent = NULL;
		std::swap(*Iterator, *(--Children.end()));
		Children.pop_back();
	}

	virtual void JustDoIt();
};

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
		//AObject->IncRefCount();
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
		//CObject::DecRefCount(temp);
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
		//CObject::DecRefCount(temp);
	}

	virtual ~CCommonManager()
	{
		/*for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
			CObject::DecRefCount(*i);*/
		Objects.clear();
	}
};

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

// SingletonHolder по Александреску
// начал, но пока не осилил.. // а вообще нахер он нам нужен?... он же громоздкий, блядь..
/*template<class T>
class CreateUsingNew
{
public:
	T* Create()
	{
		return new T;
	}
};

template<class T>
class CreateUsingMalloc
{
public:
	T* Create()
	{
		T *result = (T *) malloc(sizeof(T));
		new(result) T();
		return result;
	}
};

template<class T>
class CreateStatic
{
public:
	T* Create()
	{
		static T instance;
		return &instance;
	}
};

template<class T>
class DefaultLifetime
{

};


template<class T>
class SingleThreaded
{
public:
	typedef T VolatileType;

};

template<class T,
	template<class> class CreationPolicy = CreateUsingNew,
	template<class> class LifetimePolicy = DefaultLifetime,
	template<class> class ThreadingModel = SingleThreaded>
class CSingletonHolder
{
public:
	static T& Instance();

private:
	static void DestroySingleton();

	CSingletonHolder();

	typedef ThreadingModel<T>::VolatileType InstanceType;

	static InstanceType* _instance;
	static bool Destroyed;

};

template<...>
T& CSingletonHolder<...>::Instance()
{
	if (!_instance)
	{
		if (Destroyed)
		{
			LifetimePolicy<T>::OnDeadReference();
			Destroyed = false;
		}
		_instance = CreationPolicy<T>::Create();
		LifetimePolicy<T>::SheduleCall(&DestroySingleton);
	}

	return *_instance;
}

template<...>
void CSingletonHolder<...>::DestroySingleton()
{
	assert(!Destroyed);
	CreationPolicy<T>::Destroy(_instance);
	_instance = 0;
	Destroyed = true;
}*/


/**
 * CTSingleton - шаблонизированный класс синглтона с автоматическим удалением через SingletoneKiller.
 *
 * Для использования нужно наследовать класс от него следующим образом:
 * 	class CSomeClass : public CTSingleton<CSomeClass>
 * 	{
 * 	public:
 * 		<...>
 * 	private:
 * 		<...>
 *	protected:
 *		friend class CTSingleton<CSomeClass>;
 *		CSomeClass() { }
 * 	};
 *
 * @todo: более описательное имя, а то разных реализаций синглтона всё-таки бывает много.
 *	@todo: переделать всё это, как_у_александреску.
 */

template <typename T>
class CTSingleton : public virtual CObject
{
public:
	static T* Instance();

protected:
	CTSingleton()
	{
	}

	virtual ~CTSingleton()
	{
		_instance = 0;
	}

private:
	static T * _instance;

#ifdef _DEBUG
	static set<const type_info *> UnderConstruction;
#endif // _DEBUG
};

template <typename T>
T* CTSingleton<T>::Instance()
{
#ifdef _DEBUG
	if (UnderConstruction.count(&typeid(T)) == 1)
	{
		// well, we can't log it....
		//Log("ERROR", "Recursive singleton constructor call has been discovered, typeid: '%s'", typeid(T).name());

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
set<const type_info *> CTSingleton<T>::UnderConstruction;
#endif // _DEBUG

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
* CFile - класс, представляющий собой интерфейс к чтению и записи файлов.
*
* Каждый экземпляр класса управляет отдельным файлом, реализует принцип RAII.
* Является надстройкой над stdio. Также дополняет его всякими функциями-ленивчиками, типа получить всё содержимое в строку и т. п.
* Нельзя копировать и присваивать.
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
* CFileSystem - класс, содержащий статические функции для кросслпатформенной работы с файловой системой.
*/

class CFileSystem
{
public:
	static bool Exists(const string &APath);
	
	// TODO: extend, add more functions..

};

/**
* CLog - класс для работы с логом.
*/

class CLog : public CTSingleton<CLog>
{
public:
	enum ELogMode
	{
		LOG_MODE_STDOUT,
		LOG_MODE_STDERR,
		LOG_MODE_FILE,
	};

	enum ELogFileWriteMode
	{
		LOG_FILE_WRITE_MODE_TRUNCATE,
		LOG_FILE_WRITE_MODE_APPEND,
	};

	~CLog();

	void WriteToLog(const char *Event, const char *Format, ...);

	bool isEnabled() const;
	void Toggle(bool AEnabled);

	ELogMode GetLogMode() const;
	void SetLogMode(ELogMode ALogMode);

	ELogFileWriteMode GetLogFileWriteMode() const;
	void SetLogFileWriteMode(ELogFileWriteMode ALogFileWriteMode);

	bool GetDatedLogFileNames() const;
	void SetDatedLogFileNames(bool ADatedLogFileNames);

	void SetLogFilePath(const string &ALogFilePath);
	string GetLogFilePath() const;

	void SetLogName(const string &ALogName);
	string GetLogName() const;

protected:
	CLog();
	friend class CTSingleton<CLog>;

private:
	bool Enabled;
	ELogMode LogMode;
	ELogFileWriteMode LogFileWriteMode;
	bool DatedLogFileNames;
	ostream *Stream;
	string LogFilePath;
	string LogName;
};

// define SIMPLIFIED_LOG to use simple logging to std-out, instead of singleton-helled CLog... sometimes it's useful for debugging..
#ifdef SIMPLIFIED_LOG
	#define Log CEnvironment::LogToStdOut
#else
	#define Log CLog::Instance()->WriteToLog
#endif // SIMPLIFIED_LOG

/**
* CEnvironment - содержит статические функции и возможно какие-нибудь константы, которые относятся к окружению выполнения.
*
* В идеале, в будущем, весь (ну или не весь, хз пока) платформо-зависимый код следует скинуть куда-то в это место.
*/

class CEnvironment
{
public:
	class DateTime
	{
	public:
		static tm* GetLocalTimeAndDate();
		static string GetFormattedTime(tm *TimeStruct, const char *Format);
	};

	class Paths
	{
	public:
		static string GetExecutablePath();

		static string GetWorkingDirectory();
		static void SetWorkingDirectory(const string &AWorkingDirectory);

		static string GetConfigPath();
		static void SetConfigPath(const string &AConfigPath);

		static string GetLogPath();
		static void SetLogPath(const string &ALogPath);

		static string GetUniversalDirectory();

	private:
		static string ConfigPath;
		static string LogPath;
	};

	class Variables
	{
	public:
		static string Get(const string &AName);
		static void Set(const string &AName, const string &AValue);
	};

	static void LogToStdOut(const char *Event, const char *Format, ...);
	static string GetLineTerminator();

};


// @todo: taking into account, that global functions is evil, may be we should move such kind of functions
//	in some class, named, for example, CEnvironment// Agreed with you.
// CEnvironment has been created. Some time/date and paths functions has already been moved there.
// It's good practice to move all platform-dependent code (basically, code with ifdefs)
// 	to one dedicated place, if it's possible, so we should continue this work.

void DelFNameFromFPath(char *src); // standard function "dirname", anyone?.. or does it work in another way?.. i dunno..
void DelExtFromFName(char *src);
void DelLastDirFromPath(char *src);

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

/**
* CCaseInsensetiveComparison - класс-функтор, сравнивающий строки регистро-независимо. Используется STL-контейнерами и т. п. вещами.
*/

class CCaseInsensetiveComparison
{
public:
	bool operator()(const string &lhs, const string &rhs) const
	{
		return (SDL_strcasecmp(lhs.c_str(), rhs.c_str()) < 0);
	}
};

template<typename T>
T CEvent::GetData(const string &AName) const
{
	map<string, string>::const_iterator it = Data.find(AName);
	if (it == Data.end())
	{
		Log("ERROR", "Event '%s' doesn't have data field named '%s'", Name.c_str(), AName.c_str());
		return from_string<T>("");
	}

	return from_string<T>(it->second);
}

template<typename T>
void CEvent::SetData(const string &AName, const T &AValue)
{
	Data[AName] = to_string(AValue);
}


/**
*	Отлов утечек памяти.
*	Немного пофикшен, однако все равно нуждается в тестировании и улучшении.
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

inline void * operator new(unsigned int size, const char *file, int line)
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

#endif // _2DE_CORE_H_
