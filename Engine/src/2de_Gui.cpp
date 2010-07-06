#include "2de_Gui.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//GUI
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CGUIObjectBase

CGUIObjectBase::CGUIObjectBase()
{
	Font = NULL;
	PRender = NULL;
	Style = NULL;
	Text = "";
	CallProc = NULL;
	Caller = NULL;

	MouseState.Hovered = false;
	MouseState.Pressed = false;
	MouseState.PressedInside = false;
	MouseState.PressedOutside = false;
	PreviousMouseState.Hovered = false;
	PreviousMouseState.Pressed = false;
	PreviousMouseState.PressedInside = false;
	PreviousMouseState.PressedOutside = false;
	WidgetState.Hovered = false;
	WidgetState.Pressed = false;
	WidgetState.PressedInside = false;
	WidgetState.PressedOutside = false;
}

CGUIObjectBase::~CGUIObjectBase()
{
}

CFont* CGUIObjectBase::GetFont() const
{
	return Font;
}

void CGUIObjectBase::SetFont(CFont *AFont)
{
	Font = AFont;
}

CPrimitiveRender* CGUIObjectBase::GetPrimitiveRender() const
{
	return PRender;
}

void CGUIObjectBase::SetPrimitiveRender(CPrimitiveRender *APrimitiveRender)
{
	PRender = APrimitiveRender;
}


CGUIStyle* CGUIObjectBase::GetStyle() const
{
	return Style;
}

void CGUIObjectBase::SetStyle(CGUIStyle *AStyle)
{
	Style = AStyle;
}

Vector2 CGUIObjectBase::GlobalToLocal(const Vector2 &Coords) const
{
	Vector2 Result;
	Result.x = std::max(Coords.x - aabb.vMin.x, 0.0f);
	Result.y = std::max(Coords.y - aabb.vMin.y, 0.0f);
	return Result;
}

//////////////////////////////////////////////////////////////////////////
// CGUIRootObject

CGUIRootObject::CGUIRootObject()
{
	SetName("GUI Root Object");
}

void CGUIRootObject::Render()
{
	glLoadIdentity();
	CGUIObject *Focused = CGUIManager::Instance()->GetFocusedObject();
	if (Focused)
	{
		PRender->LineStippleEnabled = true;
		PRender->lwidth = Style->Metrics.FocusRectLineWidth;
		PRender->lClr = Style->Colors.FocusRect;
		PRender->grRectL(Focused->aabb.Inflated(Style->Metrics.FocusRectSpacing, Style->Metrics.FocusRectSpacing).vMin,
			Focused->aabb.Inflated(Style->Metrics.FocusRectSpacing, Style->Metrics.FocusRectSpacing).vMax);	
		PRender->LineStippleEnabled = false;	// bad.. object shouldn't "disable" any settings.. every object should just initialize it..
		PRender->lwidth = 1.0f;
	}
	return;
}

void CGUIRootObject::Update(float dt)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
// CGUIObject

CGUIObject::CGUIObject()
{
	CGUIManager::Instance()->AddObject(this);
	SetParent(CGUIManager::Instance()->GetRoot());
}

CGUIObject::CGUIObject(CGUIObjectBase *AParent)
{
	CGUIManager::Instance()->AddObject(this);
	SetParent(AParent);
}

CGUIObject::~CGUIObject()
{
	CGUIManager::Instance()->DeleteObject(GetID());
}

bool CGUIObject::isFocused() const
{
	return (CGUIManager::Instance()->GetFocusedObject() == this);
}

void CGUIObject::Focus()
{
	CGUIManager::Instance()->SetFocus(this);
}

void CGUIObject::SetParent(CGUIObjectBase *AParent)
{
	Parent = AParent;
	if (!AParent)
	{
		Parent = CGUIManager::Instance()->GetRoot();	// if parent feels bad, reparent GUI object to root object
		if (!Parent)					// even root object feels bad.. how terrible...
			return;
	}
	Style = Parent->GetStyle();
	Font = Parent->GetFont();
	PRender = Parent->GetPrimitiveRender();
	Color = Parent->Color;	// Color is deprecated for GUI objects (really not used in CButton, CEdit, but there is something related to Color in CMenuItem), use CGUIStyle instead
}

