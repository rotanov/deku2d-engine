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
static int CObjectCount = 0;
CList CObjectManager; // Ultimate!!!!111!!11

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

CObject::CObject()
{
	CObjectCount++;
	type = ListRefCount = 0;
	//char * tmp = new char[16];
	//SDL_itoa(CObjectCount, tmp, 10);
	name += (string)itos(CObjectCount) + " CObject ";
	//delete [] tmp;
	id = CObjectCount;
	CObjectManager.AddObject(this);
}

CObject::~CObject()
{
	//Log.Log("INFO", "DESTRUCT %s, id: %d", name.c_str(), id); //debug
}

bool CObject::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	return true;
}

void CObject::IncListRefCount()
{
	ListRefCount++;
}

void CObject::DecListRefCount()
{
	ListRefCount--;
	if (ListRefCount < 0)
	{
		Log.Log("ERROR", "CObject named %s id: %d list reference broken, it is: ", name.c_str(), id, ListRefCount);
#ifdef CRITICAL_ERRORS_MESSAGE_BOXES
		MessageBox(0, "CObject list reference broken.", "ERROR", MB_OK);
#endif // CRITICAL_ERRORS_MESSAGE_BOXES
	}
}

const int CObject::GetListRefCount() const
{
	return ListRefCount;
}

const char* CObject::GetName() const
{
	return name.c_str();
}

unsigned int CObject::GetID() const
{
	return id;
}

void CObject::SetName(const char* AObjectName)
{
	name = AObjectName;
}

void CObject::SetName(const string &AObjectName)
{
	name = AObjectName;
}

ostream& operator<<(ostream &Stream, CObject Object)
{
	Stream << Object.name << "(" << Object.id << ")";
	return Stream;
}


/************************************************************************/
/* CFile                                                                */
/************************************************************************/
CFile::CFile() : File(NULL)
{
}

CFile::CFile(const string AFileName, EOpenMode Mode)
{
	File = NULL;
	Open(AFileName, Mode);
}

CFile::~CFile()
{
	Close();
}

bool CFile::Open(const string AFileName, EOpenMode Mode)
{
	if (AFileName.empty())
	{
		Log.Log("ERROR", "Can't open file. Invalid filename");
		return false;
	}
	if (File != NULL)
	{
		Log.Log("ERROR", "Can't open file %s: another file is already opened.", AFileName.c_str());
		return false;
	}

	Filename = AFileName;

	switch (Mode)
	{
	case OPEN_MODE_READ:
		File = fopen(Filename.c_str(), "rb");
		if (File == NULL)
		{
			Log.Log("ERROR", "Can't open file %s.", Filename.c_str());
			return false;
		}
		break;
	case OPEN_MODE_WRITE:
		File = fopen(Filename.c_str(), "wb");
		if (File == NULL)
		{
			Log.Log("ERROR", "Can't open file %s.", Filename.c_str());
			return false;
		}
		break;
	default:
		Log.Log("ERROR", "Can't open file %s: invalid mode.", Filename.c_str());
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

bool CFile::ReadByte(unsigned char *Buffer)
{
	if (Buffer == NULL)
		return false;
	if (File == NULL)
		return false;
	if (fread(Buffer, 1, 1, File) != 1)
	{
		if (!Eof())
			Log.Log("ERROR", "FILE IO Error. Can't read byte.");
		return false;
	}
	return true;
}

bool CFile::WriteByte(unsigned char *Buffer)
{
	return Write(Buffer, 1);
}

bool CFile::WriteByte(unsigned char Buffer)
{
	return WriteByte(&Buffer);
}

bool CFile::Read(void *Buffer, unsigned long BytesCount)
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
			Log.Log("ERROR", "FILE IO Error. Can't read data.");
		return false;
	}
	return true;
}

bool CFile::Write(const void *Buffer, unsigned long BytesCount)
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
			Log.Log("ERROR", "FILE IO Error. Can't write data.");
		return false;
	}
	return true;
}

