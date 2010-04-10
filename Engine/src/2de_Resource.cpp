#include "2de_Resource.h"

#include "2de_Core.h"
#include "2de_Engine.h"

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
}

CFactory::~CFactory()
{
	CObject *obj;
	List.Reset();
	while (List.Enum(obj))
	{
		Log.Log("INFO", "Deleting object %s", obj->GetName());
		delete obj;
	}
}

/*CObject* CFactory::Create(int ObjectId, CreateFunc creator = NULL)
{
	if (!initialized)
	{
		Log.Log("WARNING", "Trying to create object while factory has not been initialized");
		return NULL;
	}

	CObject *tmp = NULL;
	switch(ObjectId)
	{	
	case OBJ_FONT_M:
		{
			CFont *creation = new CFont;
			CFontManager::Instance()->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_SPRITE:
		{
			CSprite* creation = new CSprite;
			CRenderManager::Instance()->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_PSYSTEM:
		{
			CParticleSystem* creation = new CParticleSystem;			
			CRenderManager::Instance()->AddObject(creation);
			CUpdateManager::Instance()->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_TEXTURE_RES:
		{
			CTexture* creation = new CTexture;
			CTextureManager::Instance()->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_SOUND_RES:
		{
			CSound* creation = new CSound;
			CSoundManager::Instance()->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_MUSIC_RES:
		{
			CMusic* creation = new CMusic;
			CMusicManager::Instance()->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_USER_DEFINED:
		{
			tmp = creator();
			AddObject(tmp);
			switch(tmp->type & T_LEFT_MASK)
			{
			case T_COBJECT:
				{
					break;
				}
			case T_RENDERABLE:
				{
					CRenderManager::Instance()->AddObject(tmp);
					break;
				}
			case T_RENDERABLE | T_COBJECT:
				{
					CRenderManager::Instance()->AddObject(tmp);
					break;
				}
			}
			switch(tmp->type & T_RIGHT_MASK)
			{
			case T_UPDATABLE:
				{
					CUpdateManager::Instance()->AddObject(tmp);
					break;
				}
			}
			break;
		}
	}
	return tmp;
}*/


//////////////////////////////////////////////////////////////////////////
// CResourceSectionLoaderBase

CResourceSectionLoaderBase::CResourceSectionLoaderBase(const string &name, CXMLTable *AResourceList) : Name(name), ResourceList(AResourceList)
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
	
	// TODO: see issue #12. Replace load from file by assigning table, returned by List.

	for (list<CResourceSectionLoaderBase *>::iterator it = SectionsLoaders.begin(); it != SectionsLoaders.end(); ++it)
	{
		if ((*it)->Load())
		{
			Log.Log("INFO", "Default section %s loaded", (*it)->GetName().c_str()); 
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
	result->filename = val;
	result->LoadFromFile();

	return result;
}*/

CResourceManager::CResourceManager()
{
	SetName("ResourceManager");
	ResourceList = new CXMLTable;
	SectionsLoaders.push_back(new CResourceSectionLoader<CFont>("Fonts", ResourceList));
	SectionsLoaders.push_back(new CResourceSectionLoader<CTexture>("Textures", ResourceList));
	SectionsLoaders.push_back(new CResourceSectionLoader<CTileset>("Tilesets", ResourceList));
	SectionsLoaders.push_back(new CResourceSectionLoader<CSound>("Sounds", ResourceList));
	SectionsLoaders.push_back(new CResourceSectionLoader<CMusic>("Music", ResourceList));
}

//////////////////////////////////////////////////////////////////////////
// CDataLister

// CXMLTable CDataLister::List(string DataRoot) // TODO: implement operator= in CXMLTable to make it possible to assign tables
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
		Log.Log("ERROR", "Error opening directory '%s', while fetching data list.", Path.c_str());
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

string CDataLister::GetFileNameWithoutExtension(string FileName)
{
	int last_dot = FileName.find_last_of('.');

	if (last_dot == string::npos)
	{
		return FileName;
	}

	FileName.erase(last_dot);

	return FileName;
}
