#ifndef _2DE_RESOURCE_H
#define _2DE_RESOURCE_H

#include <list>

#include "2de_GameUtils.h"
#include "2de_Core.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_Xml.h"
#include "2de_Sound.h"

#define DEFAULT_RESOURCE_LIST_FILE_NAME "Resources.xml"

/*#define DEFAULT_SECTION_COUNT	5
#define CR_SECTION_FONTS		"Fonts"
#define CR_SECTION_TEXTURES		"Textures"
#define CR_SECTION_TILESETS		"Tilesets"
#define CR_SECTION_SOUNDS		"Sounds"
#define CR_SECTION_MUSIC		"Music"

static CreateFunc fncInitializers[DEFAULT_SECTION_COUNT] = {CFont::NewFont, CTexture::NewTexture, CTileset::NewTileset, CSound::NewSound, CMusic::NewMusic};
static const char* strSections[DEFAULT_SECTION_COUNT] = {CR_SECTION_FONTS, CR_SECTION_TEXTURES, CR_SECTION_TILESETS, CR_SECTION_SOUNDS, CR_SECTION_MUSIC};*/

/*class CExtResRelation : public CObject
{
public:
	char* Extension;
	CreateFunc Function;
	CExtResRelation(char * AExtension, CreateFunc AFunction):Extension(AExtension), Function(AFunction)
	{
		SetName("Extension Resource Relation");
	}
};*/

//static CList ExtResRelationList;

// типы ресурсов
// #define CRESOURCE_TYPE_FONT			0x01
// #define CRESOURCE_TYPE_IMAGE			0x02
// #define	CRESOURCE_TYPE_PARTICLESYSTEM		0x03
// #define CRESOURCE_TYPE_ANIMATION		0x04
// #define CRESOURCE_TYPE_XML			0x05
// #define CRESOURCE_TYPE_SOUND			0x06
// #define CRESOURCE_TYPE_MUSIC			0x07
// #define CRESOURCE_TYPE_USER_DEFINED		0x08



// Типы объектов. Это для CFactory // И пока не используется // и никогда не будет использоваться, мвахахахаха
enum EObjectType
{
	OBJ_SPRITE,
	OBJ_PSYSTEM,
	OBJ_FONT_M,
	OBJ_USER_DEFINED,
	OBJ_TEXTURE_RES,
	OBJ_TILESET_RES,
	OBJ_SOUND_RES,
	OBJ_MUSIC_RES,
};

/**
*	Класс CFactory. Назначение классы - контроль создания любых объектов.
*/

class CFactory : public CTSingleton<CFactory>
{
public:
	template<typename T>
	T* New(const string &name)
	{
		T* result = new T;
		result->SetName(name);
		List.AddObject(result);

		return result;
	}

	template<typename T>
	T* Get(const string &name)
	{
		T* result = dynamic_cast<T *>(List.GetObject(&name));
		if (!result)
		{
			Log.Log("ERROR", "Factory can't find object named '%s'", name.c_str());
		}

		return result;
	}
	/*CObject* Create(int ObjectId, CreateFunc creator);*/

protected:
	CFactory();
	~CFactory();
	friend class CTSingleton<CFactory>;
	CList List;
};

class CResourceSectionLoaderBase
{
public:
	CResourceSectionLoaderBase(const string &name, CXMLTable *AResourceList);
	virtual bool Load() = 0;
	string GetName() const;

protected:
	string Name;
	CXMLTable *ResourceList;

};

template<typename T>
class CResourceSectionLoader : public CResourceSectionLoaderBase
{
public:
	CResourceSectionLoader(const string &name, CXMLTable *AResourceList);
	bool Load();

};

template<typename T>
CResourceSectionLoader<T>::CResourceSectionLoader(const string &name, CXMLTable *AResourceList) : CResourceSectionLoaderBase(name, AResourceList)
{
}

template<typename T>
bool CResourceSectionLoader<T>::Load()
{
	if (ResourceList == NULL)
	{
		Log.Log("WARNING", "Trying to load section %s while Resource list has not been loaded", Name.c_str());
		return false;
	}

	XMLNode x = ResourceList->First->Get(Name.c_str());
	if (x == NULL)
	{
		Log.Log("WARNING", "Section %s has not been found", Name.c_str());
		return false;
	}

	string key, val;
	int Result;

	CFactory *Factory = CFactory::Instance(); 
	CResource *Resource;

	x->ResetEnum(XMLENUM_ATTRSONLY);
	while (x->Enum(key, val, Result))
	{
		Resource = Factory->New<T>(key);
		if (Resource == NULL)
		{
			Log.Log("ERROR", "Error loading resource: '%s'", key.c_str());
			continue;
		}
		Resource->filename = val;
	}
	return true;

}

class CResourceManager : public CTSingleton<CResourceManager>
{
public:
	~CResourceManager();
	//CObject* LoadResource(char* section, char *name, CreateFunc creator);
	bool LoadResources();
	string DataPath;
protected:
	CResourceManager();
	friend class CTSingleton<CResourceManager>;
	list<CResourceSectionLoaderBase *> SectionsLoaders;
	CXMLTable *ResourceList;

};

class CDataLister
{
public:
	void List(string ADataRoot);
private:
	CXMLTable Table;
	CXMLNode *CurNode;

	void ExploreDirectory(string Path);
	string GetLastPathComponent(string Path);
	string GetFileNameWithoutExtension(string FileName);
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