bool CFile::ReadString(char *Buffer)
{
	if (File == NULL)
		return false;

	unsigned char b;
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

	unsigned char b;
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

	unsigned char b = 0;
	for (unsigned int i = 0; i < strlen(Buffer); i++)
	{
		Write(&Buffer[i], 1);
	}
	Write(&b, 1);

	return true;
}

bool CFile::WriteString(const string Buffer)
{
	if (File == NULL)
		return false;

	unsigned char b = 0;
	for (unsigned int i = 0; i < Buffer.length(); i++)
	{
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

	Data = new char[count + 1];
	for (int i = 0; i < count; i++)
		Data[i] = buffer[i];
	Data[count] = 0x00;
	delete [] buffer;

	return true;
}

bool CFile::WriteLine(string Buffer)
{
	if (File == NULL)
		return false;

	char b;

	Write((char*) Buffer.data(), (unsigned long) Buffer.length());

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
		Log.Log("ERROR", "Can't get size of %s.", Filename.c_str());
		return 0;
	}

	return FileStat.st_size;
}


/************************************************************************/
/* CList, CListNode                                                     */
/************************************************************************/
CListNode::CListNode()
{
	next = prev = NULL;
	Data = NULL;
}

CObject* CListNode::GetData()
{
	return Data;
}

void CListNode::SetData(CObject *object)
{
	Data = object;
}

CList::CList()
{
	SetName("CList");
	first = last = current = NULL;
	NodeCount = 0;
}

CList::~CList()
{
	//Clear();
}

bool CList::AddObject(CObject *AObject)
{
	if (!AObject)
		return false;
	CListNode *ListNode = new CListNode;
	ListNode->SetData(AObject);
	ListNode->prev = last;
	if (last)
		last->next = ListNode;
	else
		first = ListNode;	
	last = ListNode;
	NodeCount++;
	AObject->IncListRefCount();
	return false;
}

bool CList::DelObject(CObject *AObject)
{
	CListNode *ListNode = GetListNode(AObject);
	if (!ListNode)
	{
		Log.Log("ERROR", "Can't delete object named %s from %s id: %d: object not found", AObject->GetName(), GetName(), GetID());
		return false;
	}
	DelNode(ListNode);
	return true;
}

bool CList::DelObject(const string *AObjectName)
{
	CListNode *ListNode = GetListNode(AObjectName);
	if (!ListNode)
	{
		Log.Log("ERROR", "Can't delete object named %s from %s id: %d: object not found", AObjectName, GetName(), GetID());
		return false;
	}
	DelNode(ListNode);
	return true;
}

bool CList::DelObject(const char *AObjectName)
{
	CListNode *ListNode = GetListNode(&((string)AObjectName));
	if (!ListNode)
	{
		Log.Log("ERROR", "Can't delete object named %s from %s id: %d: object not found", AObjectName, GetName(), GetID());
		return false;
	}
	DelNode(ListNode);
	return true;
}

bool CList::DelObject(int AId)
{
	CListNode *ListNode = GetListNode(AId);
	if (!ListNode)
	{
		Log.Log("ERROR", "Can't delete object id:%d from %s id: %d: object not found", AId, GetName(), GetID());
		return false;
	}
	DelNode(ListNode);
	return true;
}

void CList::Reset()
{
	current = NULL;
}

bool CList::Enum(CObject* &result)
{
	result = NULL;
	if (!current)
		result = (current = first)?current->GetData():NULL;
	else
		result = (current = current->next)?current->GetData():NULL;
	return result;
}

CObject* CList::Next()
{
	if (!current)
		return (current = first)?current->GetData():NULL;
	return (current = current->next)?current->GetData():NULL;
}

void CList::Clear()
{
	while (first != NULL)
	{
		CListNode *tmp = first;
		first = first->next;
		//delete tmp;
		DelNode(tmp);
	}
	NodeCount = 0;
	first = NULL;
	last = NULL;
	current = NULL;
}

CObject* CList::GetObject(const string* AObjectName) const
{
	CListNode* ListNode = GetListNode(AObjectName);	
	return ListNode?ListNode->GetData():NULL;
}

CObject* CList::GetObject(const char* AObjectName) const
{
	CListNode* ListNode = GetListNode(&((string)AObjectName));	
	return ListNode?ListNode->GetData():NULL;
}

