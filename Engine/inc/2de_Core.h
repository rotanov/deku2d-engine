#ifndef _2DE_CORE_H
#define _2DE_CORE_H

#ifdef _MSC_VER
//#pragma message("Compiling CoreUtils.h")	// НЕ.Впихивать эту тему в файлы чтобы видеть в Output какой файл компилируется. "НЕ", я сказал. И так output заграмождён.

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
//after w4
#pragma warning (disable	:	4706)	//	assignment within conditional expression (!!!)
#pragma warning (disable	:	4701)	//	potentially uninitialized local variable 'origin_const' used
 //#pragma warning (disable	:	4201)	//	nonstandard extension used : nameless struct/union (!!!)
#pragma warning (disable	:	4100)	//	unreferenced formal parameter
 //#pragma warning (disable	:	4239)	//	nonstandard extension used : 'return' : conversion from 'Matrix3' to 'Matrix3 &' (!!!)
#pragma warning (disable	:	4189)	//	local variable is initialized but not referenced
 //#pragma warning (disable	:	4238)	//	nonstandard extension used : class rvalue used as lvalue (!!!)
#pragma warning (disable	:	4389)	//	signed/unsigned mismatch
#pragma warning (disable	:	4702)	//	unreachable code ^^"
#pragma warning (disable	:	4611)	//	interaction between '_setjmp' and C++ object destruction is non-portable (???)

#define VC_LEANMEAN
#define _CRT_SECURE_NO_DEPRECATE

#endif // _MSC_VER

#include <SDL.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <time.h>
#include <assert.h>

using namespace std;

#ifdef _WIN32
	#define	WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif  //_WIN32

#define USE_SDL_OPENGL
#define USING_OPENGL

#ifdef _MSC_VER
#define __INLINE __forceinline
#else
#define __INLINE inline
#endif //_MSC_VER

// Отладка. При сборке дебаг конфигурации проекта оно заранее где-то там дефайнится, так что это для релиза, ручное управление так сказать.
// А вообще это слишком большой комментарий для одной, такой короткой и самоочевидно строчки, мне следует избегать написания таких комментарие в будущем.
//#define _DEBUG

// Будут ли выскакивать MessageBoxы под виндой в случае критических ошибок или нет. #undef если чо
//	Такой способ отлова ошибок на самом деле очень нехороший, я недоволен им всячески.
#ifdef _WIN32
#define CRITICAL_ERRORS_MESSAGE_BOXES
#endif //_WIN32

#define CFILE_MAX_STRING_LENGTH	1024

#ifdef _WIN32
#define CFILE_DEFAULT_MAXIMUM_PATH_LENGTH MAX_PATH
#else
#define CFILE_DEFAULT_MAXIMUM_PATH_LENGTH 260
#endif //_WIN32

#ifdef _MSC_VER
#define snprintf _snprintf
#endif //_MSC_VER

typedef unsigned char		byte;
typedef float				scalar;

typedef bool (*Callback)();
typedef bool (*UpdateProc)(scalar);
typedef bool (*EventFunc)(SDL_Event&);

#define KEY_PRESSED		0x00
#define KEY_RELEASED	0x01

// Это для улучшения читабельности дефайн. Тоесть мы хотим бесконечный цикл: for(;;). А можно for EVER.
// Хотя выглядит как выебон.
#define EVER (;;)
#define Forever for EVER

/**
*	Это дефайны для флагов свойств объекта 
*	Как-то уныло вышло, вы не находите? Они же почти не используются. 
*	Т.е. используются, но не так широко, как задуммывалось. И не так активно.
*/
#define T_COBJECT		0x01
#define T_RENDERABLE	0x02
#define T_UPDATABLE		0x10

#define T_LEFT_MASK		0x0f
#define T_RIGHT_MASK	0xf0

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

// Lulz!
#define DEAD_BEEF 0xdeadbeef
#define DEAD_FOOD 0xdeadf00d

#define COLOR_WHITE RGBAf(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_BLACK RGBAf(1.0f, 1.0f, 1.0f, 1.0f)

/**
*	CObject - базовый класс. Всё наследовать от него.
*/

class CObject
{
public:
	unsigned int	type;	// type - флаги свойств объекта. 
	virtual			~CObject(){};
					CObject();
	virtual bool	InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	void			IncListRefCount();
	void			DecListRefCount();
	const int		GetListRefCount();
	const char*		GetName();
	void			SetName(const char* AObjectName);
	void			SetName(const string &AObjectName);
	unsigned int	GetID();
private:
	string			name;
	int				ListRefCount;
	unsigned int	id;
};

