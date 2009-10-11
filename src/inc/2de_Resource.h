#ifndef _2DE_RESOURCE_H
#define _2DE_RESOURCE_H

#pragma message("Compiling resource.h")

#include "2de_GameUtils.h"
#include "2de_Core.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_Xml.h"

#define CONFIG_FILE_NAME "config/"
#define DEFUALT_RESOURCE_LIST_PATH "config/resources.xml"

#define DEFAULT_SECTION_COUNT	3
#define CR_SECTION_FONTS		"Fonts"
#define CR_SECTION_TEXTURES		"Textures"
#define CR_SECTION_TILESETS		"Tilesets"

static CreateFunc fncInitializers[DEFAULT_SECTION_COUNT] = {CFont::NewFont, CTexture::NewTexture, CTileset::NewTileset};
static char* strSections[DEFAULT_SECTION_COUNT] = {CR_SECTION_FONTS, CR_SECTION_TEXTURES, CR_SECTION_TILESETS};

class CExtResRelation : public CObject
{
public:
	char* Extension;
	CreateFunc Function;
	CExtResRelation(char * AExtension, CreateFunc AFunction):Extension(AExtension), Function(AFunction)
	{
		name = "Extension Resource Relation";
	}
};

static CList ExtResRelationList;

// типы ресурсов
#define CRESOURCE_TYPE_FONT				0x01
#define CRESOURCE_TYPE_IMAGE			0x02
#define	CRESOURCE_TYPE_PARTICLESYSTEM	0x03
#define CRESOURCE_TYPE_ANIMATION		0x04
#define CRESOURCE_TYPE_XML				0x05
#define CRESOURCE_TYPE_USER_DEFINED		0x06



// Типы объектов. Это для CFactory
#define OBJ_SPRITE			0x01
#define OBJ_PSYSTEM			0x02
#define OBJ_FONT_M			0x03
#define	OBJ_USER_DEFINED	0x04
#define OBJ_TEXTURE_RES		0x05
#define OBJ_TILESET_RES		0x06

#define MANAGER_TYPE_REN 0x00
#define MANAGER_TYPE_FNT 0x01
#define MANAGER_TYPE_UPD 0x02
#define MANAGER_TYPE_TEX 0x03

/**
*	Класс CFactory. Назначение классы - контроль создания любых объектов.
*/

class CFactory : public CList
{
public:
	static			CFactory* Instance();
	void			FreeInst();
	CObject*		Create(int ObjectId, CreateFunc creator);
	bool			InitManagers(CUpdateManager *vUpdateManager, CRenderManager *vRenderManager);
	CList*	GetManager(int mantype)
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

#ifdef WIN32

class CDataLister
{
public:
	CXMLTable table;
	CXMLNode *cNode;
	char *MainDir;
	char *CurrDir;
	int MainDirL;
	WIN32_FIND_DATA fdata;

	CDataLister()
	{
		MainDirL = 0;
		MainDir = new char[MAX_PATH];
		CurrDir = new char[MAX_PATH];
	}
	~CDataLister()
	{
		delete [] MainDir;
		delete [] CurrDir;
	}
	void DelLastDirFromPath(char* src);
	bool List();
	void ExploreDir(HANDLE hfile);
};

#endif WIN32

class CResourceManager
{
public:
	string DataPath;
	CXMLTable *ResourceList;


	CResourceManager()
	{
		ResourceList = new CXMLTable;
	}
	~CResourceManager()
	{
		if (ResourceList != NULL)
			delete ResourceList;
	}
	bool		OpenResourceList(char *_ResourceListFileName);
	bool		LoadSection(char *SectionName, CreateFunc creator);
	CObject*	LoadResource(char* section, char *name, CreateFunc creator);
	bool		LoadResources();
private:
};

#endif // _2DE_RESOURCE_H

// конфетка. Читать александреску.
// namespace
// {
// 	// Счетчик значений
// 	template <class C>
// 	class CType2ValCounter
// 	{
// 	protected:
// 		static C Counter;
// 	};
// 
// 	template<class C> C CType2ValCounter<C>::Counter;
// 
// 	// Генератор значений
// 	template<class T, class C>
// 	class CType2ValGenerator : public CType2ValCounter<C>
// 	{
// 	public:
// 		C ID;
// 		CType2ValGenerator()
// 		{
// 			ID = Counter;
// 			++Counter;
// 		}
// 	};
// }; // namespace
// 
// /// Шаблонная функция получения идентификатора типа
// template <class T, class C>
// C Type2Val()
// {
// 	static CType2ValGenerator<T, C> ValueGenerator;
// 
// 	return ValueGenerator.ID;
// }
