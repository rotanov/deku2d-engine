#ifndef _2DE_CORE_H_
#define _2DE_CORE_H_

#ifdef _MSC_VER
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

#include <SDL/SDL.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <time.h>
#include <assert.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <typeinfo>

using namespace std;

#ifdef _WIN32
	#define	WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#define GetObject  GetObject
#endif  //_WIN32

#define USE_SDL_OPENGL
#define USING_OPENGL

// так как у меня SDL_EnableKeyRepeat и связанные с этим вещи работают и проблем не вызывают, то я не нашёл ничего лучше, кроме как сделать вот так...
// раскомментировать, если устраивает то, как работает SDL_EnableKeyRepeat и связанные с этим вещи
//#define I_LIKE_HOW_SDL_KEY_REPEAT_WORKS

#if defined(_MSC_VER)
	#define __INLINE __forceinline
#elif defined(__GNUC__)
	#define __INLINE __inline __attribute__ ((always_inline))
#else
	#define __INLINE inline
#endif

#define CFILE_MAX_STRING_LENGTH	1024

#ifdef _WIN32
	#define CFILE_DEFAULT_MAXIMUM_PATH_LENGTH MAX_PATH
#else
	#define CFILE_DEFAULT_MAXIMUM_PATH_LENGTH 260
#endif //_WIN32

#ifdef _MSC_VER
	#define snprintf _snprintf
#endif //_MSC_VER

/**
*	Отлов утечек памяти.
*	Taken from http://www.flipcode.com/archives/How_To_Find_Memory_Leaks.shtml by Dion Picco (23 May 2000)
*	@todo: Implement new[] and delete []
*	@todo: Remove inline from AddTrack and RemoveTrack and others.
*/

#if defined(_DEBUG) && defined(_MSC_VER)

#include <list>

struct ALLOC_INFO
{
	unsigned long address;
	unsigned long size;
	char file[64];
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
	strncpy(info->file, fname, 63);
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

#endif // defined(_DEBUG) && defined(_MSC_VER)


typedef unsigned char		byte;

typedef bool (*Callback)();
typedef bool (*UpdateProc)(float);
typedef bool (*EventFunc)(SDL_Event&);

#define KEY_PRESSED		0x00
#define KEY_RELEASED	0x01

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

// I did it for teh lulz!
#define DEAD_BEEF 0xdeadbeef
#define DEAD_FOOD 0xdeadf00d

/**
*	CObject - базовый класс для многих объектов.
*/

class CObject
{
private:
	bool Destroyed;
	size_t ID;
	string Name;
	size_t RefCount;
	static unsigned int CObjectCount;

public:
	CObject();
	virtual ~CObject();		
	void IncRefCount();
	static void DecRefCount(CObject* AObject);	
	const string& GetName() const;
	void SetName(const string &AObjectName);
	size_t GetID() const;
	bool isDestroyed() const;
	void SetDestroyed();
	virtual bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);	// FFUU~

	bool Managed;	// temporary in public, todo: setter/getter.. or maybe make it completely private and make CFactory to be friend of CObject
};

typedef bool (*CObjectCallback)(CObject *Caller);	// FFFFFUUUUU~
/**
*	Ф-я для принятия информации о нажатии кнопки, будь то мышь или клавиатура. Ввод с других устройств не поддерживается пока что.
*	Первый параметр - произошло ли событие KEY_PRESSED или KEY_RELEASED
*	Второй - какая клавиша была нажата; одновременно принимает значения мыши и клавиатуры: sdl_keysym.h, sdl_mouse.h - списки констант
*	Третий параметр - модификатор, т.е. зажат ли Shift, Ctrl, Alt итд
*	И четвёртый параметр - ASCII код.
*/
typedef bool (CObject::*KeyInputFunc)(Uint8, Uint16, SDLMod, char);	// FFFU~

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

	T GetObject(const string &AName)	// I think, we're better to avoid search by object's name. Search by ID or address instead.
	{					// 	hashmap, anyone?...
		T temp = NULL;
		ManagerContainer toDelete;
		for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
		{
			temp = *i;
			if ((*i)->isDestroyed())
			{
				toDelete.push_back(temp);
				continue;
			}
			if ((*i)->GetName() == AName)
			{
				temp = *i;
				break;
			}
		}
		for (ManagerIterator i = toDelete.begin(); i != toDelete.end(); ++i)
		{
			Objects.remove(*i);
			CObject::DecRefCount(*i);
		}
		return temp;
	}

	void Add(const T &AObject)
	{
		AObject->IncRefCount();
		Objects.push_back(AObject);
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
		CObject::DecRefCount(temp);
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
		CObject::DecRefCount(temp);
	}

	virtual ~CCommonManager()
	{
		for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
			CObject::DecRefCount(*i);
		Objects.clear();
	}
};

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
	CTSingleton() { }
	virtual ~CTSingleton()
	{
		_instance = 0;
	}

