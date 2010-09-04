#include "2de_Core.h"

#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CObject

unsigned int CObject::CObjectCount = 0;
CObject::CObject() : Managed(false), Destroyed(false), ID(++CObjectCount), Name(" CObject " + itos(ID)), RefCount(0)
{

}

CObject::CObject(const CObject &AObject) : Managed(false), Destroyed(false), ID(++CObjectCount), Name(" CObject " + itos(ID)), RefCount(0)
{
}

CObject& CObject::operator=(const CObject &AObject)
{
	if (this == &AObject)
		return *this;

	Destroyed = AObject.Destroyed;
	ID = ++CObjectCount;
	Name = AObject.GetName() + " copy";
	RefCount = 0;
}

CObject::~CObject()
{
}

void CObject::IncRefCount()
{
	/*RefCount++;*/
}

void CObject::DecRefCount(CObject* AObject)
{
	/*assert(AObject != NULL);
	AObject->RefCount--;
	if (AObject->RefCount <= 0 && AObject->Managed)
	{
		Log("INFO", "Destroying: %s, with id: %d", AObject->GetName().c_str(), AObject->GetID());
		delete AObject;
	}*/
}

const string& CObject::GetName() const
{
	return Name;
}

void CObject::SetName(const string &AObjectName)
{
	if (Managed)
	{
		Log("ERROR", "Name of managed objects can only be changed by calling CFactory::Rename");
		return;
	}

	Name = AObjectName;
}

size_t CObject::GetID() const
{
	return ID;
}

/**
* CObject::SetDestroyed - для не-managed объектов помечает объект как уничтоженный, для managed - вызывает CFactory::Destroy. Не выполняет никаких действий для объектов, уже помеченных как уничтоженные.
*/

void CObject::SetDestroyed()
{
	if (Destroyed)
		return;

	if (Managed)
		CFactory::Instance()->Destroy(this);
	else
		Destroyed = true;
}

bool CObject::isDestroyed() const
{
	return Destroyed;
}

bool CObject::isManaged() const
{
	return Managed;
}

bool CObject::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	return true;
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
	/*AObject->IncRefCount(); // is not necessary i think. it it is?*/
	Singletones.push(AObject);
	//Log("NOTE", "ADDED TO SINGLETONE KILLER: %s", AObject->GetName().c_str());
}

void CSingletonManager::Clear()
{
	while (!Singletones.empty())
	{
		delete Singletones.top();
		Singletones.pop();
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

CBaseResource::CBaseResource() : Loaded(false), Filename("")
{
}

const string& CBaseResource::GetFilename() const
{
	return Filename;
}

void CBaseResource::SetFilename(const string &AFilename)
{
	Filename = AFilename; // may be some check here
}

//////////////////////////////////////////////////////////////////////////
// CResource

CResource::CResource()
{
	SetName("CResource");
}

void CResource::SetName(const string &AObjectName)
{
	// эээ, неее.. имя ресурса и имя его файла - две большие разницы..
	CObject::SetName(AObjectName);
	int t0 = Filename.length() - 1;
	while (t0 > 0 && Filename[t0] != '/' && Filename[t0] != '\\')
		t0--;
	t0++;
	Filename.insert(t0, AObjectName);
	Filename.erase(t0 + AObjectName.length(), Filename.length() - t0 - AObjectName.length() - 4);
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
		Log("ERROR", "Can't open file '%s': another file is already opened", AFilename.c_str());
		return false;
	}

	if (AFilename.empty())
	{
		Log("ERROR", "Can't open file with empty filename");
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
		Log("ERROR", "Can't open file '%s'", Filename.c_str());
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
		Log("ERROR", "File IO error occured in attempt to read data from '%s'", Filename.c_str());
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
		Log("ERROR", "File IO error occured in attempt to write data to '%s'", Filename.c_str());
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

		if (b == 0)	// that's needed, because std::string is buggy in handling \0 in strings...
			break;

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
		Log("ERROR", "Can't get size of '%s'", Filename.c_str());
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
	WriteText(CEnvironment::GetLineTerminator());

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CFileSystem

bool CFileSystem::Exists(const string &APath)
{
	struct stat FileStat;
	return (stat(APath.c_str(), &FileStat) == 0);
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
		return;

	if (Stream == NULL)
	{
		SetLogMode(LogMode);
		if (Stream == NULL)
		{
			return;
		}
	}

	va_list args;
	va_start(args, Format);

	int MessageLength = vsnprintf(NULL, 0, Format, args) + 1;

	char *buffer = new char[MessageLength + 1];

	vsnprintf(buffer, MessageLength, Format, args);
	buffer[MessageLength] = 0;

	va_end(args);

	*Stream << "[";
#ifdef LOG_TIME_TICK
	*Stream << SDL_GetTicks() << "]\t\t[";
#else
	*Stream << CEnvironment::DateTime::GetFormattedTime(CEnvironment::DateTime::GetLocalTimeAndDate(), "%c") << "] [";
#endif // LOG_TIME_TICK

	*Stream << Event << "] " << buffer << endl;

	delete[] buffer;
}

bool CLog::isEnabled() const
{
	return Enabled;
}

void CLog::Toggle(bool AEnabled)
{
	Enabled = AEnabled;
}

CLog::ELogMode CLog::GetLogMode() const 
{
	return LogMode;
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

		if (DatedLogFileNames)
		{
			tm* TimeStruct = CEnvironment::DateTime::GetLocalTimeAndDate();
			NewLogFileName += "-" + CEnvironment::DateTime::GetFormattedTime(TimeStruct, "%y%m%d-%H%M%S");
		}
		NewLogFileName += ".log";

		Stream = new ofstream(NewLogFileName.c_str(), ios_base::out | om);
		if (Stream == NULL || !dynamic_cast<ofstream*>(Stream)->is_open())
		{
			cerr << "Can't open log file '" << NewLogFileName << "'." << endl;
			return;
		}
		Log("INFO", "Log to file '%s' started", NewLogFileName.c_str());
		break;
	}
	LogMode = ALogMode;
}

CLog::ELogFileWriteMode CLog::GetLogFileWriteMode() const
{
	return LogFileWriteMode;
}

void CLog::SetLogFileWriteMode(CLog::ELogFileWriteMode ALogFileWriteMode)
{
	LogFileWriteMode = ALogFileWriteMode;
}

bool CLog::GetDatedLogFileNames() const
{
	return DatedLogFileNames;
}

void CLog::SetDatedLogFileNames(bool ADatedLogFileNames)
{
	DatedLogFileNames = ADatedLogFileNames;
}

void CLog::SetLogFilePath(const string &ALogFilePath)
{
	LogFilePath = ALogFilePath;
}

string CLog::GetLogFilePath() const
{
	return LogFilePath;
}

void CLog::SetLogName(const string &ALogName)
{
	LogName = ALogName;
}

string CLog::GetLogName() const
{
	return LogName;
}

//////////////////////////////////////////////////////////////////////////
// CEnvironment

tm* CEnvironment::DateTime::GetLocalTimeAndDate()
{
	time_t RawTime;
	time(&RawTime);
	return localtime(&RawTime);
}

string CEnvironment::DateTime::GetFormattedTime(tm *TimeStruct, const char *Format)
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

		allocated = (strftime(buffer, AllocSize - 1, Format, TimeStruct) != 0);
		AllocSize *= 2;
	}
	
	string result = buffer;
	SAFE_DELETE_ARRAY(buffer);
	return result;
}

