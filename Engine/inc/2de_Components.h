#ifndef _2DE_COMPONENTS_H_
#define _2DE_COMPONENTS_H_

#include "2de_Core.h"
#include "2de_GraphicsLow.h"
#include "2de_LuaUtils.h"
#include "2de_Resource.h"
#include "2de_Xml.h"

namespace Deku2d
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

		CGameObject* FindFirstOfClass(const string &AClassName, bool ExceedPrototype = false);

		bool IsActive() const;
		void SetActive(bool AActive);

		bool IsPrototype() const;

		virtual void Deserialize(CXMLNode *AXML);
		void FinalizeCreation();
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
			CFactory::Instance()->Add(AObject, ACloneName.empty() ? GetName() + "copy" + itos(AObject->GetID()) : ACloneName);	// may be names will be too long..
			AObject->SetScript(AObject->GetScript());
			AObject->FinalizeCreation();	// i'm not sure about this.. this is wrong for prototypes, i think.. // but it works..
			return AObject;
		}

	private:
		typedef vector<CGameObject *> ChildrenContainer;
		typedef ChildrenContainer::iterator ChildrenIterator;
		typedef ChildrenContainer::const_iterator ChildrenConstIterator;

		static void _DestroySubtree(CGameObject *NextObject);
		CGameObject* FindPrototype();
		void UpdateParentAndProtoFields();
		void CreateLuaObject();

		CGameObject *Parent;
		CAbstractScene *Scene;
		CScript *Script;
		ChildrenContainer Children;
		string ClassName;
		bool Prototype;
		bool Created;
		bool Active;

		// not used
		bool Dead;	
		bool Enabled;

		friend class CFactory;
	};

	/**
	*	Serves as transform node in transform tree.
	*/
	class CPlaceableComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CPlaceableComponent();

		CPlaceableComponent* Clone(const string &ACloneName = "") const;

		float GetAngle() const;
		const CBox& GetBox() const;
		float GetDepth() const;
		int GetLayer() const;	
		float GetScaling() const;
		const Vector2& GetPosition() const;
		Vector2& GetPosition();

		void SetAngle(float AAngle); //	(Degrees)

		// temp
		void SetBox(const CBox &ABox);

		/**
		*	Layers should be from SOME_NEGATIVE_VALUE to SOME_POSITIVE_VALUE. Layer with greater number is drawn over layer with lower one.
		*	implicitly Depth => [-1; 1]?	
		*/
		void SetLayer(int Layer);
		void SetScaling(float AScaling);
		void SetPosition(const Vector2 &APosition);

		CTransformation& GetTransformation();
		const CTransformation& GetTransformation() const;
		void SetTransformation(const CTransformation &ATransformation);

		bool isMirrorHorizontal() const;
		void SetMirrorHorizontal(bool MirrorOrNot);
		bool isMirrorVertical() const;
		void SetMirrorVertical(bool MirrorOrNot);
		bool isIgnoringParentTransform() const;
		void SetIgnoreParentTransform(bool doIgnore);
		void UpdateBox(const CBox& ABox);
		void Deserialize(CXMLNode *AXML);

	private:
		CBox Box;
		
		bool doIgnoreCamera;	// all previous transformations are ignored if true
		bool doMirrorHorizontal;
		bool doMirrorVertical;

		CTransformation Transformation;
	};

	/**
	 *	CGeometricComponent - class of component that represents geometrical interpretation of some object
	 *	It has to have some connection with something. Obvious "something" in this case is a connection with
	 *	corresponding physical component. But there are some questions:
	 *		a) Is there some other, non-physical component?
	 *		b) How to support this kind of relation?
	 */
	class CGeometricComponent : public CGameObject	// "Geometric" or "Geometrical"?
	{
	public:
		D2D_DECLARE_VISITABLE()

		CGeometricComponent() : Box(0, 0, 0, 0)
		{

		}

		// Danger: When we use it as arg to DrawSolidBox() then it apply scaling two times. @todo: fix this <--
		// why const CBox? it's a copy of CBox object, not a reference, why should it be const?..	
		const CBox GetBox() const
		{
			CBox TempBox = Box;
	//		TempBox.Min *= GetScaling();	// FUCK FUCK FUCK
	//		TempBox.Max *= GetScaling();
	//		TempBox.Offset(GetPosition());
			//Box.RotateByAngle(Angle);
			return TempBox;
		}

		virtual void SetBox(const CBox &ABox)
		{
			Box = ABox;
		}

		float Width()
		{
			return Box.Width();
		}

		float Height()
		{
			return Box.Height();
		}
		

	private:
		CBox Box; //	Axis Aligned Bounding Box for culling
	};

	/**
	* CRenderableComponent - AOP way replacement for CRenderable. // these historical notes are very very funny, but we need to write the actual documentation, not the tales of the past..
	*/

	class CRenderableComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CTransformation WorldTransform;

		CRenderableComponent(CModel *AModel = NULL);
		virtual ~CRenderableComponent();

		CRenderableComponent* Clone(const string &ACloneName = "") const;

		const CBox& GetBox() const;
		void SetBox(const CBox& ABox);
		bool GetVisibility() const;
		virtual void SetVisibility(bool AVisible);
		const RGBAf& GetColor() const;
		RGBAf& GetColor();
		void SetColor(const RGBAf &AColor);
		EBlendingMode GetBlendingMode() const;
		void SetBlendingMode(EBlendingMode ABlendingMode);
		CModel* GetModel() const;
		void SetModel(CModel *AModel);
		CRenderConfig& GetConfiguration();
		const CRenderConfig& GetConfiguration() const;
		void SetConfiguration(const CRenderConfig &AConfiguraton);
		void Deserialize(CXMLNode *AXML);

		CBox Box;
		void UpdateBox(const CBox& ABox);

	protected:
		CRenderableComponent(const CRenderableComponent &ARenderableComponent);

		CResourceRefCounter<CModel> Model;

	private:
		CRenderConfig Configuration;
		bool Visible;	
	};

	class CDebugBoxComponent : public CRenderableComponent
	{
	public:
		D2D_DECLARE_VISITABLE()

		CDebugBoxComponent()
		{
			CRenderConfig NewConfiguration = GetConfiguration();
			NewConfiguration.doIgnoreTransform = true;
			SetConfiguration(NewConfiguration);
		}
		virtual ~CDebugBoxComponent(){}
	};

	/**
	*	Text.
	*	Shouldn't render itself, should be inherited from CRenderableComponent, not Renderable
	*	So it will have Model and Configuration and there will become some unclear stuff.
	*	Cause CModel wants to be Resource.
	*/

	class CText : public CRenderableComponent
	{
	public:
		// Treat as CRenderableComponent for now, if not uncomment the following and support visitors with one more branch
		//D2D_DECLARE_VISITABLE()

		CText();
		CText(const string &AText);
		~CText();

		CText* Clone(const string &ACloneName = "") const;

		CFont* GetFont() const;
		string& GetText();
		const string& GetText() const;
		void SetFont(CFont *AFont);
		void SetText(const string &AText);
		CText& operator =(const string &AText);
		unsigned char operator [] (unsigned index) const;
		unsigned Length() const;
		void Deserialize(CXMLNode *AXML);

	protected:
		CText(const CText &AText);

	private:
		string Characters;
		CResourceRefCounter<CFont> Font;

		void _UpdateSelfModel();
	};

	/**
	* CTimerComponent - component that represents a timer.
	*/

	class CTimerComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CTimerComponent(float AInterval = 0.0f);

		CTimerComponent* Clone(const string &ACloneName = "") const;

		void ProcessEvent(const CEvent &AEvent);

		bool isEnabled() const;
		void SetEnabled(bool AEnabled);

		float GetInterval() const;
		void SetInterval(float AInterval);

		void Deserialize(CXMLNode *AXML);

	private:
		bool Enabled;
		float Interval;
		float Accumulated;

	};

	/**
	* CPrototype - resource that represents XML prototype - hierarchy of components.
	*/

	class CPrototype : public CResource
	{
	public:
		CPrototype();
		~CPrototype();

		bool Load();
		void Unload();

		CXMLNode* GetRootNode();

	private:
		CXML XML;

	};

}	//	namespace Deku2d

#endif // _2DE_COMPONENTS_H_
