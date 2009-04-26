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

//#define _DEBUG
#define VC_LEANMEAN
#define _CRT_SECURE_NO_DEPRECATE

#define CFILE_READ				0x01
#define CFILE_WRITE				0x02
#define CFILE_MAX_STRING_LENGTH	1024

#define LOG_TIME_TICK_
#define SAFE_DELETE(obj){if (obj) {delete obj; obj = NULL;}}

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

inline char *str(string Str)
{
	return (char*)Str.c_str();
}

#define USE_SDL_OPENGL

// Это для улучшения читабельности дефайн. Тоесть мы хотим бесконечный цикл: for(;;). А можно for EVER.
#define EVER (;;)
#define Forever for(;;)

// Это дефайны для флагов свойств объекта
#define T_COBJECT		0x01
#define T_RENDERABLE	0x02
#define T_UPDATABLE		0x10

#define T_LEFT_MASK		0x0f
#define T_RIGHT_MASK	0xf0

/**
*	Есть такая вещь как паттерны. Есть паттерн синглтон. Этот класс демонстрирует как его делать.
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
	CObject()
	{
		type = 0;
		name += "CObject ";
		id = 0;
	};
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
	CBaseResource():loaded(false), filename(""){}
};

class CResource : public CBaseResource, virtual public CObject
{
public:
	//CResource(){}
};

/**
*	CUpdateManager - менеджер объектов, который следует обновлять. Такие дела.
*	Да, тут мало кода, надо ещё какие-нибуть ф-ии нахерачить. TODO!
*/

class CUpdateManager : public CObjectList
{
public:
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
*	Затем что эта поебня с инстансами, конечно вещь крутая, но жутко волосатая.
*	Надеюсь из названий все понятно(для особо умных) - получить высоту/ширину экрана(или все вместе)
*	или поменять их.
*/

void SetScreenParams(int Width, int Height);
void GetScreenParams(int &Width, int &Height);
int ScreenW();
int ScreenH();

/**
*	CFile - класс для работы с файлом. Названия ф-ий интуитивны.
*	Как с ним работать я думаю тоже интуитивно понятно.
*/

class CFile
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
	/**
	*	Reads characters until reaching 0x00
	*	data - is a pointer to a char passed by
	*	link(&) i.e. the function changes the value
	*	of data. If data is pointing to something,
	*	funtion realeses the memory using delete []
	*	then allocates new meory using new.
	*/
	// Вверху бесполезный комментарий на ломаном английском. Впрочем такой же бесполезный как и этот.
	bool ReadLine(char* &data);
	bool Eof();
	bool Seek(unsigned int offset, byte kind);
};

/**
*	Не вижу ниче непонятного... ну да ладно...
*	void CreateLogFile(char *fname) - создание лог файла(хз зачем нужно - можно прощще хР)
*	void Log(char* Event, char* Format, ...) - вывод в лог сообщений формата
*									"[Date] [$Event] $Format ted string\n"
*	void ToggleLog(bool _Enabled) - отключение/включение лога.
*/

void CreateLogFile(char *fname);
void Log(char* Event, char* Format, ...);
void ToggleLog(bool _Enabled);

/**
*	Внизу временные ф-ии для отлова утечек памяти. TODO!
*/

void MemChp1();
void MemChp2();
void MemCheck();

/**
*	хитрый план создавать лог. мне не нравится. TODO!
*	то то и оно проще запхать все в ф-ю Log и на первом вызове создавать логфаель.
*/

class CDummy
{
public:
	CDummy(void)
	{
		CreateLogFile("System.log");
	}
};

void DeleteFileNameFromEndOfPathToFile(char *src);
void DeleteExtFromFilename(char *src);

#endif CORE_UTILS_H	