//////////////////////////////////////////////////////////////////////////
// CGUIManager

CGUIManager::CGUIManager(): KeyHoldRepeatDelay(300), KeyHoldRepeatInterval(50), TimerAccum(0), TabHolded(false)
{
	SetName("GUI Manager");
	Root = new CGUIRootObject;
	Root->SetStyle(new CGUIStyle);
	Root->SetPrimitiveRender(new CPrimitiveRender);
	Root->SetFont(Root->GetStyle()->Font);

	CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
}

void CGUIManager::Update(float dt)
{
#ifndef I_LIKE_HOW_SDL_KEY_REPEAT_WORKS
	if (TabHolded)
	{
		TimerAccum += dt*1000;
		if (!RepeatStarted)
		{
			if (TimerAccum > KeyHoldRepeatDelay)
			{
				TimerAccum = 0;
				RepeatStarted = true;
			}
		}
		else
		{
			if (TimerAccum > KeyHoldRepeatInterval)
			{
				TimerAccum = 0;
				InputHandling(KEY_PRESSED, SDLK_TAB, SDLMod(0), 0);
			}
		}
	}
	else
	{
		TimerAccum = 0;
		RepeatStarted = false;
	}
#endif
}

bool CGUIManager::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	
	switch(state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_TAB:
			TabHolded = true;
			if (Focus != List.end()) // if iterator correct
			{
				if (mod & KMOD_SHIFT)
					Focus--;
				else
					Focus++;									
			}

			if (Focus == List.end()) // if iterator incorrect totally i.e. pointing to illegal object
			{
				if (mod & KMOD_SHIFT)
					Focus = List.end();
				else
					Focus = List.begin();
			}
			break;
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_TAB:
			TabHolded =  false;
			break;
		}
		break;
	}

	(*Focus)->InputHandling(state, key, mod, letter);
		
	return true;
}

CGUIObject* CGUIManager::GetFocusedObject() const
{
	return *Focus;
}

void CGUIManager::SetFocus(CGUIObject* AObject)
{
	Focus = find(List.begin(), List.end(), AObject);;
}

CGUIRootObject* CGUIManager::GetRoot() const
{
	return Root;
}

void CGUIManager::AddObject(CGUIObject *AObject)
{
	List.push_back(AObject);
	Focus = List.begin();
}

CGUIObject* CGUIManager::GetObject(const string &AObjectName) const
{
	for(vector<CGUIObject*>::const_iterator i = List.begin(); i != List.end(); ++i)
		if ((*i)->GetName() == AObjectName)
			return *i;
	return NULL;
}

void CGUIManager::DeleteObject(int AId)
{
	//List.remove(AId);
}

CGUIManager::~CGUIManager()
{
	delete Root->GetPrimitiveRender();
	delete Root->GetStyle();
	//delete Root;
}


//////////////////////////////////////////////////////////////////////////
// CLabel

CLabel::CLabel(const string &AText /*= ""*/)
{
	Text = AText;
}

void CLabel::Render()
{
	CEngine *engine = CEngine::Instance();

	Font->tClr = Style->Colors.LabelText;
	Font->Pos.x = aabb.vMin.x; //(int)((aabb.vMin + aabb.vMax) / 2.0f - Vector2(Font->GetStringWidth(Text.c_str()), Font->GetStringHeight(Text.c_str())) / 2.0f).x;
	Font->Pos.y = aabb.vMin.y; //(int)((aabb.vMin + aabb.vMax) / 2.0f - Vector2(Font->GetStringWidth(Text.c_str()), Font->GetStringHeight(Text.c_str())) / 2.0f).y;

	PRender->sClr = Style->Colors.ButtonFace;
	PRender->lClr = Style->Colors.ButtonBorder;

// 	PRender->grRectS(aabb.vMin, aabb.vMax);
// 	PRender->grRectL(aabb.vMin, aabb.vMax);
	Font->Print(Text.c_str());
}


