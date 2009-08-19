#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#pragma message("Compiling CoreUtils.h")	// ��������� ��� ���� � ����� ����� ������ � Output ����� ���� �������������.

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

// �������, ��� �����... ��� ������ ����� ������������ ������� ��� ������� ���-�� ��� ����������, ��� ��� ��� ��� ������, ������ ���������� ��� �������.
// � ������ ��� ������� ������� ����������� ��� �����, ����� �������� � ������������ �������, ��� ������� �������� ��������� ����� ����������� � �������.
//#define _DEBUG

// ����� �� ����������� MessageBox� ��� ������ � ������ ����������� ������ ��� ���. #undef ���� ��
#define CRITICAL_ERRORS_MESSAGE_BOXES

__INLINE void SAFE_DELETE(void* obj)
{
	if (obj)
		delete obj;
	obj = NULL;
};

#define CFILE_READ				0x01
#define CFILE_WRITE				0x02
#define CFILE_MAX_STRING_LENGTH	1024

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

typedef float scalar;

#ifdef WIN32
// ������ �������� - ���������. ��� ������� ��� �� ��������, � ���� �� ����������� � �������� - � �� �� ��������.
typedef bool (*DirectoryWalkFunc)(char*, int); 
#endif WIN32

typedef bool (*Callback)();
typedef bool (*EventFunc)(SDL_Event&);

#define KEY_PRESSED		0x00
#define KEY_RELEASED	0x01

// ��� ��� ��������� ������������� ������. ������ �� ����� ����������� ����: for(;;). � ����� for EVER.
// ���� �������� ��� ������.
#define EVER (;;)
#define Forever for EVER


// ��� ������� ��� ������ ������� �������
#define T_COBJECT		0x01
#define T_RENDERABLE	0x02
#define T_UPDATABLE		0x10

#define T_LEFT_MASK		0x0f
#define T_RIGHT_MASK	0xf0

/**
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
	CObject();
	virtual bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
};

/**
*	�-� ��� �������� ���������� � ������� ������, ���� �� ���� ��� ����������. ���� � ������ ��������� �� �������������� ���� ���.
*	������ �������� - ��������� �� ������� KEY_PRESSED ��� KEY_RELEASED
*	������ - ����� ������� ���� ������; ������������ ��������� �������� ���� � ����������: sdl_keysym.h, sdl_mouse.h - ������ ��������
*	������ �������� - �����������, �.�. ����� �� Shift, Ctrl, Alt ���
*	� �������� �������� - ASCII ���.
*/
typedef bool (CObject::*KeyInputFunc)(Uint8, Uint16, SDLMod, char);
// 1� � 2� ��������� - � � � ��������������
// ������ �������o ����������� ����� � ������� �� ���, ��� ����. ��������, � ������.
typedef bool (*MouseInputFunc)(scalar, scalar); 


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
public:
	CObject *data;
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
	CNodeObject* GetObjectNodeByPointer(const CObject* AObject) const;	// ��� �� ����� �������� ��������� �� ����, ���������� ������, ���� ����� ���������� �������. �������� �������.
	void DumpToLog();
};

extern CObjectList CObjectManager;

/**
*	���� ��������.
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
	bool CheckLoad()
	{
		return loaded = !loaded?LoadFromFile():true;
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
*	CUpdateManager - �������� ��������, ������� ������� ���������. ����� ����.
*	��, ��� ���� ����, ���� ��� �����-������ �-�� ����������. TODO!
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
*	CFile - ����� ��� ������ � ������. �������� �-�� ����������.
*	��� � ��� �������� � ����� ���� ���������� �������.
*/

class CFile // ������������ 
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
*	void CreateLogFile(char *fname) - �������� ��� �����(�� ����� ����� - ����� ������ ��) (���, ��� ������ �����������.)
*	void Log(char* Event, char* Format, ...) - ����� � ��� ��������� ������� "[Date] [$Event] $Format ted string\n"
*	void ToggleLog(bool _Enabled) - ����������/��������� ����. - ����� ��� ������ ����, �?
*/

void CreateLogFile(char *fname);
void Log(char* Event, char* Format, ...);
void ToggleLog(bool _Enabled);

#ifdef WIN32

const int DIRWALK_STATE_FILE		=	0x01;	// ����
const int DIRWALK_STATE_FOLDER		=	0x02;	// �����
const int DIRWALK_STATE_HIGHER		=	0x20;	// �� ���������� ���� �������� �� ���� �� ������� ����, �.�. ���������
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

// �-�� ��� ������ �� ��������, � ��������� ������� ������ � ��� �����

void DelFNameFromFPath(char *src);
void DelExtFromFName(char *src);
void DelLastDirFromPath(char* src);

/**
*	����� ��������� �-�� ��� ������ ������ ������.
*	�� ��� ����� - ���� ������ �����, ���� ������� ���������.
*	�� ��� ������, ��� ��� ���� ����� ����� ��� ����, �� ����� ������ �� ������������
*/

void MemChp1();
void MemChp2();
void MemCheck();


#endif CORE_UTILS_H	
