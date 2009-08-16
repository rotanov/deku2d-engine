#include "resource.h"

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
		Log("INFO", "Factory deleted from memory.");
		_instance = NULL;
	}
}

CFactory::CFactory():initialized(false), UpdateManager(NULL), RenderManager(NULL), FontManager(NULL)
{
	//...
	//this->AddObject(this);
}

CFactory::~CFactory()
{
	CObject *obj;
	Reset();
	while (Enum(obj))
	{
		Log("INFO", "Deleting object %s", obj->name.c_str());
		delete obj;
	}

	FontManager->FreeInst("resource.cpp");
	TextureManager->FreeInst();
}

CObject* CFactory::Create(int ObjectId, CreateFunc creator = NULL)
{
	if (!initialized)
	{
		Log("ERROR", "Trying to create object while factory has not been initialized");
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
		Log("ERROR", "Error, UpdateManager has not been initialized");
		return false;
	}
	if ((RenderManager = vRenderManager) == NULL)
	{
		Log("ERROR", "Error, RenderManager has not been initialized");
			return false;
	}

	FontManager = CFontManager::Instance("resource.cpp");
	TextureManager = CTextureManager::Instance();
	initialized = true;
	return true;
}

CFactory* CFactory::_instance = 0;
int CFactory::_refcount = 0;


CResourceManager::CResourceManager()
{
	DataPath = "";
	ResourceListFileName = NULL;
	ResourceList = NULL;
}

bool CResourceManager::LoadSection(char *SectionName, CreateFunc creator)
{
	if (ResourceList == NULL)
	{
		Log("ERROR", "Trying to load section %s while Resource list has not been loaded", SectionName);
		return false;
	}
	XMLNode x = ResourceList->First->Get(SectionName);
	if (x == NULL)
	{
		Log("ERROR", "Section %s has not been found", SectionName);
		return false;
	}
	string key, val;
	int Result;
	CFactory *Factory = CFactory::Instance(); 
	CResource *Resource;
	while (x->Enum(key, val, Result))
	{
		Resource = dynamic_cast<CResource*>(Factory->Create(OBJ_USER_DEFINED, creator));
		if (Resource == NULL) 
			return false;
		Resource->name = key;
		Resource->filename = val;
	}
	Factory->FreeInst();
	return true;
}

bool CResourceManager::LoadResources()
{
	table.First->Name = "Data";
	cNode = table.First;

	CDirectoryWalk ResLister;
	ResLister.UserExploreFunc = ResourceListerFunc;
	//if rescan directory
		ResLister.List();

	//if save to file
		table.SaveToFile(RESOURCE_LIST_PATH); // пиздец. Заменить константу на нормальную переменную. и вообще. TODO

		if (ResourceList != NULL)
			delete ResourceList;
		ResourceList = new XMLTable;
		//ResourceListFileName = _ResourceListFileName;
		if (!ResourceList->LoadFromFile(ResourceListFileName))
		{
			Log("ERROR", "Error while loading %s Resource list", ResourceListFileName);
			return false;
		}



	for(int i = 0; i < DEFAULT_SECTION_COUNT; i++)
		if (!LoadSection(strSections[i], fncInitializers[i]))
		{
			Log("ERROR","Error loading section %s", strSections[i]);
			return false;
		}
		else
			Log("INFO", "Default section %s loaded", strSections[i]);
	return true;
}

CObject* CResourceManager::LoadResource(char* section, char *name, CreateFunc creator)
{
	if (ResourceList == NULL)
	{
		return false;
	}
	XMLNode x = ResourceList->First->Get(section);
	string val;
	CFactory *Factory = CFactory::Instance();
	CResource *result;
	val = x->Get(name)->GetValue();

	result = dynamic_cast<CResource*>(Factory->Create(OBJ_USER_DEFINED, creator));
	result->name = name;
	result->filename = val;
	result->LoadFromFile();

	Factory->FreeInst();
	return result;
}


bool CResourceManager::ResourceListerFunc(char* AFileName, int AState)
{
	switch(AState & DIRWALK_STATE_RIGHT_MASK)
	{
		case DIRWALK_STATE_FILE:
			char *tmp = new char [MAX_PATH];
			tmp[0] = 0;
			strcat(tmp, fdata.cFileName);
			DelExtFromFName(tmp);
			if (strnicmp(tmp, "thumbs", MAX_PATH) != 0)
				cNode->Add(tmp , (string(CurrDir+MainDirL) + string(fdata.cFileName)).c_str() );
			delete [] tmp;

		break;
		case DIRWALK_STATE_FOLDER:
			_XMLNode *tmp = cNode;
			cNode = cNode->Add(fdata.cFileName, fdata.cFileName);	
			//...
			cNode = tmp;

		break;
	}

	if ((AState & DIRWALK_STATE_LEFT_MASK) == DIRWALK_STATE_HIGHER)
	{
	}
	return true;
}