/**
*	Ф-я для принятия информации о нажатии кнопки, будь то мышь или клавиатура. Ввод с других устройств не поддерживается пока что.
*	Первый параметр - произошло ли событие KEY_PRESSED или KEY_RELEASED
*	Второй - какая клавиша была нажата; одновременно принимает значения мыши и клавиатуры: sdl_keysym.h, sdl_mouse.h - списки констант
*	Третий параметр - модификатор, т.е. зажат ли Shift, Ctrl, Alt итд
*	И четвёртый параметр - ASCII код.
*/
typedef bool (CObject::*KeyInputFunc)(Uint8, Uint16, SDLMod, char);
/** 
*	1й и 2й параметры - х и у соответственно
*	кстати довольнo бесполезная штука в отличие от той, что выше. Неудобно, я считаю.
*/
typedef bool (*MouseInputFunc)(scalar, scalar); 

/**
*	CNodeObject - это узел списка.
*/

class CListNode
{
public:
	CListNode *next, *prev;
	CListNode();
	CObject* GetData();
	void SetData(CObject *AData);
private:
	CObject *Data;					// Содержимое узла.
};

/**
*	ObjCompCall - тип ф-и для сравнения двух объектов
*/
typedef bool (*ObjCompCall)(CObject*, CObject*);

/**
*	ObjCall - тип ф-и для выполнения действий над объектом
*/
typedef bool (*ObjCall)(CObject*);


/**
*	CObjectList - список объектов. Двусвязный.
*	Внимание, после лекции Лудова по спискам я заявляю, что эта реализация если не ГОВНО, то
*	говно. Да-да, надо переписывать. Опять. Итераторы и всё такое. И концепция контейнеров.
*/

class CList : public virtual CObject
{
public:
					CList();
					~CList();
	virtual bool		AddObject(CObject *AObject);
	bool				DelObject(CObject *AObject);
	bool				DelObject(const string *AObjectName);
	bool				DelObject(const char *AObjectName);
	bool				DelObject(int AId);			
	void				Reset();
	bool				Enum(CObject* &result);
	CObject*			Next();	
	void				Clear();
	CObject*			GetObject(const string* AObjectName);					
	CObject*			GetObject(const char* AObjectName);					
	CObject*			GetObject(int AId);
	CListNode*			GetListNode(const CObject* AObject);
	CListNode*			GetListNode(const string* AObjectName);
	CListNode*			GetListNode(int AId);	
	int					GetObjectsCount();
	CListNode*			GetFirst();
	CListNode*			GetLast();																	
	bool				Call(ObjCall callproc);							
	void				Sort(ObjCompCall comp);							
	void				DumpToLog();
private:
	CListNode			*first;					
	CListNode			*last;					
	CListNode			*current;					
	int					NodeCount;
	void				DelNode(CListNode* AListNode);
	void				SwapNodes(CListNode *Node0, CListNode *Node1);
protected:
	CListNode*			RelativeNext(CListNode* AListNode);
	CListNode*			RelativePrev(CListNode* AListNode);
};

extern CList CObjectManager;

/**
*	Стэк объектов.
*/

const int MAX_STACK_ELEMENTS_COUNT = 16;

class CObjectStack : public CObject
{
public:
	CObjectStack();
	bool Push(CObject* AObject);
	CObject* Pop();
	bool Empty();
private:
	CObject *Objects[MAX_STACK_ELEMENTS_COUNT];
	int last;
};

class CUpdateObject : public virtual CObject
{
public:
	bool Active;
	CUpdateObject();
	~CUpdateObject();
	virtual bool Update(float dt) = 0;
};

typedef CObject* (*CreateFunc)();


/**
*	CUpdateManager - менеджер объектов, который следует обновлять. Такие дела.
*	Да, тут мало кода, надо ещё какие-нибуть ф-ии нахерачить. TODO!
*	И вообще что-то мне подсказывает, что он не здесь должен быть.
*/

class CUpdateManager : public CList
{
public:
	CUpdateManager();
	bool UpdateObjects();
};


/**
*	Все загружаемые из файла объекты следует наследовать от
*	CResource. класс CBaseResource нужен чтобы всё работало и компилятор не ругался.
*/

class CBaseResource
{
public:
	bool loaded;		// loaded должна быть истина если экземпляр объекта был РЕАЛЬНО загружен, а не просто проиндексирован.
	string filename;	// Полный^W хоть-какой-нибуть путь к файлу.
	virtual bool LoadFromFile();;
	virtual bool SaveToFile();;
	bool CheckLoad();
	CBaseResource();
};

class CResource : public CBaseResource, virtual public CObject
{
public:
	CResource();
};


struct CRectub
{
	byte x0, x1, y0, y1;
};

struct CRecti
{
	int x0, x1, y0, y1;
};

class RGBAub
{
public:
	byte r, g, b, a;
	RGBAub(byte _r, byte _g, byte _b, byte _a) : r(_r), g(_g), b(_b), a(_a){}
	RGBAub(void){}
};

