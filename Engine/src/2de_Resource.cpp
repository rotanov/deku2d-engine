#include "2de_Resource.h"

#include "2de_Core.h"
#include "2de_Engine.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_GameUtils.h"
#include "2de_Sound.h"

#ifdef _WIN32
#include "dirent/dirent.h"
#else
#include <dirent.h>
#endif

//////////////////////////////////////////////////////////////////////////
// CFactory

CFactory::CFactory()
{
	SetName("Factory");
	List.SetName("CFactory list");
}

CFactory::~CFactory()
{	
	for(CCommonManager< list <CObject*> >::ManagerIterator it = List.Objects.begin(); it != List.Objects.end(); ++it)
	{
		CObject *obj = *it;
		Log("INFO", "Deleting object %s", obj->GetName());
		CObject::Destroy(obj);
	}
}

//////////////////////////////////////////////////////////////////////////
// CResourceSectionLoaderBase

CResourceSectionLoaderBase::CResourceSectionLoaderBase(const string &AName, CXMLTable *AResourceList) : Name(AName), ResourceList(AResourceList)
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
	if (ResourceList != NULL)
		delete ResourceList;
	
	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		delete *it;
	}
}

bool CResourceManager::LoadResources()
{
	CDataLister DataLister;
	DataLister.List(DataPath);

	ResourceList->LoadFromFile(string(CEngine::Instance()->ConfigFilePath + DEFAULT_RESOURCE_LIST_FILE_NAME).c_str());
	
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

/*CObject* CResourceManager::LoadResource(char* section, char *AResourceName, CreateFunc creator)
{
	if (ResourceList == NULL)
	{
		return false;
	}
	XMLNode x = ResourceList->First->Get(section);
	string val;
	CFactory *Factory = CFactory::Instance();
	CResource *result;
	val = x->Get(AResourceName)->GetValue();

	result = dynamic_cast<CResource*>(Factory->Create(OBJ_USER_DEFINED, creator));
	result->SetName(AResourceName);
	result->Filename = val;
	result->LoadFromFile();

	return result;
}*/

CResourceManager::CResourceManager()
{
	SetName("ResourceManager");
	ResourceList = new CXMLTable;
}

//////////////////////////////////////////////////////////////////////////
// CDataLister

// CXMLTable CDataLister::List(string DataRoot) // @todo: implement operator= in CXMLTable to make it possible to assign tables
void CDataLister::List(string DataRoot)
{
	CurNode = Table.First;
	
	ExploreDirectory(DataRoot);
	Table.SaveToFile(string(CEngine::Instance()->ConfigFilePath + DEFAULT_RESOURCE_LIST_FILE_NAME).c_str());

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

	CurNode->Name = GetLastPathComponent(Path);

	dirent *entry = NULL;

	while (entry = readdir(dirp))
	{
		DIR *testdir = opendir(string(Path + "/" + string(entry->d_name)).c_str());

		//if (entry->d_type == DT_DIR)
		if (testdir)
		{
			if (entry->d_name[0] == '.')
			{
				continue;
			}
			CXMLNode *PreviousNode = CurNode;
			CurNode = CurNode->Add(entry->d_name, entry->d_name);
			ExploreDirectory(Path + "/" + string(entry->d_name));
			CurNode = PreviousNode;
			closedir(testdir);
		}
		//else if (entry->d_type == DT_REG)
		else
		{
			CurNode->Add(GetFileNameWithoutExtension(string(entry->d_name)),
				     Path + "/" + string(entry->d_name));
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
