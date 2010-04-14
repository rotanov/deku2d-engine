#ifndef _2DE_GUI_H_
#define _2DE_GUI_H_

#include "2de_GraphicsHigh.h"

//////////////////////////////////////////////////////////////////////////
//Gui 

/**
*	Поскольку Gui петра хуй попользуешься, так как он развёз какую-то еботу 
*	и не может доделать её уже год (сейчас август 09го), то тут будет мини гуи;
*	это будет маленький и удобный и, я надеюсь, временный набор классов. Пока что
*	будет Edit, Button и менеджер. Если будет пиздец, то буду его расширять, появятся табы
*	и всякие свистоперделки жизнено необходимые.
*/

/**
*	11.03.2010 Update. GUI Петра уничтожен. 
*	>Если будет пиздец, то буду его расширять, появятся...
*	Да, буду.
*/

/**
 * CGUIStyle - класс, представляющий стиль GUI, содержащий цвета и размеры стандартных элементов GUI.
 *
 * Для создания нового стиля можно либо наследовать и перегрузить конструктор, либо инстанцировать и изменить нужные значения.
 */

// небольшой комментарий: возможно, кому-то не понравится эта идея, возможно, кто-то любит разукрашивать разные кнопки в разные цвета - и т. п.
// 			  я предлагаю такое решение: у каждого элемента будет указатель на стиль
// 			  поэтому, можно будет создать по стилю на каждую кнопку и указать его
// 			  или же наследовать кнопку и перегрузить в ней конструктор, который будет создавать определённый стиль
// 			  вобщем - гибкость рулит, но и унификация стиля - хорошая вещь

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
	// на данный момент я буду юзать один шрифт для всего ГУИ для простоты и потому что некоторые моменты использования шрифтов не до коцна ясны и требуют пересмотра
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

		Font = CFontManager::Instance()->GetFont("Font");
	}
	CGUIStyleColors Colors;
	CGUIStyleMetrics Metrics;
	CFont *Font;
	//CGUIStyleFonts Fonts;

	// TODO: loading style from XML file (and even maybe saving)
};

class CGUIObjectBase : public CRenderObject, public CUpdateObject
{
public:
	struct CMouseState
	{
		bool Hovered;
		bool Pressed;
		bool PressedInside;
		bool PressedOutside;
	};
	string				Text;

	CGUIObjectBase();
	~CGUIObjectBase();

	CFont* GetFont() const;
	void SetFont(CFont *AFont);

	CPrimitiveRender* GetPrimitiveRender() const;
	void SetPrimitiveRender(CPrimitiveRender *APrimitiveRender);
	
	CGUIStyle* GetStyle() const;
	void SetStyle(CGUIStyle *AStyle);

	void SetCallback(CObjectCallback ACallProc, CObject *ACaller)
	{
		Caller = ACaller;
		CallProc = ACallProc;
	}

	Vector2 GlobalToLocal(const Vector2& Coords) const;
protected:
	CObjectCallback		CallProc;	//	Указатель на пользовательскую коллбэк ф-ю, будет вызываться для каждого наследника по своему
	CObject			*Caller;

				// вот зачем эти очвевидные комментарии? неужели кому-то не понятно, что CFont *Font - это указатель на шрифт?
				// нет, блядь, это наверное число ядерных распадов на Солнце с момента создания объекта....
				//	Я оставил их тут, потому что у меня есть хитрый план. Непосвящённые не знают.
	CFont				*Font;				//	Указатель на шрифт. // deprecated, я считаю, ибо всегда можно взять из стиля
	CPrimitiveRender	*PRender;			//	Указатель на рендер примитивов.
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

protected:
	CGUIObjectBase *Parent;		//	Указатель на родительский объект. На будущее; иерархии виджетов пока нет
					// 	ну она как бы есть, но не совсем иерархия.. и да, родителем объекта может быть и Base, поэтому тут будет он
};

// вот этот класс (CGUIManager) наследован одновременно и от синглтона, и от CGUIObjectBase (который CUpdateObject и CRenderObject)..
// получаем всякие гадости в логах при удалении, потому что его сначала удаляет синглтон-киллер, а потом пытается удалить апдейт-менеджер и т. д.
class CGUIManager : public CTSingleton<CGUIManager>, public CUpdateObject
{
public:
				~CGUIManager();
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	void		Update(float dt);
	CGUIObject* GetFocusedObject() const;
	void		SetFocusedNodeTo(CListNode *AFocusedNode);
	void		SetFocus(CGUIObject *AObject);
	CGUIRootObject* GetRoot() const;
	void AddObject(CGUIObject *AObject);
	CGUIObject* GetObject(const string &AObjectName) const;
	void DeleteObject(int AId);
private:
	int			KeyHoldRepeatDelay;				// множественный костыль! TODO: fix
	CListNode	*FocusedOnListNode;
	CGUIObject	*FocusedOn;
	int			KeyHoldRepeatInterval;
	int			TimerAccum;
	bool		tabholded;
	bool		repeatstarted;
	CList List;
	CGUIRootObject *Root;
protected:
	CGUIManager();
	friend class CTSingleton<CGUIManager>;
};

class CLabel : public CGUIObject
{
public:
	CLabel(const string &AText = "");
	void Render();
	void Update(float dt){}
};

class CButton : public CGUIObject
{
public:
	CButton();
	CButton(CAABB ARect, const char* AText, RGBAf AColor);
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
	void Render();
	void Update(float dt);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod, char letter);

private:
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
	CLabeledEdit(CAABB Aaabb, const string &ALabelText) : Label(ALabelText)
	{
		aabb = Aaabb;
		Label.aabb = aabb;
		Label.aabb.Offset(0.0f, aabb.Height() + 2.0f);
		int TextHeight = Label.GetFont()->GetStringHeight(Label.Text.c_str());
		Label.aabb.vMax.y = Label.aabb.vMin.y + TextHeight;
	}
};

class CMenuItem : public CGUIObject, public CList
{
public:
	bool		isCycledMenuSwitch;
				CMenuItem();
				CMenuItem(CMenuItem* AParent, char* AMenuText);
				~CMenuItem();
	void		Render();
	void		Update(float dt);
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	bool		AddObject(CObject *AObject);
private:
	CMenuItem*	FocusedOnItem;
	CListNode*	FocusedOnListNode;
};

#endif // _2DE_GUI_H_
