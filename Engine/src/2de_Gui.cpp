#include "2de_Gui.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CGUIObjectBase
CGUIObjectBase::CGUIObjectBase()
{
	Font = NULL;
	PRender = NULL;
	Style = NULL;
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
	Result.x = std::max(Coords.x - GetBox().Min.x, 0.0f);
	Result.y = std::max(Coords.y - GetBox().Min.y, 0.0f);
	return Result;
}

void CGUIObjectBase::SetCallback( CObjectCallback ACallProc, CObject *ACaller )
{
	Caller = ACaller;
	CallProc = ACallProc;
}

void CGUIObject::SetVisibility( bool AVisible )
{
	CRenderable::SetVisibility(AVisible);
	Text.SetVisibility(AVisible);
}
//////////////////////////////////////////////////////////////////////////
// CGUIRootObject
CGUIRootObject::CGUIRootObject(): KeyHoldRepeatDelay(300), KeyHoldRepeatInterval(50), TimerAccum(0), TabHolded(false)
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
		PRender->grRectL(Focused->GetBox().Inflated(Style->Metrics.FocusRectSpacing, Style->Metrics.FocusRectSpacing).Min,
			Focused->GetBox().Inflated(Style->Metrics.FocusRectSpacing, Style->Metrics.FocusRectSpacing).Max);	
		PRender->LineStippleEnabled = false;	// bad.. object shouldn't "disable" any settings.. every object should just initialize it..
		PRender->lwidth = 1.0f;
	}
	return;
}

void CGUIRootObject::Update(float dt)
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
				CGUIManager::Instance()->InputHandling(KEY_PRESSED, SDLK_TAB, SDLMod(0), 0);
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

//////////////////////////////////////////////////////////////////////////
// CGUIObject
CGUIObject::CGUIObject()
{
	CGUIManager::Instance()->Add(this);
	SetParent(CGUIManager::Instance()->GetRoot());
}

CGUIObject::CGUIObject(CGUIObjectBase *AParent)
{
	CGUIManager::Instance()->Add(this);
	SetParent(AParent);
}

CGUIObject::~CGUIObject()
{
	CGUIManager::Instance()->Remove(this);
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
	Color = Parent->Color;	// Color is deprecated for GUI objects 
							// (really not used in CButton, CEdit, but there is something
							// related to Color in CMenuItem), use CGUIStyle instead
}

//////////////////////////////////////////////////////////////////////////
// CGUIManager

CGUIManager::CGUIManager()
{
	SetName("GUI Manager");
	Root = new CGUIRootObject;
	Root->SetStyle(new CGUIStyle);
	Root->SetPrimitiveRender(new CPrimitiveRender);
	Root->SetFont(Root->GetStyle()->Font);
	Focus = Objects.begin();
	CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
}

bool CGUIManager::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	// TODO: move this function to CGUIRootObject..	
	switch(state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_TAB:
			Root->TabHolded = true;
			if (Focus != Objects.end()) // if iterator correct
			{
				if (mod & KMOD_SHIFT)
					Focus--;
				else
					Focus++;									
			}

			if (Focus == Objects.end()) // if iterator incorrect totally i.e. pointing to illegal object
			{
				if (mod & KMOD_SHIFT)
					Focus = Objects.end();
				else
					Focus = Objects.begin();
			}
			break;
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_TAB:
			Root->TabHolded =  false;
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
	Focus = find(Objects.begin(), Objects.end(), AObject);
}

CGUIRootObject* CGUIManager::GetRoot() const
{
	return Root;
}

void CGUIManager::Add(CGUIObject *AObject)
{
	CCommonManager <list <CGUIObject*> >::Add(AObject);
	Focus = Objects.begin();
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
	Text.Color = Style->Colors.LabelText;
}

void CLabel::Render()
{
}


//////////////////////////////////////////////////////////////////////////
// CButton

CButton::CButton()
{
	Text = "Button " + itos(GetID()); // @todo: think, then remove
	Text.Color = Style->Colors.ButtonText;
}

