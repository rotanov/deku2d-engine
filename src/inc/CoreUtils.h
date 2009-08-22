#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#pragma message("Compiling CoreUtils.h")	// Впихивать эту тему в файлы чтобы видеть в Output какой файл компилируется.

#pragma warning (disable	:	4312)
#pragma warning (disable	:	4311)
#pragma warning (disable	:	4267)
#pragma warning (disable	:	4305) 
#pragma warning (disable	:	4244) 
#pragma warning (disable	:	4996) 
#pragma warning (disable	:	4172)
#pragma warning (disable	:	4996) 
#pragma warning (disable	:	4312) 
#pragma warning (disable	:	4800) 
#pragma warning (disable	:	4018) 
#pragma warning (disable	:	4715) 

#define VC_LEANMEAN
#define _CRT_SECURE_NO_DEPRECATE


#include <SDL.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <time.h>


using namespace std;

#ifdef WIN32
	#define	WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#define USE_SDL_OPENGL
#define USING_OPENGL

#ifdef WIN32
#define __INLINE __forceinline
#else
#define __INLINE inline
#endif

// Отладка. При сборке дебаг конфигурации проекта оно заранее где-то там дефайнится, так что это для релиза, ручное управление так сказать.
// А вообще это слишком большой комментарий для одной, такой короткой и самоочевидно строчки, мне следует избегать написания таких комментарие в будущем.
//#define _DEBUG

// Будут ли выскакивать MessageBoxы под виндой в случае критических ошибок или нет. #undef если чо
#ifdef WIN32
#define CRITICAL_ERRORS_MESSAGE_BOXES
#endif WIN32

#define CFILE_READ				0x01
#define CFILE_WRITE				0x02
#define CFILE_MAX_STRING_LENGTH	1024

typedef unsigned char		byte;
typedef byte*				pbyte;
typedef char*				pchar;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef char				CHAR;
typedef short				SHORT;
typedef unsigned			uint;
typedef long				LONG;

typedef float scalar;

#ifdef WIN32
// Второй параметр - состояние. Эта система ещё не доделана, и если не потребуется в будуещем - я от неё откажусь.
typedef bool (*DirectoryWalkFunc)(char*, int); 
#endif WIN32

typedef bool (*Callback)();
typedef bool (*UpdateProc)(scalar);
typedef bool (*EventFunc)(SDL_Event&);

#define KEY_PRESSED		0x00
#define KEY_RELEASED	0x01

// Это для улучшения читабельности дефайн. Тоесть мы хотим бесконечный цикл: for(;;). А можно for EVER.
// Хотя выглядит как выебон.
#define EVER (;;)
#define Forever for EVER


// Это дефайны для флагов свойств объекта
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
	uint			id;
	string			name;
	virtual ~CObject(){};
	CObject();
	virtual bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	void IncListRefCount();
	void DecListRefCount();
private:
	int ListRefCount;
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
*/

class CList : public virtual CObject
{
public:
					CList();
					~CList();
	virtual bool		AddObject(CObject *AObject);
	bool				DelObject(CObject *AObject);
	bool				DelObject(const char *AObjectName);
	bool				DelObject(int AId);			
	void				Reset();
	bool				Enum(CObject* &result);
	CObject*			Next();	
	void				Clear();
	CObject*			GetObject(const char* AObjectName);					
	CObject*			GetObject(int AId);
	CListNode*			GetListNode(const CObject* AObject);
	CListNode*			GetListNode(const char* AObjectName);
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
	virtual bool Update(float dt) = 0;
};

typedef CObject* (*CreateFunc)();


/**
*	CUpdateManager - менеджер объектов, который следует обновлять. Такие дела.
*	Да, тут мало кода, надо ещё какие-нибуть ф-ии нахерачить. TODO!
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

class CFile // унаследовать 
{
public:
	FILE *file;

	CFile(void) : file(NULL){}
	CFile(char *filename, int mode);

	bool Open(const char * filename, int mode);
	bool Close();
	bool ReadByte(pbyte buffer);
	bool Read(void* buffer, DWORD nbytes);
	bool WriteByte(pbyte buffer);
	bool WriteByte(byte buffer);
	bool Write(const void* buffer, DWORD nbytes);
	bool ReadString(char* buffer);
	bool WriteString(const char* buffer);
	bool ReadString(string &buffer);
	bool WriteString(const string buffer);
	bool Writeln(string buffer);
	DWORD Size();
	bool ReadLine(char* &data);
	bool Eof();
	bool Seek(unsigned int offset, byte kind);
};

/**
*	void CreateLogFile(char *fname) - создание лог файла(хз зачем нужно - можно прощще хР) (Ага, жду твоего предложения.)
*	void Log(char* Event, char* Format, ...) - вывод в лог сообщений формата "[Date] [$Event] $Format ted string\n"
*	void ToggleLog(bool _Enabled) - отключение/включение лога. - нахуй это вообще надо, а?
*/

void CreateLogFile(char *fname);
void Log(char* Event, char* Format, ...);
void ToggleLog(bool _Enabled);

#ifdef WIN32

const int DIRWALK_STATE_FILE		=	0x01;	// файл
const int DIRWALK_STATE_FOLDER		=	0x02;	// папка
const int DIRWALK_STATE_HIGHER		=	0x20;	// на предыдущем шаге рекурсии мы были на каталог ниже, т.е. вернулись
const int DIRWALK_STATE_RIGHT_MASK	=	0x0F;
const int DIRWALK_STATE_LEFT_MASK	=	0xF0;

class CDirectoryWalk
{
public:
	char *MainDir;
	char *CurrDir;
	int MainDirL;
	WIN32_FIND_DATA fdata;
	DirectoryWalkFunc UserExploreFunc;
	CDirectoryWalk();
	~CDirectoryWalk();
	
	bool List();
	void ExploreDir(HANDLE hfile);
};

#endif WIN32

// Ф-ии для работы со строками, в частности именами файлов и так далее

void DelFNameFromFPath(char *src);
void DelExtFromFName(char *src);
void DelLastDirFromPath(char* src);
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



#endif CORE_UTILS_H	
