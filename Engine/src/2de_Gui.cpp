#include "2de_Gui.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CGUIStyle

CGUIStyle::CGUIStyle()
{
	// default style values - very ugly style :) I'm programmer, not fucking "эстет" :) you're welcome to fix colors to more beautiful ones

	Colors.FocusRect = RGBAf(0.5f, 0.5f, 0.5f, 1.0f);
	Colors.ButtonFace = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
	Colors.ButtonFaceHovered = RGBAf(0.6f, 0.6f, 0.6f, 1.0f);
	Colors.ButtonFacePressed = RGBAf(0.45f, 0.45f, 0.45f, 1.0f);
	Colors.ButtonBorder = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
	Colors.ButtonBorderHovered = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
	Colors.ButtonBorderPressed = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
	Colors.ButtonText = color::BLACK;
	Colors.ButtonInactiveText = RGBAf(0.2f, 0.2f, 0.2f, 1.0f);
	Colors.EditBackground = color::WHITE;
	Colors.EditBackgroundHovered = color::WHITE;
	Colors.EditBorder = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
	Colors.EditBorderHovered = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
	Colors.EditText = color::BLACK;
	Colors.EditInactiveText = RGBAf(0.2f, 0.2f, 0.2f, 1.0f);
	Colors.EditSelection = RGBAf(0.0f, 0.4f, 0.8f, 0.5f);
	Colors.LabelText = color::WHITE;

	Metrics.FocusRectSpacing = 5.0f;
	Metrics.FocusRectLineWidth = 0.5f;
	Metrics.EditMargins = Vector2(4.0f, 6.0f);
	Metrics.EditBorderWidth = 2.0f;

	Font = CFontManager::Instance()->GetDefaultFont();
}

//////////////////////////////////////////////////////////////////////////
// CGUIObjectBase

CGUIObjectBase::CGUIObjectBase()
{
	Font = NULL;
	Style = NULL;
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
// 	Result.x = std::max(Coords.x - GetBox().Min.x, 0.0f);
// 	Result.y = std::max(Coords.y - GetBox().Min.y, 0.0f);
	return Result;
}

void CGUIObject::SetVisibility(bool AVisible)
{
	CRenderableComponent::SetVisibility(AVisible);
	Text.SetVisibility(AVisible);
}

//////////////////////////////////////////////////////////////////////////
// CGUIRootObject

CGUIRootObject::CGUIRootObject() : TabHolded(false), KeyHoldRepeatDelay(300), KeyHoldRepeatInterval(50), TimerAccum(0)
{
	CRenderableComponent::SetName("GUI Root Object");
}

// void CGUIRootObject::Render()
// {
// 	CGUIObject *Focused = CGUIManager::Instance()->GetFocusedObject();
// 	if (Focused)
// 	{
// 		//PRender->lwidth = Style->Metrics.FocusRectLineWidth;
// 		Color = Style->Colors.FocusRect;
// 		CRenderManager::Instance()->DrawLinedBox(this,	Focused->GetBox().Inflated(Style->Metrics.FocusRectSpacing, Style->Metrics.FocusRectSpacing));
// 	}
// 	return;
// }

void CGUIRootObject::Update(float dt)
{
#if 0
#ifndef I_LIKE_HOW_SDL_KEY_REPEAT_WORKS
	//  этот код по-моему вообще не работает...
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
				CGUIManager::Instance()->InputHandling(KEY_PRESS, SDLK_TAB, SDLMod(0), 0);
			}
		}
	}
	else
	{
		TimerAccum = 0;
		RepeatStarted = false;
	}
#endif
#endif
}

//////////////////////////////////////////////////////////////////////////
// CGUIObject

CGUIObject::CGUIObject()
{
	//Text.SetLayer(5);
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
//	PRender = Parent->GetPrimitiveRender();
	//Color = Parent->Color;	// Color is deprecated for GUI objects 
							// (really not used in CButton, CEdit, but there is something
							// related to Color in CMenuItem), use CGUIStyle instead
}

const string& CGUIObject::GetText() const
{
	return Text.GetText();
}

void CGUIObject::SetText(const string &AText)
{
	Text = AText;
	//Text.SetPosition(((GetBox().Min + GetBox().Max) - Vector2(Text.Width(), Text.Height())) * 0.5f);
}

void CGUIObject::SetBox(const CBox &box)
{
	//CRenderable::SetBox(box);
	//Text.SetLayer(10);
	// maybe smthng else
}
//////////////////////////////////////////////////////////////////////////
// CGUIManager