string CEnvironment::Paths::GetExecutablePath()
{
	string result;

#ifdef _WIN32
	char *MainDir = new char[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
	DelFNameFromFPath(MainDir);

	result = MainDir;

	delete [] MainDir;
#else
	// executable path is useless on *nix-like systems, so return empty string
#endif // _WIN32

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

void CEnvironment::Paths::SetWorkingDirectory(const string &AWorkingDirectory)
{
#ifdef _WIN32
	SetCurrentDirectory(AWorkingDirectory.c_str());
#else
	chdir(AWorkingDirectory.c_str());
#endif // _WIN32
}

string CEnvironment::Paths::GetConfigPath()
{
	return ConfigPath;

}

void CEnvironment::Paths::SetConfigPath(const string &AConfigPath)
{
	ConfigPath = AConfigPath;
}

string CEnvironment::Paths::GetLogPath()
{
	return LogPath;

}

void CEnvironment::Paths::SetLogPath(const string &ALogPath)
{
	LogPath = ALogPath;
}

/**
* CEnvironment::Paths::GetUniversalDirectory - возвращает стандартную для системы директорию для помещения всех изменяемых файлов программы, таких как конфиг, лог, возможно сэйвы и т. п.
*
* Название Universal здесь не очень удачное, но ничего лучше я придумать не смог...
*/

string CEnvironment::Paths::GetUniversalDirectory()
{
	string result;
#ifdef _WIN32
	result = CEnvironment::Variables::Get("APPDATA");
	if (!result.empty())
		result += "\\" + CEngine::Instance()->GetProgramName() + "\\";
#else
	string HomeDir = CEnvironment::Variables::Get("HOME");
	if (HomeDir.empty())
	{
		string UserName = CEnvironment::Variables::Get("USER");
		if (UserName.empty())
			UserName = CEnvironment::Variables::Get("USERNAME");

		if (!UserName.empty())
			HomeDir = "/home/" + UserName;
	}

	if (!HomeDir.empty())
		result = HomeDir + "/" + CEngine::Instance()->GetProgramName() + "/";
#endif // _WIN32

	return result;
}

string CEnvironment::Paths::ConfigPath = "Config/";
string CEnvironment::Paths::LogPath = "Logs/";

string CEnvironment::Variables::Get(const string &AName)
{
	char *var = getenv(AName.c_str());
	return ((var == NULL) ? "" : string(var));
}

void CEnvironment::Variables::Set(const string &AName, const string &AValue)
{
	static string PutEnvString;
	PutEnvString = AName + "=" + AValue;
	SDL_putenv(const_cast<char *>(PutEnvString.c_str()));
}

/**
* CEnvironment::LogToStdOut - упрощённый лог на стандартный вывод, в обход синглтонного ада CLog'а.
*
* Эта функция используется макросом Log при задефайненом SIMPLIFIED_LOG (по-умолчанию это не так). Также можно использовать вручную.
* В основном нужна при отладке, а также, когда движок не запущен, а залогировать что-то очень хочется.
*/

void CEnvironment::LogToStdOut(const char *Event, const char *Format, ...)
{
	va_list args;
	va_start(args, Format);

	int MessageLength = vsnprintf(NULL, 0, Format, args) + 1;

	char *buffer = new char[MessageLength + 1];

	vsnprintf(buffer, MessageLength, Format, args);
	buffer[MessageLength] = 0;

	va_end(args);

	cout << "[" << CEnvironment::DateTime::GetFormattedTime(CEnvironment::DateTime::GetLocalTimeAndDate(), "%c") << "] ["
		<< Event << "] " << buffer << endl;
}

string CEnvironment::GetLineTerminator()
{
#ifdef _WIN32
	static string LineTerminator = "\r\n";
#else
	static string LineTerminator = "\n";
#endif // _WIN32

	return LineTerminator;
}

//////////////////////////////////////////////////////////////////////////
// CFactory

CFactory::CFactory()
{
	SetName("Factory");
}

CFactory::~CFactory()
{	
}

/**
* CFactory::Rename - переименовывает управляемый объект.
*/

void CFactory::Rename(const string &AName, const string &ANewName)
{
	if (Objects.count(AName) == 0)
	{
		Log("WARNING", "Factory isn't managing object named '%s'", AName.c_str());
		return;
	}

	CObject *Object = Objects[AName];
	Objects.erase(AName);
	Object->SetName(ANewName);
	Objects[ANewName] = Object;
}

/**
* CFactory::Destroy - ставит объект в очередь на уничтожение и удаляет его из списка управляемых объектов (при этом не снимая флаг управляемости).
*/

void CFactory::Destroy(CObject *AObject)
{
	map<string, CObject*>::iterator i;
	
	for (i = Objects.begin(); i != Objects.end(); ++i)
	{
		if (i->second == AObject)
			break;
	}

	if (i == Objects.end())
	{
		Log("ERROR", "Factory isn't managing object with given pointer");
		return;
	}
	
	AObject->Destroyed = true;

	Objects.erase(i);
	Deletion.push(AObject);

	//CObject::DecRefCount(AObject);
}

/**
* CFactory::CleanUp - очищает очередь уничтожения объектов, освобождая память каждого объекта.
*/

void CFactory::CleanUp()
{
	CObject *object;

	// we can implement any additional logic here, based for example on size() of queue or whatever, if it's needed..
	while (!Deletion.empty())
	{
		object = Deletion.front();
		Log("INFO", "Destroying: %s, with id: %d", object->GetName().c_str(), object->GetID());

		delete object;
		// delete Deletion.front();

		Deletion.pop();
	}
}

// well, maybe there is better way to do final all-destroy clean-up..
void CFactory::DestroyAll()
{
	for (map<string, CObject*>::iterator i = Objects.begin(); i != Objects.end(); ++i)
		Deletion.push(i->second);

	Objects.clear();
	CleanUp();
}


void DelFNameFromFPath(char *src)
{
	int i = strlen(src)-1;
	while (src[i] != '/' && src[i] != '\\' && src[i] != ':')
		i--;
	src[i+1] = 0;
}

void DelExtFromFName(char *src)
{
	int i = strlen(src)-1;
	while (src[i] != '.')
		i--;
	src[i] = 0;
}

void DelLastDirFromPath(char *src)
{
	int i = strlen(src)-1;
	while (src[i] == '\\' || src[i] == '/')
		i--;
	while (src[i] != '\\' && src[i] != '/')
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

	if (_CrtMemDifference(MemState3, MemState1, MemState2))
		_CrtMemDumpStatistics(MemState3);
	CloseHandle(hLogFile);
}
#endif //_WIN32

#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

AllocList *allocList = NULL;

void RemoveTrack(unsigned long addr)
{
	AllocList::iterator i;

	if (allocList == NULL)
		return;
	for (i = allocList->begin(); i != allocList->end(); i++)
	{
		if ((*i)->address == addr)
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

	for (i = allocList->begin(); i != allocList->end(); i++)
	{
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
			(*i)->file, (*i)->line, (*i)->address, (*i)->size);
		fprintf(fo, "%s", buf);
		//Log("MEM", "%s", buf); - было бы неплохо.
		totalSize += (*i)->size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
	fprintf(fo, "%s", buf);
	sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	fprintf(fo, "%s", buf);
	fclose(fo);
};
#endif // defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)