//////////////////////////////////////////////////////////////////////////
// CButton

CButton::CButton()
{
	aabb = CAABB(0, 0, 100, 100);
	Color = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);	// deprecated, use style
	//Text = "You dumb! You called default constructor!";
	Text = "Button " + itos(GetID());
}

CButton::CButton(CAABB ARect, const char* AText, RGBAf AColor)
{
	aabb = ARect;
	Color = AColor;	// deprecated, use style
	Text = AText;
}

void CButton::Render()
{	
	CEngine *engine = CEngine::Instance();

	// Font = Style->Fonts.ButtonFont; // later: we will be able to change style on fly, so assign font to the pointer every render..

	Font->Pos.x = (int)((aabb.vMin + aabb.vMax) / 2.0f - Vector2(Font->GetStringWidth(Text.c_str()), Font->GetStringHeight(Text.c_str())) / 2.0f).x;
	Font->Pos.y = (int)((aabb.vMin + aabb.vMax) / 2.0f - Vector2(Font->GetStringWidth(Text.c_str()), Font->GetStringHeight(Text.c_str())) / 2.0f).y;

	Font->tClr = Style->Colors.ButtonText;

	PRender->sClr = Style->Colors.ButtonFace;
	PRender->lClr = Style->Colors.ButtonBorder;
	if (WidgetState.Hovered)
	{
		PRender->sClr = Style->Colors.ButtonFaceHovered;
		PRender->lClr = Style->Colors.ButtonBorderHovered;
	}
	if (WidgetState.Pressed)
	{
		PRender->sClr = Style->Colors.ButtonFacePressed;
		PRender->lClr = Style->Colors.ButtonFacePressed;
	}
	PRender->grRectS(aabb.vMin, aabb.vMax);
	PRender->grRectL(aabb.vMin, aabb.vMax);
	Font->Print(Text.c_str());
}


void CButton::Update(float dt)
{
	Vector2 mouse;
	CEngine::Instance()->GetState(CEngine::STATE_MOUSE_XY, &mouse);

	MouseState.Hovered = aabb.Inside(mouse);
	MouseState.Pressed = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1);
	MouseState.PressedInside = MouseState.Hovered && MouseState.Pressed;

	if (!MouseState.Hovered && MouseState.Pressed && !PreviousMouseState.Pressed)
		MouseState.PressedOutside = true;

	if (!MouseState.Pressed && PreviousMouseState.Pressed && MouseState.PressedOutside)
		MouseState.PressedOutside = false;


	if (MouseState.Hovered && !PreviousMouseState.Hovered)
	{
		WidgetState.Hovered = true;
	}

	if (!MouseState.Hovered && PreviousMouseState.Hovered)
	{
		WidgetState.Hovered = false;
	}

	if (MouseState.PressedInside && !PreviousMouseState.PressedInside && !MouseState.PressedOutside)
	{
		WidgetState.Pressed = true;
		Focus();
	}

	if (!MouseState.PressedInside && PreviousMouseState.PressedInside && !MouseState.PressedOutside)
	{
		WidgetState.Pressed = false;
		if (CallProc && MouseState.Hovered && isFocused())
			CallProc(Caller);
	}

	PreviousMouseState = MouseState;
}

bool CButton::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	switch(state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_SPACE:
			WidgetState.Pressed = true;
			break;
		case SDLK_RETURN:
			if (CallProc)
				CallProc(Caller);
			break;
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_SPACE:
			WidgetState.Pressed = false;
			if (CallProc)
				CallProc(Caller);
			break;
		}
		break;
	}
	return true;

}


//////////////////////////////////////////////////////////////////////////
// CEdit

CEdit::CEdit() : CursorPos(-1), VisibleTextOffset(0)
{
	Selection.Clear(CursorPos);
}