CObject* CList::GetObject(int AId) const
{
	CListNode* ListNode = GetListNode(AId);	
	return ListNode?ListNode->GetData():NULL;
}

CListNode* CList::GetListNode(const CObject* AObject) const
{
	if (!AObject)
	{
		Log.Log("ERROR", "CList::GetListNode: Trying to find object with NULL adress in %s id: %u", GetName(), GetID());
		return NULL;
	}
	CListNode* TempNode = first;
	while (TempNode)
	{
		if (TempNode->GetData() == AObject)
			return TempNode;
		TempNode = TempNode->next;
	}
	Log.Log("ERROR", "CList::GetListNode: Object named '%s' not found in %s id: %u", AObject->GetName(), this->GetName(), this->GetID());
	return NULL;
}

CListNode* CList::GetListNode(const string* AObjectName) const
{
	if (!AObjectName)
	{
		Log.Log("ERROR", "CList::GetListNode: Trying to find object with NULL name pointer in %s id: %u", GetName(), GetID());
		return NULL;
	}
	CListNode* TempNode = first;
	while (TempNode)
	{		
		// case insensitive due to the fact that win32 filenames are too.
		if (SDL_strcasecmp( TempNode->GetData()->GetName(),AObjectName->c_str()) == 0)  
			return TempNode;
		TempNode = TempNode->next;
	}
	Log.Log("ERROR", "CList::GetListNode: Object named '%s' not found in %s id: %u", AObjectName->c_str(), GetName(), GetID());
	return NULL;
}

CListNode* CList::GetListNode(unsigned int AId) const
{
	CListNode* TempNode = first;
	while (TempNode)
	{
		if (TempNode->GetData()->GetID() == AId)
			return TempNode;
		TempNode = TempNode->next;
	}
	Log.Log("ERROR", "CList::GetListNode: Object with id: %d not found in %s id: %u", AId, GetName(), GetID());
	return NULL;
}

int CList::GetObjectsCount() const
{
	return NodeCount;
}

CListNode* CList::GetFirst()
{
	return first;
}

CListNode* CList::GetLast()
{
	return last;
}

bool CList::Contains(const string &AObjectName) const
{
	CListNode* TempNode = first;
	while (TempNode)
	{
		if (SDL_strcasecmp(TempNode->GetData()->GetName(), AObjectName.c_str()) == 0)
			return true;
		TempNode = TempNode->next;
	}
	return false;
}

bool CList::Call(ObjCall callproc)
{
	CListNode *tmp = first;
	while (tmp)
	{
		if (!(callproc)(tmp->GetData()))
			return false;		
		tmp = tmp->next;
	}
	return true;
}

void CList::Sort(ObjCompCall comp)
{
	if (first == NULL)
		return;
	bool b = true;
	while (b)
	{
		b = false;
		CListNode *tmp = first;
		while (tmp != NULL)
		{
			if (tmp->next != NULL)
			{
				if (!(comp)(tmp->GetData(), tmp->next->GetData()))
				{
					b = true;
					SwapNodes(tmp, tmp->next);
				}
			}
			tmp = tmp->next;
		}
	}
}

void CList::DumpToLog()
{
	CObject *Temp = NULL;
	Reset();
	while (Enum(Temp))
		Log.Log("CObject full list", "\x9Name:\x9\x9%s\x9\x9ID:\x9%d", Temp->GetName(), Temp->GetID());
}

void CList::DelNode(CListNode* AListNode)
{
	if (!AListNode)
		return;
	if (current == AListNode)
		current = AListNode->next;
	if (!AListNode->prev)
		first = AListNode->next;
	else
		AListNode->prev->next = AListNode->next;
	if (!AListNode->next)
		last = AListNode->prev;
	else
		AListNode->next->prev = AListNode->prev;
	CObject * Data = AListNode->GetData();
	if (Data !=	NULL)
		Data->DecListRefCount();
// 	if (Data->GetListRefCount() == 0)
// 		SAFE_DELETE(Data);
	SAFE_DELETE(AListNode);
	NodeCount--;
}

