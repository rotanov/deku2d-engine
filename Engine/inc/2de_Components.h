#ifndef _2DE_COMPONENTS_H_
#define _2DE_COMPONENTS_H_

#include "2de_Core.h"
#include "2de_GraphicsLow.h"
#include "2de_LuaUtils.h"
#include "2de_Resource.h"
#include "2de_Xml.h"

/**
* CGameObject - the base class for all components.
*/

class CGameObject : public CObject
{
private:

	class traverse_iterator
	{
	protected:
		CGameObject *GameObject;

		bool IsEnd() const;
		bool IsValid() const;

	public:
		traverse_iterator(CGameObject &AGameObject);
		CGameObject& operator *();
		const CGameObject& operator *() const;
	};
	
	CAbstractScene *Scene;

public:	
	bool Active;
// 	bool isDead() const;	// @todo: Think about applyng this part of CUpdatable interface into CGameObject
// 	void SetDead();
//	virtual void Update(float dt) = 0;
//	void PutIntoScene(CAbstractScene *AScene);
// 	CAbstractScene* GetScene() const;
// 	CAbstractScene *Scene;
	bool Dead;	

	CGameObject *Parent;
	std::vector<CGameObject*> Children;

	class traverse_iterator_bfs : public traverse_iterator
	{
	private:
		std::queue<CGameObject*> Queue;
		unsigned int Index; // Index of current object in it's parent Children

	public:
		traverse_iterator_bfs(CGameObject &AGameObject);

		bool Ok();

		traverse_iterator_bfs& operator++();
		traverse_iterator_bfs& operator--();

		bool operator ==(const traverse_iterator_bfs &rhs) const;
		bool operator !=(const traverse_iterator_bfs &rhs) const;
	};

	class traverse_iterator_dfs : public traverse_iterator
	{
	private:
		std::vector<std::pair<CGameObject*, unsigned int> > Path;

	public:
		traverse_iterator_dfs(CGameObject &AGameObject) : traverse_iterator(AGameObject)
		{

		}

	};

	CGameObject();
	virtual ~CGameObject();

	void Attach(CGameObject* AGameObject);

	void SetParent(CGameObject* AGameObject);

	template<typename TypeIterator>
	void Detach(const TypeIterator &Iterator)
	{
		(*Iterator)->Parent = NULL;
		std::swap(*Iterator, *(--Children.end()));
		Children.pop_back();
	}

	void PutIntoScene(CAbstractScene *AScene);
	CAbstractScene* GetScene() const;
	virtual void JustDoIt();

	virtual void Deserialize(CXMLNode *AXML);
};

// SUDDENLY NEW COMPONENT APPEARS
class CPlaceableComponent : public CGameObject
{
public:
	CPlaceableComponent();
	float GetAngle() const;
	float GetDepth() const;
	int GetLayer() const;	
	float GetScaling() const;
	const Vector2& GetPosition() const;
	Vector2& GetPosition();

	void SetAngle(float AAngle); //	(Degrees)

	void SetScaling(float AScaling);
	/**
	*	Layers should be from SOME_NEGATIVE_VALUE to SOME_POSITIVE_VALUE. Layer with greater number is drawn over layer with lower one.
	*	implicitly Depth => [-1; 1]?	
	*/
	void SetLayer(int Layer);
	void SetPosition(const Vector2 &APosition);

	void SetTransformation(const CTransformation &ATransformation);
	CTransformation& GetTransformation();
	const CTransformation& GetTransformation() const;
	bool isMirrorVertical() const;
	void SetMirrorVertical(bool MirrorOrNot);
	bool isMirrorHorizontal() const;
	void SetMirrorHorizontal(bool MirrorOrNot);
	bool isIgnoringParentTransform() const;
	void SetIgnoreParentTransform(bool doIgnore);

private:
	CTransformation Transformation;
	bool doIgnoreCamera;	// all previous transformations are ignored if true
	bool doMirrorHorizontal;
	bool doMirrorVertical;

};

// SUDDENLY ONE MORE COMPONENT APPEALS
class CGeometricComponent : public CGameObject	// "Geometric" or "Geometrical"?
{
public:
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
	CRenderableComponent(CModel *AModel = NULL);
	virtual ~CRenderableComponent();
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

private:
	CRenderConfig Configuration;
	CResourceRefCounter<CModel> Model;
	bool Visible;	
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
	CText();
	~CText();
	CText(const string &AText);
	CFont* GetFont() const;
	string& GetText();
	const string& GetText() const;
	void SetFont(CFont *AFont);
	void SetText(const string &AText);
	CText& operator =(const string &AText);
	unsigned char operator [] (unsigned int index) const;
	// Such function could be there no more. Text has no info about box and world position now, But We can generate info 'bout box anytime instead
	//float StringCoordToCursorPos(int x, int y) const;	// Тот кто это писал - объясни, зачем нам передавать "y"?
	unsigned int Length() const;

private:
	string Characters;
	CResourceRefCounter<CFont> Font;

	void _UpdateSelfModel();
};

/**
* CScriptableComponent - component that is intended to make its parent scriptable.
*/

class CScriptableComponent : public CGameObject
{
public:
	CScriptableComponent();

	void SetScript(CScript *AScript);

	void ProcessEvent(const CEvent &AEvent);

	void Deserialize(CXMLNode *AXML);
};

/**
* CTimerComponent - component that represents a timer.
*/

class CTimerComponent : public CGameObject
{
public:
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
