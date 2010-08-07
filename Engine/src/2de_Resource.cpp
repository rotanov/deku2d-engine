#include "2de_Resource.h"

#include "2de_Core.h"
#include "2de_Engine.h"

#ifdef _WIN32
#include "dirent/dirent.h"
#else
#include <dirent.h>
#endif // _WIN32

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
// CDataLister

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

	while (entry = readdir(dirp))
	{
		CurrentPath = Path + "/" + string(entry->d_name);
		testdir = opendir(CurrentPath.c_str());

		//if (entry->d_type == DT_DIR)
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
		//else if (entry->d_type == DT_REG)
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

CResourceManager::~CResourceManager()
{
	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		delete *it;
	}
}

bool CResourceManager::LoadResources()
{
	ResourceList = DataLister.List(DataPath);

	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		if ((*it)->Load(ResourceList))
		{
			Log("INFO", "Resources section '%s' loaded", (*it)->GetName().c_str()); 
		}
	}
	return true;
}

CResourceManager::CResourceManager()
{
	SetName("ResourceManager");
}

