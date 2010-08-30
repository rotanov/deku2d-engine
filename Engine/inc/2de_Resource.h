#ifndef _2DE_RESOURCE_H_
#define _2DE_RESOURCE_H_

#include "2de_Core.h"
#include "2de_Xml.h"


/**
* CResourceSectionLoaderBase - базовый класс загрузчика секций ресурсов для полиморфизма.
*/

class CResourceSectionLoaderBase
{
public:
	CResourceSectionLoaderBase(const string &AName);
	virtual ~CResourceSectionLoaderBase();
	virtual bool Load(CXML *ResourceList) = 0;
	string GetName() const;

protected:
	string Name;

};

/**
* CResourceSectionLoaderBase - шаблонизированный класс загрузчика секций ресурсов.
*/

template<typename T>
class CResourceSectionLoader : public CResourceSectionLoaderBase
{
public:
	CResourceSectionLoader(const string &AName);
	bool Load(CXML *ResourceList);

};

template<typename T>
CResourceSectionLoader<T>::CResourceSectionLoader(const string &AName) : CResourceSectionLoaderBase(AName)
{
}

template<typename T>
bool CResourceSectionLoader<T>::Load(CXML *ResourceList)
{
	CXMLNode *section = ResourceList->Root.First("Resources")->Children.First(Name);
	if (section->IsErroneous())
	{
		Log("WARNING", "Section '%s' has not been found", Name.c_str());
		return false;
	}

	CXMLNode *ResNode = NULL;
	string ResName;

	CFactory *Factory = CFactory::Instance();
	CResource *Resource = NULL;

	for (CXMLNode::ChildrenIterator it = section->Children.Begin(); it != section->Children.End(); ++it)
	{
		ResNode = *it;
		ResName = ResNode->GetAttribute("name");
		if (ResName.empty())
		{
			Log("WARNING", "Name isn't specified for resource in resource list XML");
			continue;
		}

		Resource = Factory->New<T>(ResName);
		if (Resource == NULL)
		{
			Log("WARNING", "Error loading resource: '%s'", ResName.c_str());
			continue;
		}

		Resource->SetFilename(ResNode->Children.First()->GetValue());
	}

	return true;
}

/**
* CDataLister - перечислитель файлов ресурсов.
*/

class CDataLister
{
public:
	CDataLister();
	CXML* List(string ADataRoot, bool ForceReindex = false);
private:
	CXML XML;
	CXMLNode *CurNode;

	void ExploreDirectory(string Path);
	string GetLastPathComponent(string Path);
	string GetFileNameWithoutExtension(string Filename);

	const char *DEFAULT_RESOURCE_LIST_FILE_NAME;
};

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

	string DataPath;
protected:
	CResourceManager();
	friend class CTSingleton<CResourceManager>;
	list<CResourceSectionLoaderBase *> SectionsLoaders;
	CDataLister DataLister;
	CXML *ResourceList;

};

template<typename T>
void CResourceManager::AddSection(const string &SectionName)
{
	SectionsLoaders.push_back(new CResourceSectionLoader<T>(SectionName));
}

#endif // _2DE_RESOURCE_H_

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
