#ifndef _2DE_FACTORY_H_
#define _2DE_FACTORY_H_

#include "2de_Core.h"
#include "2de_Xml.h"
#include "2de_Log.h"

namespace Deku2D
{
	class CGameObject;

	/**
	* CFactory - oversees creation of any objects. The real purpose at this moment: managing memory at creation and destuction of each object and getting pointer by name.
	*/

	class CFactory : public CObject
	{
	public:
		typedef CObject* (CFactory::*TNewFunction)(const string &);
		typedef map<string, TNewFunction> ComponentsContainer;
		typedef map<string, int> UsedPrototypesContainer;

		template<typename T>
		T* New(const string &AName);

		template<typename T>
		T* New();

		CGameObject* CreateGameObject(const string &AClassName, const string &AName = "");

		CGameObject* CreateComponent(const string &AClassName, const string &AName = "");

		template<typename T>
		T* CreateComponent(const string &AName = "");

		CGameObject* InstantiatePrototype(const string &AProtoName, const string &AName = "");

		//CObject* CreateByName(const string &AClassName, const string &AName, UsedPrototypesContainer *UsedPrototypes = NULL);

		template<typename T>
		bool Add(T *AObject, const string &AName = "");

		template<typename T>
		T* Get(const string &AName, bool AMustExist = true);

		template<typename T>
		T* Remove(const string &AName);

		void Rename(const string &AName, const string &ANewName);
		void Destroy(CObject *AObject);
		void CleanUp();
		void DestroyAll();

		bool IsComponentExists(const string &AClassName);

	protected:
		typedef map<string, CObject *> ObjectsContainer;
		typedef ObjectsContainer::iterator ObjectsIterator;

		CFactory();
		~CFactory();
		friend class CTSingleton<CFactory>;

		void AddComponent(const string &AClassName, TNewFunction ANewFunctionPointer);

		void TraversePrototypeNode(CXMLNode *ANode, CGameObject *AObject, CGameObject *CurrentProto);

		CXMLNode* GetPrototypeXML(const string &AName);
		CGameObject* TryUseCachedPrototype(const string &AClassName, const string &AName);

		void IncreaseCreationLevel(CGameObject *AObject);
		void DecreaseCreationLevel();

	#if defined(_DEBUG) && !defined(DISABLE_DEBUG_BOXES)
		void InsertDebugInfo( CObject* Source );
	#endif

		template<typename T>
		CObject* InternalNew(const string &AName);

		ComponentsContainer Components;
		ObjectsContainer Objects;
		queue<CObject *> Deletion;
		map<string, CGameObject *> CachedProtos;
		list<CGameObject *> CreationQueue;
		int CreationLevel;

	};

	static CTSingleton<CFactory> Factory;

	/**
	* CFactory::New - creates managed object of specified class and returns pointer to it.
	*/

	template<typename T>
	T* CFactory::New(const string &AName)
	{
		return dynamic_cast<T*>(InternalNew<T>(AName));
	}

	template <typename T>
	T* CFactory::New()
	{
		return New<T>("");
	}

	template<typename T>
	CObject* CFactory::InternalNew(const string &AName)
	{
		/*if (Objects.count(AName) != 0)	// нахуй проверять 2 раза, это не очень долго, но все равно нахуй
		{
			Log("ERROR", "Object with name '%s' already exists", AName.c_str());
			return NULL;
			//throw std::logic_error("Object with name '" + AName + "' already exists.");
		}*/

		T *result = new T;

		if (!Add(result, AName))
			return NULL;

	#if defined(_DEBUG) && !defined(DISABLE_DEBUG_BOXES)

		InsertDebugInfo(result);

	#endif

		return result;
	}

	template<typename T>
	T* CFactory::CreateComponent(const string &AName /*= ""*/)
	{
		T *result = New<T>(AName);
		IncreaseCreationLevel(result);
		DecreaseCreationLevel();
		return result;
	}

	/**
	* CFactory::Add - adds object to the list of managed objects. Object must have unique name, so it will be generated, if not specified. Returns true on success, false otherwise.
	*/

	template<typename T>
	bool CFactory::Add(T *AObject, const string &AName /*= ""*/)
	{
		if (AObject == NULL)
		{
			Log("ERROR", "NULL pointer passed to CFactory::Add");
			return false;
		}

		// we generate name for object if it isn't specified..
		if (AName.empty())
			AObject->SetName(AObject->GetStandardName());
		else
			AObject->SetName(AName);

		string ObjectName = AObject->GetName();

		if (!Objects.insert(make_pair(ObjectName, AObject)).second)
		{
			Log("ERROR", "Object with name '%s' already exists", ObjectName.c_str());
			return false;
		}

		AObject->Managed = true;

		return true;
	}

	/**
	* CFactory::Get - returns pointer to object by its unique name.
	* Swears in log and returns NULL in case of failure.
	*/

	template<typename T>
	T* CFactory::Get(const string &AName, bool AMustExist /*= true*/)
	{
		ObjectsIterator it = Objects.find(AName);
		if (it == Objects.end())
		{
			if (AMustExist)
				Log("ERROR", "Factory can't find object named '%s'", AName.c_str());
			return NULL;
		}

		T *result = dynamic_cast<T *>(it->second);
		if (!result)
		{
			Log("ERROR", "Dynamic cast to '%s' failed for object '%s'", typeid(T).name(), AName.c_str());
		}

		return result;
	}

	/**
	* CFactory::Remove - removes object from the list of managed objects and returns pointer to it.
	* Swears in log and returns NULL in case of failure.
	*/

	template<typename T>
	T* CFactory::Remove(const string &AName)
	{
		ObjectsIterator it = Objects.find(AName);
		if (it == Objects.end())
		{
			Log("ERROR", "Can't remove object named '%s' from factory: factory can't find this object", AName.c_str());
			return NULL;
		}

		T *result = dynamic_cast<T *>(it->second);
		if (!result)
		{
			Log("ERROR", "Dynamic cast to '%s' failed for object '%s'", typeid(T).name(), AName.c_str());
			return NULL;
		}

		it->second->Managed = false;
		Objects.erase(it);

		return result;
	}

}	//	namespace Deku2D

#endif // _2DE_FACTORY_H_
