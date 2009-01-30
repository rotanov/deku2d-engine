#include "CoreUtils.h"

#define _CRT_SECURE_NO_DEPRECATE

int ___w=0, ___h=0;
bool Enabled = true;

CObject::CObject()
{
	type = T_COBJECT;
	name = "CObject";
}

CFile::CFile(char *filename, int mode)
{
	if (mode == CFILE_READ)
		file = fopen(filename, "rb");
	else
		if (mode == CFILE_WRITE)
			file = fopen(filename, "wb");
		else
		{
			file = NULL;
			Log("AHTUNG", "Can't open file %s", filename);
		}
}
bool CFile::Open(const char *filename, int mode)
{
	if (filename == NULL)
	{
		Log("AHTUNG", "Can't open file. Invalid filename");
		return false;
	}
	if (file != NULL)
	{
		Log("AHTUNG", "Can't open file %s: another file is already opened.", filename);
		return false;
	}
	if (mode == CFILE_READ)
	{
		file = fopen(filename, "rb");
		if (file == NULL)
		{
			Log("AHTUNG", "Can't open file %s.", filename);
			return false;
		}
		return true;
	}
	if (mode == CFILE_WRITE)
	{
		file = fopen(filename, "wb");
		if ( file == NULL )
		{
			Log("AHTUNG", "Can't open file %s.", filename);
			return false;
		}
		return true;
	}
	if (mode!=CFILE_READ && mode!=CFILE_WRITE)
	{
		Log("AHTUNG", "Can't open file %s: invalid mode.", filename);
		return false;
	}
	return true;
}

bool CFile::Close()
{
	if (file == NULL)
		return false;
	fclose(file);
	return true;
}

bool CFile::ReadByte(pbyte buffer)
{
	if (buffer == NULL)
		return false;
	if (file == NULL)
		return false;
	if (fread(buffer, 1, 1, file) != 1)
	{
		if (!Eof())
			Log("AHTUNG", "FILE IO Error. Can't read byte.");
		return false;
	}
	return true;
}

bool CFile::Write(const void* buffer, DWORD nbytes)
{
	if (buffer == NULL)
		return false;
	if (file == NULL)
		return false;
	if (nbytes == 0)
		return false;
	if (fwrite(buffer, 1, nbytes, file) != nbytes)
	{
		if (!Eof())
			Log("AHTUNG", "FILE IO Error. Can't write data.");
		return false;
	}
	return true;
}

bool CFile::WriteByte(pbyte buffer)
{
	if (buffer == NULL)
		return false;
	if (file == NULL)
		return false;
	if (fwrite(buffer, 1, 1, file) != 1)
	{
		if (!Eof())
			Log("AHTUNG", "FILE IO Error. Can't write byte.");
		return false;
	}
	return true;
}

bool CFile::WriteByte(byte buffer)
{
	if (file == NULL)
		return false;
	if (fwrite(&buffer, 1, 1, file) != 1)
	{
		if (!Eof())
			Log("AHTUNG", "FILE IO Error. Can't write byte.");
		return false;
	}
	return true;
}

bool CFile::Read(void* buffer, DWORD nbytes)
{
	if (buffer == NULL)
		return false;
	if (file == NULL)
		return false;
	if (nbytes == 0)
		return false;
	if (fread(buffer, 1, nbytes, file) != nbytes)
	{
		if (!Eof())
			Log("AHTUNG", "FILE IO Error. Can't read data.");
		return false;
	}
	return true;
}

bool CFile::ReadLine(char* &data)
{
	if (file == NULL)
		return false;

	char *buffer = new char[CFILE_MAX_STRING_LENGTH];
	int count = -1;
	DWORD read = 0;
	do
	{
		fread(buffer+count+1, 1, 1, file);
		count++;
	} while ((buffer[count] != 0x00) && (count < CFILE_MAX_STRING_LENGTH));
	if (data != NULL)
		delete [] data;
	data = new char[count+1];
	for(int i=0; i<count; i++)
		data[i] = buffer[i];
	data[count] = 0x00;
	delete [] buffer;
	return true;
}

//note buffer must exists!!!!
bool CFile::ReadString(char* buffer)
{
	if (file == NULL)
		return false;
	byte b;
	int i = 0;
	if (!Read(&b,1))
		return false;
	while (b != 0)
	{
		buffer[i] = b;
		i++;
		if (!Read(&b,1))
			return false;
	}
	return true;
}

bool CFile::WriteString(const char* buffer)
{
	if (file == NULL)
		return false;
	byte b = 0;
	for (unsigned int i = 0;i < strlen(buffer);i++)
	{
		Write(&buffer[i], 1);
	}
	Write(&b, 1);
	return true;
}

bool CFile::ReadString(string &buffer)
{
	if (file == NULL)
		return false;
	byte b;
	buffer = "";
	int i = 0;
	if (!Read(&b, 1))
		return false;
	while (b != 0)
	{
		buffer += b;
		i++;
		if (!Read(&b, 1))
			return false;
	}
	return true;
}

