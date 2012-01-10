#ifndef _2DE_GAME_OBJECT_H_
#define _2DE_GAME_OBJECT_H_

#include "2de_Core.h"
#include "2de_Event.h"
#include "2de_GraphicsLow.h"
#include "2de_LuaUtils.h"

namespace Deku2D
{
	// shouldn't be here..
	#if defined(GetClassName)
		#undef GetClassName
	#endif

	/**
	* CGameObject - the base class for all components.
	*/

	class CGameObject : public IVisitableObject<>
	{
	public:	
		D2D_DECLARE_VISITABLE()

		typedef map<string, CGameObject*> LNOMType;	// ???
		LNOMType LocalNameObjectMapping; // ???

		typedef map<CGameObject *, CGameObject *> AlreadyClonedContainer;

		CGameObject();
		virtual ~CGameObject();

		virtual CGameObject* Clone(const string &ACloneName = "") const;
		CGameObject* CloneTree(const string &ACloneName = "", AlreadyClonedContainer *AlreadyCloned = NULL) const;

		void Attach(CGameObject *AGameObject);
		void Detach(CGameObject *AGameObject);
		void Detach(unsigned index);

		void SetDestroyedSubtree();

		const string& GetClassName() const;
		void SetClassName(const string &AClassName);

		CGameObject* GetParent() const;
		void SetParent(CGameObject *AGameObject);

		CAbstractScene* GetScene() const;
		void PutIntoScene(CAbstractScene *AScene);

		CGameObject* GetChild(unsigned index);
		CGameObject* GetObjectByLocalName(const string &AName);
		unsigned GetChildCount();

		void AddLocalName(const string &ALocalName, CGameObject *AChild);

		CGameObject* FindFirstOfClass(const string &AClassName, bool ExceedPrototype = false);

		bool IsActive() const;
		void SetActive(bool AActive);

		bool IsPrototype() const;

		virtual void Deserialize(CXMLNode *AXML);
		virtual void FinalizeCreation();
		void ProcessEvent(const CEvent &AEvent);
		CScript* GetScript() const;
		void SetScript(CScript *AScript);

		void DFSIterate(CGameObject *Next, IVisitorBase *Visitor); // Not here

		virtual void JustDoIt();

	// 	bool isDead() const;	// @todo: Think about applyng this part of CUpdatable interface into CGameObject
	// 	void SetDead();
	//	virtual void Update(float dt) = 0;

	protected:
		CGameObject(const CGameObject &AGameObject);

		template<typename T>
		T CloneHelper(T AObject, const string &ACloneName = "") const
		{
			Factory->Add(AObject, ACloneName.empty() ? AObject->GetStandardName() : ACloneName);	// may be names will be too long..
			AObject->SetScript(AObject->GetScript());
			AObject->FinalizeCreation();	// i'm not sure about this.. this is wrong for prototypes, i think.. // but it works..
			return AObject;
		}

		bool Created;

	private:
		typedef vector<CGameObject *> ChildrenContainer;
		typedef ChildrenContainer::iterator ChildrenIterator;
		typedef ChildrenContainer::const_iterator ChildrenConstIterator;

		static void _DestroySubtree(CGameObject *NextObject);
		CGameObject* FindPrototype();
		void UpdateParentAndProtoFields();
		void CreateLuaObject();
		void DestroyLuaObject();

		CGameObject *Parent;
		CAbstractScene *Scene;
		CScript *Script;
		ChildrenContainer Children;
		bool Prototype;
		bool Active;

		// not used
		bool Dead;	
		bool Enabled;

		friend class CFactory;
	};

}	//	namespace Deku2D

#endif	//	_2DE_GAME_OBJECT_H_
