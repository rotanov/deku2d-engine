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
}

bool CResourceManager::LoadResources(char *ResourceList)
{
	XMLTable XMLResourceList;
	if (!XMLResourceList.LoadFromFile(ResourceList))
	{
		Log("AHTUNG", "Can't load XMLTable %s", ResourceList);
		return false;
	}

	XMLNode x = XMLResourceList.First->Get(CRESOURCE_SECTION_FONTS);
	string key, val;
	int Result;
	CFactory *Factory = CFactory::Instance();
	CFont *Font; 

	while (x->Enum(key, val, Result))
	{
		Font = (CFont*)Factory->Create(OBJ_FONT, NULL);
		Font->name = key;
		Font->LoadFromFile((char*)(DataPath+FontsFldr+val).data());
	}

	x->ResetEnum(0);
	Factory->FreeInst();
	return true;
}

CObject* CResourceManager::LoadResource(char *ResourceList, char* section, char *name, CreateFunc creator)
{
	XMLTable XMLResourceList;
	if (!XMLResourceList.LoadFromFile(ResourceList))
	{
		Log("AHTUNG", "Can't load XMLTable %s", ResourceList);
		return false;
	}

	XMLNode x = XMLResourceList.First->Get(section);
	string val;
	CFactory *Factory = CFactory::Instance();
	CResource *result;
	val = x->Get(name)->GetValue();

	result = static_cast<CResource*>(Factory->Create(OBJ_USER_DEFINED, creator)); // May be not safe, be careful here: creator sholud return class derived from CResource
	result->name = name;
	result->filename = (char*)((DataPath+val).data());
	result->LoadFromFile();

	Factory->FreeInst();
}

bool CResourceManager::LoadTextures(char *ResourceList)
{
	XMLTable XMLResourceList;
	if (!XMLResourceList.LoadFromFile(ResourceList))
	{
		Log("AHTUNG", "Can't load XMLTable %s", ResourceList);
		return false;
	}

	XMLNode x = XMLResourceList.First->Get(CRESOURCE_SECTION_TEXTURES);
	string key, val;
	int Result;
	CFactory *Factory = CFactory::Instance(); 

	CTexture * TextureRes;
	while (x->Enum(key, val, Result))
	{
		TextureRes = (CTexture*)Factory->Create(OBJ_TEXTURE_RES, CTexture::NewTextureRes);
		TextureRes->name = key;
		TextureRes->filename = DataPath + TexturesFldr + val;
	}
	
	x->ResetEnum(0);
	Factory->FreeInst();
	return true;
};
