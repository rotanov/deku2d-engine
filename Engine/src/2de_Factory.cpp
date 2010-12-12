#include "2de_Factory.h"

//////////////////////////////////////////////////////////////////////////
// CFactory

CFactory::CFactory()
{
	SetName("Factory");
}

CFactory::~CFactory()
{	
}

/**
* CFactory::Rename - renames managed object.
*/

void CFactory::Rename(const string &AName, const string &ANewName)
{
	if (Objects.count(AName) == 0)
	{
		Log("WARNING", "Factory isn't managing object named '%s'", AName.c_str());
		return;
	}

	CObject *Object = Objects[AName];
	Objects.erase(AName);
	Object->SetName(ANewName);
	Objects[ANewName] = Object;
}

/**
* CFactory::Destroy - adds object to deletion queue and deletes it from the list of managed objects (not removing Managed flag).
*/

void CFactory::Destroy(CObject *AObject)
{
	map<string, CObject*>::iterator i;
	
	for (i = Objects.begin(); i != Objects.end(); ++i)
	{
		if (i->second == AObject)
			break;
	}

	if (i == Objects.end())
	{
		Log("ERROR", "Factory isn't managing object with given pointer");
		return;
	}
	
	AObject->Destroyed = true;

	Objects.erase(i);
	Deletion.push(AObject);

	//CObject::DecRefCount(AObject);
}

/**
* CFactory::CleanUp - clears objects destroying queue, freeing each object's memory.
*/

void CFactory::CleanUp()
{
	CObject *object;

	// we can implement any additional logic here, based for example on size() of queue or whatever, if it's needed..
	while (!Deletion.empty())
	{
		object = Deletion.front();
		Log("INFO", "Destroying: %s, with id: %d", object->GetName().c_str(), object->GetID());

		delete object;
		// delete Deletion.front();

		Deletion.pop();
	}
}

// well, maybe there is better way to do final all-destroy clean-up..
void CFactory::DestroyAll()
{
	for (map<string, CObject*>::iterator i = Objects.begin(); i != Objects.end(); ++i)
		Deletion.push(i->second);

	Objects.clear();
	CleanUp();
}

