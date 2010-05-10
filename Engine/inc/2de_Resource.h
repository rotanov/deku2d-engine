#ifndef _2DE_RESOURCE_H
#define _2DE_RESOURCE_H

#include <list>
#include <stdexcept>

#include "2de_Core.h"
#include "2de_Xml.h"

#define DEFAULT_RESOURCE_LIST_FILE_NAME "Resources.xml"

/**
*	Класс CFactory. Назначение классы - контроль создания любых объектов.
*/

class CFactory : public CTSingleton<CFactory>
{
public:
	template<typename T>
	T* New(const string &AName);

	template<typename T>
	T* Get(const string &AName) const;

protected:
	CFactory();
	~CFactory();
	friend class CTSingleton<CFactory>;
	CCommonManager<CObject> List;
};

template<typename T>
T* CFactory::New(const string &AName)
{
	//if (List.Contains(AName)) TODO uncomment and fix
	{
//		throw std::logic_error("Object with this name already exists.");
	}

	T* result = new T;
	result->SetName(AName);
	List.AddObject(result);

	return result;
}

template<typename T>
T* CFactory::Get(const string &AName) const
{
	T* result = dynamic_cast<T *>(List.GetObject(AName));
	if (!result)
	{
		Log("ERROR", "Factory can't find object named '%s'", AName.c_str());
	}

	return result;
}

/**
 * CResourceSectionLoaderBase - базовый класс загрузчика секций ресурсов для полиморфизма.
 */

class CResourceSectionLoaderBase
{
public:
	CResourceSectionLoaderBase(const string &AName, CXMLTable *AResourceList);
	virtual ~CResourceSectionLoaderBase();
	virtual bool Load() = 0;
	string GetName() const;

protected:
	string Name;
	CXMLTable *ResourceList;

};

/**
 * CResourceSectionLoaderBase - шаблонизированный класс загрузчика секций ресурсов.
 */

template<typename T>
class CResourceSectionLoader : public CResourceSectionLoaderBase
{
public:
	CResourceSectionLoader(const string &AName, CXMLTable *AResourceList);
	bool Load();

};

template<typename T>
CResourceSectionLoader<T>::CResourceSectionLoader(const string &AName, CXMLTable *AResourceList) : CResourceSectionLoaderBase(AName, AResourceList)
{
}

template<typename T>
bool CResourceSectionLoader<T>::Load()
{
	if (ResourceList == NULL)
	{
		Log("ERROR", "Trying to load section '%s' while Resource list has not been loaded", Name.c_str());
		return false;
	}

	XMLNode x = ResourceList->First->Get(Name.c_str());
	if (x == NULL)
	{
		Log("WARNING", "Section '%s' has not been found", Name.c_str());
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
			Log("WARNING", "Error loading resource: '%s'", key.c_str());
			continue;
		}
		Resource->Filename = val;
	}
	return true;

}

/**
 * CResourceManager - менеджер ресурсов.
 */

class CResourceManager : public CTSingleton<CResourceManager>
{
public:
	~CResourceManager();
	bool LoadResources();

	template<typename T>
	void AddSection(const string &SectionName);

	//CObject* LoadResource(char* section, char *name, CreateFunc creator);
	string DataPath;
protected:
	CResourceManager();
	friend class CTSingleton<CResourceManager>;
	list<CResourceSectionLoaderBase *> SectionsLoaders;
	CXMLTable *ResourceList;

};

template<typename T>
void CResourceManager::AddSection(const string &SectionName)
{
	SectionsLoaders.push_back(new CResourceSectionLoader<T>(SectionName, ResourceList));
}

/**
 * CDataLister - перечислитель файлов ресурсов.
 */

class CDataLister
{
public:
	void List(string ADataRoot);
private:
	CXMLTable Table;
	CXMLNode *CurNode;

	void ExploreDirectory(string Path);
	string GetLastPathComponent(string Path);
	string GetFileNameWithoutExtension(string Filename);
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