void CList::SwapNodes(CListNode *Node0, CListNode *Node1)
{
	CListNode *lft = Node0->prev;
	Node0->prev = Node1->prev;
	Node1->prev = lft;
	if (Node0->prev != NULL)
		Node0->prev->next = Node0;
	else
		first = Node0;
	if (Node1->prev != NULL)
		Node1->prev->next = Node1;
	else
		first = Node1;
	lft = Node0->next;
	Node0->next = Node1->next;
	Node1->next = lft;
	if (Node0->next != NULL)
		Node0->next->prev = Node0;
	else
		last = Node0;
	if (Node1->next != NULL)
		Node1->next->prev = Node1;
	else
		last = Node1;
}

CListNode* CList::RelativeNext(CListNode* AListNode)
{
	if (!AListNode)
		return first;
	if (!(AListNode->next))
		return AListNode;
	return AListNode->next;
}

CListNode* CList::RelativePrev(CListNode* AListNode)
{
	if (!AListNode)
		return first;
	if (!(AListNode->prev))
		return AListNode;
	return AListNode->prev;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

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
		Log.Log("INFO", "Singletone deleted from memory.");
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


/************************************************************************/
/* CLog                                                                 */
/************************************************************************/

CLog::CLog()
{
	Enabled = true;
	LogMode = LOG_MODE_FILE;	// well, usual default behaviour for user-space (non-system) programs is to log on a console,
					// but for now i left the behaviour, some of us used to... may be changed any time by calling
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

void CLog::Log(const char *Event, const char *Format, ...)
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
	strcpy(TimeString, asctime(&GetLocalTimeAndDate()));
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
			tm TimeStruct = GetLocalTimeAndDate();
			NewLogFileName += "-" + GetFormattedTime(TimeStruct, "%y%m%d-%H%M%S");
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

// CLog global instance
CLog Log;


string GetWorkingDir()
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

tm GetLocalTimeAndDate()
{
	time_t RawTime;
	tm *TimeStruct;
	time(&RawTime);
	TimeStruct = localtime(&RawTime);
	return *TimeStruct;
}

string GetFormattedTime(const tm TimeStruct, const char *Format)
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
	return result;
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
		Log.Log("CAUTION", "Invalid MemCheck call: MemChp1() or MemChp2() has not been called");
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

CObjectStack::CObjectStack() : last(-1)
{

}

bool CObjectStack::Push(CObject* AObject)
{
	if (++last >= MAX_STACK_ELEMENTS_COUNT)
		return false;
	Objects[last] = AObject;
	return true;
}

CObject* CObjectStack::Pop()
{
	if (last >= 0)
		return Objects[last--];
	return NULL;
}

bool CObjectStack::Empty()
{
	return last == -1;
}

CUpdateObject::CUpdateObject() : Active(true)
{
	type |= T_UPDATABLE;
	SetName("CUpdateObject");
	CUpdateManager::Instance()->AddObject(this);
}

CUpdateObject::~CUpdateObject()
{
	CUpdateManager::Instance()->DelObject(GetID());
}

CUpdateManager::CUpdateManager()
{
	SetName("CUpdateManager");
}

bool CUpdateManager::UpdateObjects()
{
	Reset();
	CEngine *engine = CEngine::Instance();
	CUpdateObject *data = dynamic_cast<CUpdateObject*>(Next());
	while (data)
	{
		if (!data->Active)
			continue;
		// FIXED_DELTA_TIME
		float dt = 0;
		CEngine::Instance()->GetState(CEngine::STATE_DELTA_TIME, &dt);
		data->Update(dt); // TODO: подумать что использоваьт: фиксированную дельту или реальную engine->Getdt()
		data = dynamic_cast<CUpdateObject*>(Next());
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
	return loaded = !loaded ? LoadFromFile() : true;
}

CBaseResource::CBaseResource() :loaded(false), filename("")
{

}

CResource::CResource()
{
	SetName("CResource");
}


CGarbageCollector SingletoneKiller;

CGarbageCollector::CGarbageCollector()
{
	SetName("Some CGarbageCollector");
}
