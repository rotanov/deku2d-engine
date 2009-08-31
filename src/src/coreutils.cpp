#include "CoreUtils.h"


bool Enabled = true;
static int CObjectCount = 0;
CList CObjectManager; // Ultimate!!!!111!!11

CObject::CObject()
{
	CObjectCount++;
	type = ListRefCount = 0;
	char * tmp = new char[16];
	itoa(CObjectCount, tmp, 10);
	name += (string)tmp + " CObject ";
	delete [] tmp;
	id = CObjectCount;
	CObjectManager.AddObject(this);
}

bool CObject::InputHandling( Uint8 state, Uint16 key, SDLMod mod, char letter )
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
		Log("ERROR", "CObject named %s id: %d list reference broken, it is: ", name, id, ListRefCount);
#ifdef CRITICAL_ERRORS_MESSAGE_BOXES
		MessageBox(0, "CObject list reference broken.", "ERROR", MB_OK);
#endif CRITICAL_ERRORS_MESSAGE_BOXES
	}
}
/************************************************************************/
/* CFile                                                                */
/************************************************************************/
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
			Log("ERROR", "Can't open file %s", filename);
		}
}

bool CFile::Open(const char *filename, int mode)
{
	if (filename == NULL)
	{
		Log("ERROR", "Can't open file. Invalid filename");
		return false;
	}
	if (file != NULL)
	{
		Log("ERROR", "Can't open file %s: another file is already opened.", filename);
		return false;
	}
	if (mode == CFILE_READ)
	{
		file = fopen(filename, "rb");
		if (file == NULL)
		{
			Log("ERROR", "Can't open file %s.", filename);
			return false;
		}
		return true;
	}
	if (mode == CFILE_WRITE)
	{
		file = fopen(filename, "wb");
		if ( file == NULL )
		{
			Log("ERROR", "Can't open file %s.", filename);
			return false;
		}
		return true;
	}
	if (mode!=CFILE_READ && mode!=CFILE_WRITE)
	{
		Log("ERROR", "Can't open file %s: invalid mode.", filename);
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
			Log("ERROR", "FILE IO Error. Can't read byte.");
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
			Log("ERROR", "FILE IO Error. Can't write data.");
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
			Log("ERROR", "FILE IO Error. Can't write byte.");
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
			Log("ERROR", "FILE IO Error. Can't write byte.");
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
			Log("ERROR", "FILE IO Error. Can't read data.");
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
	name = "Object list";
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
		Log("ERROR", "Can't delete object named %s from %s id: %d: object not found", AObject->name, name, id);
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
		Log("ERROR", "Can't delete object named %s from %s id: %d: object not found", AObjectName, name, id);
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
		Log("ERROR", "Can't delete object named %s from %s id: %d: object not found", AObjectName, name, id);
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
		Log("ERROR", "Can't delete object id:%d from %s id: %d: object not found", AId, name, id);
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
		delete tmp;
	}
	NodeCount = 0;
	first = NULL;
	last = NULL;
	current = NULL;
}

CObject* CList::GetObject(const string* AObjectName)
{
	CListNode* ListNode = GetListNode(AObjectName);	
	return ListNode?ListNode->GetData():NULL;
}

CObject* CList::GetObject(const char* AObjectName)
{
	CListNode* ListNode = GetListNode(&((string)AObjectName));	
	return ListNode?ListNode->GetData():NULL;
}

CObject* CList::GetObject(int AId)
{
	CListNode* ListNode = GetListNode(AId);	
	return ListNode?ListNode->GetData():NULL;
}

CListNode* CList::GetListNode(const CObject* AObject)
{
	if (!AObject)
	{
		Log("ERROR", "Function CObjectList::GetObjectNodeByPointer; Trying to find object with NULL adress in %s", name);
		return NULL;
	}
	CListNode* TempNode = first;
	while (TempNode)
	{
		if (TempNode->GetData() == AObject)
			return TempNode;
		TempNode = TempNode->next;
	}
	Log("ERROR", "Function CObjectList::GetObjectNodeByPointer; object named %s not found in %s", AObject->name, name);
	return NULL;
}

CListNode* CList::GetListNode(const string* AObjectName)
{
	if (!AObjectName)
	{
		Log("ERROR", "Function CObjectList::GetObjectNodeByObjectName; Trying to find object with NULL name pointer", name);
		return NULL;
	}
	CListNode* TempNode = first;
	while (TempNode)
	{
		if ((TempNode->GetData()->name) == *(AObjectName))
			return TempNode;
		TempNode = TempNode->next;
	}
	Log("ERROR", "Function CObjectList::GetObjectNodeByPointer; object named %s not found in %s", AObjectName, name);
	return NULL;
}

CListNode* CList::GetListNode(int AId)
{
	CListNode* TempNode = first;
	while (TempNode)
	{
		if (TempNode->GetData()->id == AId)
			return TempNode;
		TempNode = TempNode->next;
	}
	Log("ERROR", "Function CObjectList::GetObjectNodeByPointer; object named, id:%d not found in %s", AId, name);
	return NULL;
}