void CEdit::Render()
{
	CEngine *engine = CEngine::Instance();

	float StringWidth = Font->GetStringWidth(Text.c_str());
	float StringHeight = Font->GetStringHeight(Text.c_str());
	float CursorDistance = Font->GetStringWidthEx(0, (CursorPos - VisibleTextOffset), GetVisibleText().c_str());

	// Font = Style->Fonts.EditFont; // later: we will be able to change style on fly, so assign font to the pointer every render..

	Font->Pos.x = (int)aabb.vMin.x + (int)Style->Metrics.EditMargins.x;
	Font->Pos.y = (int)((aabb.vMin.y + aabb.vMax.y) / 2.0f - StringHeight / 2.0f);
	Font->tClr = Style->Colors.EditText;

	if (MouseState.Hovered)
	{
		PRender->lClr = Style->Colors.EditBorderHovered;
		PRender->sClr = Style->Colors.EditBackgroundHovered;
	}
	else
	{
		PRender->lClr = Style->Colors.EditBorder;
		PRender->sClr = Style->Colors.EditBackground;
	}
	PRender->lwidth = Style->Metrics.EditBorderWidth;
	PRender->grRectS(aabb.vMin, aabb.vMax);
	PRender->grRectL(aabb.vMin, aabb.vMax);
	Font->Print(GetVisibleText().c_str());
	if (isFocused())
	{
		if (Selection.Exists())
		{
			CAABB SelBox(aabb.vMin.x + Style->Metrics.EditMargins.x +
				Font->GetStringWidthEx(0, std::max(Selection.RangeStart() - VisibleTextOffset, -1), GetVisibleText().c_str()),
				aabb.vMin.y + Style->Metrics.EditMargins.y,
				aabb.vMin.x + Style->Metrics.EditMargins.x + Font->GetStringWidthEx(0,
				std::min(Selection.RangeEnd() - VisibleTextOffset, (int)GetVisibleText().length() - 1), GetVisibleText().c_str()),
				aabb.vMax.y - Style->Metrics.EditMargins.y);

			PRender->sClr = Style->Colors.EditSelection;
			PRender->grRectS(SelBox.vMin, SelBox.vMax);
		}
		PRender->psize = 2.0f;
		PRender->lClr = Style->Colors.EditText;
		PRender->lwidth = 1.0f;
		PRender->grSegment(Vector2(Font->Pos.x + CursorDistance, aabb.Inflated(0.0f, -Style->Metrics.EditMargins.y).vMax.y),
			Vector2(Font->Pos.x + CursorDistance, aabb.Inflated(0.0f, -Style->Metrics.EditMargins.y).vMin.y));
	}
}

void CEdit::Update(float dt)
{
	Vector2 mouse;
	CEngine::Instance()->GetState(CEngine::STATE_MOUSE_XY, &mouse);

	MouseState.Hovered = aabb.Inside(mouse);
	MouseState.Pressed = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) && MouseState.Hovered;


	if (MouseState.Pressed && !PreviousMouseState.Pressed)
		Focus();

	if (MouseState.Pressed)
	{
		CursorPos = MouseToCursorPos(mouse);
		if (!PreviousMouseState.Pressed)
			Selection.Start = CursorPos;
		else
			Selection.End = CursorPos;

		CAABB LeftScrollArea(aabb.vMin.x, aabb.vMin.y, aabb.vMin.x + 5.0f, aabb.vMax.y);
		CAABB RightScrollArea(aabb.vMax.x - 5.0f, aabb.vMin.y, aabb.vMax.x, aabb.vMax.y);

		if (LeftScrollArea.Inside(mouse))
		{
			// @todo: move this copy-paste from here and from SDLK_LEFT handler to some function...
			if (((CursorPos - VisibleTextOffset)) < 0 && (VisibleTextOffset > 0))
				VisibleTextOffset--;
		}

		if (RightScrollArea.Inside(mouse))
		{
			string RightIncText = Text.substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 2);

			while (!isTextFits(RightIncText.c_str()))
			{
				VisibleTextOffset++;
				RightIncText = Text.substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 2);
			}
		}


	}

	PreviousMouseState = MouseState;
}

