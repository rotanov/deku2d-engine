#include <stdlib.h>
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <string>
#include <sys/stat.h>
#ifndef _WIN32
	#include <unistd.h>
#endif //_WIN32

#include "2de_Core.h"
#include "2de_Engine.h"

bool Enabled = true;
unsigned int CObject::CObjectCount = 0;

#if defined(_DEBUG) && defined(_MSC_VER)

AllocList *allocList = NULL;

void RemoveTrack(unsigned long addr)
{
	AllocList::iterator i;

	if (allocList == NULL)
		return;
	for(i = allocList->begin(); i != allocList->end(); i++)
	{
		if((*i)->address == addr)
		{
			allocList->remove((*i));
			break;
		}
	}
};

void DumpUnfreed()
{
	FILE *fo = fopen("Memory.log", "w");
	AllocList::iterator i;
	unsigned long totalSize = 0;
	char buf[1024];

	if (allocList == NULL)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++) {
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
			(*i)->file, (*i)->line, (*i)->address, (*i)->size);
		fprintf(fo, "%s", buf);
		totalSize += (*i)->size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
	fprintf(fo, "%s", buf);
	sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	fprintf(fo, "%s", buf);
	fclose(fo);
};



#endif // defined(_DEBUG) && defined(_MSC_VER)

//////////////////////////////////////////////////////////////////////////
// CObject

CObject::CObject() : Destroyed(false), ID(++CObjectCount), Name(" CObject " + itos(ID)), RefCount(0), Managed(false)
{

}

CObject::~CObject()
{
}

bool CObject::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	return true;
}

void CObject::IncRefCount()
{
	RefCount++;
}

void CObject::DecRefCount(CObject* AObject)
{
	assert(AObject != NULL);
	AObject->RefCount--;
	if (AObject->RefCount <= 0 && AObject->Managed)
	{
		Log("INFO", "Destroying: %s, with id: %d", AObject->GetName().c_str(), AObject->GetID());
		delete AObject;
	}
}

const string& CObject::GetName() const
{
	return Name;
}

size_t CObject::GetID() const
{
	return ID;
}

void CObject::SetName(const string &AObjectName)
{
	Name = AObjectName;
}

void CObject::SetDestroyed()
{
	Destroyed = true;
}

bool CObject::isDestroyed() const
{
	return Destroyed;
}
//////////////////////////////////////////////////////////////////////////
// CFile

CFile::CFile() : File(NULL){}

CFile::CFile(const string &AFilename, EOpenMode Mode)
{
	File = NULL;
	Open(AFilename, Mode);
}

CFile::~CFile()
{
	Close();
}

bool CFile::Open(const string &AFilename, EOpenMode Mode)
{
	if (AFilename.empty())
	{
		Log("ERROR", "Can't open file. Invalid Filename");
		return false;
	}
	if (File != NULL)
	{
		Log("ERROR", "Can't open file %s: another file is already opened.", AFilename.c_str());
		return false;
	}

	Filename = AFilename;

	switch (Mode)
	{
	case OPEN_MODE_READ:
		File = fopen(Filename.c_str(), "rb");
		break;
	case OPEN_MODE_WRITE:
		File = fopen(Filename.c_str(), "wb");
		break;
	default:
		Log("ERROR", "Can't open file %s: invalid mode.", Filename.c_str());
		return false;
	}
	if (File == NULL)
	{
		Log("ERROR", "Can't open file %s.", Filename.c_str());
		return false;
	}
	return true;
}

bool CFile::Close()
{
	if (File == NULL)
		return false;

	fclose(File);
	File = NULL;

	Filename.clear();

	return true;
}

bool CFile::ReadByte(byte *Buffer)
{
	if (Buffer == NULL)
		return false;
	if (File == NULL)
		return false;
	if (fread(Buffer, 1, 1, File) != 1)
	{
		if (!Eof())
			Log("ERROR", "FILE IO Error. Can't read byte.");
		return false;
	}
	return true;
}

bool CFile::WriteByte(byte *Buffer)
{
	return Write(Buffer, 1);
}

bool CFile::WriteByte(byte Buffer)
{
	return WriteByte(&Buffer);
}

bool CFile::Read(void *Buffer, size_t BytesCount)
{
	if (Buffer == NULL)
		return false;
	if (File == NULL)
		return false;
	if (BytesCount == 0)
		return false;

	if (fread(Buffer, 1, BytesCount, File) != BytesCount)
	{
		if (!Eof())
			Log("ERROR", "FILE IO Error. Can't read data.");
		return false;
	}
	return true;
}

