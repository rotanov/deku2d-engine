#ifndef _2DE_RESOURCE_H_
#define _2DE_RESOURCE_H_

#include "2de_Core.h"
#include "2de_Factory.h"
#include "2de_Update.h"
#include "2de_Xml.h"

/**
* CResourceSectionLoaderBase - base class of resources sections loader for polymorphism.
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
* CResourceSectionLoader - class template of resources section loader.
*/

template<typename T>
class CResourceSectionLoader : public CResourceSectionLoaderBase
{
public:
	CResourceSectionLoader(const string &AName);
	bool Load(CXML *ResourceList);

};

/**
* CResourceRefCounterState - class that stores state of reference counting in its static members.
*/

class CResourceRefCounterState
{
public:
	static void DisableRC();
	static bool IsRCDisabled();

private:
	static bool RCDisabled;

};

/**
* CResourceRefCounter - resource reference counter class template, used to automate process of loading and unloading of resources.
*/

template<typename T>
class CResourceRefCounter
{
public:
	CResourceRefCounter(T *Source = NULL);
	CResourceRefCounter(const CResourceRefCounter &Source);
	~CResourceRefCounter();

	void Assign(T *Source);

	CResourceRefCounter& operator=(const CResourceRefCounter &Source);
	CResourceRefCounter& operator=(T *Source);

	T* operator->() const;
	T& operator*() const;
	operator T*() const;
	bool operator==(T *Operand) const;
	bool operator!=(T *Operand) const;

private:
	T *Pointer;

};

/**
* CResource - base class for resources.
*/

class CResource : public virtual CObject
{
public:
	enum ELoadSource
	{
		LOAD_SOURCE_UNSPECIFIED,
		LOAD_SOURCE_FILE,
		LOAD_SOURCE_MEMORY,
	};

	CResource();
	virtual ~CResource();

	string GetFilename() const;

	ELoadSource GetLoadSource() const;
	void SetLoadSource(const string &AFilename);
	void SetLoadSource(byte *AData, size_t ALength);

	virtual bool Load();
	virtual void Unload();

	// make CSaveableResource
	virtual bool SaveToFile(const string &AFilename)
	{
		throw std::logic_error("Unimplemented for this type of resource");
		return false;
	}

	bool CheckLoad();

	bool isPersistent() const;
	void SetPersistent(bool APersistent);

	int RefCount;

protected:
	ELoadSource Source;
	string Filename;
	byte *MemoryLoadData;
	size_t MemoryLoadLength;

	bool Loaded;
	bool FirstTimeLoaded;
	bool Persistent;

};

/**
* CDataLister - resources files lister.
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
* CResourceManager - resources manager.
*/

class CResourceManager : public CTSingleton<CResourceManager>
{
public:
	bool LoadResources();

	template<typename T>
	void AddSection(const string &SectionName);

	void AddToUnloadQueue(CResource *AResource);
	void RemoveFromUnloadQueue(CResource *AResource);

	void PerformUnload();

	void ProcessEvent(const CEvent &AEvent);

	float GetAutoUnloadInterval() const;
	void SetAutoUnloadInterval(float AAutoUnloadInterval);

	string GetDataPath() const;
	void SetDataPath(const string &ADataPath);

protected:
	CResourceManager();
	~CResourceManager();
	friend class CTSingleton<CResourceManager>;

private:
	list<CResourceSectionLoaderBase *> SectionsLoaders;
	set<CResource *> UnloadQueue;
	CDataLister DataLister;
	CXML *ResourceList;
	string DataPath;

	float SinceLastUnload;
	float AutoUnloadInterval;
};


//////////////////////////////////////////////////////////////////////////
// Templates implementations

//////////////////////////////////////////////////////////////////////////
// CResourceSectionLoader

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

		Resource->SetLoadSource(ResNode->Children.First()->GetValue());
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CResourceManager

template<typename T>
void CResourceManager::AddSection(const string &SectionName)
{
	SectionsLoaders.push_back(new CResourceSectionLoader<T>(SectionName));
}

//////////////////////////////////////////////////////////////////////////
// CResourceRefCounter

template<typename T>
CResourceRefCounter<T>::CResourceRefCounter(T *Source /*= NULL*/) : Pointer(NULL)
{
	Assign(Source);
}

template<typename T>
CResourceRefCounter<T>::CResourceRefCounter(const CResourceRefCounter<T> &Source) : Pointer(NULL)
{
	Assign(Source.Pointer);
}

template<typename T>
CResourceRefCounter<T>::~CResourceRefCounter()
{
	Assign(NULL);
}

template<typename T>
void CResourceRefCounter<T>::Assign(T *Source)
{
	if (CResourceRefCounterState::IsRCDisabled())
		return;

	if (Pointer != NULL)
	{
		Pointer->RefCount--;
		if (!Pointer->isPersistent() && Pointer->RefCount == 0)
		{
			CResourceManager::Instance()->AddToUnloadQueue(Pointer);
		}
	}

	Pointer = Source;

	if (Pointer != NULL)
	{
		if (Pointer->RefCount == 0)
		{
			CResourceManager::Instance()->RemoveFromUnloadQueue(Pointer);
		}
		Pointer->RefCount++;
	}
}

template<typename T>
CResourceRefCounter<T>& CResourceRefCounter<T>::operator=(const CResourceRefCounter &Source)
{
	if (this == &Source)
		return *this;

	Assign(Source.Pointer);

	return *this;
}

template<typename T>
CResourceRefCounter<T>& CResourceRefCounter<T>::operator=(T *Source)
{
	Assign(Source);
	return *this;
}

template<typename T>
T* CResourceRefCounter<T>::operator->() const
{
	return Pointer;
}

template<typename T>
T& CResourceRefCounter<T>::operator*() const
{
	return *Pointer;
}

template<typename T>
CResourceRefCounter<T>::operator T*() const
{
	return Pointer;
}

template<typename T>
bool CResourceRefCounter<T>::operator==(T *Operand) const
{
	return (Pointer == Operand);
}

template<typename T>
bool CResourceRefCounter<T>::operator!=(T *Operand) const
{
	return (Pointer != Operand);
}

#endif // _2DE_RESOURCE_H_
