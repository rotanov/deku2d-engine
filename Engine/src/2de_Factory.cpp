#include "2de_Factory.h"

#include "2de_Components.h"

//////////////////////////////////////////////////////////////////////////
// CFactory

CFactory::CFactory()
{
	SetName("Factory");

	AddClass("RenderableComponent", &CFactory::InternalNew<CRenderableComponent>);
	AddClass("PlaceableComponent", &CFactory::InternalNew<CPlaceableComponent>);
	AddClass("GeometricComponent", &CFactory::InternalNew<CGeometricComponent>);
	AddClass("ScriptableComponent", &CFactory::InternalNew<CScriptableComponent>);
	AddClass("Text", &CFactory::InternalNew<CText>);
	AddClass("TimerComponent", &CFactory::InternalNew<CTimerComponent>);
}

CFactory::~CFactory()
{	
}

/**
* CFactory::CreateByName - creates an object of AClassName class with AName name.
*/

CObject* CFactory::CreateByName(const string &AClassName, const string &AName, set<string> *UsedPrototypes /*= NULL*/)
{
	ClassesContainer::iterator it = Classes.find(AClassName);
	if (it != Classes.end())
	{
		return (this->*(it->second.NewFunction))(AName);
	}

	CPrototype *proto = CFactory::Instance()->Get<CPrototype>(AClassName);
	if (!proto)
	{
		Log("ERROR", "Class '%s' can't be created by name", AClassName.c_str());
		return NULL;
	}

	CXMLNode *xml = proto->GetRootNode();
	if (xml->IsErroneous() || !xml->HasAttribute("Name") || xml->GetAttribute("Name") != AClassName)
	{
		Log("ERROR", "Can't create prototype-class '%s' due to an error in XML", AClassName.c_str());
		return NULL;
	}

	CGameObject *result = CFactory::Instance()->New<CGameObject>(AName);

	set<string> *FirstUsedPrototypes = NULL;

	if (!UsedPrototypes)
	{
		FirstUsedPrototypes = new set<string>;
		UsedPrototypes = FirstUsedPrototypes;
	}
	UsedPrototypes->insert(AClassName);

	TraversePrototypeNode(xml, result, UsedPrototypes);

	if (FirstUsedPrototypes)
	{
		delete FirstUsedPrototypes;
	}

	return result;
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

bool CFactory::IsClassExists(const string &AName)
{
	return (Classes.count(AName) != 0);
}

void CFactory::AddClass(const string &AClassName, TNewFunction ANewFunctionPointer, bool AIsComponent /*= true*/)
{
	Classes[AClassName] = CClassDescription(ANewFunctionPointer, AIsComponent);
}

void CFactory::TraversePrototypeNode(CXMLNode *ANode, CGameObject *AObject, set<string> *UsedPrototypes)
{
	CGameObject *child;
	string NodeName;
	string ChildName;

	for (CXMLChildrenList::Iterator it = ANode->Children.Begin(); it != ANode->Children.End(); ++it)
	{
		if ((*it)->GetType() != CXMLNode::XML_NODE_TYPE_NORMAL)
			continue;

		NodeName = (*it)->GetName(); 
		if (IsClassExists(NodeName))
		{
			if (!Classes[NodeName].IsComponent)
			{
				Log("ERROR", "Prototypes can't contain non-component clasees");
				continue;
			}
		}
		else
		{
			if (UsedPrototypes->count(NodeName) != 0)
			{
				Log("ERROR", "Recursive reference in prototype");
				continue;
			}
	
		}

		ChildName = (*it)->HasAttribute("Name") ? (*it)->GetAttribute("Name") : "";
		child = dynamic_cast<CGameObject *>(CreateByName(NodeName, ChildName, UsedPrototypes));

		if (!child)
		{
			Log("ERROR", "Can't create object from prototype: NULL returned by CreateByName");
			continue;
		}

		child->Deserialize(*it);

		AObject->Attach(child);

		UsedPrototypes->insert(NodeName);

		//// i don't see any point in forbidding it..
		//	children correctly attach themselves to the root of prototype reference..
		//	more than that, it's may be useful for extending prototype references in different places..
		//	if you see any point in forbidding it - uncomment the following if-check:

		//if (IsClassExists(NodeName))
		//{
			TraversePrototypeNode(*it, child, UsedPrototypes);
		//}
		//else
		//{
			//Log("ERROR", "Prototype references can't have children");

		//}
	}
}

