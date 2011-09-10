#include "2de_Factory.h"

#include "2de_Prototype.h"
#include "2de_GameObject.h"
#include "2de_RenderableComponent.h"
#include "2de_PlaceableComponent.h"
#include "2de_GeometricComponent.h"
#include "2de_Text.h"
#include "2de_TimerComponent.h"
#include "2de_DebugBox.h"

namespace Deku2D
{
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
	* CFactory::CreateByName - creates an object of AClassName type with AName name.
	*
	* AClassName could be an internal class or a prototype resource name.
	*/

	//#define USE_PROTOTYPE_CACHING

	CObject* CFactory::CreateByName(const string &AClassName, const string &AName, UsedPrototypesContainer *UsedPrototypes /*= NULL*/)
	{
		if (CObject *result = CreateClassInstance(AClassName, AName))
			return result;

	#ifdef USE_PROTOTYPE_CACHING
		if (CGameObject *cached = TryUseCachedPrototype(AClassName, AName))
			return cached;
	#endif

		CXMLNode *xml = GetPrototypeXML(AClassName);
		if (!xml)
		{
			Log("ERROR", "Can't create '%s' by name", AClassName.c_str());
			return NULL;
		}

		CGameObject *result = New<CGameObject>(AName);
		if (AName.empty())
			Rename(result->GetName(), AClassName + itos(result->GetID()));

		result->Prototype = true;
		result->Deserialize(xml);

		if (result->GetClassName().empty())
		{
			result->SetClassName("GameObject");
			result->SetScript(Get<CScript>("BaseComponents"));
		}

		UsedPrototypesContainer *FirstUsedPrototypes = NULL;
		if (!UsedPrototypes)
			UsedPrototypes = FirstUsedPrototypes = new UsedPrototypesContainer;

		int ProtoRecursionLimit = -1;
		if (xml->HasAttribute("RecursionLimit"))
			ProtoRecursionLimit = from_string<int>(xml->GetAttribute("RecursionLimit"));

		if (!UsedPrototypes->count(AClassName))
			(*UsedPrototypes)[AClassName] = ProtoRecursionLimit;

		TraversePrototypeNode(xml, result, UsedPrototypes, result);

		result->FinalizeCreation();

		if (FirstUsedPrototypes)
		{
			delete FirstUsedPrototypes;

	#ifdef USE_PROTOTYPE_CACHING
			CachedProtos[AClassName] = result->CloneTree("proto" + AClassName);
	#endif
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
		Object->Name = ANewName;
		Objects[ANewName] = Object;
	}

	/**
	* CFactory::Destroy - adds object to deletion queue and deletes it from the list of managed objects (not removing Managed flag).
	*/

	void CFactory::Destroy(CObject *AObject)
	{
		ObjectsIterator i;
		
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
		for (ObjectsIterator i = Objects.begin(); i != Objects.end(); ++i)
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
		string LocalChildName;

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
			LocalChildName = (*it)->HasAttribute("LocalName") ? (*it)->GetAttribute("LocalName") : "";

			child = dynamic_cast<CGameObject *>(CreateByName(NodeName, ChildName, UsedPrototypes));

			if (!LocalChildName.empty())
				CurrentProto->LocalNameObjectMapping[LocalChildName] = child;

			if (!child)
			{
				Log("ERROR", "Can't create object from prototype: NULL returned by CreateByName");
				continue;
			}
			child->Deserialize(*it);
			AObject->Attach(child);

			// Uncomment the following if you want to disallow additional children attached to prototype included within other prototype.
			//if (!IsClassExists(NodeName))
			//	Log("ERROR", "Prototype references can't have children");
			//else

			TraversePrototypeNode(*it, child, UsedPrototypes, CurrentProto);

			if (UsedPrototypes->count(NodeName) != 0)
				++(*UsedPrototypes)[NodeName];
		}
	}

	CXMLNode* CFactory::GetPrototypeXML(const string &AName)
	{
		CPrototype *proto = Get<CPrototype>(AName);
		if (!proto)
		{
			Log("ERROR", "No such prototype: '%s'", AName.c_str());
			return NULL;
		}

		CXMLNode *xml = proto->GetRootNode();
		if (xml->IsErroneous() || !xml->HasAttribute("Name") || xml->GetAttribute("Name") != AName)
		{
			Log("ERROR", "Prototype '%s' XML contains an error", AName.c_str());
			return NULL;
		}

		return xml;
	}

	CObject* CFactory::CreateClassInstance(const string &AClassName, const string &AName)
	{
		ClassesContainer::iterator it = Classes.find(AClassName);
		if (it == Classes.end())
			return NULL;

		CObject *result = (this->*(it->second.NewFunction))(AName);

		CGameObject *go = dynamic_cast<CGameObject*>(result);
		if (go)
		{
			go->SetClassName(AClassName);
			go->SetScript(Get<CScript>("BaseComponents"));
			go->FinalizeCreation();
		}

		return result;
	}

	CGameObject* CFactory::TryUseCachedPrototype(const string &AClassName, const string &AName)
	{
		map<string, CGameObject *>::iterator it = CachedProtos.find(AClassName);

		if (it == CachedProtos.end())
			return NULL;

		return it->second->CloneTree(AName);
	}


	#if defined(_DEBUG) && !defined(DISABLE_DEBUG_BOXES)

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

}	//	namespace Deku2D
