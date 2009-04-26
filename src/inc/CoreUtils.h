#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#pragma message("Compiling CoreUtils.h")	// ��������� ��� ���� � ����� ����� �����...�����...

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

// ��� ��� ��������� ������������� ������. ������ �� ����� ����������� ����: for(;;). � ����� for EVER.
#define EVER (;;)
#define Forever for(;;)

// ��� ������� ��� ������ ������� �������
#define T_COBJECT		0x01
#define T_RENDERABLE	0x02
#define T_UPDATABLE		0x10

#define T_LEFT_MASK		0x0f
#define T_RIGHT_MASK	0xf0

/**
*	���� ����� ���� ��� ��������. ���� ������� ��������. ���� ����� ������������� ��� ��� ������.
*	���� ���������� � ���, ��� � ������ ������ ������������ ������ ���� ��������� ������.
*	���� ����� ������� ����� �� ������������ � ����� �����, ����� �� ������ ��� ������ ��������.
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
*	CObject - ����� ������� �����. �� ����������� �� ����.
*/

class CObject
{
public:
	unsigned int type, id;	// type - ����� ������� �������. id - ����������������� ����� �������. ���� �� ������������. TODO!
	string name;			// name - ��� �������. ������ ���������� � �������� �� ������. � � ��� ������ ������.
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
*	CNodeObject - ��� ���� ������.
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
*	ObjCompCall - ��� �-� ��� ��������� ���� ��������
*/
typedef bool (*ObjCompCall)(CObject*, CObject*);

/**
*	ObjCall - ��� �-� ��� ���������� �������� ��� ��������
*/
typedef bool (*ObjCall)(CObject*);


/**
*	CObjectList - ������ ��������. ����������.
*/

class CObjectList : public virtual CObject
{
private:
	CNodeObject* GetNodeObject(string objectname);			// ��������� �� ��������� �� �����
	void SwapObjs(CNodeObject *obj1, CNodeObject *obj2);	// ������ ����� �-� ������ � ������ ������� 2 �������. 
protected:
	CNodeObject *first, *last, *current;					// ��������� �� ������������� ������, ��������� � ������� ������ � ������.
	int	count;												// ���-�� ��-���
public:
	CObjectList();
	~CObjectList();
	int	GetCount();											// ���-�� ��-���
	CObject* Next();										// current ����������� �� ��������� �������
	bool Enum(CObject* &result);							// ����� �������� �������� � ��������� ��������� �� ���������
															// ������ ������� ��������� �� ���� ������, ������� ����, ��� ������� ����������(�����) � �������, � �������� ��������� �� ���������
	void Reset();											// ��������� ������� ���������(������� current) �� ������ ��-�
	void Clear();											// ������� ������ �� ������ ��������...
	bool Call(ObjCall callproc);							// ��� ���� �������� ������� ������� callproc
	void Sort(ObjCompCall comp);							// ����������. � ������� ��������� �-� ��������� 2� ��������. ���� ������������.
	virtual bool AddObject(CObject *object);						// �������� ������ � ����� ������.
	bool DelObject(string objectname);						// ������� ������ � ������ objectname
	bool DelObj(int ind);									// ������� ������ ������������� ����������� ���������� 
	CObject* GetObjectByName(string objectname);					// �������� ��������� �� ������ �� �����
};

/**
*	��� ����������� �� ����� ������� ������� ����������� ��
*	CResource. ����� CBaseResource ����� ����� �� �������� � ���������� �� �������.
*/

class CBaseResource
{
public:
	bool loaded;		// loaded ������ ���� ������ ���� ��������� ������� ��� ������� ��������, � �� ������ ���������������.
	string filename;	// ������^W ����-�����-������ ���� � �����.
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
*	CUpdateManager - �������� ��������, ������� ������� ���������. ����� ����.
*	��, ��� ���� ����, ���� ��� �����-������ �-�� ����������. TODO!
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
*	����� ��� ��� ������ � ����������, ������� ���� ������, �� ����� ���������.
*	������� �� �������� ��� �������(��� ����� �����) - �������� ������/������ ������(��� ��� ������)
*	��� �������� ��.
*/

void SetScreenParams(int Width, int Height);
void GetScreenParams(int &Width, int &Height);
int ScreenW();
int ScreenH();

/**
*	CFile - ����� ��� ������ � ������. �������� �-�� ����������.
*	��� � ��� �������� � ����� ���� ���������� �������.
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
	// ������ ����������� ����������� �� ������� ����������. ������� ����� �� ����������� ��� � ����.
	bool ReadLine(char* &data);
	bool Eof();
	bool Seek(unsigned int offset, byte kind);
};

/**
*	�� ���� ���� �����������... �� �� �����...
*	void CreateLogFile(char *fname) - �������� ��� �����(�� ����� ����� - ����� ������ ��)
*	void Log(char* Event, char* Format, ...) - ����� � ��� ��������� �������
*									"[Date] [$Event] $Format ted string\n"
*	void ToggleLog(bool _Enabled) - ����������/��������� ����.
*/

void CreateLogFile(char *fname);
void Log(char* Event, char* Format, ...);
void ToggleLog(bool _Enabled);

/**
*	����� ��������� �-�� ��� ������ ������ ������. TODO!
*/

void MemChp1();
void MemChp2();
void MemCheck();

/**
*	������ ���� ��������� ���. ��� �� ��������. TODO!
*	�� �� � ��� ����� ������� ��� � �-� Log � �� ������ ������ ��������� ��������.
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
