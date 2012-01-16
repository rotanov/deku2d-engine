#include "2de_GameObject.h"

#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CGameObject
	D2D_DEFINE_RTTI(CGameObject, IVisitableObject<>)

	CGameObject::CGameObject() : Parent(NULL), Scene(NULL), Script(NULL), Prototype(false), Created(false), Active(true), Dead(false), Enabled(true)
	{
		ClassName = "GameObject";
		PutIntoScene(SceneManager->GetCurrentScene());
		EventManager->Subscribe("Create", this);
	}

	CGameObject::~CGameObject()
	{
		Scene->Remove(this);
		//	Note, that here we adding children to parent in reverse order, i think it's not that important for now.
		//	That order is not important at all. Overall comment should be removed.
		while (!Children.empty())
			Children.back()->SetParent(Parent);

		SetParent(NULL);

		LuaVirtualMachine->DestroyLuaObject(*this);
	}

	CGameObject* CGameObject::Clone(const string &ACloneName /*= ""*/) const
	{
		return CloneHelper(new CGameObject(*this), ACloneName);
	}

	CGameObject* CGameObject::CloneTree(const string &ACloneName /*= ""*/, AlreadyClonedContainer *AlreadyCloned /*= NULL*/) const
	{
		CGameObject *result = Clone(ACloneName);

		map<CGameObject *, CGameObject *> *FirstAlreadyCloned = NULL;
		if (!AlreadyCloned)
			AlreadyCloned = FirstAlreadyCloned = new AlreadyClonedContainer;

		CGameObject *obj = NULL;

		for (LNOMType::const_iterator it = LocalNameObjectMapping.begin(); it != LocalNameObjectMapping.end(); ++it)
		{
			obj = it->second->CloneTree("", AlreadyCloned);
			result->LocalNameObjectMapping[it->first] = obj;
			(*AlreadyCloned)[it->second] = obj;
		}


		for (ChildrenConstIterator it = Children.begin(); it != Children.end(); ++it)
		{
			if (AlreadyCloned->count(*it))
				obj = (*AlreadyCloned)[*it];
			else
				obj = (*it)->CloneTree("", AlreadyCloned);

			result->Attach(obj);
		}

		if (IsPrototype())
			result->FinalizeCreation();

		if (FirstAlreadyCloned)
			delete FirstAlreadyCloned;

		return result;
	}

	void CGameObject::Attach(CGameObject *AGameObject)
	{
		if (!AGameObject)
		{
			Log("ERROR", "NULL pointer passed to CGameObject::Attach");
			return;
		}

		AGameObject->SetParent(this);
	}

	void CGameObject::Detach(CGameObject *AGameObject)
	{
		if (!AGameObject)
		{
			Log("ERROR", "NULL pointer passed to CGameObject::Detach");
			return;
		}

		AGameObject->SetParent(NULL);
	}

	void CGameObject::Detach(unsigned index)
	{
		if (index >= Children.size())
		{
			Log("ERROR", "CGameObject::Detach: index out of bounds");
			return;
		}

		CGameObject *AGameObject = *(Children.begin() + index);
		AGameObject->SetParent(NULL);
	}

	void CGameObject::SetDestroyedSubtree()
	{
		CGameObject::_DestroySubtree(this);
	}

	const string& CGameObject::GetClassName() const
	{
		return ClassName;
	}

	void CGameObject::SetClassName(const string &AClassName)
	{
		ClassName = AClassName;
	}

	CGameObject* CGameObject::GetParent() const
	{
		return Parent;
	}

	void CGameObject::SetParent(CGameObject *AGameObject)
	{
		if (AGameObject == this)
		{
			Log("ERROR", "Object '%s' can't be parent of itself", GetName().c_str());
			return;
		}

		if (Parent != NULL)
		{
			ChildrenIterator it = find(Parent->Children.begin(), Parent->Children.end(), this);

			if (it != Parent->Children.end())
			{
				Parent->Children.erase(it);

				// i already hate this isFinalizing kludge, but the following call segfaults at finalizing time without it..
				// may be we will get rid of it when we have some cool memory managers, etc., but now i leave it as is..
				if (!CEngine::Instance()->isFinalizing() && LuaVirtualMachine->IsMethodFunctionExists(GetName(), "OnDetached"))
					LuaVirtualMachine->CallMethodFunction(GetName(), "OnDetached");
			}
		}

		Parent = AGameObject;

		if (Parent != NULL)
		{
			if (find(Parent->Children.begin(), Parent->Children.end(), this) == Parent->Children.end()) // probably remove this find..
			{
				Parent->Children.push_back(this);

				if (!CEngine::Instance()->isFinalizing() && LuaVirtualMachine->IsMethodFunctionExists(GetName(), "OnAttached"))
					LuaVirtualMachine->CallMethodFunction(GetName(), "OnAttached");
			}
		}

		if (Created)
			UpdateParentAndProtoFields();
	}

	CAbstractScene* CGameObject::GetScene() const
	{
		assert(Scene != NULL);
		return Scene;
	}

	void CGameObject::PutIntoScene(CAbstractScene *AScene)
	{
		assert(AScene != NULL);
		if (Scene != NULL)
			Scene->Remove(this);
		Scene = AScene;
		Scene->Add(this);
	}

	CGameObject* CGameObject::GetChild(unsigned index)
	{
		return Children[index];
	}

	CGameObject* CGameObject::GetObjectByLocalName(const string &AName)
	{
		LNOMType::const_iterator it = LocalNameObjectMapping.find(AName);
		if (it == LocalNameObjectMapping.end()) {
			return NULL;
		}

		return it->second;
	}

	unsigned CGameObject::GetChildCount()
	{
		return Children.size();
	}

	CGameObject* CGameObject::FindFirstOfClass(const string &AClassName, bool ExceedPrototype /*= false*/)
	{
		queue<CGameObject *> SearchQueue;
		CGameObject *current = NULL;

		SearchQueue.push(this);

		while (!SearchQueue.empty())
		{
			current = SearchQueue.front();
			SearchQueue.pop();

			if (current->ClassName == AClassName && current != this)
				return current;

			for (CGameObject::ChildrenIterator it = current->Children.begin(); it != current->Children.end(); ++it)
			{
				if (ExceedPrototype || !(*it)->IsPrototype())
					SearchQueue.push(*it);
			}
		}

		return NULL;
	}

	bool CGameObject::IsActive() const
	{
		return Active;
	}

	void CGameObject::SetActive(bool AActive)
	{
		Active = AActive;
	}

	bool CGameObject::IsPrototype() const
	{
		return Prototype;
	}

	void CGameObject::Deserialize(CXMLNode *AXML)
	{
		if (AXML->HasAttribute("ClassName"))
		{
			string cls = AXML->GetAttribute("ClassName");
			if (!cls.empty())
				ClassName = cls;
		}

		if (AXML->HasAttribute("Script"))
		{
			SetScript(Factory->Get<CScript>(AXML->GetAttribute("Script")));
		}
	}

	void CGameObject::FinalizeCreation()
	{
		LuaVirtualMachine->SetLocalNamesFields(this);
		UpdateParentAndProtoFields();
		Created = true;
	}

	void CGameObject::ProcessEvent(const CEvent &AEvent)
	{
		CLuaFunctionCall fc(GetName(), "On" + AEvent.GetName());
		LuaVirtualMachine->PushEventTable(AEvent);
		fc.SetArgumentsCount(1);
		fc.Call();
	}

	CScript* CGameObject::GetScript() const
	{
		return Script;
	}

	void CGameObject::SetScript(CScript *AScript)
	{
		Script = AScript;

		LuaVirtualMachine->DestroyLuaObject(*this);
		LuaVirtualMachine->RunScript(AScript);
		CreateLuaObject();

		if (Created)
		{
			LuaVirtualMachine->SetLocalNamesFields(this);
			UpdateParentAndProtoFields();
		}
	}

	void CGameObject::DFSIterate(CGameObject *Next, IVisitorBase *Visitor)
	{
		if (!Next->IsActive())
			return;
		Next->AcceptOnEnter(*Visitor);
		for (unsigned i = 0; i < Next->GetChildCount(); i++)
			DFSIterate(Next->Children[i], Visitor);
		Next->AcceptOnLeave(*Visitor);
	}

	void CGameObject::JustDoIt()
	{

	}

	CGameObject::CGameObject(const CGameObject &AGameObject)
	{
		Parent = NULL;
		Scene = AGameObject.Scene;
		ClassName = AGameObject.ClassName;
		Prototype = AGameObject.Prototype;
		Created = false;
		Active = AGameObject.Active;

		Dead = false;
		Enabled = true;

		PutIntoScene(Scene);
		EventManager->Subscribe("Create", this);
		Script = AGameObject.Script;
	}

	void CGameObject::_DestroySubtree(CGameObject *NextObject)
	{
		if (NextObject == NULL)
			return;
		for (vector<CGameObject *>::iterator i = NextObject->Children.begin(); i != NextObject->Children.end(); ++i)
			_DestroySubtree(*i);
		NextObject->SetDestroyed();
	}

	CGameObject* CGameObject::FindPrototype()
	{
		CGameObject *result = Parent;

		while (result && !result->Prototype)
			result = result->Parent;

		return result;
	}

	void CGameObject::UpdateParentAndProtoFields()
	{
		// this recursive call may greatly affect performance, but it's needed because of creation order to make parentProto work
		for (ChildrenIterator it = Children.begin(); it != Children.end(); ++it)
			(*it)->UpdateParentAndProtoFields();

		LuaVirtualMachine->SetReferenceField(this, "parent", Parent);

		CGameObject *proto = FindPrototype();
		LuaVirtualMachine->SetReferenceField(this, "proto", proto);

		CGameObject *ParentProto = NULL;
		if (proto)
			ParentProto = proto->FindPrototype();
		LuaVirtualMachine->SetReferenceField(this, "parentProto", ParentProto);
	}

	void CGameObject::CreateLuaObject()
	{
		if (!LuaVirtualMachine->IsObjectExists(GetName()))
			LuaVirtualMachine->CreateLuaObject(ClassName, GetName(), this);
	}

}	//	namespace Deku2D
