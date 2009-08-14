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
		Log("WARNING", "Trying to create object while factory has not been initialized");
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
		Log("WARNING", "Error, UpdateManager has not been initialized");
		return false;
	}
	if ((RenderManager = vRenderManager) == NULL)
	{
		Log("WARNING", "Error, RenderManager has not been initialized");
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
		Log("WARNING", "Trying to load section %s while Resource list has not been loaded", SectionName);
		return false;
	}
	XMLNode x = ResourceList->First->Get(SectionName);
	if (x == NULL)
	{
		Log("WARNING", "Section %s has not been found", SectionName);
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

bool CResourceManager::OpenResourceList(char *_ResourceListFileName)
{
	if (ResourceList != NULL)
		delete ResourceList;
	ResourceList = new XMLTable;
	ResourceListFileName = _ResourceListFileName;
	if (!ResourceList->LoadFromFile(ResourceListFileName))
	{
		Log("WARNING", "Error while loading %s Resource list", ResourceListFileName);
		return false;
	}
	return true;
}

#ifdef WIN32

void CDataLister::DelLastDirFromPath( char* src )
{
	int i = strlen(src)-1;
	while(src[i] == '\\' || src[i] == '/')
		i--;
	while(src[i] != '\\' && src[i] != '/')
		i--;
	src[i+1] = 0;
}

bool CDataLister::List()
{
	HANDLE hfile;
	char TempDir[MAX_PATH];
	TempDir[0] = 0;
	GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
	DeleteFileNameFromEndOfPathToFile(MainDir);
	strcat(TempDir, MainDir);
	MainDirL = strlen(MainDir);
	strcat(MainDir, "data\\");
	SetCurrentDirectory(MainDir);

	table.First->Name = "Data";

	CurrDir[0] = 0;
	strcat(CurrDir, MainDir);
	cNode = table.First;

	Log("----------", "");
	hfile = FindFirstFile("*.*", &fdata);
	ExploreDir(hfile);
	Log("----------", "");

	SetCurrentDirectory(MainDir);

	table.SaveToFile("..\\Config\\Resources.xml");

	SetCurrentDirectory(TempDir);	// Ебано как-то, но таки хуй с ним.

	return 0x0;
}

void CDataLister::ExploreDir( HANDLE hfile )
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

			_XMLNode *tmp = cNode;
			cNode = cNode->Add(fdata.cFileName, fdata.cFileName);

			HANDLE thandle = FindFirstFile("*.*", &fdata);
			ExploreDir(thandle);

			cNode = tmp;

			DelLastDirFromPath(CurrDir);
		}
		else
		{
			//Log("FILE", "%s", fdata.cFileName);
			char *tmp = new char [MAX_PATH];
			tmp[0] = 0;
			strcat(tmp, fdata.cFileName);
			DeleteExtFromFilename(tmp);
			if (strnicmp(tmp, "thumbs", MAX_PATH) != 0)
				cNode->Add(tmp , (string(CurrDir+MainDirL) + string(fdata.cFileName)).c_str() );
			delete [] tmp;
		}
	}
}

#endif WIN32