CGUIManager::CGUIManager()
{
	SetName("GUI Manager");
	Root = new CGUIRootObject;
	Root->SetStyle(new CGUIStyle);
//Root->SetPrimitiveRender(new CPrimitiveRender);
	Root->SetFont(Root->GetStyle()->Font);
	Focus = Objects.begin();

	CEventManager::Instance()->Subscribe("KeyDown", this);
	CEventManager::Instance()->Subscribe("KeyPress", this);
	CEventManager::Instance()->Subscribe("KeyUp", this);
	CEventManager::Instance()->Subscribe("MouseDown", this);
	CEventManager::Instance()->Subscribe("MouseUp", this);
}

void CGUIManager::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyPress")
	{
		if (AEvent.GetData<Uint16>("Sym") == SDLK_TAB)
		{
			Root->TabHolded = true;
			if (AEvent.GetData<Uint16>("Modifiers") & KMOD_SHIFT)
			{
				if (Focus != Objects.begin())
					Focus--;

				if (Focus == Objects.begin())
				{
					Focus = Objects.end();
					Focus--;
				}
			}
			else
			{
				if (Focus != Objects.end())
					Focus++;

				if (Focus == Objects.end())
					Focus = Objects.begin();
			}
		}
	}
	else if (AEvent.GetName() == "KeyUp")
	{
		if (AEvent.GetData<Uint16>("Sym") == SDLK_TAB)
			Root->TabHolded = false;
	}

//	if (Focus != Objects.end())
//		(*Focus)->ProcessEvent(AEvent);
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
//	delete Root->GetPrimitiveRender();
	delete Root->GetStyle();
	delete Root;
}


//////////////////////////////////////////////////////////////////////////
// CLabel

CLabel::CLabel(const string &AText /*= ""*/)
{
	Text = AText;
//	Text.Color = Style->Colors.LabelText;
}

void CLabel::Render()
{
}

void CLabel::SetBox(const CBox &box)
{
	CGUIObject::SetBox(box);
	//Text.SetPosition(Vector2(box.Min.x, box.Min.y));
}
//////////////////////////////////////////////////////////////////////////
// CButton

CButton::CButton()
{
	Text = "Button " + itos(CRenderableComponent::GetID()); // @todo: think, then remove
	Text.SetColor(Style->Colors.ButtonText);
}

CButton::CButton(CBox ARect, const char* AText, RGBAf AColor)
{
	SetBox(ARect);
	Text = AText;
	Text.SetColor(Style->Colors.ButtonText);
}

void CButton::SetBox(const CBox &box)
{
	CGUIObject::SetBox(box);
	//Text.SetPosition((box.Min + box.Max) / 2.0f - Vector2(Text.Width(), Text.Height()) / 2.0f);
}

void CButton::Render()
{	
	// Вынести. Сильно вынести отсюда.
	//(GetBox().Min, GetBox().Max);

	SetColor(Style->Colors.ButtonFace);
	//temprect_lines.Color = Style->Colors.ButtonBorder;
	if (WidgetState.Hovered)
	{
		SetColor(Style->Colors.ButtonFaceHovered);
		//temprect_lines.Color = Style->Colors.ButtonBorderHovered;
	}
	if (WidgetState.Pressed)
	{
		SetColor(Style->Colors.ButtonFacePressed);
		//temprect_lines.Color = Style->Colors.ButtonBorderPressed;
	}

	//SetLayer(1);
	//CRenderManager::Instance()->DrawSolidBox(this, GetBox().Inflated(4.0f, 4.0f));
//	CRenderManager::Instance()->DrawSolidBox(this, GetBox());
}


void CButton::Update(float dt)
{
	Vector2 mouse = CEngine::Instance()->GetMousePosition();

	//MouseState.Hovered = GetBox().Inside(mouse);
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
		if (MouseState.Hovered && isFocused())
		{
// 			if (CallProc)
// 				CallProc(Caller);

			CEventManager::Instance()->TriggerEvent("ButtonClick", this);
		}
	}

	PreviousMouseState = MouseState;
}

void CButton::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_SPACE:
			WidgetState.Pressed = true;
			break;
		case SDLK_RETURN:
// 			if (CallProc)
// 				CallProc(Caller);
			break;
		}
	}
	else if (AEvent.GetName() == "KeyUp")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_SPACE:
			WidgetState.Pressed = false;
// 			if (CallProc)
// 				CallProc(Caller);
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// CEdit