bool CFile::Write(const void *Buffer, size_t BytesCount)
{
	if (Buffer == NULL)
		return false;
	if (File == NULL)
		return false;
	if (BytesCount == 0)
		return false;

	if (fwrite(Buffer, 1, BytesCount, File) != BytesCount)
	{
		if (!Eof())
			Log("ERROR", "FILE IO Error. Can't write data.");
		return false;
	}
	return true;
}

bool CFile::ReadString(char *Buffer)
{
	if (File == NULL)
		return false;

	byte b;
	int i = 0;

	if (!Read(&b, 1))
		return false;

	while (b != 0)
	{
		Buffer[i] = b;
		i++;
		if (!Read(&b, 1))
			return false;
	}

	return true;
}

bool CFile::ReadString(string &Buffer)
{
	if (File == NULL)
		return false;

	byte b;
	Buffer = "";
	int i = 0;

	if (!Read(&b, 1))
		return false;

	while (b != 0)
	{
		Buffer += b;
		i++;
		if (!Read(&b, 1))
			return false;
	}

	return true;
}

//note buffer must exists!!!!
bool CFile::WriteString(const char *Buffer)
{
	if (File == NULL)
		return false;

	byte b = 0;
	for (unsigned int i = 0; i < strlen(Buffer); i++)
	{
		Write(&Buffer[i], 1);
	}
	Write(&b, 1);

	return true;
}

bool CFile::WriteString(const string &Buffer)
{
	if (File == NULL)
		return false;

	byte b = 0;
	for (unsigned int i = 0; i < Buffer.length(); i++)
	{
		// посимвольная запись?!.. о, щи....
		Write(&Buffer[i], 1);
	}
	Write(&b, 1);

	return true;
}

bool CFile::ReadLine(char* &Data)
{
	if (File == NULL)
		return false;

	char *buffer = new char[CFILE_MAX_STRING_LENGTH];
	int count = -1;
	// unsigned long read = 0; // unused variable

	do
	{
		fread(buffer + count + 1, 1, 1, File);
		count++;
	} while ((buffer[count] != 0x00) && (count < CFILE_MAX_STRING_LENGTH));

	if (Data != NULL)
		delete [] Data;

	Data = new char[count + 1]; // @todo: CFile should not allocate any memory for user. User should. There is leak there.
	for (int i = 0; i < count; i++)
		Data[i] = buffer[i];
	Data[count] = 0x00;
	delete [] buffer;

	return true;
}

bool CFile::WriteLine(const string &Buffer)
{
	if (File == NULL)
		return false;

	char b;

	Write((char*) Buffer.data(), Buffer.length());

#ifdef _WIN32
	// почему это вообще надо?.. лишний байт для чтения в глупых редакторах, не поддерживающих LF онли? или что?.. 
	// а если уж и нужно, то вообще надо бы сделать константу, дефайн, зависящий от платформы..
	b = 13;
	Write(&b,1);
#endif //_WIN32
	b = 10;
	Write(&b,1);

	return true;
}

bool CFile::Seek(unsigned int Offset, ESeekOrigin Origin)
{
	if (File == NULL)
		return false;

	int origin_const;

	switch (Origin)
	{
	case SEEK_ORIGIN_BEGINNING:
		origin_const = SEEK_SET;
		break;
	case SEEK_ORIGIN_CURRENT:
		origin_const = SEEK_CUR;
		break;
	case SEEK_ORIGIN_END:
		origin_const = SEEK_END;
		break;
	}

	return (fseek(File, Offset, origin_const) == 0);
}

bool CFile::Eof() const
{
	return !!feof(File);
}

size_t CFile::Size() const
{
	struct stat FileStat;

	if(stat(Filename.c_str(), &FileStat))
	{
		Log("ERROR", "Can't get size of %s.", Filename.c_str());
		return 0;
	}

	return FileStat.st_size;
}

//////////////////////////////////////////////////////////////////////////
// CLog

CLog::CLog()
{
	SetName("Log");
	Enabled = true;
	LogMode = LOG_MODE_FILE;	// well, usual default behavior for user-space (non-system) programs is to log on a console,
					// but for now i left the behavior, some of us used to... may be changed any time by calling
					// CLog::SetLogMode(CLog::ELogMode ALogMode);

	LogFileWriteMode = LOG_FILE_WRITE_MODE_TRUNCATE;
	DatedLogFileNames = true;
	Stream = NULL;
	LogFilePath = "";
	LogName = "System";
}