bool CEdit::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	// this function has many copy-pasted blocks.. they're all marked with fN, where N is 1, 2, 3, ...
	// @todo: move 'em all to separate functions
	// or maybe selection will be handled by some CText or whatever... i just don't know yet..
	switch(state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_BACKSPACE:
			if (Selection.Exists())
			{
				// f1
				CursorPos = Selection.RangeStart();
				Text.erase(Selection.RangeStart() + 1, Selection.Length());
				Selection.Clear();
			}
			else
			{
				if (CursorPos >= 0)
				{
					Text.erase(CursorPos, 1);
					CursorPos--;

					// f2
					if (VisibleTextOffset > 0)
						VisibleTextOffset--;
				}
			}
			break;
		case SDLK_DELETE:
			if (Selection.Exists())
			{
				// f1
				CursorPos = Selection.RangeStart();
				Text.erase(Selection.RangeStart() + 1, Selection.Length());
				Selection.Clear();
			}
			else
			{
				Text.erase(CursorPos + 1, 1);
				// f2
				if (VisibleTextOffset > 0)
					VisibleTextOffset--;
			}
			break;
		case SDLK_LEFT:				
			if (--CursorPos < -1)
				CursorPos++;

			if (((CursorPos - VisibleTextOffset)) < 0 && (VisibleTextOffset > 0))
				VisibleTextOffset--;

			// f3
			if (mod & KMOD_SHIFT)
				Selection.End = CursorPos;
			else
				Selection.Clear(CursorPos);
			break;
		case SDLK_RIGHT:
			{
				if (++CursorPos >= Text.length())
					CursorPos--;

				// f3
				if (mod & KMOD_SHIFT)
					Selection.End = CursorPos;
				else
					Selection.Clear(CursorPos);

				string RightIncText = Text.substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 1);

				while (!isTextFits(RightIncText.c_str()))
				{
					VisibleTextOffset++;
					RightIncText = Text.substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 1);
				}
				break;
			}
		case SDLK_HOME:
			CursorPos = -1;

			// f3
			if (mod & KMOD_SHIFT)
				Selection.End = CursorPos;
			else
				Selection.Clear(CursorPos);

			VisibleTextOffset = 0;
			break;
		case SDLK_END:
			{
				CursorPos = Text.length() - 1;

				// f3
				if (mod & KMOD_SHIFT)
					Selection.End = CursorPos;
				else
					Selection.Clear(CursorPos);

				string RightEndText = Text.substr(VisibleTextOffset);

				while (!isTextFits(RightEndText.c_str()))
				{
					VisibleTextOffset++;
					RightEndText = Text.substr(VisibleTextOffset);
				}
				break;
			}
		default:
			if (letter > 31 || letter < 0)
			{
				if (Selection.Exists())
				{
					// f1
					CursorPos = Selection.RangeStart();
					Text.erase(Selection.RangeStart() + 1, Selection.Length());
					Selection.Clear();
				}
				Text.insert(CursorPos + 1, &letter, 1);
				CursorPos++;

				Selection.Clear(CursorPos);

				string NewText = GetVisibleText() + letter;
				while (!isTextFits(NewText.c_str()))
				{
					VisibleTextOffset++;
					NewText = GetVisibleText() + letter;
				}
			}
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_AMPERSAND:
			break;
		default:;
		}
		break;
	}
	return true;
	// ужоснах, индусский код в чистом виде...
}

int CEdit::MouseToCursorPos(const Vector2& MousePosition) const
{
	if (!aabb.Inside(MousePosition))
		return -1;

	return Font->StringCoordToCursorPos(GetVisibleText().c_str(), MousePosition.x, MousePosition.y) + VisibleTextOffset;
}

string CEdit::GetVisibleText() const
{
	string VisibleText = Text.substr(VisibleTextOffset);
	int i = 0;

	while (isTextFits(VisibleText.substr(0, i + 1).c_str()) && (i < VisibleText.length()))
		i++;

	return VisibleText.substr(0, i);
}