CEdit::CEdit() : CursorPos(-1), VisibleTextOffset(0)
{
	Selection.Clear(CursorPos);
	Text.SetColor(Style->Colors.EditText);
}

void CEdit::Render()
{
	//SetLayer(1);
	//float StringWidth = Text.Width();
	//float StringHeight = Text.Height();
	float CursorDistance = Font->GetStringWidthEx(0, (CursorPos - VisibleTextOffset), GetVisibleText().c_str());

	// Font = Style->Fonts.EditFont; // later: we will be able to change style on fly, so assign font to the pointer every render..


	if (MouseState.Hovered)
	{
//		PRender->lClr = Style->Colors.EditBorderHovered;
		GetColor() = Style->Colors.EditBackgroundHovered;
	}
	else
	{
		//PRender->lClr = Style->Colors.EditBorder;
		GetColor() = Style->Colors.EditBackground;
	}
	//PRender->lwidth = Style->Metrics.EditBorderWidth;
	
	//CRenderManager::Instance()->DrawSolidBox(this, GetBox());
	//PRender->grRectL(GetBox().Min, GetBox().Max);
	if (isFocused())
	{
		if (Selection.Exists())
		{
		//	CBox SelBox(GetBox().Min.x + Style->Metrics.EditMargins.x +
		//		Font->GetStringWidthEx(0, std::max(Selection.RangeStart() - VisibleTextOffset, -1), GetVisibleText().c_str()),
/*	*///			GetBox().Min.y + Style->Metrics.EditMargins.y,
		//		GetBox().Min.x + Style->Metrics.EditMargins.x + Font->GetStringWidthEx(0,
		//		std::min(Selection.RangeEnd() - VisibleTextOffset, (int)GetVisibleText().length() - 1), GetVisibleText().c_str()),
		//		GetBox().Max.y - Style->Metrics.EditMargins.y);

			GetColor() = Style->Colors.EditSelection;
			//CRenderManager::Instance()->DrawSolidBox(this, SelBox);
		}
		GetColor() = Style->Colors.EditText;
// 		CRenderManager::Instance()->DrawLine(this, Vector2(Text.GetPosition().x + CursorDistance, GetBox().Inflated(0.0f, -Style->Metrics.EditMargins.y).Max.y),
// 			Vector2(Text.GetPosition().x + CursorDistance, GetBox().Inflated(0.0f, -Style->Metrics.EditMargins.y).Min.y));
	}
}

void CEdit::Update(float dt)
{
	Text = GetVisibleText();
	Vector2 mouse = CEngine::Instance()->GetMousePosition();

	//MouseState.Hovered = GetBox().Inside(mouse);
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

		//float Xmi = GetBox().Min.x, Ymi = GetBox().Min.y;
		//float Xma = GetBox().Min.x, Yma = GetBox().Min.y;

// 		CBox LeftScrollArea(Xmi, Ymi, Xmi + 5.0f, Yma);
// 		CBox RightScrollArea(Xma - 5.0f, Ymi, Xma, Yma);

// 		if (LeftScrollArea.Inside(mouse))
// 		{
// 			// @todo: move this copy-paste from here and from SDLK_LEFT handler to some function...
// 			if (((CursorPos - VisibleTextOffset)) < 0 && (VisibleTextOffset > 0))
// 				VisibleTextOffset--;
// 		}
// 
// 		if (RightScrollArea.Inside(mouse))
// 		{
// 			string RightIncText = Text.GetText().substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 2);
// 
// 			while (!isTextFits(RightIncText.c_str()))
// 			{
// 				VisibleTextOffset++;
// 				RightIncText = Text.GetText().substr(VisibleTextOffset, CursorPos - VisibleTextOffset + 2);
// 			}
// 		}
	}

	PreviousMouseState = MouseState;
}

void CEdit::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyPress")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
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
			if (AEvent.GetData<Uint16>("Modifiers") & KMOD_SHIFT)
				Selection.End = CursorPos;
			else
				Selection.Clear(CursorPos);
			break;
		case SDLK_RIGHT:
			{
				if (++CursorPos >= ActualText.length())
					CursorPos--;

				// f3
				if (AEvent.GetData<Uint16>("Modifiers") & KMOD_SHIFT)
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
			if (AEvent.GetData<Uint16>("Modifiers") & KMOD_SHIFT)
				Selection.End = CursorPos;
			else
				Selection.Clear(CursorPos);

			VisibleTextOffset = 0;
			break;
		case SDLK_END:
			{
				CursorPos = ActualText.length() - 1;

				// f3
				if (AEvent.GetData<Uint16>("Modifiers") & KMOD_SHIFT)
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
			char letter = AEvent.GetData<char>("Char");
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

	}
}

