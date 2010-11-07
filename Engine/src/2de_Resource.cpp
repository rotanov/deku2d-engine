#include "2de_Resource.h"

#include "2de_Core.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CResourceSectionLoaderBase

CResourceSectionLoaderBase::CResourceSectionLoaderBase(const string &AName) : Name(AName)
{
}

CResourceSectionLoaderBase::~CResourceSectionLoaderBase()
{
}

string CResourceSectionLoaderBase::GetName() const
{
	return Name;
}

//////////////////////////////////////////////////////////////////////////
// CResourceRefCounterState

void CResourceRefCounterState::DisableRC()
{
	RCDisabled = true;
}

bool CResourceRefCounterState::IsRCDisabled()
{
	return RCDisabled;
}

bool CResourceRefCounterState::RCDisabled = false;

//////////////////////////////////////////////////////////////////////////
// CResource

CResource::CResource() : MemoryLoadData(NULL), MemoryLoadLength(0), Loaded(false), FirstTimeLoaded(false), Persistent(false), RefCount(0)
{
	SetName("CResource");
}

CResource::~CResource()
{
}

string CResource::GetFilename() const
{
	return Filename;
}

CResource::ELoadSource CResource::GetLoadSource() const
{
	return Source;
}

void CResource::SetLoadSource(const string &AFilename)
{
	Source = LOAD_SOURCE_FILE;
	Filename = AFilename;
	FirstTimeLoaded = false;
}

void CResource::SetLoadSource(void *AData, size_t ALength)
{
	Source = LOAD_SOURCE_MEMORY;
	MemoryLoadData = AData;
	MemoryLoadLength = ALength;
	FirstTimeLoaded = false;
}

bool CResource::Load()
{
	if (Loaded)
		return true;

	Log("INFO", "Resources: LOAD - %s", GetName().c_str());
	return (Loaded = FirstTimeLoaded = true);
}

void CResource::Unload()
{
	if (!Loaded)
		return;

	Log("INFO", "Resources: UNLOAD - %s", GetName().c_str());
	Loaded = false;
}

bool CResource::CheckLoad()
{
	return Loaded = !Loaded ? Load() : true;
}

bool CResource::isPersistent() const
{
	return Persistent;
}

void CResource::SetPersistent(bool APersistent)
{
	Persistent = APersistent;
}

//////////////////////////////////////////////////////////////////////////
// CDataLister

CDataLister::CDataLister() : DEFAULT_RESOURCE_LIST_FILE_NAME("Resources.xml")
{
}

CXML* CDataLister::List(string DataRoot, bool ForceReindex /*= false*/)
{
	string ResourcesListFile = CEnvironment::Paths::GetConfigPath() + DEFAULT_RESOURCE_LIST_FILE_NAME;

	if (!ForceReindex && CFileSystem::Exists(ResourcesListFile))
	{
		XML.LoadFromFile(ResourcesListFile);
		if (!XML.Root.First("Resources")->IsErroneous())
		{
			Log("INFO", "Used cached resources list file: '%s'", ResourcesListFile.c_str());
			return &XML;
		}
		else
			Log("WARNING", "Cached resources list file '%s' is invalid. Trying to regenerate it...", ResourcesListFile.c_str());
	}

	XML.Root.DeleteAll();

	XML.Root.AddLast(new CXMLPrologNode);
	CXMLNode *ResourcesRoot = new CXMLNormalNode("Resources");
	XML.Root.AddLast(ResourcesRoot);

	CurNode = ResourcesRoot;
	
	ExploreDirectory(DataRoot);
	XML.SaveToFile(ResourcesListFile);

	Log("INFO", "Resources list generated and cached to '%s'", ResourcesListFile.c_str());

	return &XML;
}

void CDataLister::ExploreDirectory(string Path)
{
	DIR *dirp = opendir(Path.c_str());
	if (!dirp)
	{
		Log("ERROR", "Error opening directory '%s', while fetching data list.", Path.c_str());
		return;
	}


	string CurrentPath;
	dirent *entry = NULL;
	DIR *testdir = NULL;

	while ((entry = readdir(dirp)))
	{
		CurrentPath = Path + "/" + string(entry->d_name);
		testdir = opendir(CurrentPath.c_str());

		if (testdir)
		{
			closedir(testdir);

			if (entry->d_name[0] == '.')
			{
				continue;
			}

			CXMLNode *PreviousNode = CurNode;
			CurNode = new CXMLNormalNode(entry->d_name);
			ExploreDirectory(CurrentPath);
			PreviousNode->Children.AddLast(CurNode);
			CurNode = PreviousNode;
		}
		else
		{
			CXMLNode *ResNode = new CXMLNormalNode("Resource");
			ResNode->SetAttribute("name", GetFileNameWithoutExtension(entry->d_name));
			ResNode->Children.AddLast(new CXMLTextNode(CurrentPath));
			CurNode->Children.AddLast(ResNode);
		}

	}
	closedir(dirp);
}

string CDataLister::GetLastPathComponent(string Path)
{
	if (Path.empty() || ((Path.length() == 1) && (Path[0] == '/')))
	{
		return "";
	}

	if (Path.at(Path.length() - 1) == '/')
	{
		Path.erase(Path.length() - 1);
	}

	int last_slash = Path.find_last_of('/');

	if (last_slash == string::npos)
	{
		last_slash = 0;
	}
	else
	{
		last_slash++;
	}

	return Path.substr(last_slash);
}

string CDataLister::GetFileNameWithoutExtension(string Filename)
{
	int last_dot = Filename.find_last_of('.');

	if (last_dot == string::npos)
	{
		return Filename;
	}

	Filename.erase(last_dot);

	return Filename;
}

//////////////////////////////////////////////////////////////////////////
// CResourceManager

bool CResourceManager::LoadResources()
{
	ResourceList = DataLister.List(DataPath, CConfig::Instance()->Section("Data")["ForceReindex"]);

	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		if ((*it)->Load(ResourceList))
		{
			Log("INFO", "Resources section '%s' loaded", (*it)->GetName().c_str());
		}
	}
	return true;
}

void CResourceManager::AddToUnloadQueue(CResource *AResource)
{
	UnloadQueue.insert(AResource);
}

void CResourceManager::RemoveFromUnloadQueue(CResource *AResource)
{
	UnloadQueue.erase(AResource);
}

void CResourceManager::PerformUnload()
{
	if (UnloadQueue.empty())
		return;

	if (SinceLastUnload < AutoUnloadInterval)
		return;

	for (set<CResource *>::iterator it = UnloadQueue.begin(); it != UnloadQueue.end(); ++it)
	{
		(*it)->Unload(); 
	}

	UnloadQueue.clear();

	SinceLastUnload = 0.0f;
	Log("INFO", "Resource manager cleared auto-unload queue");
}

float CResourceManager::GetAutoUnloadInterval() const
{
	return AutoUnloadInterval;
}

void CResourceManager::SetAutoUnloadInterval(float AAutoUnloadInterval)
{
	AutoUnloadInterval = AAutoUnloadInterval;
}

CResourceManager::CResourceManager() : SinceLastUnload(0.0f), AutoUnloadInterval(15.0f)
{
	SetName("ResourceManager");
	CEventManager::Instance()->Subscribe("EveryFrame", this);
}

CResourceManager::~CResourceManager()
{
	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		delete *it;
	}
}

void CResourceManager::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "EveryFrame")
	{
		SinceLastUnload += CEngine::Instance()->GetDeltaTime();
	}
}