bool CEdit::isTextFits(const char *AText) const
{
	return ((Font->GetStringWidth(AText) + 2.0f * Style->Metrics.EditMargins.x) < aabb.Width());
}


//////////////////////////////////////////////////////////////////////////
// CEdit::CTextSelection

void CEdit::CTextSelection::Clear(int ACursorPos)
{
	Start = End = ACursorPos;
}

void CEdit::CTextSelection::Clear()
{
	Start = End;
}

int CEdit::CTextSelection::Length() const
{
	return (RangeEnd() - RangeStart());
}

int CEdit::CTextSelection::RangeEnd() const
{
	return std::max(Start, End);
}

int CEdit::CTextSelection::RangeStart() const
{
	return std::min(Start, End);
}

bool CEdit::CTextSelection::Exists() const
{
	return (Start != End);
}

void CEdit::CTextSelection::Set(int AStart, int AEnd)
{
	Start = AStart;
	End = AEnd;
}

CEdit::CTextSelection::CTextSelection(int AStart, int AEnd)
{
	Start = AStart;
	End = AEnd;
}

CEdit::CTextSelection::CTextSelection()
{
	Start = End = -1;
}


//////////////////////////////////////////////////////////////////////////
// CMenuItem

#define DEFAULT_DISTANCE_BEETWEEN_ITEMS 20

CMenuItem::CMenuItem() : Focus(Objects.begin()), isCycledMenuSwitch(true)
{
	Visible = false;
}

CMenuItem::CMenuItem(CMenuItem* AParent, char* AMenuText)
{
	doIgnoreCamera = true;
	Focus = Objects.begin();
	Visible = false;
	isCycledMenuSwitch = true;
	SetName(Text = AMenuText);
	SetParent(AParent);
	if (AParent)
		AParent->AddObject(this);
}

CMenuItem::~CMenuItem()
{

}

void CMenuItem::Render()
{
	for(ManagerIterator it = Objects.begin(); it != Objects.end(); ++it)
	{		
		CMenuItem *ChildMenuItem = *it;
		Font->tClr = RGBAf(1.0,1.0,1.0,1.0);//ChildMenuItem->color;
		Font->scale = Vector2(1.0f, 1.0f);
		Font->Pos = ChildMenuItem->Position;
		Font->Print(ChildMenuItem->Text.c_str());
	}	
	Color = COLOR_WHITE;
	PRender->grCircleS((*Focus)->Position - Vector2(20.0f, -10.0f), 5);
}

void CMenuItem::Update(float dt){}

bool CMenuItem::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	if (!Visible)
		return false;
	if (state == KEY_PRESSED)
	{
		switch(key)
		{
		case SDLK_UP:
			// Вероятно эту логику можно записать и покороче @todo
			if (Focus == Objects.begin() && isCycledMenuSwitch)
			{
				Focus = Objects.end();
				break;
			}
			Focus--;
			break;
		case SDLK_DOWN:
			if (Focus == Objects.end() && isCycledMenuSwitch)
			{
				Focus = Objects.begin();
				break;
			}
			Focus++;
			break;
		case SDLK_RETURN:
			if ((*Focus)->CallProc)
				(*Focus)->CallProc(Caller);
// 			else
// 				if ((*Focus)->)
// 				{
// 					Visible = false;
// 					(*Focus)->Visible = true;
// 					CGUIManager::Instance()->SetFocus(Focus);
// 				}
				break;
		case SDLK_ESCAPE:
// 			if (!Parent)
// 				break;
// 			Visible = false;
// 			(dynamic_cast<CMenuItem*>(Parent))->Visible = true;
// 			CGUIManager::Instance()->SetFocus(dynamic_cast<CMenuItem*>(Parent));
			break;
		}
	}
	else
	{

	}
	
	return true;
}

bool CMenuItem::AddObject(CMenuItem *AObject)
{
	CCommonManager <list <CMenuItem*> >::Add(AObject);
 	Focus = Objects.begin();
 	return true;
}