CLog::~CLog()
{
	if (Stream != NULL)
	{
		Log("INFO", "Log finished\n");

		if (LogMode == LOG_MODE_FILE)
		{
			dynamic_cast<ofstream*>(Stream)->close();
			delete Stream;
		}
	}
}

void CLog::WriteToLog(const char *Event, const char *Format, ...)
{
	if (!Enabled)
	{
		return;
	}

	if (Stream == NULL)
	{
		SetLogMode(LogMode);
		if (Stream == NULL)
		{
			return;
		}
	}

	char TimeAndEvent[64];

#ifdef LOG_TIME_TICK
	snprintf(TimeAndEvent, sizeof(TimeAndEvent) - 1, "[%d]%c%c[%s] ", SDL_GetTicks(), 9, 9, Event);
	TimeAndEvent[sizeof(TimeAndEvent) - 1] = 0;
#else
	char TimeString[64];
	strcpy(TimeString, asctime(&CEnvironment::DateTime::GetLocalTimeAndDate()));
	TimeString[strlen(TimeString) - 1] = 0;
	snprintf(TimeAndEvent, sizeof(TimeAndEvent) - 1, "[%s] [%s] ", TimeString, Event);
#endif

	char buffer[1024];

	va_list args;
	va_start(args, Format);
	vsnprintf(buffer, sizeof(buffer) - 1, Format, args);	// safe, but writes only first 1024 chars per log line...
	buffer[sizeof(buffer) - 1] = 0;				// should be sufficent for all cases, though...
	va_end(args);

	*Stream << TimeAndEvent << buffer << endl;
}

void CLog::SetLogMode(CLog::ELogMode ALogMode)
{
	if (Stream != NULL)
	{
		Log("INFO", "Log finished\n");

		if (LogMode == LOG_MODE_FILE)
		{
			dynamic_cast<ofstream*>(Stream)->close();
			delete Stream;
		}
	}

	switch (ALogMode)
	{
	case LOG_MODE_STDOUT:
		Stream = &cout;
		Log("INFO", "Log to stdout started");
		break;
	case LOG_MODE_STDERR:
		Stream = &cerr;
		Log("INFO", "Log to stderr started");
		break;
	case LOG_MODE_FILE:
		ios_base::openmode om;

		switch (LogFileWriteMode)
		{
		case LOG_FILE_WRITE_MODE_TRUNCATE:
			om = ios_base::trunc;
			break;
		case LOG_FILE_WRITE_MODE_APPEND:
			om = ios_base::app;
			break;
		}

		string NewLogFileName = LogFilePath + LogName;

		if (DatedLogFileNames) {
			tm TimeStruct = CEnvironment::DateTime::GetLocalTimeAndDate();
			NewLogFileName += "-" + CEnvironment::DateTime::GetFormattedTime(TimeStruct, "%y%m%d-%H%M%S");
		}
		NewLogFileName += ".log";

		Stream = new ofstream(NewLogFileName.c_str(), ios_base::out | om);
		if (Stream == NULL || !dynamic_cast<ofstream*>(Stream)->is_open())
		{
			cerr << "Can't open log file \"" << NewLogFileName << "\"." << endl;
			return;
		}
		Log("INFO", "Log to file \"%s\" started", NewLogFileName.c_str());
		break;
	}
	LogMode = ALogMode;
}


#ifdef _WIN32

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
#endif //_WIN32

CUpdateObject::CUpdateObject() : Active(true), Dead(false)
{
	SetName("CUpdateObject");
	CUpdateManager::Instance()->Add(this);
}

CUpdateObject::~CUpdateObject()
{
	//CUpdateManager::Instance()->Remove(GetID());
}

void CUpdateObject::SetDead()
{
	Dead = true;
	Active = false;
}

bool CUpdateObject::isDead() const
{
	return Dead;
}
CUpdateManager::CUpdateManager()
{
	SetName("CUpdateManager");
}

bool CUpdateManager::UpdateObjects()
{
	ManagerContainer toDelete;	
	CEngine *engine = CEngine::Instance();
	for(ManagerIterator it = Objects.begin(); it != Objects.end(); ++it)
	{
		CUpdateObject *data = (*it);
		if (!data->Active)
			continue;
		if (data->isDestroyed())
		{
			toDelete.push_back(data);
			continue;
		}
		// FIXED_DELTA_TIME
		float dt = 0;
		CEngine::Instance()->GetState(CEngine::STATE_DELTA_TIME, &dt);
		data->Update(dt); // @todo: подумать что использоваьт: фиксированную дельту или реальную engine->Getdt()
	}
	for(ManagerIterator i = toDelete.begin(); i != toDelete.end(); ++i)
	{
		Objects.remove(*i);
		CObject::DecRefCount(*i);
	}
	return true;
}