private:
	static T * _instance;
	bool Constructed;
};

void RegisterSingletoneInEngineMainClassMotherfuckers(CObject* Singletone);

template <typename T>
T* CTSingleton<T>::Instance()
{
	if (!_instance)
	{
		_instance = new T;
		_instance->Managed = true;

		RegisterSingletoneInEngineMainClassMotherfuckers(_instance);
	}
	return _instance;
}

template <typename T>
T* CTSingleton<T>::_instance = 0;

class CUpdateObject : public virtual CObject
{
public:
	bool Active;
	CUpdateObject();
	~CUpdateObject();
	bool isDead() const;
	void SetDead();
	virtual void Update(float dt) = 0;
private:
	bool Dead;
};

/**
*	CUpdateManager - менеджер объектов, которые следует обновлять. Такие дела.
*	Да, тут мало кода, надо ещё какие-нибуть ф-ии нахерачить. @todo!
*	И вообще что-то мне подсказывает, что он не здесь должен быть.
*/

class CUpdateManager : public CCommonManager <list<CUpdateObject*> >, public CTSingleton <CUpdateManager>
{
public:	
	bool UpdateObjects();
protected:
	CUpdateManager();
	friend class CTSingleton<CUpdateManager>;
};

class CBaseResource
{
protected:
	bool Loaded;		// Loaded должна быть истина если экземпляр объекта был РЕАЛЬНО загружен, а не просто проиндексирован.
public:
	string Filename;	// Полный^W хоть-какой-нибудь путь к файлу.

	CBaseResource();	// const string &AFilename
	virtual ~CBaseResource(){};
	virtual bool LoadFromFile();
	virtual bool SaveToFile();
	bool CheckLoad();
};

class CResource : public CBaseResource, virtual public CObject
{
public:
	CResource();
	virtual ~CResource(){};
};

struct CRecti	// @todo: Избавиться от этого типа.
{
	int x0, x1, y0, y1;
};

class CFile
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

	CFile();
	CFile(const string &AFilename, EOpenMode Mode);
	~CFile();

	bool Open(const string &AFilename, EOpenMode Mode);
	bool Close();
	bool ReadByte(byte *Buffer);
	bool WriteByte(byte *Buffer);
	bool WriteByte(byte Buffer);
	bool Read(void *Buffer, size_t BytesCount);
	bool Write(const void *Buffer, size_t BytesCount);
	bool ReadString(char *Buffer);
	bool ReadString(string &Buffer);
	bool WriteString(const char *Buffer);
	bool WriteString(const string &Buffer);
	bool ReadLine(char* &Data);
	bool WriteLine(const string &Buffer);
	bool Seek(size_t Offset, ESeekOrigin Origin);
	bool Eof() const;
	size_t Size() const;

private:
	FILE *File;
	string Filename;
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

	__INLINE void Toggle(bool AEnabled)
	{
		Enabled = AEnabled;
	}
	__INLINE bool isEnabled() const
	{
		return Enabled;
	}

	void SetLogMode(ELogMode ALogMode);
	__INLINE ELogMode GetLogMode() const 
	{
		return LogMode;
	}

	__INLINE void SetLogFileWriteMode(ELogFileWriteMode ALogFileWriteMode)
	{
		LogFileWriteMode = ALogFileWriteMode;
	}
	__INLINE ELogFileWriteMode GetLogFileWriteMode() const
	{
		return LogFileWriteMode;
	}

	__INLINE void SetDatedLogFileNames(bool ADatedLogFileNames)
	{
		DatedLogFileNames = ADatedLogFileNames;
	}
	__INLINE bool GetDatedLogFileNames() const
	{
		return DatedLogFileNames;
	}

	__INLINE void SetLogFilePath(string ALogFilePath)
	{
		LogFilePath = ALogFilePath;
	}
	__INLINE string GetLogFilePath() const
	{
		return LogFilePath;
	}

	__INLINE void SetLogName(string ALogName)
	{
		LogName = ALogName;
	}
	__INLINE string GetLogName() const
	{
		return LogName;
	}

