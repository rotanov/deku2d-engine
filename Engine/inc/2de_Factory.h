#ifndef _2DE_FACTORY_H_
#define _2DE_FACTORY_H_

#include "2de_Core.h"
#include "2de_Event.h"

/**
* CFactory - oversees creation of any objects. The real purpose at this moment: managing memory at creation and destuction of each object and getting pointer by name.
*/

class CFactory : public CTSingleton<CFactory>
{
public:
	typedef CObject* (CFactory::*NewFunction)(const string &);
	typedef map<string, NewFunction> ClassesContainer;

	template<typename T>
	T* New(const string &AName);

	template<typename T>
	T* New();

	CObject* CreateByName(const string &AClassName, const string &AName);

	template<typename T>
	void Add(T *AObject, const string &AName = "");

	template<typename T>
	T* Get(const string &AName);

	template<typename T>
	T* Remove(const string &AName);

	void Rename(const string &AName, const string &ANewName);
	void Destroy(CObject *AObject);
	void CleanUp();
	void DestroyAll();

protected:
	CFactory();
	~CFactory();
	friend class CTSingleton<CFactory>;

	void AddClass(const string &AClassName, NewFunction ANewFunctionPointer);

	template<typename T>
	CObject *InternalNew(const string &AName);

	ClassesContainer Classes;

	map<string, CObject*> Objects;

	queue<CObject *> Deletion;
};

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
	if (Objects.count(AName) != 0)
	{
		Log("ERROR", "Object with name '%s' already exists", AName.c_str());
		throw std::logic_error("Object with name '" + AName + "' already exists.");
	}

	T* result = new T;

	Add(result, AName);

	return result;
}

/**
* CFactory::Add - adds object to the list of managed objects. Object must have unique name, so it will be generated, if not specified.
*/

template<typename T>
void CFactory::Add(T *AObject, const string &AName /*= ""*/)
{
	if (AObject == NULL)
	{
		Log("ERROR", "NULL pointer passed to CFactory::Add");
		return;
	}

	// we generate name for object if it isn't specified..
	if (AName.empty())
		AObject->SetName(typeid(T).name() + itos(AObject->GetID()));
	else
		AObject->SetName(AName);

	string ObjectName = AObject->GetName();

	if (Objects.count(ObjectName) != 0)
	{
		Log("ERROR", "Object with name '%s' already exists", ObjectName.c_str());
		throw std::logic_error("Object with name '" + ObjectName + "' already exists.");
		return;
	}

	Objects[ObjectName] = AObject;
	//AObject->IncRefCount();
	AObject->Managed = true;
}

/**
* CFactory::Get - returns pointer to object by its unique name.
* Swears in log and returns NULL in case of failure.
*/

template<typename T>
T* CFactory::Get(const string &AName)
{
	if (Objects.count(AName) == 0)
	{
		Log("ERROR", "Factory can't find object named '%s'", AName.c_str());
		return NULL;
	}

	T* result = dynamic_cast<T*>(Objects[AName]);
	if (result == NULL)
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
	if (Objects.count(AName) == 0)
	{
		Log("ERROR", "Factory can't find object named '%s'", AName.c_str());
		return NULL;
	}

	T* result = dynamic_cast<T *>(Objects[AName]);
	if (result == NULL)
	{
		Log("ERROR", "Dynamic cast to '%s' failed for object '%s'", typeid(T).name(), AName.c_str());
	}
	else
	{
		result->Managed = false;
		Objects.erase(AName);
		//CObject::DecRefCount(result);
	}

	return result;
}



#endif // _2DE_FACTORY_H_
