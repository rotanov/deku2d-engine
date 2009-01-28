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

#define T_COBJECT		0x01
#define T_RENDERABLE	0x02

#define T_UPDATABLE		0x10

#define T_LEFT_MASK		0x0f
#define T_RIGHT_MASK	0xf0

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


class CObject
{
public:
	int type, id;
	string name;
	virtual bool Update(float dt)
	{
		return false;
	};
	CObject();
	virtual ~CObject(){};
};

typedef CObject* (*CreateFunc)();

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

typedef bool (*ObjCompCall)(CObject*, CObject*);
typedef bool (*ObjCall)(CObject*);

class CObjectList : public CObject
{
private:
	CNodeObject* GetNodeObject(string objectname);
	void SwapObjs(CNodeObject *obj1, CNodeObject *obj2);
protected:
	CNodeObject *first, *last, *current;
public:
	CObjectList();
	~CObjectList();
	CObject* Next();
	bool Enum(CObject* &result);
	void Reset();
	void Clear();
	bool Call(ObjCall callproc);
	void Sort(ObjCompCall comp);
	bool AddObject(CObject *object);
	bool DelObject(string objectname);
	bool DelObj(int ind);
	CObject* GetObject(string objectname);
};

/**
*	Any object which you want to load from file
*	should be derived from CResource.
*/
class CBaseResource
{
public:
	bool loaded;		// Should be true if instance has been loaded from file.
	string filename;	// Full path to file.
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

void SetScreenParams(int Width, int Height);
void GetScreenParams(int &Width, int &Height);
int ScreenW();
int ScreenH();

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
	bool ReadLine(char* &data);
	bool Eof();
	bool Seek(unsigned int offset, byte kind);
};

//here are log function(s)
void CreateLogFile(char *fname);
void Log(char* Event, char* Format, ...);
void ToggleLog(bool _Enabled);

void MemChp1();
void MemChp2();
void MemCheck();

/**
*	used to initialize log.h
*	with strange method
*	Yes, this is important to note.
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
