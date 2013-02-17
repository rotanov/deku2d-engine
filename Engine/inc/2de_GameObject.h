#ifndef _2DE_GAME_OBJECT_H_
#define _2DE_GAME_OBJECT_H_

#include "2de_Core.h"
#include "2de_Event.h"
#include "2de_GraphicsLow.h"
#include "2de_LuaUtils.h"
#include "2de_Visitor.h"

namespace Deku2D
{
	// shouldn't be here..
	#if defined(GetClassName)
		#undef GetClassName
	#endif

	/**
	* CGameObject - the base class for all components.
	*/

	class CGameObject : public IVisitableObject
	{
		D2D_INJECT_TYPE_INFO(CGameObject);
		D2D_DECLARE_VISITABLE();

	public:	
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

		CGameObject* FindPrototype();

		void AddLocalName(const string &ALocalName, CGameObject *AChild);

		CGameObject* FindFirstOfClass(const string &AClassName, bool ExceedPrototype = false);

		bool IsActive() const;
		void SetActive(bool AActive);

		bool IsEnabled() const;
		void SetEnabled(bool AEnabled);

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

		typedef vector<CGameObject *> ChildrenContainer;
		typedef ChildrenContainer::iterator ChildrenIterator;
		typedef ChildrenContainer::const_iterator ChildrenConstIterator;

		void SetChildren( const ChildrenContainer& ) { throw "pizdec"; }
		ChildrenContainer GetChildren() const { throw "pizdec"; }

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

		static void _DestroySubtree(CGameObject *NextObject);
		void CreateLuaObject();
		void DestroyLuaObject();

		CGameObject *Parent;
		CAbstractScene *Scene;
		CScript *Script;
		ChildrenContainer Children;
		bool Prototype;

		// don't mix these two:
		// 	Active - means that an object is participating in current tree iteration,
		// 		it's internal flag that is rewritten on every iteration and should not be changed by general public
		// 	Enabled - is the flag that provides an ability to enable/disable an object unconditionally,
		// 		can be used anywhere by anyone

		bool Active;
		bool Enabled;

		// not used
		bool Dead;

		friend class CFactory;
	};

}	//	namespace Deku2D

#endif	//	_2DE_GAME_OBJECT_H_