CButton::CButton(CBox ARect, const char* AText, RGBAf AColor)
{
	SetBox(ARect);
	Color = AColor;	// deprecated, use style
	Text = AText;
	Text.Color = Style->Colors.ButtonText;
}

void CButton::Render()
{	
	CEngine *engine = CEngine::Instance();

	// Font = Style->Fonts.ButtonFont; // later: we will be able to change style on fly, so assign font to the pointer every render..

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
	PRender->grRectS(GetBox().Min, GetBox().Max);
	PRender->grRectL(GetBox().Min, GetBox().Max);	
}


void CButton::Update(float dt)
{
	Vector2 mouse = CEngine::Instance()->MousePos;

	MouseState.Hovered = GetBox().Inside(mouse);
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
	Text.Color = Style->Colors.EditText;
}

void CEdit::Render()
{
	CEngine *engine = CEngine::Instance();

	float StringWidth = Text.Width();
	float StringHeight = Text.Height();
	float CursorDistance = Font->GetStringWidthEx(0, (CursorPos - VisibleTextOffset), GetVisibleText().c_str());

	// Font = Style->Fonts.EditFont; // later: we will be able to change style on fly, so assign font to the pointer every render..


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
	PRender->grRectS(GetBox().Min, GetBox().Max);
	PRender->grRectL(GetBox().Min, GetBox().Max);
	if (isFocused())
	{
		if (Selection.Exists())
		{
			CBox SelBox(GetBox().Min.x + Style->Metrics.EditMargins.x +
				Font->GetStringWidthEx(0, std::max(Selection.RangeStart() - VisibleTextOffset, -1), GetVisibleText().c_str()),
				GetBox().Min.y + Style->Metrics.EditMargins.y,
				GetBox().Min.x + Style->Metrics.EditMargins.x + Font->GetStringWidthEx(0,
				std::min(Selection.RangeEnd() - VisibleTextOffset, (int)GetVisibleText().length() - 1), GetVisibleText().c_str()),
				GetBox().Max.y - Style->Metrics.EditMargins.y);

			PRender->sClr = Style->Colors.EditSelection;
			PRender->grRectS(SelBox.Min, SelBox.Max);
		}
		PRender->psize = 2.0f;
		PRender->lClr = Style->Colors.EditText;
		PRender->lwidth = 1.0f;
		PRender->grSegment(Vector2(Text.Position.x + CursorDistance, GetBox().Inflated(0.0f, -Style->Metrics.EditMargins.y).Max.y),
			Vector2(Text.Position.x + CursorDistance, GetBox().Inflated(0.0f, -Style->Metrics.EditMargins.y).Min.y));
	}
}

