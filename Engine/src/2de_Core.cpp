#include "2de_Core.h"

#include <fstream>
#include <sys/stat.h>

#ifndef _WIN32
	#include <unistd.h>
#endif //_WIN32


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

CObject::CObject(const CObject &AObject) : Destroyed(false), Name(" CObject " + itos(ID)), RefCount(0), Managed(false)
{
	ID = reinterpret_cast<int>(this);
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

CFile::CFile() : File(NULL), READ_BUFFER_DEFAULT_SIZE(256)
{
}

CFile::CFile(const string &AFilename, EOpenMode Mode) : File(NULL), READ_BUFFER_DEFAULT_SIZE(256)
{
	Open(AFilename, Mode);
}

CFile::~CFile()
{
	Close();
}

bool CFile::Open(const string &AFilename, EOpenMode Mode)
{
	if (File != NULL)
	{
		Log("ERROR", "Can't open file '%s': another file is already opened.", AFilename.c_str());
		return false;
	}

	if (AFilename.empty())
	{
		Log("ERROR", "Can't open file with empty filename.");
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
	}

	if (File == NULL)
	{
		Log("ERROR", "Can't open file '%s'.", Filename.c_str());
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

/**
* CFile::Read - читает из файла BytesCount байт ElementsCount раз и записывает в память по адресу Buffer.
*/

bool CFile::Read(void *Buffer, size_t BytesCount, size_t ElementsCount /*= 1*/)
{
	if (Buffer == NULL || File == NULL || BytesCount == 0 || ElementsCount == 0)
		return false;

	if (fread(Buffer, BytesCount, ElementsCount, File) != ElementsCount && !Eof())
	{
		Log("ERROR", "File IO error occured in attempt to read data from '%s'.", Filename.c_str());
		return false;
	}

	return true;
}

/**
* CFile::Write - записывает в файл данные из массива из ElementsCount элементов размером BytesCount байт каждый, находящегося по адресу Data.
*/

bool CFile::Write(const void *Data, size_t BytesCount, size_t ElementsCount /*= 1*/)
{
	if (Data == NULL || File == NULL || BytesCount == 0 || ElementsCount == 0)
		return false;

	if (fwrite(Data, BytesCount, ElementsCount, File) != ElementsCount)
	{
		Log("ERROR", "File IO error occured in attempt to write data to '%s'.", Filename.c_str());
		return false;
	}

	return true;
}

bool CFile::ReadByte(byte *Buffer)
{
	return Read(Buffer, sizeof(byte));
}

bool CFile::WriteByte(byte *Data)
{
	return Write(Data, sizeof(byte));
}

bool CFile::WriteByte(byte Data)
{
	return WriteByte(&Data);
}

/**
* CFile::ReadString - читает из файла строку, завершённую нулевым байтом, максимальной длиной ASize и записывает её в память по адресу Buffer.
*
* \todo Хорошо бы либо вынести куда-нибудь эту логику, либо вообще перестать юзать подобные способы хранения строк.
*/

bool CFile::ReadString(char *Buffer, int ASize)
{
	if (File == NULL || Buffer == NULL || ASize == 0)
		return false;
	
	byte b;
	int i = 0;

	do
	{
		if (!ReadByte(&b))
			return false;

		Buffer[i] = b;
		i++;
	} while (b != 0 && i < ASize);

	return true;
}

/**
* CFile::ReadString - читает из файла строку, завершённую нулевым байтом и записывает её в строку Buffer.
* В отличие от версии с char* не требует явного указания максимальной длины строки.
*
* \todo Хорошо бы либо вынести куда-нибудь эту логику, либо вообще перестать юзать подобные способы хранения строк.
*/

bool CFile::ReadString(string &Buffer)
{
	if (File == NULL)
		return false;
	
	Buffer.clear();

	byte b;

	if (!ReadByte(&b))
		return false;

	// theoretically (not tested) makes processing somewhat faster on average strings, but consumes more memory..
	Buffer.reserve(READ_BUFFER_DEFAULT_SIZE);

	while (b != 0)
	{
		Buffer += b;

		if (!ReadByte(&b))
			return false;
	}

	return true;
}

/**
* CFile::WriteString - записывает строку Data в файл и заканчивает запись нулевым байтом.
*
* \todo Хорошо бы либо вынести куда-нибудь эту логику, либо вообще перестать юзать подобные способы хранения строк.
*/

bool CFile::WriteString(const char *Data)
{
	if (Data == NULL)
		return false;

	return Write(Data, strlen(Data) + 1);
}

/** \copydoc WriteString(const char *) */

bool CFile::WriteString(const string &Data)
{
	return Write(Data.c_str(), Data.length() + 1);
}

/**
* CFile::WriteText - записывает текст из Data в файл, не завершая его ничем.
*
* В отличие от WriteString, эта функция добрая и полезная.
*/

bool CFile::WriteText(const char *Data)
{
	if (Data == NULL)
		return false;
	
	return Write(Data, strlen(Data));
}

/** \copydoc WriteText(const char *) */

bool CFile::WriteText(const string &Data)
{
	return Write(Data.c_str(), Data.length());
}

/**
* CFile::ReadLine - читает строку, завершённую символом конца строки, из файла и записывает её в память по адресу Buffer.
* Чтение заканчивается при достижении ASize символа, символа конца строки или конца файла, что встретится первее.
* 
* \param[out] Buffer указатель на участок памяти, в который будет записана строка, 
* \param ASize максимальное число символов для чтения
*/

bool CFile::ReadLine(char *Buffer, int ASize)
{
	if (File == NULL || Buffer == NULL || ASize == 0)
		return false;

	if (fgets(Buffer, ASize, File) == NULL)
	{
		Log("ERROR", "File IO error occurend in attempt to read line from '%s'", Filename.c_str());
		return false;
	}

	return true;
}

/**
* CFile::ReadLine - читает строку из файла до символа конца строки или конца файла, что встретися первее, и записывает её в Buffer.
*/

bool CFile::ReadLine(string &Buffer)
{
	if (File == NULL)
		return false;
	
	Buffer.clear();

	byte b;

	// theoretically (not tested) makes processing somewhat faster on average strings, but consumes more memory..
	Buffer.reserve(READ_BUFFER_DEFAULT_SIZE);

	while (!Eof())
	{
		if (!ReadByte(&b))
			return false;

		Buffer += b;

		if (b == 10)
			break;
	}

	return true;
}

/**
* CFile::WriteLine - записывает строку Data в файл и завршает запись символами конца строки, соответствующими текущей системе.
*/

bool CFile::WriteLine(const char *Data)
{
	if (Data == NULL)
		return false;

	return WriteLine(Data, strlen(Data));
}

/*! \copydoc WriteLine(const char *) */

bool CFile::WriteLine(const string &Data)
{
	return WriteLine(Data.c_str(), Data.length());
}

/**
* CFile::GetContent - возвращает всё содержимое файла в виде строки.
*
* Не влияет на позицию курсора для других операций чтения.
*/

string CFile::GetContent()
{
	if (File == NULL)
		return "";

	string result;
	if (result.max_size() < Size())
	{
		Log("ERROR", "File '%s' is too large to be stored in single string", Filename.c_str());
		return "";
	}

	fpos_t OldPos;
	fgetpos(File, &OldPos);
	rewind(File);

	string tmp;

	while (!Eof())
	{
		ReadLine(tmp);
		result += tmp;
	}

	fsetpos(File, &OldPos);

	return result;
}

/**
* CFile::SetContent - записывает в файл строку Data с его начала.
*
* Позиция курсора остаётся в месте окончания записи, поэтому, очевидно, влияет на другие операции записи.
*/

bool CFile::SetContent(const string &Data)
{
	Rewind();
	return WriteText(Data);
}

/**
* CFile::Seek - перемещает внутренний указатель текущей позиции на указанное место.
*
* \param Offset смещение в байтах.
* \param Origin показывает, откуда считать Offset.
*/

bool CFile::Seek(long Offset, ESeekOrigin Origin)
{
	if (!File)
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

bool CFile::Rewind()
{
	return Seek(0L, SEEK_ORIGIN_BEGINNING);
}

bool CFile::Flush()
{
	if (!File)
		return false;

	return (fflush(File) == 0);
}

bool CFile::Eof() const
{
	if (!File)
		return true;

	return (feof(File) != 0);
}

long CFile::Size() const
{
	struct stat FileStat;

	if (stat(Filename.c_str(), &FileStat))
	{
		Log("ERROR", "Can't get size of %s.", Filename.c_str());
		return 0;
	}

	return FileStat.st_size;
}

/**
* CFile::WriteLine - внутренняя версия WriteLine, для выноса общего кода из версий с std::string и 0-terminated строкой.
*/

bool CFile::WriteLine(const char *Data, size_t Size)
{
	if (File == NULL || Data == NULL)
		return false;

	Write(Data, Size);

	byte b;

#ifdef _WIN32
	b = 13;
	WriteByte(&b);
#endif //_WIN32

	b = 10;
	WriteByte(&b);

	return true;

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
	strcpy(TimeString, asctime(CEnvironment::DateTime::GetLocalTimeAndDate()));
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
			tm* TimeStruct = CEnvironment::DateTime::GetLocalTimeAndDate();
			NewLogFileName += "-" + CEnvironment::DateTime::GetFormattedTime(*TimeStruct, "%y%m%d-%H%M%S");
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


//////////////////////////////////////////////////////////////////////////
// CSingletonManager

CSingletonManager *CSingletonManager::_instance = NULL;

CSingletonManager* CSingletonManager::Instance()
{
	return _instance;
}

void CSingletonManager::Init()
{
	_instance = new CSingletonManager;
}

void CSingletonManager::Add(CObject *AObject)
{
	AObject->IncRefCount(); // is not necessary i think. it it is?
	Singletones.push_front(AObject);
	//Log("NOTE", "ADDED TO SINGLETONE KILLER: %s", AObject->GetName().c_str());
}

void CSingletonManager::Clear()
{
	for(list<CObject*>::iterator i = Singletones.begin(); i != Singletones.end(); ++i)
	{
		//Log("INFO", "Singletone killer deleting object named: %s id: %u", (*i)->GetName().c_str(), (*i)->GetID());
		CObject *Object = *i;
		*i = NULL;
		CObject::DecRefCount(Object);
	}
}

void CSingletonManager::Finalize()
{
	delete _instance;
	_instance = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CBaseResource

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

//////////////////////////////////////////////////////////////////////////
// CResource

CResource::CResource()
{
	SetName("CResource");
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

tm* CEnvironment::DateTime::GetLocalTimeAndDate()
{
	time_t RawTime;
	time(&RawTime);
	return localtime(&RawTime);
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

