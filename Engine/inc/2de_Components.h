#ifndef _2DE_COMPONENTS_H_
#define _2DE_COMPONENTS_H_

#include "2de_Core.h"
#include "2de_GraphicsLow.h"
#include "2de_LuaUtils.h"
#include "2de_Resource.h"
#include "2de_Xml.h"

#if defined(GetClassName)
	#undef GetClassName
#endif

/**
* CGameObject - the base class for all components.
*/

class CGameObject : public IVisitableObject<>
{
	friend class CFactory;
public:	
	typedef std::map< std::string, CGameObject* > LNOMType;	// ???
	LNOMType LocalNameObjectMapping; // ???

	D2D_DECLARE_VISITABLE()
	CGameObject();
	virtual ~CGameObject();
	void DFSIterate(CGameObject *Next, IVisitorBase *Visitor); // Not here
	void Attach(CGameObject* AGameObject);
	void Detach(CGameObject* AGameObject);
	void Detach(unsigned index);
	void SetScript(CScript *AScript);
	void FinalizeCreation();
	void ProcessEvent(const CEvent &AEvent);
	const string& GetClassName() const;
	void SetClassName(const string & AClassName);
	CGameObject* GetParent() const;
	void SetParent(CGameObject* AGameObject);
	void PutIntoScene(CAbstractScene *AScene);
	CAbstractScene* GetScene() const;
	virtual void JustDoIt();
	virtual void Deserialize(CXMLNode *AXML);
	void SetDestroyedSubtree();
	CGameObject* GetChild(unsigned index);
	unsigned GetChildCount();
	bool IsActive() const;
	void SetActive(bool AActive);
// 	bool isDead() const;	// @todo: Think about applyng this part of CUpdatable interface into CGameObject
// 	void SetDead();
//	virtual void Update(float dt) = 0;

private:
	CGameObject *Parent;
	CAbstractScene *Scene;
	vector<CGameObject *> Children;
	string ClassName;
	// not used
	bool Active;
	bool Dead;	
	bool Enabled;

	static void _DestroySubtree(CGameObject *NextObject);
};

/**
*	Serves as transform node in transform tree.
*/
class CPlaceableComponent : public CGameObject
{
public:
	D2D_DECLARE_VISITABLE()
	CPlaceableComponent();

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

public: // TODO: TEMPORARILITYTRIORUOYTORT I was need to make transform between origins
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


//protected:
	//KLUDGE!!!
public:
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
	// Treat as CRenderableComponent for now
	//D2D_DECLARE_VISITABLE()

	CText();
	CText(const string &AText);
	~CText();

	CFont* GetFont() const;
	string& GetText();
	const string& GetText() const;
	void SetFont(CFont *AFont);
	void SetText(const string &AText);
	CText& operator =(const string &AText);
	unsigned char operator [] (unsigned index) const;
	// Such function could be there no more. Text has no info about box and world position now, But We can generate info 'bout box anytime instead
	//float StringCoordToCursorPos(int x, int y) const;	// Тот кто это писал - объясни, зачем нам передавать "y"?
	unsigned Length() const;

	void Deserialize(CXMLNode *AXML);

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

	CTimerComponent();
	CTimerComponent(float AInterval);

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

#endif // _2DE_COMPONENTS_H_