/**
*	CFile - класс для работы с файлом. Названия ф-ий интуитивны.
*	Как с ним работать я думаю тоже интуитивно понятно.
*/

class CFile // унаследовать? // или нет? 
{
public:
	enum EOpenMode
	{
		OPEN_MODE_READ,
		OPEN_MODE_WRITE
	};
	
	enum ESeekOrigin
	{
		SEEK_ORIGIN_BEGINNING,
		SEEK_ORIGIN_CURRENT,
		SEEK_ORIGIN_END
	};

	CFile() : File(NULL) {}
	CFile(const string AFileName, EOpenMode Mode);

	bool Open(const string AFileName, EOpenMode Mode);
	bool Close();
	bool ReadByte(unsigned char *Buffer);
	bool WriteByte(unsigned char *Buffer);
	bool WriteByte(unsigned char Buffer);
	bool Read(void *Buffer, unsigned long BytesCount);
	bool Write(const void *Buffer, unsigned long BytesCount);
	bool ReadString(char *Buffer);
	bool ReadString(string &Buffer);
	bool WriteString(const char *Buffer);
	bool WriteString(const string Buffer);
	bool ReadLine(char* &Data);
	bool WriteLine(string Buffer);
	bool Seek(unsigned int Offset, ESeekOrigin Origin);
	bool Eof();
	size_t Size();

private:
	FILE *File;
	string FileName;
};


/**
 * CLog - класс для работы с логом.
 */

class CLog
{
public:
	enum ELogMode
	{
		LOG_MODE_STDOUT,
		LOG_MODE_STDERR,
		LOG_MODE_FILE
	};

	enum ELogFileWriteMode
	{
		LOG_FILE_WRITE_MODE_TRUNCATE,
		LOG_FILE_WRITE_MODE_APPEND
	};

	CLog();

	~CLog();

	void Log(const char *Event, const char *Format, ...);

	__INLINE void Toggle(bool AEnabled) { Enabled = AEnabled; }
	__INLINE bool isEnabled() { return Enabled; }

	void SetLogMode(ELogMode ALogMode);
	__INLINE ELogMode GetLogMode() { return LogMode; }

	__INLINE void SetLogFileWriteMode(ELogFileWriteMode ALogFileWriteMode) { LogFileWriteMode = ALogFileWriteMode; }
	__INLINE ELogFileWriteMode GetLogFileWriteMode() { return LogFileWriteMode; }

	__INLINE void SetLogFilePath(string ALogFilePath) { LogFilePath = ALogFilePath; }
	__INLINE string GetLogFilePath() { return LogFilePath; }

private:
	bool Enabled;
	ELogMode LogMode;
	ELogFileWriteMode LogFileWriteMode;
	ostream *Stream;
	string LogFilePath;
};

// CLog global instance
extern CLog Log;


// TODO: taking into account, that global functions is evil, may be we should move such kind of functions
// 	 in some class, named, for example, CEnvironment

string GetWorkingDir();

//

// Ф-ии для работы со строками, в частности именами файлов и так далее

void DelFNameFromFPath(char *src);
void DelExtFromFName(char *src);
void DelLastDirFromPath(char *src);
void DelInterval(string *src, const int s0, const int s1);

/**
*	Внизу временные ф-ии для отлова утечек памяти.
*	То что внизу - либо убрать нахуй, либо сделать нормально.
*	Но это мелочи, так что пока пусть будет как есть, всё равно теперь не используется
*/

void MemChp1();
void MemChp2();
void MemCheck();

/**
*	Суть синглотона в том, что в памяти всегда присутствует только один экзмепляр класса.
*	Этот класс реально нигде не используется и лежит здесь, чтобы не забыть как делать синглтон.
*/ 

class CPSingleTone
{
public:
	static CPSingleTone* Instance();
	void FreeInst();
protected:
	CPSingleTone();
	~CPSingleTone();
	static CPSingleTone * _instance;
	static int _refcount;
};


class CGarbageCollector : public CList
{
public:
	CGarbageCollector();
	~CGarbageCollector()
	{
		CObject *data;
		while(Enum(data))
		{
			Log.Log("INFO", "Singletone killer deleting object named: %s id: %u", data->GetName(), data->GetID());
			delete data;
		}
	}
};
extern CGarbageCollector SingletoneKiller;
typedef CGarbageCollector CSingletoneKiller;

__INLINE string itos(int i)
{
	stringstream s;
	s << i;
	return s.str();
}

// And then
// #include <sstream>
// 
// template <class T>
// inline std::string to_string (const T& t)
// {
// 	std::stringstream ss;
// 	ss << t;
// 	return ss.str();
// }


extern CList CObjectManager;


#endif // _2DE_CORE_H


