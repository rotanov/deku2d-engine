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

	CFactory::CFactory() : CreationLevel(0)
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

	CGameObject* CFactory::CreateGameObject(const string &AClassName, const string &AName /*= ""*/)
	{
		CGameObject *result;

		if (IsComponentExists(AClassName))
			result = CreateComponent(AClassName, AName);
		else
			result = InstantiatePrototype(AClassName, AName);

		return result;
	}

	CGameObject* CFactory::CreateComponent(const string &AClassName, const string &AName /*= ""*/)
	{
		ComponentsContainer::iterator it = Components.find(AClassName);
		if (it == Components.end())
		{
			Log("ERROR", "Can't create component: no such component class: '%s'", AClassName.c_str());
			return NULL;
		}

		CGameObject *result = dynamic_cast<CGameObject *>((this->*(it->second))(AName));
		IncreaseCreationLevel(result);
		DecreaseCreationLevel();
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
		IncreaseCreationLevel(result);

		// TODO: Rename looks weird.. make it possible to use SetName
		if (AName.empty())
			Rename(result->GetName(), AProtoName + itos(result->GetID()));

		result->Prototype = true;
		result->Deserialize(xml);

		TraversePrototypeNode(xml, result, result);

		DecreaseCreationLevel();
		return result;
	}

	/**
	* CFactory::CreateByName - creates an object of AClassName type with AName name.
	*
	* AClassName could be an internal class or a prototype resource name.
	*/

	//#define USE_PROTOTYPE_CACHING

	/*CObject* CFactory::CreateByName(const string &AClassName, const string &AName, UsedPrototypesContainer *UsedPrototypes [>= NULL<])
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
		IncreaseCreationLevel(result);

		if (AName.empty())
			Rename(result->GetName(), AClassName + itos(result->GetID()));

		result->Prototype = true;
		result->Deserialize(xml);

		assert(!result->GetClassName().empty());

		if (result->GetClassName() == "GameObject")
			result->SetScript(Get<CScript>("BaseComponents"));

		UsedPrototypesContainer *FirstUsedPrototypes = NULL;
		if (!UsedPrototypes)
			UsedPrototypes = FirstUsedPrototypes = new UsedPrototypesContainer;

		int ProtoRecursionLimit = xml->HasAttribute("RecursionLimit") ?
			from_string<int>(xml->GetAttribute("RecursionLimit")) : -1;

		if (!UsedPrototypes->count(AClassName))
			(*UsedPrototypes)[AClassName] = ProtoRecursionLimit;

		TraversePrototypeNode(xml, result, UsedPrototypes, result);

		if (FirstUsedPrototypes)
		{
			delete FirstUsedPrototypes;

	#ifdef USE_PROTOTYPE_CACHING
			CachedProtos[AClassName] = result->CloneTree("proto" + AClassName);
	#endif
		}

		DecreaseCreationLevel();
		return result;
	}*/

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
			ChildName = (*it)->SafeGetAttribute("Name");
			LocalChildName = (*it)->SafeGetAttribute("LocalName");

			child = CreateGameObject(NodeName, ChildName);
			if (!child)
			{
				Log("ERROR", "Can't create object of class '%s' from prototype", NodeName.c_str());
				continue;
			}

			CurrentProto->AddLocalName(LocalChildName, child);

			child->Deserialize(*it);
			AObject->Attach(child);

			TraversePrototypeNode(*it, child, CurrentProto);
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

	CGameObject* CFactory::TryUseCachedPrototype(const string &AClassName, const string &AName)
	{
		map<string, CGameObject *>::iterator it = CachedProtos.find(AClassName);

		if (it == CachedProtos.end())
			return NULL;

		return it->second->CloneTree(AName);
	}

	void CFactory::IncreaseCreationLevel(CGameObject *AObject)
	{
		++CreationLevel;
		CreationQueue.push_front(AObject);
	}

	void CFactory::DecreaseCreationLevel()
	{
		if (CreationLevel == 1)
		{
			list<CGameObject *>::iterator dit;
			while (!CreationQueue.empty())
			{
				for (list<CGameObject *>::iterator it = CreationQueue.begin(); it != CreationQueue.end();)
				{
					(*it)->FinalizeCreation();
					dit = it++;
					CreationQueue.erase(dit);
				}
			}
		}
		--CreationLevel;
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
