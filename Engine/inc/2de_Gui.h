#ifndef _2DE_GUI_H_
#define _2DE_GUI_H_

#include "2de_GraphicsHigh.h"

//////////////////////////////////////////////////////////////////////////
//Gui 

/**
 * CGUIStyle - класс, представляющий стиль GUI, содержащий цвета и размеры стандартных элементов GUI.
 *
 * Для создания нового стиля можно либо наследовать и перегрузить конструктор, либо инстанцировать и изменить нужные значения.
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
	// на данный момент я буду юзать один шрифт для всего ГУИ для простоты
	// и потому что некоторые моменты использования шрифтов не до коцна ясны и требуют пересмотра
	/*struct CGUIStyleFonts
	{
		CFont* ButtonFont;
		CFont* EditFont;
	};*/

	CGUIStyle()
	{
		// default style values - very ugly style :) I'm programmer, not fucking "эстет" :) you're welcome to fix colors to more beautiful ones

		Colors.FocusRect = RGBAf(0.5f, 0.5f, 0.5f, 1.0f);
		Colors.ButtonFace = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonFaceHovered = RGBAf(0.6f, 0.6f, 0.6f, 1.0f);
		Colors.ButtonFacePressed = RGBAf(0.45f, 0.45f, 0.45f, 1.0f);
		Colors.ButtonFacePressed = RGBAf(0.45f, 0.45f, 0.45f, 1.0f);
		Colors.ButtonBorder = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonBorderHovered = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonBorderPressed = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonText = COLOR_BLACK;
		Colors.ButtonInactiveText = RGBAf(0.2f, 0.2f, 0.2f, 1.0f);
		Colors.EditBackground = COLOR_WHITE;
		Colors.EditBackgroundHovered = COLOR_WHITE;
		Colors.EditBorder = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.EditBorderHovered = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.EditText = COLOR_BLACK;
		Colors.EditInactiveText = RGBAf(0.2f, 0.2f, 0.2f, 1.0f);
		Colors.EditSelection = RGBAf(0.0f, 0.4f, 0.8f, 0.5f);
		Colors.LabelText = COLOR_WHITE;

		Metrics.FocusRectSpacing = 5.0f;
		Metrics.FocusRectLineWidth = 0.5f;
		Metrics.EditMargins = Vector2(4.0f, 6.0f);
		Metrics.EditBorderWidth = 2.0f;

		Font = CFontManager::Instance()->GetDefaultFont();
	}
	CGUIStyleColors Colors;
	CGUIStyleMetrics Metrics;
	CFont *Font;
	//CGUIStyleFonts Fonts;

	// @todo: loading style from XML file (and even maybe saving)
};

class CGUIObjectBase : public CRenderable, public CUpdatable
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
	CPrimitiveRender* GetPrimitiveRender() const;
	void SetPrimitiveRender(CPrimitiveRender *APrimitiveRender);
	CGUIStyle* GetStyle() const;
	void SetStyle(CGUIStyle *AStyle);
	void SetCallback(CObjectCallback ACallProc, CObject *ACaller);
	Vector2 GlobalToLocal(const Vector2& Coords) const;
//	virtual void SetVisibility(bool AVisible);

protected:
	CObjectCallback CallProc;	//	Указатель на пользовательскую коллбэк ф-ю,
								//	будет вызываться для каждого наследника по своему
	CObject *Caller;
	// вот зачем эти очвевидные комментарии? неужели кому-то не понятно,
	// что CFont *Font - это указатель на шрифт?
	// нет, блядь, это наверное число ядерных распадов на Солнце с момента создания объекта....
	//	Я оставил их тут, потому что у меня есть хитрый план. Непосвящённые не знают.
	CFont *Font;	//	Указатель на шрифт.
					// deprecated, я считаю, ибо всегда можно взять из стиля
	CPrimitiveRender *PRender;	//	Указатель на рендер примитивов.
	CGUIStyle *Style;
	CMouseState MouseState;
	CMouseState PreviousMouseState;
	CMouseState WidgetState;
};

/**
 * CGUIRootObject - корневой объект GUI, всеобщий родитель.
 */

class CGUIRootObject : public CGUIObjectBase
{
public:
	CGUIRootObject();
	void Render();
	void Update(float dt);
	bool TabHolded; // temporary, ofcourse..
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

	const string& GetText() const
	{
		return Text.GetText();
	}

	void SetText(const string &AText)
	{
		Text = AText;
		Text.Position = ((GetBox().Min + GetBox().Max) - Vector2(Text.Width(), Text.Height())) * 0.5f;
	}

	void SetBox(const CBox &box)
	{
		CRenderable::SetBox(box);
		Text.SetLayer(10);
		// maybe smthng else
	}

	virtual void SetVisibility( bool AVisible );

protected:
	CText Text;
	CGUIObjectBase *Parent;		//	Указатель на родительский объект. На будущее; иерархии виджетов пока нет
					// 	ну она как бы есть, но не совсем иерархия.. и да, родителем объекта может быть и Base, поэтому тут будет он
};

// вот этот класс (CGUIManager) наследован одновременно и от синглтона,
// и от CGUIObjectBase (который CUpdateObject и CRenderObject)..
// получаем всякие гадости в логах при удалении, потому что его сначала
// удаляет синглтон-киллер, а потом пытается удалить апдейт-менеджер и т. д.
class CGUIManager : public CCommonManager <list <CGUIObject*> >, public CTSingleton<CGUIManager>
{
public:
	~CGUIManager();
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
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
	void SetBox(const CBox &box)
	{
		CGUIObject::SetBox(box);
		Text.Position = Vector2(box.Min.x, box.Min.y);
	}
	void Render();
	void Update(float dt){}
};

class CButton : public CGUIObject
{
public:
	CButton();
	CButton(CBox ARect, const char* AText, RGBAf AColor);
	void SetBox(const CBox &box)
	{
		CGUIObject::SetBox(box);
		Text.Position.x = (int)((box.Min + box.Max) / 2.0f - Vector2(Text.Width(), Text.Height()) / 2.0f).x;
		Text.Position.y = (int)((box.Min + box.Max) / 2.0f - Vector2(Text.Width(), Text.Height()) / 2.0f).y;
	}
	void Render();
	void Update(float dt);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod, char letter);
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
	const string& GetText() const
	{
		return ActualText;
	}
	void Render();
	void Update(float dt);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod, char letter);

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
	void SetBox(const CBox &box)
	{
		CEdit::SetBox(box);		
		CBox tempBox = box;
		tempBox.Offset(0.0f, box.Height() + 2.0f);
		int TextHeight = Label.GetFont()->GetStringHeight(Label.GetText());
		tempBox.Max.y = tempBox.Min.y + TextHeight;
		Label.SetBox(tempBox);
	}
	CLabeledEdit(CBox Aaabb, const string &ALabelText) : Label(ALabelText)
	{
		SetBox(Aaabb);
	}
};

class CMenuItem : public CGUIObject, public CCommonManager <list <CMenuItem*> >/*public CList*/
{
public:
	bool isCycledMenuSwitch;

	CMenuItem();
	CMenuItem(CMenuItem* AParent, char* AMenuText);
	~CMenuItem();
	void Render();
	void Update(float dt);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	bool AddObject(CMenuItem *AObject);

private:
	CCommonManager <list <CMenuItem*> >::ManagerIterator Focus;
};

//////////////////////////////////////////////////////////////////////////
//CPanel

class CPanel : CGUIObject
{

};
#endif // _2DE_GUI_H_