bool CBaseResource::LoadFromFile()
{
	return false;
}

bool CBaseResource::SaveToFile()
{
	return false;
}

bool CBaseResource::CheckLoad()
{
	return Loaded = !Loaded ? LoadFromFile() : true;
}

CBaseResource::CBaseResource() : Loaded(false), Filename(""){}

CResource::CResource()
{
	SetName("CResource");
}


void RegisterSingletoneInEngineMainClassMotherfuckers(CObject* Singletone)
{
	CEngine::Instance()->RegisterSingletone(Singletone);
}

//////////////////////////////////////////////////////////////////////////
// CFactory

CFactory::CFactory()
{
	SetName("Factory");
}

CFactory::~CFactory()
{	
	for(list<CObject*>::iterator i = Objects.begin(); i != Objects.end(); ++i)
		CObject::DecRefCount(*i);
	Objects.clear();
}

/**
* CFactory::Add - берёт управление памятью объекта на себя. У объекта обязательно должно быть уникальное имя, но допускается его задание в другом месте.
*/

void CFactory::Add(CObject *AObject, const string &AName /*= ""*/)
{
	// we generally allow passing empty name, when it set somewhere else
	if (AName != "")
	{
		AObject->SetName(AName);
	}

	Objects.push_back(AObject); // not only this
	AObject->IncRefCount();
	AObject->Managed = true;
}

/**
* CFactory::Destroy - ставит объект в очередь на уничтожение и удаляет его из списка управляемых объектов (при этом не снимая флаг управляемости).
*/

void CFactory::Destroy(CObject *AObject)
{
	list<CObject*>::iterator i = find(Objects.begin(), Objects.end(), AObject);

	if (i == Objects.end())
	{
		Log("ERROR", "Factory isn't managing object with given pointer");
		return;
	}
	
	AObject->SetDestroyed();
	Objects.erase(i);
	CObject::DecRefCount(AObject);
}


//////////////////////////////////////////////////////////////////////////
// CEnvironment

tm CEnvironment::DateTime::GetLocalTimeAndDate()
{
	time_t RawTime;
	tm *TimeStruct;
	time(&RawTime);
	TimeStruct = localtime(&RawTime);
	return *TimeStruct;
}

string CEnvironment::DateTime::GetFormattedTime(const tm TimeStruct, const char *Format)
{
	int AllocSize = 128;
	bool allocated = false;

	char *buffer = NULL;

	while (!allocated)
	{
		delete[] buffer;
		buffer = new char[AllocSize];
		if (!buffer)
			return string("");

		allocated = (strftime(buffer, AllocSize - 1, Format, &TimeStruct) != 0);
		AllocSize *= 2;
	}
	
	string result = buffer;
	SAFE_DELETE_ARRAY(buffer);
	return result;
}

string CEnvironment::Paths::GetWorkingDirectory()
{
	string result;
	char dir[CFILE_DEFAULT_MAXIMUM_PATH_LENGTH];

#ifdef _WIN32
	GetCurrentDirectory(CFILE_DEFAULT_MAXIMUM_PATH_LENGTH, dir);
#else
	getcwd(dir, CFILE_DEFAULT_MAXIMUM_PATH_LENGTH);
#endif //_WIN32

	result = dir;
	return result;
}

void CEnvironment::Paths::SetWorkingDirectory()
{
#ifdef _WIN32
	char *MainDir = new char[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
	DelFNameFromFPath(MainDir);
	SetCurrentDirectory(MainDir);
	delete [] MainDir;
#endif // _WIN32
	// *nix-like systems don't need current directory to be set to executable path - they use different directory structure
}


void DelFNameFromFPath(char *src)
{
	int i = strlen(src)-1;
	while(src[i] != '/' && src[i] != '\\' && src[i] != ':')
		i--;
	src[i+1] = 0;
}

void DelExtFromFName(char *src)
{
	int i = strlen(src)-1;
	while(src[i] != '.')
		i--;
	src[i] = 0;
}

void DelLastDirFromPath(char *src)
{
	int i = strlen(src)-1;
	while(src[i] == '\\' || src[i] == '/')
		i--;
	while(src[i] != '\\' && src[i] != '/')
		i--;
	src[i+1] = 0;
}

