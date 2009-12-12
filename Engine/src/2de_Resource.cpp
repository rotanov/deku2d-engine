#ifdef _WIN32
#include "dirent/dirent.h"
#else
#include <dirent.h>
#endif

#include "2de_Resource.h"
#include "2de_Core.h"

//-------------------------------------------//
//				Factory stuff				 //
//-------------------------------------------//
CFactory* CFactory::Instance()
{
	if (_instance == NULL)
	{
		_instance = new CFactory;
	}
	_refcount++;
	return _instance;
}

void CFactory::FreeInst()
{
	_refcount--;
	if (!_refcount)
	{		
		delete this;
		Log.Log("INFO", "Factory deleted from memory.");
		_instance = NULL;
	}
}

CFactory::CFactory():initialized(false), UpdateManager(NULL), RenderManager(NULL), FontManager(NULL)
{
	SetName("Factory");
}

CFactory::~CFactory()
{
	CObject *obj;
	Reset();
	while (Enum(obj))
	{
		Log.Log("INFO", "Deleting object %s", obj->GetName());
		delete obj;
	}

}

CObject* CFactory::Create(int ObjectId, CreateFunc creator = NULL)
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
			FontManager->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_SPRITE:
		{
			CSprite* creation = new CSprite;
			RenderManager->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_PSYSTEM:
		{
			CParticleSystem* creation = new CParticleSystem;			
			RenderManager->AddObject(creation);
			UpdateManager->AddObject(creation);
			AddObject(creation);
			tmp = creation;
			break;
		}
	case OBJ_TEXTURE_RES:
		{
			CTexture* creation = new CTexture;
			TextureManager->AddObject(creation);
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
					RenderManager->AddObject(tmp);
					break;
				}
			case T_RENDERABLE | T_COBJECT:
				{
					RenderManager->AddObject(tmp);
					break;
				}
			}
			switch(tmp->type & T_RIGHT_MASK)
			{
			case T_UPDATABLE:
				{
					UpdateManager->AddObject(tmp);
					break;
				}
			}
			break;
		}
	}
	return tmp;
}

bool CFactory::InitManagers( CUpdateManager *vUpdateManager, CRenderManager *vRenderManager )
{
	if ((UpdateManager = vUpdateManager) == NULL)
	{
		Log.Log("WARNING", "Error, UpdateManager has not been initialized");
		return false;
	}
	if ((RenderManager = vRenderManager) == NULL)
	{
		Log.Log("WARNING", "Error, RenderManager has not been initialized");
		return false;
	}

	FontManager = CFontManager::Instance();
	TextureManager = CTextureManager::Instance();
	initialized = true;
	return true;
}

CFactory* CFactory::_instance = 0;
int CFactory::_refcount = 0;



bool CResourceManager::LoadSection(const char *SectionName, CreateFunc creator)
{
	if (ResourceList == NULL)
	{
		Log.Log("WARNING", "Trying to load section %s while Resource list has not been loaded", SectionName);
		return false;
	}
	XMLNode x = ResourceList->First->Get(SectionName);
	if (x == NULL)
	{
		Log.Log("WARNING", "Section %s has not been found", SectionName);
		return false;
	}
	string key, val;
	int Result;
	CFactory *Factory = CFactory::Instance(); 
	CResource *Resource;
	x->ResetEnum( XMLENUM_ATTRSONLY );
	while (x->Enum(key, val, Result))
	{
		Resource = dynamic_cast<CResource*>(Factory->Create(OBJ_USER_DEFINED, creator));
		if (Resource == NULL) 
			return false;
		Resource->SetName(key);
		Resource->filename = val;
	}
	Factory->FreeInst();
	return true;
}

bool CResourceManager::LoadResources()
{
	// TODOODODODOO~!!!!!

	CDataLister DataLister;
	DataLister.List(DataPath);

	ResourceList->LoadFromFile(DEFAULT_RESOURCE_LIST_PATH);
	
	// TODO: see issue #12. Replace load from file by assigning table, returned by List.

	for(int i = 0; i < DEFAULT_SECTION_COUNT; i++)
		if (!LoadSection(strSections[i], fncInitializers[i]))
		{
			Log.Log("ERROR","Error loading section %s", strSections[i]);
			return false;
		}
		else
			Log.Log("INFO", "Default section %s loaded", strSections[i]);
	return true;
}

CObject* CResourceManager::LoadResource(char* section, char *AResourceName, CreateFunc creator)
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

	Factory->FreeInst();
	return result;
}

// CXMLTable CDataLister::List(string DataRoot) // TODO: implement operator= in CXMLTable to make it possible to assign tables
void CDataLister::List(string DataRoot)
{
	WorkDir = GetWorkingDir() + "/";

	CurNode = Table.First;
	
	ExploreDirectory(DataRoot);

	Table.SaveToFile("Config/Resources.xml");

	// return Table;
}

void CDataLister::ExploreDirectory(string Path)
{
	DIR *dirp = opendir(string(WorkDir + Path).c_str());
	if (!dirp)
	{
		Log.Log("ERROR", "Error opening directory '%s', while fetching data list.", Path.c_str());
		return;
	}

	CurNode->Name = GetLastPathComponent(Path);

	dirent *entry = NULL;

	while (entry = readdir(dirp))
	{
		if (entry->d_type == DT_DIR)
		{
			if (entry->d_name[0] == '.')
			{
				continue;
			}
			CXMLNode *PreviousNode = CurNode;
			CurNode = CurNode->Add(entry->d_name, entry->d_name);
			ExploreDirectory(Path + "/" + string(entry->d_name));
			CurNode = PreviousNode;
		}
		else if (entry->d_type == DT_REG)
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