bool CFile::WriteString(const string buffer)
{
	if (file == NULL)
		return false;
	byte b = 0;
	for (unsigned int i = 0;i<buffer.length();i++)
	{
		Write(&buffer[i], 1);
	}
	Write(&b, 1);
	return true;
}

bool CFile::Writeln(string buffer)
{
	if (file==NULL)
		return false;
	char b=13;
	Write((char*)buffer.data(), (DWORD)buffer.length());
	Write(&b,1);
	//for windows only 'cause of 13 10=)
#ifdef WIN32
	b = 10;
	Write(&b,1);
#endif
	return true;
}

DWORD CFile::Size()
{
// Size return goes here.	
	return 0;
}

bool CFile::Eof()
{
	return !!feof(file);
}
bool CFile::Seek(unsigned int offset, byte kind)
{
	if (fseek(file, offset, kind))
		return true;
	return false;
}


//----------------------------------//
//			Log Stuff				//
//----------------------------------//
void SetScreenParams(int Width, int Height){
	___w=Width;
	___h=Height;
}

void GetScreenParams(int &Width, int &Height){
	Width=___w;
	Height=___h;
}

int ScreenW()
{
	return ___w;
}
int ScreenH()
{
	return ___h;
}


char LogFile[2048];
int __l=0;
char _day[7][4];
char _mon[12][4];
CDummy InitLog = CDummy();
void CreateLogFile(char *fname)
{
	memset(LogFile,0,2048);//(LogFile,2048);
	memcpy(LogFile,fname,strlen(fname));
	FILE *hf = NULL;
	hf = fopen(LogFile, "w");
#ifdef LOG_TIME_TICK
	fprintf(hf, "[0]%c%c[INFO] Log file \"%s\" created\n", 9,9,fname);
#else
	__time64_t long_time;
	_time64( &long_time ); 
	struct tm *newtime;
	newtime = localtime(&long_time);
	char buff[256];
	memset(buff,0,256);
	strcpy(buff,asctime(newtime));
	for (unsigned int i=0;i<strlen(buff);i++){
		if (buff[i]<=13)
			buff[i] = 0;
	}
	fprintf(hf, "[%s] [INFO] Log file \"%s\" created\n", buff,fname);
#endif
	fclose(hf);
}
void Log(char* Event,char* Format,...)
{
	if (!Enabled)
		return;
	va_list ap;
	FILE *hf = NULL;
	hf = fopen(LogFile, "a");
#ifdef LOG_TIME_TICK
	fprintf(hf,"[%d]%c%c[%s] ", SDL_GetTicks(), 9, 9, Event);
#else
	__time64_t long_time;
	_time64( &long_time ); 
	struct tm *newtime;
	newtime = localtime(&long_time);
	char buff[256];
	memset(buff,0,256);
	strcpy(buff,asctime(newtime));
	for (unsigned int i=0;i<strlen(buff);i++){
		if (buff[i]<=13)
			buff[i] = 0;
	}
	fprintf(hf,"[%s] [%s] ", buff, Event);
#endif
	va_start(ap, Format);
	vfprintf(hf, Format, ap);
	va_end(ap);
	fprintf(hf,"\n");
	fclose(hf);
}

void ToggleLog( bool _Enabled )
{
	Enabled = _Enabled;
}

CNodeObject::CNodeObject()
{
	next = prev = NULL;
	data = NULL;
}

CObject* CNodeObject::GetData()
{
	return data;
}

void CNodeObject::SetData(CObject *object)
{
	data = object;
}

CObjectList::CObjectList()
{
	first = last = current = NULL;
}

CObjectList::~CObjectList()
{
/*	if (first == NULL) 
		return;
	CNodeObject *that = first, *next = NULL;
	while (that != NULL)
	{
		next = that->next;
		delete that;
		that = next;
	}*/
	Clear();
}

bool CObjectList::AddObject(CObject *object)
{
	if (object == NULL)
		return false;
//	CNodeObject *prev = NULL, **next = &first;
//	while ((*next) != NULL)
//	{
//		prev = *next;
//		next = &((*next)->next);
//	}
//	last = *next = new CNodeObject;
	CNodeObject *tmp = new CNodeObject();
	tmp->prev = last;
	if (last == NULL)
		first = tmp;
	else
		last->next = tmp;
	last = tmp;
	tmp->SetData(object);
/*	(*next)->SetData(object);
	(*next)->prev = prev;*/
	Reset();
	return true;
}

bool CObjectList::DelObject(string objectname)
{
	CNodeObject *tmp = GetNodeObject(objectname);
	if (tmp != NULL)
	{
		if (tmp->prev != NULL)
			tmp->prev->next = tmp->next;
		else
			first = tmp->next;
		if (tmp->next != NULL)
			tmp->next->prev = tmp->prev;
		else
			last = tmp->prev;
		delete tmp;
		return true;
	}
	return false;
}

