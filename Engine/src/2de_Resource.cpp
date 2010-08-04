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

CResourceSectionLoaderBase::CResourceSectionLoaderBase(const string &AName, CXML &AResourceList) : Name(AName), ResourceList(AResourceList)
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
	CDataLister DataLister;
	DataLister.List(DataPath);

	ResourceList.LoadFromFile(string(CEngine::Instance()->ConfigFilePath + DEFAULT_RESOURCE_LIST_FILE_NAME).c_str());
	
	// @todo: see issue #12. Replace load from file by assigning table, returned by List.

	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		if ((*it)->Load())
		{
			Log("INFO", "Section '%s' loaded", (*it)->GetName().c_str()); 
		}
	}
	return true;
}

CResourceManager::CResourceManager()
{
	SetName("ResourceManager");
}

//////////////////////////////////////////////////////////////////////////
// CDataLister

// CXMLTable CDataLister::List(string DataRoot) // @todo: implement operator= in CXMLTable to make it possible to assign tables
void CDataLister::List(string DataRoot)
{
	XML.Root.DeleteAll();

	XML.Root.AddLast(new CXMLPrologNode);
	CXMLNode *ResourcesRoot = new CXMLNormalNode("Resources");
	XML.Root.AddLast(ResourcesRoot);

	CurNode = ResourcesRoot;
	
	ExploreDirectory(DataRoot);
	XML.SaveToFile(string(CEngine::Instance()->ConfigFilePath + DEFAULT_RESOURCE_LIST_FILE_NAME).c_str());

	// return Table;
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