void CEdit::Update(float dt)
{
	Text = GetVisibleText();
	Vector2 mouse = CEngine::Instance()->MousePos;

	MouseState.Hovered = GetBox().Inside(mouse);
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

		float Xmi = GetBox().Min.x, Ymi = GetBox().Min.y;
		float Xma = GetBox().Min.x, Yma = GetBox().Min.y;

		CBox LeftScrollArea(Xmi, Ymi, Xmi + 5.0f, Yma);
		CBox RightScrollArea(Xma - 5.0f, Ymi, Xma, Yma);

		if (LeftScrollArea.Inside(mouse))
		{
			// @todo: move this copy-paste from here and from SDLK_LEFT handler to some function...
			if (((CursorPos - VisibleTextOffset)) < 0 && (VisibleTextOffset > 0))
				VisibleTextOffset--;
		}

		if (RightScrollArea.Inside(mouse))
		{
			string RightIncText = Text.GetText().substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 2);

			while (!isTextFits(RightIncText.c_str()))
			{
				VisibleTextOffset++;
				RightIncText = Text.GetText().substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 2);
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
				ActualText.erase(Selection.RangeStart() + 1, Selection.Length());
				Selection.Clear();
			}
			else
			{
				if (CursorPos >= 0)
				{
					ActualText.erase(CursorPos, 1);
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
				ActualText.erase(Selection.RangeStart() + 1, Selection.Length());
				Selection.Clear();
			}
			else
			{
				ActualText.erase(CursorPos + 1, 1);
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
				if (++CursorPos >= ActualText.length())
					CursorPos--;

				// f3
				if (mod & KMOD_SHIFT)
					Selection.End = CursorPos;
				else
					Selection.Clear(CursorPos);

				string RightIncText = ActualText.substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 1);

				while (!isTextFits(RightIncText.c_str()))
				{
					VisibleTextOffset++;
					RightIncText = ActualText.substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 1);
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
				CursorPos = ActualText.length() - 1;

				// f3
				if (mod & KMOD_SHIFT)
					Selection.End = CursorPos;
				else
					Selection.Clear(CursorPos);

				string RightEndText = ActualText.substr(VisibleTextOffset);

				while (!isTextFits(RightEndText.c_str()))
				{
					VisibleTextOffset++;
					RightEndText = ActualText.substr(VisibleTextOffset);
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
					ActualText.erase(Selection.RangeStart() + 1, Selection.Length());
					Selection.Clear();
				}
				ActualText.insert(CursorPos + 1, &letter, 1);
				CursorPos++;
				Selection.Clear(CursorPos);

				string NewText = GetVisibleText() + letter;
				while (!isTextFits(NewText.c_str()))
				{
					VisibleTextOffset++;
					NewText = GetVisibleText() + letter;
				}
				Text.SetText(NewText);
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
	if (!GetBox().Inside(MousePosition))
		return -1;
	return Text.StringCoordToCursorPos(MousePosition.x, MousePosition.y) + VisibleTextOffset;
}

string CEdit::GetVisibleText() const
{
	string VisibleText = ActualText; //Text.GetText();
	VisibleText = VisibleText.substr(VisibleTextOffset);
	int i = 0;
	while (isTextFits(VisibleText.substr(0, i + 1).c_str()) && (i < VisibleText.length()))
		i++;
	return VisibleText.substr(0, i);
}

bool CEdit::isTextFits(const char *AText) const
{
	return ((Font->GetStringWidth(AText) + 2.0f * Style->Metrics.EditMargins.x) < GetBox().Width());
}

void CEdit::SetBox( const CBox &box )
{
	float StringHeight = Text.Height();
	CGUIObject::SetBox(box);
	Text.Position.x = (int)box.Min.x + (int)Style->Metrics.EditMargins.x;
	/*Text.Position.y = (int)((box.Min.y + box.Max.y) / 2.0f - StringHeight / 2.0f);*/
	Text.Position.y = (((GetBox().Min + GetBox().Max) - Vector2(Text.Width(), Text.Height())) * 0.5f).y;

}

void CEdit::SetText( const string &AText )
{
	ActualText = AText;			
	/*Text.Position.y = (int)((GetBox().Min.y + GetBox().Max.y) / 2.0f - Text.Height() / 2.0f);*/
	Text = GetVisibleText();
	Text.Position.x = (int)GetBox().Min.x + (int)Style->Metrics.EditMargins.x;
	Text.Position.y = (((GetBox().Min + GetBox().Max) - Vector2(Text.Width(), Text.Height())) * 0.5f).y;
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
	SetVisibility(false);
}

CMenuItem::CMenuItem(CMenuItem* AParent, char* AMenuText)
{
	doIgnoreCamera = true;
	Focus = Objects.begin();
	SetVisibility(false);
	isCycledMenuSwitch = true;
	SetName((Text = AMenuText).GetText());
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
		//Text.Color = RGBAf(1.0,1.0,1.0,1.0);
		//Text.SetScaling(1.0f);
		Text.Position  = ChildMenuItem->Position;
		//Font->Print(ChildMenuItem->Text.c_str());
	}	
	Color = COLOR_WHITE;
	PRender->grCircleS((*Focus)->Position - Vector2(20.0f, -10.0f), 5);
}

void CMenuItem::Update(float dt){}

bool CMenuItem::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	if (!GetVisibility())
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

