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

// �������. ��� ������ ����� ������������ ������� ��� ������� ���-�� ��� ����������, ��� ��� ��� ��� ������, ������ ���������� ��� �������.
// � ������ ��� ������� ������� ����������� ��� �����, ����� �������� � ������������ �������, ��� ������� �������� ��������� ����� ����������� � �������.
//#define _DEBUG

// ����� �� ����������� MessageBox� ��� ������ � ������ ����������� ������ ��� ���. #undef ���� ��
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
// ������ �������� - ���������. ��� ������� ��� �� ��������, � ���� �� ����������� � �������� - � �� �� ��������.
typedef bool (*DirectoryWalkFunc)(char*, int); 
#endif WIN32

typedef bool (*Callback)();
typedef bool (*UpdateProc)(scalar);
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
*	CObject - ������� �����. �� ����������� �� ����.
*/

class CObject
{
public:
	unsigned int	type;	// type - ����� ������� �������. 
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
*	�-� ��� �������� ���������� � ������� ������, ���� �� ���� ��� ����������. ���� � ������ ��������� �� �������������� ���� ���.
*	������ �������� - ��������� �� ������� KEY_PRESSED ��� KEY_RELEASED
*	������ - ����� ������� ���� ������; ������������ ��������� �������� ���� � ����������: sdl_keysym.h, sdl_mouse.h - ������ ��������
*	������ �������� - �����������, �.�. ����� �� Shift, Ctrl, Alt ���
*	� �������� �������� - ASCII ���.
*/
typedef bool (CObject::*KeyInputFunc)(Uint8, Uint16, SDLMod, char);
/** 
*	1� � 2� ��������� - � � � ��������������
*	������ �������o ����������� ����� � ������� �� ���, ��� ����. ��������, � ������.
*/
typedef bool (*MouseInputFunc)(scalar, scalar); 

/**
*	CNodeObject - ��� ���� ������.
*/

class CListNode
{
public:
	CListNode *next, *prev;
	CListNode();
	CObject* GetData();
	void SetData(CObject *AData);
private:
	CObject *Data;					// ���������� ����.
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
*	���� ��������.
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
*	CUpdateManager - �������� ��������, ������� ������� ���������. ����� ����.
*	��, ��� ���� ����, ���� ��� �����-������ �-�� ����������. TODO!
*/

class CUpdateManager : public CList
{
public:
	CUpdateManager();
	bool UpdateObjects();
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
	CDirectoryWalk();
	~CDirectoryWalk();
	
	bool List();
	void ExploreDir(HANDLE hfile);
};

#endif WIN32

// �-�� ��� ������ �� ��������, � ��������� ������� ������ � ��� �����

void DelFNameFromFPath(char *src);
void DelExtFromFName(char *src);
void DelLastDirFromPath(char* src);
void DelInterval(string *src, const int s0, const int s1);

/**
*	����� ��������� �-�� ��� ������ ������ ������.
*	�� ��� ����� - ���� ������ �����, ���� ������� ���������.
*	�� ��� ������, ��� ��� ���� ����� ����� ��� ����, �� ����� ������ �� ������������
*/

void MemChp1();
void MemChp2();
void MemCheck();

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



#endif CORE_UTILS_H	