int CList::GetObjectsCount()
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
		Log("CObject full list", "\x9Name:\x9\x9%s\x9\x9ID:\x9%d", Temp->name.c_str(), Temp->id);
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
	AListNode->GetData()->DecListRefCount();
	SAFE_DELETE(AListNode);
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


//----------------------------------//
//			Log Stuff				//
//----------------------------------//
char LogFile[2048];
int __l=0;
char _day[7][4];
char _mon[12][4];

void CreateLogFile(char *fname)
{
	memset(LogFile,0,2048);//(LogFile,2048);
	memcpy(LogFile,fname,strlen(fname));
	FILE *hf = NULL;

#ifdef WIN32
	char *MainDir = new char[MAX_PATH], *LastDir = new char[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, LastDir);
	GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
	DelFNameFromFPath(MainDir);
	SetCurrentDirectory(MainDir);
#endif WIN32 


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

#ifdef WIN32
	SetCurrentDirectory(LastDir);
	delete [] MainDir;
	delete [] LastDir;
#endif

}
void Log(char* Event,char* Format,...)
{
	if (!Enabled)
		return;

#ifdef WIN32
	char *MainDir = new char[MAX_PATH], *LastDir = new char[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, LastDir);
	GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
	DelFNameFromFPath(MainDir);
	SetCurrentDirectory(MainDir);
#endif WIN32 

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

#ifdef WIN32
	SetCurrentDirectory(LastDir);
	delete [] MainDir;
	delete [] LastDir;
#endif

}

void ToggleLog( bool _Enabled )
{
	Enabled = _Enabled;
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

void DelLastDirFromPath( char* src )
{
	int i = strlen(src)-1;
	while(src[i] == '\\' || src[i] == '/')
		i--;
	while(src[i] != '\\' && src[i] != '/')
		i--;
	src[i+1] = 0;
}


#ifdef WIN32

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

void DelInterval(string *src, const int s0, const int s1)
{
	if (s1 < s0)
		return;
	if (s0 < 0 || s1 >= src->length())
		return;
	char *Temp0 = new char [s0+1], *Temp1 = new char [src->length() - s1];
	Temp0[s0] = 0, Temp1[src->length() - s1 - 1] = 0;
	src->copy(Temp0, s0, 0);
	src->copy(Temp1, src->length() - s1 - 1, s1+1);
	*src = (string)Temp0 + Temp1;
}
#ifdef WIN32

bool CDirectoryWalk::List()
{
	HANDLE hfile;
	char TempDir[MAX_PATH];
	TempDir[0] = 0;
	GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
	DelFNameFromFPath(MainDir);
	strcat(TempDir, MainDir);
	MainDirL = strlen(MainDir);
	strcat(MainDir, "data\\");
	SetCurrentDirectory(MainDir);



	CurrDir[0] = 0;
	strcat(CurrDir, MainDir);


//	Log("----------", "");
	hfile = FindFirstFile("*.*", &fdata);
	ExploreDir(hfile);
//	Log("----------", "");

	SetCurrentDirectory(MainDir);


	SetCurrentDirectory(TempDir);	// ����� ���-��, �� ���� ��� � ���.

	return 0x0;
}

void CDirectoryWalk::ExploreDir( HANDLE hfile )
{
	while (FindNextFile(hfile, &fdata))
	{
		if (fdata.cFileName[0] == '.' || fdata.cFileName[0] == '_')
			continue;
		if (fdata.dwFileAttributes == 16)
		{
			//Log("FOLDER", "%s", fdata.cFileName);			
			strcat(CurrDir, (string(fdata.cFileName)+string("\\")).c_str());
			SetCurrentDirectory(CurrDir);
			UserExploreFunc(fdata.cFileName, DIRWALK_STATE_FOLDER);			
			HANDLE thandle = FindFirstFile("*.*", &fdata);
			ExploreDir(thandle);
			UserExploreFunc(fdata.cFileName, DIRWALK_STATE_FILE | DIRWALK_STATE_HIGHER);			
			DelLastDirFromPath(CurrDir);
		}
		else
		{
			//Log("FILE", "%s", fdata.cFileName);
			UserExploreFunc(fdata.cFileName, DIRWALK_STATE_FOLDER);			
		}
	}
}

CDirectoryWalk::CDirectoryWalk()
{
	MainDirL = 0;
	MainDir = new char[MAX_PATH];
	CurrDir = new char[MAX_PATH];
}

CDirectoryWalk::~CDirectoryWalk()
{
	delete [] MainDir;
	delete [] CurrDir;
}
#endif WIN32

#endif

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
	name = "Update Object";
}

CUpdateManager::CUpdateManager()
{
	name += "CUpdateManager";
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
	return loaded = !loaded?LoadFromFile():true;
}

CBaseResource::CBaseResource() :loaded(false), filename("")
{

}

CResource::CResource()
{
	name += "CResource";
}


CGarbageCollector SingletoneKiller;

CGarbageCollector::CGarbageCollector()
{

}
