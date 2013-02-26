#include "2de_Factory.h"

#include "2de_Prototype.h"
#include "2de_GameObject.h"
#include "2de_RenderableComponent.h"
#include "2de_PlaceableComponent.h"
#include "2de_GeometricComponent.h"
#include "2de_Text.h"
#include "2de_TimerComponent.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CFactory

	CFactory::CFactory()
	{
		SetName("Factory");

		AddComponent("RenderableComponent", &CFactory::InternalNew<CRenderableComponent>);
		AddComponent("PlaceableComponent", &CFactory::InternalNew<CPlaceableComponent>);
		AddComponent("GeometricComponent", &CFactory::InternalNew<CGeometricComponent>);
		AddComponent("GameObject", &CFactory::InternalNew<CGameObject>);
		AddComponent("Text", &CFactory::InternalNew<CText>);
		AddComponent("TimerComponent", &CFactory::InternalNew<CTimerComponent>);
	}

	CFactory::~CFactory()
	{	
	}

	CGameObject* CFactory::CreateGameObject(const string &AClassName, const string &AName /*= ""*/, bool AFinalizeCreation /*= true*/)
	{
		CGameObject *result;

		if (IsComponentExists(AClassName))
			result = CreateComponent(AClassName, AName, AFinalizeCreation);
		else
			result = InstantiatePrototype(AClassName, AName);		

		return result;
	}

	CGameObject* CFactory::CreateComponent(const string &AClassName, const string &AName /*= ""*/, bool AFinalizeCreation /*= true*/)
	{
		ComponentsContainer::iterator it = Components.find(AClassName);
		if (it == Components.end())
		{
			Log("ERROR", "Can't create component: no such component class: '%s'", AClassName.c_str());
			return NULL;
		}

		CGameObject *result = dynamic_cast<CGameObject *>((this->*(it->second))(AName));

		if (AFinalizeCreation)
			result->FinalizeCreation();

		return result;
	}

	CGameObject* CFactory::InstantiatePrototype(const string &AProtoName, const string &AName /*= ""*/)
	{
		CXMLNode *xml = GetPrototypeXML(AProtoName);
		if (!xml)
		{
			Log("ERROR", "Can't create prototype instance: no such prototype: '%s'", AProtoName.c_str());
			return NULL;
		}

		CGameObject *result = New<CGameObject>(AName);

		if (AName.empty())
			result->SetName(AProtoName + itos(result->GetID()));

		result->Prototype = true;
		result->Deserialize(xml);

		SetRecursionLimit(AProtoName, from_string<int>(xml->SafeGetAttribute("RecursionLimit", "-1")));

		TraversePrototypeNode(xml, result, result);

		result->FinalizeCreation();

		Log("INFO", "Prototype INSTANTIATED: '%s', instance name: '%s'", AProtoName.c_str(), result->GetName().c_str());

		return result;
	}


	/**
	* CFactory::Rename - renames managed object.
	*/

	void CFactory::Rename(const string &AName, const string &ANewName)
	{
		ObjectsIterator it = Objects.find(AName);
		if (it == Objects.end())
		{
			Log("ERROR", "Factory isn't managing object named '%s'", AName.c_str());
			return;
		}

		CObject *Object = it->second;

		if (!Objects.insert(make_pair(ANewName, Object)).second)
		{
			Log("ERROR", "Can't rename object '%s' to '%s': object with requested name already exists", AName.c_str(), ANewName.c_str());
			return;
		}

		Object->Name = ANewName;
		Objects.erase(it);
	}

	/**
	* CFactory::Destroy - adds object to deletion queue and deletes it from the list of managed objects (not removing Managed flag).
	*/

	void CFactory::Destroy(CObject *AObject)
	{
		if (AObject == NULL)
		{
			Log("ERROR", "NULL pointer passed to CFactory::Destroy");
			return;
		}

		ObjectsIterator it = Objects.find(AObject->GetName());
		if (it == Objects.end())
		{
			Log("ERROR", "Can't destroy object named '%s': factory isn't managing this object", AObject->GetName().c_str());
			return;
		}

		AObject->Destroyed = true;

		Objects.erase(it);
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

	bool CFactory::IsComponentExists(const string &AClassName)
	{
		return (Components.count(AClassName) != 0);
	}

	void CFactory::AddComponent(const string &AClassName, TNewFunction ANewFunctionPointer)
	{
		Components[AClassName] = ANewFunctionPointer;
	}

	void CFactory::TraversePrototypeNode(CXMLNode *ANode, CGameObject *AObject, CGameObject *CurrentProto)
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

			int recursionsLeft = GetRecursionsLeft(NodeName);
			if (recursionsLeft < 1)
			{
				if (recursionsLeft == -1)
					Log("ERROR", "Unlimited recursive reference of prototype '%s'", NodeName.c_str());

				continue;
			}

			IncreaseUsedCount(NodeName);

			ChildName = (*it)->SafeGetAttribute("Name");
			LocalChildName = (*it)->SafeGetAttribute("LocalName");

			child = CreateGameObject(NodeName, ChildName, false);
			if (!child)
			{
				Log("ERROR", "Can't create object of class '%s' from prototype", NodeName.c_str());
				continue;
			}

			CurrentProto->AddLocalName(LocalChildName, child);

			child->Deserialize(*it);
			AObject->Attach(child);

			if (!child->IsPrototype())
				child->FinalizeCreation();

			TraversePrototypeNode(*it, child, CurrentProto);

			DecreaseUsedCount(NodeName);
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

	void CFactory::SetRecursionLimit(const string &AProtoName, int ARecursionLimit)
	{
		if (!UsedPrototypes.count(AProtoName))
			UsedPrototypes[AProtoName] = ARecursionLimit;
	}

	int CFactory::GetRecursionsLeft(const string &AProtoName)
	{
		int result = 1;

		if (UsedPrototypes.count(AProtoName))
			result = UsedPrototypes[AProtoName];

		return result;
	}

	void CFactory::IncreaseUsedCount(const string &AProtoName)
	{
		if (UsedPrototypes.count(AProtoName))
			--UsedPrototypes[AProtoName];
	}

	void CFactory::DecreaseUsedCount(const string &AProtoName)
	{
		if (UsedPrototypes.count(AProtoName))
			++UsedPrototypes[AProtoName];
	}

	CGameObject* CFactory::TryUseCachedPrototype(const string &AClassName, const string &AName)
	{
		map<string, CGameObject *>::iterator it = CachedProtos.find(AClassName);

		if (it == CachedProtos.end())
			return NULL;

		return it->second->CloneTree(AName);
	}

}	//	namespace Deku2D