int CEdit::MouseToCursorPos(const Vector2& MousePosition) const
{
	//if (!GetBox().Inside(MousePosition))
	//	return -1;
	//return Text.StringCoordToCursorPos(MousePosition.x, MousePosition.y) + VisibleTextOffset;
	return -1;
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
	return true;//((Font->GetStringWidth(AText) + 2.0f * Style->Metrics.EditMargins.x) < GetBox().Width());
}

void CEdit::SetBox(const CBox &box)
{
	//float StringHeight = Text.Height();
	CGUIObject::SetBox(box);
// 	Text.SetPosition
// 		(Vector2(
// 			(int)box.Min.x + (int)Style->Metrics.EditMargins.x,
// 			(((GetBox().Min + GetBox().Max) - Vector2(Text.Width(), Text.Height())) * 0.5f).y
// 		));
}

void CEdit::SetText(const string &AText)
{
	ActualText = AText;
	/*Text.Position.y = (int)((GetBox().Min.y + GetBox().Max.y) / 2.0f - Text.Height() / 2.0f);*/
	Text = GetVisibleText();
// 	Text.SetPosition(Vector2(
// 			(int)GetBox().Min.x + (int)Style->Metrics.EditMargins.x,
// 			(((GetBox().Min + GetBox().Max) - Vector2(Text.Width(), Text.Height())) * 0.5f).y
// 		));
	Selection.Clear((CursorPos = -1));
}

const string& CEdit::GetText() const
{
	return ActualText;
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

CMenuItem::CMenuItem() : isCycledMenuSwitch(true)//, Focus(Objects.begin())
{
	SetVisibility(false);
}

CMenuItem::CMenuItem(CMenuItem *AParent, const string &AMenuText)
{
	//SetIgnoreParentTransform(true);
//Focus = Objects.begin();
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
// 	for (ManagerIterator it = Objects.begin(); it != Objects.end(); ++it)
// 	{		
// 		CMenuItem *ChildMenuItem = *it;
// 		//Text.Color = RGBAf(1.0,1.0,1.0,1.0);
// 		//Text.SetScaling(1.0f);
// 		Text.SetPosition(ChildMenuItem->GetPosition());
// 		//Font->Print(ChildMenuItem->Text.c_str());
// 	}	
// 	GetColor() = COLOR_WHITE;
// 
// 	if (Focus != Objects.end())
// 	{
// 		//PRender->grCircleS((*Focus)->Position - Vector2(20.0f, -10.0f), 5);
// 		SetPosition((*Focus)->GetPosition());
// //		CRenderManager::Instance()->DrawSolidBox(this, CBox(V2_QuadBinCenter).Inflated(4.0f, 4.0f));
// 	}
}

void CMenuItem::Update(float dt)
{
}

void CMenuItem::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_UP:
			// Вероятно эту логику можно записать и покороче @todo
			// 	записал чуть короче, да и баг, кажется, пофиксил..
// 			if (Focus != Objects.begin())
// 				Focus--;
// 			else if (isCycledMenuSwitch)
// 			{
// 				Focus = Objects.end();
// 				Focus--;
// 			}
			break;
		case SDLK_DOWN:
// 			if (Focus != --Objects.end())
// 				Focus++;
// 			else if (isCycledMenuSwitch)
// 				Focus = Objects.begin();

			break;
		case SDLK_RETURN:
// 			if ((Focus != Objects.end()) && (*Focus)->CallProc)
// 				(*Focus)->CallProc(Caller);
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
}

bool CMenuItem::AddObject(CMenuItem *AObject)
{
// 	CCommonManager <list <CMenuItem*> >::Add(AObject);
//  	Focus = Objects.begin();
 	return true;
}


void CLabeledEdit::SetBox(const CBox &box)
{
	CEdit::SetBox(box);
	CBox tempBox = box;
	tempBox.Offset(0.0f, box.Height() + 2.0f);
	int TextHeight = Label.GetFont()->GetStringHeight(Label.GetText());
	tempBox.Max.y = tempBox.Min.y + TextHeight;
	Label.SetBox(tempBox);
}

CLabeledEdit::CLabeledEdit(CBox Aaabb, const string &ALabelText) : Label(ALabelText)
{
	SetBox(Aaabb);
}