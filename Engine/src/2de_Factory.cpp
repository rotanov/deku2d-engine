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
	AddClass("GameObject", &CFactory::InternalNew<CGameObject>);
	AddClass("Text", &CFactory::InternalNew<CText>);
	AddClass("TimerComponent", &CFactory::InternalNew<CTimerComponent>);
}

CFactory::~CFactory()
{	
}

/**
* CFactory::CreateByName - creates an object of AClassName class with AName name.
*/

CObject* CFactory::CreateByName(const string &AClassName, const string &AName, UsedPrototypesContainer *UsedPrototypes /*= NULL*/)
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
	result->Deserialize(xml);

	UsedPrototypesContainer *FirstUsedPrototypes = NULL;

	if (!UsedPrototypes)
	{
		FirstUsedPrototypes = new UsedPrototypesContainer;
		UsedPrototypes = FirstUsedPrototypes;
	}

	int ProtoRecursionLimit = -1;
	if (xml->HasAttribute("RecursionLimit"))
	{
		ProtoRecursionLimit = from_string<int>(xml->GetAttribute("RecursionLimit"));
	}

	if (!UsedPrototypes->count(AClassName))
		(*UsedPrototypes)[AClassName] = ProtoRecursionLimit;

	TraversePrototypeNode(xml, result, UsedPrototypes, result);

	if (FirstUsedPrototypes)
	{
		delete FirstUsedPrototypes;
	}

	result->FinalizeCreation();
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

void CFactory::TraversePrototypeNode(CXMLNode *ANode, CGameObject *AObject, UsedPrototypesContainer *UsedPrototypes, CGameObject *CurrentProto)
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
				if ((*UsedPrototypes)[NodeName] < 1)
				{
					if ((*UsedPrototypes)[NodeName] <= -1)
						Log("ERROR", "Recursive reference in prototype");
					continue;
				}
				--(*UsedPrototypes)[NodeName];
			}
	
		}

		ChildName = (*it)->HasAttribute("Name") ? (*it)->GetAttribute("Name") : "";
		string localChildName = (*it)->HasAttribute("LocalName") ? (*it)->GetAttribute("LocalName") : "";
		child = dynamic_cast<CGameObject *>(CreateByName(NodeName, ChildName, UsedPrototypes));
		if (localChildName != "")
			CurrentProto->LocalNameObjectMapping[ localChildName ] = child;

		if (!child)
		{
			Log("ERROR", "Can't create object from prototype: NULL returned by CreateByName");
			continue;
		}
		child->Deserialize(*it);
		AObject->Attach(child);

//		Uncomment the following if you want to disallow additional children attached to prototype included within other prototype.
//
// 		if (IsClassExists(NodeName))
// 		{
			TraversePrototypeNode(*it, child, UsedPrototypes, CurrentProto);
// 		}
// 		else
// 		{
// 			Log("ERROR", "Prototype references can't have children");
// 		}

		

		if (UsedPrototypes->count(NodeName) != 0)
			++(*UsedPrototypes)[NodeName];
	}
}

#ifdef _DEBUG

void CFactory::InsertDebugInfo( CObject* Source )
{
	if (typeid(*Source) == typeid(CDebugBoxComponent))
		return;
	CGameObject *GameObject = dynamic_cast<CGameObject*>(Source);
	if (GameObject == NULL)
		return;
	GameObject->Attach(New<CDebugBoxComponent>());
}

#endif