CObject* CObjectList::GetObject(string objectname)
{
	CNodeObject *tmp = first;
	while (tmp != NULL)
	{
		CObject * dat = tmp->GetData();
		if (dat->name == objectname)
		{
			return dat;
		}
		tmp = tmp->next;
	}
	Log("WARNING", "Object with name %s requested from GetObject() not found in %s", objectname.c_str(), name.c_str());
	return NULL;
	//Reset();
	//CObject *result = Next();
	//while (result->name != objectname)
	//{
	//	result = Next();
	//}
	//return result;
}

CNodeObject* CObjectList::GetNodeObject(string objectname)
{
	CNodeObject *tmp = first;
	while (tmp != NULL)
	{
		if (tmp->GetData()->name == objectname)
		{
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

bool CObjectList::Enum(CObject* &result)
{
	result = NULL;
	if (current != NULL)	
	{
		result = current->GetData();
		current = current->next;
	}
	return result != NULL;
}

CObject* CObjectList::Next()
{
	CObject* result = NULL;
	if (current == NULL)	
		current = first;
	else
	{
		result = current->GetData();
		current = current->next;
	}
	return result;
}

void CObjectList::Reset()
{
	current = first;
}

void CObjectList::Clear()
{
	while (first != NULL)
	{
		CNodeObject *tmp = first;
		first = first->next;
		delete tmp;
	}
	first = NULL;
	last = NULL;
	current = NULL;
}

void CObjectList::Sort(ObjCompCall comp)
{
	if (first == NULL)
		return;
	bool b = true;
	while (b)
	{
		b = false;
		CNodeObject *tmp = first;
		while (tmp != NULL)
		{
			if (tmp->next != NULL)
			{
				if (!(comp)(tmp->GetData(), tmp->next->GetData()))
				{
					b = true;
					SwapObjs(tmp, tmp->next);
				}
			}
			tmp = tmp->next;
		}
	}
}

void CObjectList::SwapObjs(CNodeObject *obj1, CNodeObject *obj2)
{
	CNodeObject *lft = obj1->prev;
	obj1->prev = obj2->prev;
	obj2->prev = lft;
	if (obj1->prev != NULL)
		obj1->prev->next = obj1;
	else
		first = obj1;
	if (obj2->prev != NULL)
		obj2->prev->next = obj2;
	else
		first = obj2;
	lft = obj1->next;
	obj1->next = obj2->next;
	obj2->next = lft;
	if (obj1->next != NULL)
		obj1->next->prev = obj1;
	else
		last = obj1;
	if (obj2->next != NULL)
		obj2->next->prev = obj2;
	else
		last = obj2;
}

bool CObjectList::Call(ObjCall callproc)
{
	CNodeObject *tmp = first;
	while (tmp != NULL)
	{
		if (!(callproc)(tmp->GetData()))
			return false;		
		tmp = tmp->next;
	}
	return true;
}

bool CObjectList::DelObj( int ind )
{
	CNodeObject *tmp = first;
	for (int i = 0; i < ind; i++)
		tmp = tmp->next;
	if (tmp != NULL)
	{
		if (tmp->prev != NULL)
			tmp->prev->next = tmp->next;
		else
			first = tmp->next;
		if (tmp->next != NULL)
			tmp->next->prev = tmp->prev;
		else
			last = tmp->prev;
		delete tmp;
		return true;
	}
	return false;
}
CPSingleTone* CPSingleTone::Instance()
{
	if (_instance == NULL)
	{
		_instance = new CPSingleTone;
	}
	_refcount++;
	return _instance;
}

void CPSingleTone::FreeInst()
{
	_refcount--;
	if (!_refcount)
	{
		delete this;
		Log("INFO", "Singletone deleted from memory.");
		_instance = NULL;
	}
}

CPSingleTone::CPSingleTone()
{

}

CPSingleTone::~CPSingleTone()
{

}

CPSingleTone* CPSingleTone::_instance = 0;
int CPSingleTone::_refcount = 0;

#ifdef WIN32
#include <Windows.h>

_CrtMemState *MemState1 = NULL, *MemState2 = NULL, *MemState3 = NULL;

void MemChp1()
{
	if (!MemState1)
		MemState1 = new _CrtMemState;
	_CrtMemCheckpoint(MemState1);
}

void MemChp2()
{
	if (!MemState2)
		MemState2 = new _CrtMemState;
	_CrtMemCheckpoint(MemState2);

}

void MemCheck()
{
	if (!MemState1 || !MemState2)
	{
		Log("CAUTION", "Invalid MemCheck call: MemChp1() or MemChp2() has not been called");
		return;
	}
	if (!MemState3)
		MemState3 = new _CrtMemState;

	HANDLE hLogFile;
	hLogFile = CreateFile("Memlog.txt", GENERIC_WRITE, 
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, hLogFile);

	_RPT0(_CRT_WARN, "file message\n");

	if ( _CrtMemDifference( MemState3, MemState1, MemState2 ) )
		_CrtMemDumpStatistics( MemState3 );
	CloseHandle(hLogFile);
}

#endif