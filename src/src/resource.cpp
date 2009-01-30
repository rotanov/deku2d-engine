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
	case OBJ_FONT:
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
		Log("AHTUNG", "Error, UpdateManager has not been initialized");
		return false;
	}
	if ((RenderManager = vRenderManager) == NULL)
	{
		Log("AHTUNG", "Error, RenderManager has not been initialized");
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
	TexturesFldr = "";
	FontsFldr = "";
	ResourceListFileName = NULL;
	ResourceList = NULL;
}

bool CResourceManager::LoadSection(char *SectionName)
{

	return true;
}

bool CResourceManager::LoadResources()
{
	if (!LoadTextures())
	{
		Log("ERROR","Error loading textures");
		return false;
	}
	if (!LoadFonts())
	{
		Log("ERROR", "ERROR loading fonts");
		return false;
	}
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

	result = dynamic_cast<CResource*>(Factory->Create(OBJ_USER_DEFINED, creator)); // May be not safe, be careful here: creator sholud return class derived from CResource
	result->name = name;
	result->filename = DataPath + val;
	result->LoadFromFile();

	Factory->FreeInst();
	return result;
}

bool CResourceManager::LoadFonts()
{
	if (ResourceList == NULL)
	{
		Log("WARNING", "Trying to load fonts while Resource list has not been loaded");
		return false;
	}
	XMLNode x = ResourceList->First->Get(CRESOURCE_SECTION_FONTS);
	if (x == NULL)
		return false;
	string key, val;
	int Result;
	CFactory *Factory = CFactory::Instance();
	CFont *Font;
	while (x->Enum(key, val, Result))
	{
		Font = (CFont*)Factory->Create(OBJ_FONT, NULL);
		if (Font == NULL)
			return false;
		Font->name = key;
		Font->LoadFromFile((char*)(DataPath+FontsFldr+val).data());
	}
	Factory->FreeInst();
	return true;
}

bool CResourceManager::LoadTextures()
{
	if (ResourceList == NULL)
	{
		Log("WARNING", "Trying to load textures while Resource list has not been loaded");
		return false;
	}
	XMLNode x = ResourceList->First->Get(CRESOURCE_SECTION_TEXTURES);
	if (x == NULL)
		return false;
	string key, val;
	int Result;
	CFactory *Factory = CFactory::Instance(); 
	CTexture *TextureRes;
	while (x->Enum(key, val, Result))
	{
		TextureRes = dynamic_cast<CTexture*>(Factory->Create(OBJ_TEXTURE_RES, CTexture::NewTextureRes));
		if (TextureRes == NULL) 
			return false;
		TextureRes->name = key;
		TextureRes->filename = DataPath + TexturesFldr + val;
	}
	Factory->FreeInst();
	return true;
};

bool CResourceManager::OpenResourceList(char *_ResourceListFileName)
{
	if (ResourceList != NULL)
		delete ResourceList;
	ResourceList = new XMLTable;
	ResourceListFileName = _ResourceListFileName;
	if (!ResourceList->LoadFromFile(ResourceListFileName))
	{
		Log("AHTUNG", "Error while loading %s Resource list", ResourceListFileName);
		return false;
	}
	return true;
}