private:
	bool Enabled;
	ELogMode LogMode;
	ELogFileWriteMode LogFileWriteMode;
	bool DatedLogFileNames;
	ostream *Stream;
	string LogFilePath;
	string LogName;
protected:
	friend class CTSingleton<CLog>;
	CLog();
};

#define Log CLog::Instance()->WriteToLog

/**
*	Класс CFactory. Назначение классы - контроль создания любых объектов.
*/

class CFactory : public CTSingleton<CFactory>
{
public:
	template<typename T>
	T* New(const string &AName);
	void Add(CObject *AObject, const string &AName = "");
	template<typename T>
	T* Get(const string &AName);
	template<typename T>
	T* Remove(const string &AName);
	void Destroy(CObject *AObject);

protected:
	CFactory();
	~CFactory();
	friend class CTSingleton<CFactory>;
	list <CObject*> Objects; // таки хорошо бы сделать это map'ом от string'а
};

/**
* CFactory::New - создаёт объект заданного класса, берёт управление его памятью на себя и возвращает указатель на созданный объект.
*/

template<typename T>
T* CFactory::New(const string &AName)
{
	// Поддерживаем уникальность имени здесь, наверное, да.
	//if (List.Contains(AName)) @todo uncomment and fix
	{
//		throw std::logic_error("Object with this name already exists.");
	}
	T* result = new T;

	string NewName = AName;
	if (NewName.length() == 0)
		NewName = typeid(T).name() + itos(result->GetID());
	Add(result, NewName);

	return result;
}

/**
* CFactory::Get - возвращает указатель на объект по его уникальному имени.
* Ругается в лог и возвращает NULL в случае неудачи.
*/

template<typename T>
T* CFactory::Get(const string &AName)
{
	T* result = NULL;
	for(list<CObject*>::iterator i = Objects.begin(); i != Objects.end(); ++i)
		if ((*i)->GetName() == AName)
		{
			result = dynamic_cast<T *>(*i);
			break;
		}
	if (result == NULL)
	{
		Log("ERROR", "Factory can't find object named '%s'", AName.c_str()); // not only "can't find" case, but dynamic_cast can also fail
	}

	return result;
}

/**
* CFactory::Remove - прекращают управление памятью объекта и возвращает указатель на объект на растерзание программисту.
* Ругается в лог и возвращает NULL в случае неудачи.
*/

template<typename T>
T* CFactory::Remove(const string &AName)
{
	T* result = NULL;
	list<CObject*>::iterator i;
	for(i = Objects.begin(); i != Objects.end(); ++i)
		if ((*i)->GetName() == AName)
		{
			result = dynamic_cast<T *>(*i);
			break;
		}
	
	if (result == NULL)
	{
		Log("ERROR", "Factory can't find object named '%s'", AName.c_str());
	}
	else
	{
		result->Managed = false;
		Objects.erase(i);
		CObject::DecRefCount(result);
	}

	return result;
}

/**
* Класс CEnvironment содержит статические функции и возможно какие-нибудь константы, которые относятся к окружению выполнения.
* В идеале, в будущем, весь (ну или не весь, хз пока) платформо-зависимый код следует скинуть куда-то в это место.
*/

class CEnvironment
{
public:
	class DateTime
	{
	public:
		static tm GetLocalTimeAndDate();
		static string GetFormattedTime(const tm TimeStruct, const char *Format);
	};

	class Paths
	{
	public:
		static string GetWorkingDirectory();
		static void SetWorkingDirectory();
	};

};

// @todo: taking into account, that global functions is evil, may be we should move such kind of functions
// 	 in some class, named, for example, CEnvironment
// Agreed with you.
// 	CEnvironment has been created. Some time/date and paths functions has already been moved there. It's good practice to move all platform-dependent code (basically, code with ifdefs) to one dedicated place, if it's possible, so we should continue this work.

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

#endif // _2DE_CORE_H_
