#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#pragma warning (disable  : 4996) 
#pragma warning (disable  : 4312) 

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
typedef long				LONG;

//#define _DEBUG
#define VC_LEANMEAN
#define _CRT_SECURE_NO_DEPRECATE

#define CFILE_READ				0x01
#define CFILE_WRITE				0x02
#define CFILE_MAX_STRING_LENGTH	1024

#define LOG_TIME_TICK_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <time.h>
using namespace std;

inline char *str(string Str)
{
	return (char*)Str.c_str();
}

// Это для улучшения читабельности дефайн. Тоесть мы хотим бесконечный цикл: for(;;). А можно for EVER.
#define EVER (;;)

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
	int type, id;	// type - флаши свойств объекта. id - идентификационный номер объекта. Пока не используется. TODO!
	string name;	// name - имя объекта. Удобно обращаться к объектам по именам. И в лог писать удобно.
	virtual bool Update(float dt)
	{
		return false;
	};
	CObject();
	virtual ~CObject(){};
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
*	Это тайпдефайнил Петя, я не ебу что это. DOC!
*/

typedef bool (*ObjCompCall)(CObject*, CObject*);
typedef bool (*ObjCall)(CObject*);

/**
*	CObjectList - список объектов. Двусвязный.
*/

class CObjectList : public CObject
{
private:
	CNodeObject* GetNodeObject(string objectname);			// неебу что она делает DOC!
	void SwapObjs(CNodeObject *obj1, CNodeObject *obj2);	// Скорей всего ф-я меняет в списке местами 2 объекта. 
protected:
	CNodeObject *first, *last, *current;					// указатели на соответсвенно первый, последний и текущий объект в списке.
public:
	CObjectList();
	~CObjectList();
	CObject* Next();										// неебу что она делает  DOC!
	bool Enum(CObject* &result);							// перечисление вроде как DOC!
	void Reset();											// неебу что она делает DOC!
	void Clear();											// неебу что она делает... хотя возможно очищает список DOC!
	bool Call(ObjCall callproc);							// неебу что она делает DOC!
	void Sort(ObjCompCall comp);							// Сортировка. В парметр принимает ф-ю сравнения 2х объектов. Пока квадратичная.
	virtual bool AddObject(CObject *object);						// Добавить объект в конец списка.
	bool DelObject(string objectname);						// удалить объект с именем objectname
	bool DelObj(int ind);									// удалить объект рукоодствуясь непонятнвым параметром 
	CObject* GetObject(string objectname);					// получить указатель на объект по имени
};

/**
*	Все загружаемые из файла объекты следует наследовать от
*	CResource. класс CBaseResource нужен чтобы всё работало и компилятор не ругался.
*/

class CBaseResource
{
public:
	bool loaded;		// loaded должна быть истина если экземпляр объекта был РЕАЛЬНО загружен, а не просто проиндексирован.
	string filename;	// Полный путь к файлу.
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

class CResource : public CObject, public CBaseResource
{
public:
	CResource(){}
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
*	Петя. ... я незнаю зачем. Но выглядит как костыль. DOC!
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
	bool Write(void* buffer, DWORD nbytes);
	bool ReadString(char* buffer);
	bool WriteString(char* buffer);
	bool ReadString(string &buffer);
	bool WriteString(string buffer);
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
	// Вверху бесполезный комментарий на ломаном английском. Впрочем аткой же бесполезный как и этот.
	bool ReadLine(char* &data);
	bool Eof();
	bool Seek(unsigned int offset, byte kind);
};

/**
*	Внизу ф-ии для работы с логом. DOC!
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
*	хитрый план создавать лог. мне не нравиться. TODO!
*/

class CDummy
{
public:
	CDummy(void)
	{
		CreateLogFile("System.log");
	}
};

#endif CORE_UTILS_H	
