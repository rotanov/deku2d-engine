#ifndef _2DE_GUI_H_
#define _2DE_GUI_H_

#include "2de_GraphicsHigh.h"

/**
*	CGUIStyle - класс, представляющий стиль GUI, содержащий цвета и размеры стандартных элементов GUI.
*	Для создания нового стиля можно либо наследовать и перегрузить конструктор, либо инстанцировать и изменить нужные значения.
*/

//	небольшой комментарий: возможно, кому-то не понравится эта идея, возможно,
//	кто-то любит разукрашивать разные кнопки в разные цвета - и т. п.
//		я предлагаю такое решение: у каждого элемента будет указатель на стиль
//		поэтому, можно будет создать по стилю на каждую кнопку и указать его
//		или же наследовать кнопку и перегрузить в ней конструктор, который будет создавать определённый стиль
//		вобщем - гибкость рулит, но и унификация стиля - хорошая вещь

class CGUIStyle
{
public:
	struct CGUIStyleColors
	{
		RGBAf FocusRect;
		RGBAf ButtonFace;
		RGBAf ButtonFaceHovered;
		RGBAf ButtonFacePressed;
		RGBAf ButtonBorder;
		RGBAf ButtonBorderHovered;
		RGBAf ButtonBorderPressed;
		RGBAf ButtonText;
		RGBAf ButtonInactiveText;
		RGBAf EditBackground;
		RGBAf EditBackgroundHovered;
		RGBAf EditBorder;
		RGBAf EditBorderHovered;
		RGBAf EditText;
		RGBAf EditInactiveText;
		RGBAf EditSelection;
		RGBAf LabelText;
	};
	struct CGUIStyleMetrics
	{
		float FocusRectSpacing;
		float FocusRectLineWidth;
		Vector2 EditMargins;
		float EditBorderWidth;
	};
	/*struct CGUIStyleFonts
	{
		CFont* ButtonFont;
		CFont* EditFont;
	};*/

	CGUIStyle();

	CGUIStyleColors Colors;
	CGUIStyleMetrics Metrics;
	CResourceRefCounter<CFont> Font;
	//CGUIStyleFonts Fonts;

	// @todo: loading style from XML file (and even maybe saving)
};

class CGUIObjectBase : /*public CRenderable*/ public CRenderableComponent
{
public:
	struct CMouseState
	{
		bool Hovered;
		bool Pressed;
		bool PressedInside;
		bool PressedOutside;
	};

	CGUIObjectBase();
	~CGUIObjectBase();
	CFont* GetFont() const;
	void SetFont(CFont *AFont);
	CGUIStyle* GetStyle() const;
	void SetStyle(CGUIStyle *AStyle);
	Vector2 GlobalToLocal(const Vector2& Coords) const;

protected:
	CObject *Caller;
	CFont *Font;
	CGUIStyle *Style;
	CMouseState MouseState;
	CMouseState PreviousMouseState;
	CMouseState WidgetState;
};

/**
* CGUIRootObject - everyone's father.
*/

class CGUIRootObject : public CGUIObjectBase
{
public:
	CGUIRootObject();
	void Update(float dt);
	bool TabHolded; // temporary, of course..
private:
	int KeyHoldRepeatDelay;
	int KeyHoldRepeatInterval;
	int TimerAccum;
	bool RepeatStarted;
};

class CGUIObject : public CGUIObjectBase
{
public:
	CGUIObject();
	CGUIObject(CGUIObjectBase *AParent);
	~CGUIObject();
	bool isFocused() const;
	void Focus();
	void SetParent(CGUIObjectBase *AParent);
	const string& GetText() const;
	void SetText(const string &AText);
	void SetBox(const CBox &box);
	virtual void SetVisibility(bool AVisible);

protected:
	CText Text;
	CGUIObjectBase *Parent;
};

class CGUIManager : public CCommonManager <list <CGUIObject*> >, public CTSingleton<CGUIManager>
{
public:
	~CGUIManager();
	void ProcessEvent(const CEvent &AEvent);
	CGUIObject* GetFocusedObject() const;
	void SetFocus(CGUIObject *AObject);
	CGUIRootObject* GetRoot() const;
	void Add(CGUIObject *AObject);

private:
	ManagerIterator Focus;
	CGUIRootObject *Root;

protected:
	CGUIManager();
	friend class CTSingleton<CGUIManager>;
};

class CLabel : public CGUIObject
{
public:
	CLabel(const string &AText = "");
	void SetBox(const CBox &box);
	void Render();
	void Update(float dt){}
};

class CButton : public CGUIObject
{
public:
	CButton();
	CButton(CBox ARect, const char* AText, RGBAf AColor);
	void SetBox(const CBox &box);
	void Render();
	void Update(float dt);
	void ProcessEvent(const CEvent &AEvent);
};

class CEdit : public CGUIObject
{
public:
	class CTextSelection
	{
	public:
		int Start;
		int End;
		CTextSelection();
		CTextSelection(int AStart, int AEnd);
		void Set(int AStart, int AEnd);
		bool Exists() const;
		int RangeStart() const;
		int RangeEnd() const;
		int Length() const;
		void Clear();
		void Clear(int ACursorPos);
	};

	CEdit();
	void SetBox(const CBox &box);
	void SetText(const string &AText);
	const string& GetText() const;
	void Render();
	void Update(float dt);
	void ProcessEvent(const CEvent &AEvent);

private:
	string ActualText;
	int MouseToCursorPos(const Vector2& MousePosition) const;
	string GetVisibleText() const;
	bool isTextFits(const char *AText) const;
	int CursorPos;
	CTextSelection Selection;
	int VisibleTextOffset;
};

class CLabeledEdit : public CEdit
{
private:
	CLabel Label;
public:
	void SetBox(const CBox &box);
	CLabeledEdit(CBox Aaabb, const string &ALabelText);
};

class CMenuItem : public CGUIObject
{
public:
	bool isCycledMenuSwitch;

	CMenuItem();
	CMenuItem(CMenuItem *AParent, const string &AMenuText);
	~CMenuItem();
	void Render();
	void Update(float dt);
	void ProcessEvent(const CEvent &AEvent);
	bool AddObject(CMenuItem *AObject);

private:
//	CCommonManager <list <CMenuItem*> >::ManagerIterator Focus;
};

class CPanel : CGUIObject
{

};

#endif // _2DE_GUI_H_
