#ifndef _RES_H
#define _RES_H

#include "CoreUtils.h"
#include "OpenglUtils.h"
#include "xml.h"

// зашитые в код названия файлов и секций, todo: подумать как избаиться от зашития в код
#define CONFIG_FILE_NAME "config/configuration.xml"
#define CRESOURCE_SECTION_FONTS "font"
#define CRESOURCE_SECTION_TEXTURES "textures"

// типы ресурсов
#define CRESOURCE_TYPE_FONT				0x01
#define CRESOURCE_TYPE_IMAGE			0x02
#define	CRESOURCE_TYPE_PARTICLESYSTEM	0x03
#define CRESOURCE_TYPE_ANIMATION		0x04
#define CRESOURCE_TYPE_XML				0x05
#define CRESOURCE_TYPE_USER_DEFINED		0x06

// Ещё типы ресурсов. Какого хрена?!
enum EResType {RT_FONT, RT_IMAGE, RT_PSYSTEM, RT_ANIMATION, RT_XML};

// Типы объектов. Это для CFactory
#define OBJ_SPRITE			0x01
#define OBJ_PSYSTEM			0x02
#define OBJ_FONT			0x03
#define	OBJ_USER_DEFINED	0x04
#define OBJ_TEXTURE_RES		0x05

#define MANAGER_TYPE_REN 0x00
#define MANAGER_TYPE_FNT 0x01
#define MANAGER_TYPE_UPD 0x02
#define MANAGER_TYPE_TEX 0x03

/**
*	Класс CFactory. Назначение классы - контроль создания любых объектов.
*
*
*
*/

class CFactory : public CObjectList
{
public:
	static			CFactory* Instance();
	void			FreeInst();
	CObject*		Create(int ObjectId, CreateFunc creator);
	bool			InitManagers(CUpdateManager *vUpdateManager, CRenderManager *vRenderManager);
	CObjectList*	GetManager(int mantype)
	{
		switch (mantype)
		{
		case MANAGER_TYPE_REN:
			return RenderManager;
			break;
		case MANAGER_TYPE_FNT:
			return FontManager;
			break;
		case MANAGER_TYPE_UPD:
			return UpdateManager;
			break;
		case MANAGER_TYPE_TEX:
			return TextureManager;
			break;
		default:
			return this;
			break;
		}
	}

protected:
	bool initialized;
	CFontManager *FontManager;
	CUpdateManager *UpdateManager;
	CRenderManager *RenderManager;
	CTextureManager *TextureManager;

	CFactory();
	~CFactory();
	static			CFactory* _instance;
	static int		_refcount;
};


class CResourceManager
{
public:
	string DataPath, TexturesFldr, FontsFldr;
	char* ResourceListFileName;
	XMLTable *ResourceList;

	CResourceManager();
	CResourceManager(char *_ResourceListFileName) : ResourceListFileName(_ResourceListFileName)
	{
		ResourceList = new XMLTable;
		if (!ResourceList->LoadFromFile(ResourceListFileName))
		{
			Log("AHTUNG", "Error while loading %s Resource list", ResourceListFileName);
		}
	}
	~CResourceManager()
	{
		if (ResourceList != NULL)
			delete ResourceList;
	}
	bool OpenResourceList(char *_ResourceListFileName);
	bool LoadSection(char *SectionName);
	CObject* LoadResource(char* section, char *name, CreateFunc creator);
	bool LoadResources();
	bool LoadFonts();
	bool LoadTextures();
private:
};

#endif _RES_H