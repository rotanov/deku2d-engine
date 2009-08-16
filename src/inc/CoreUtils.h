#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#pragma message("Compiling CoreUtils.h")	// Впихивать эту тему в файлы чтобы знать...знать...

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

//#define _DEBUG
#define CRITICAL_ERRORS_MESSAGE_BOXES // Будут ли выскакивать MessageBoxы под виндой в случае критических ошибок или нет. #undef если чо
#define VC_LEANMEAN
#define _CRT_SECURE_NO_DEPRECATE

#define CFILE_READ				0x01
#define CFILE_WRITE				0x02
#define CFILE_MAX_STRING_LENGTH	1024

#define LOG_TIME_TICK_
#define SAFE_DELETE(obj){if (obj) {delete obj; obj = NULL;}}

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

typedef unsigned char		byte;
typedef byte*				pbyte;
typedef char*				PCHAR;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef char				CHAR;
typedef short				SHORT;
typedef unsigned			uint;
typedef long				LONG;

#ifdef WIN32
typedef bool (*DirectoryWalkFunc)(char*, int); // name and state State may be Folder in, File in, File after folder out, Folder after folder out
#endif WIN32
typedef bool (*Callback)();
typedef bool (*KeyFunc)(char, SDLKey);
typedef bool (*MouseFunc)(int, int, unsigned char);
typedef bool (*InputFunc)(SDL_MouseButtonEvent&, SDL_MouseMotionEvent&, SDL_KeyboardEvent&);
typedef bool (*EventFunc)(SDL_Event&);

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

/**
*	CObject - самый базовый класс. Всё наследовать от него.
*/



class CObject
{
public:
	unsigned int type, id;	// type - флаги свойств объекта. id - идентификационный номер объекта. Пока не используется. TODO!
	string name;			// name - имя объекта. Удобно обращаться к объектам по именам. И в лог писать удобно.
	virtual ~CObject(){};
	CObject();
};

class CUpdateObject : public virtual CObject
{
public:
	CUpdateObject()
	{
		type |= T_UPDATABLE;
		name += "CUdateObject ";
		id = 0;
	}
	virtual bool Update(float dt) = 0;
};

typedef CObject* (*CreateFunc)();

/**
*	CNodeObject - это узел списка.
*/

class CNodeObject
{
private:
	CObject *data;
public:
	CNodeObject *next, *prev;
	CNodeObject();
	CObject* GetData();
	void SetData(CObject *object);
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

class CObjectList : public virtual CObject
{
private:
	CNodeObject* GetNodeObject(string objectname);			// указатель на нодобжект по имени
	void SwapObjs(CNodeObject *obj1, CNodeObject *obj2);	// Скорей всего ф-я меняет в списке местами 2 объекта. 
protected:
	CNodeObject *first, *last, *current;					// указатели на соответсвенно первый, последний и текущий объект в списке.
	int	count;												// кол-во эл-тов
public:
	CObjectList();
	~CObjectList();
	int	GetCount();											// кол-во эл-тов
	CObject* Next();										// current переместить на следующий элемент
	bool Enum(CObject* &result);							// адрес текущего элемента с переводом указателя на следующий
															// короче вначале указывает на один объект, вызвали енум, она вернула содержимое(адрес) в результ, и сдвинула указатель на следующий
	void Reset();											// скидывает текущий указатель(помоему current) на первый эл-т
	void Clear();											// очищает список НЕ удаляя элементы...
	bool Call(ObjCall callproc);							// для всех объектов списька вызвать callproc
	void Sort(ObjCompCall comp);							// Сортировка. В парметр принимает ф-ю сравнения 2х объектов. Пока квадратичная.
	virtual bool AddObject(CObject *object);						// Добавить объект в конец списка.
	bool DelObject(string objectname);						// удалить объект с именем objectname
	bool DelObj(int ind);									// удалить объект рукоодствуясь непонятнвым параметром 
	CObject* GetObjectByName(string objectname);					// получить указатель на объект по имени
	void DumpToLog();
};

extern CObjectList CObjectManager;

/**
*	Стэк объектов.
*/

const int MAX_STACK_ELEMENTS_COUNT = 16;

class CObjectStack : public CObject
{
public:
	CObjectStack() : last(-1){}
	bool Push(CObject* AObject)
	{
		if (++last >= MAX_STACK_ELEMENTS_COUNT)
			return false;
		Objects[last] = AObject;
		return true;
	}
	CObject* Pop()
	{
		if (last >= 0)
			return Objects[last--];
		return NULL;
	}
	bool Empty()
	{
		return last == -1;
	}
private:
	CObject *Objects[MAX_STACK_ELEMENTS_COUNT];
	int last;
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
	virtual bool LoadFromFile()
	{
		return false;
	};
	virtual bool SaveToFile()
	{
		return false;
	};
	bool CheckLoad()
	{
		return !loaded?LoadFromFile():true;
	}
	CBaseResource():loaded(false), filename(""){}
};

class CResource : public CBaseResource, virtual public CObject
{
public:
	CResource()
	{
		name += "CResource";
	}
};

/**
*	CUpdateManager - менеджер объектов, который следует обновлять. Такие дела.
*	Да, тут мало кода, надо ещё какие-нибуть ф-ии нахерачить. TODO!
*/

class CUpdateManager : public CObjectList
{
public:
	CUpdateManager()
	{
		name += "CUpdateManager";
	}
	bool UpdateObjects();
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
	CDirectoryWalk()
	{
		MainDirL = 0;
		MainDir = new char[MAX_PATH];
		CurrDir = new char[MAX_PATH];
	}
	~CDirectoryWalk()
	{
		delete [] MainDir;
		delete [] CurrDir;
	}
	
	bool List();
	void ExploreDir(HANDLE hfile);
};

#endif WIN32

// Ф-ии для работы со строками, в частности именами файлов и так далее

void DelFNameFromFPath(char *src);
void DelExtFromFName(char *src);
void DelLastDirFromPath(char* src);

/**
*	Внизу временные ф-ии для отлова утечек памяти.
*	То что внизу - либо убрать нахуй, либо сделать нормально.
*	Но это мелочи, так что пока пусть будет как есть, всё равно теперь не используется
*/

void MemChp1();
void MemChp2();
void MemCheck();


#endif CORE_UTILS_H	
