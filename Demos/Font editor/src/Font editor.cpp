#include "Font editor.h"
#include "2de_Engine.h"
#include "2de_LuaUtils.h"
#include "2de_GameUtils.h"




// char Family[LF_FACESIZE]; // 

// void BuildList()
// {
// 	HDC hdc = CreateCompatibleDC(0);
// 	LOGFONT lf;
// 
// 	lf.lfCharSet=DEFAULT_CHARSET;
// 	lf.lfFaceName[0]='\0';
// 	lf.lfPitchAndFamily=0;
// 
// 	EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)this /*App defined data*/, 0);
// 	DeleteDC(hdc);
// 
// }
// 
// int CALLBACK EnumFontFamExProc(
// 							   ENUMLOGFONTEX *lpelfe,    // logical-font data
// 							   NEWTEXTMETRICEX *lpntme,  // physical-font data
// 							   DWORD FontType,           // type of font
// 							   LPARAM lParam             // application-defined data
// 							   )
// {
// 	((CFontList *)lParam)->FindSortAdd(lpelfe->elfLogFont.lfFaceName);
// 	return 1;
// }

// На самом деле это не то чтобы Load() а скорее Acquire() ибо все доступные нам шрифты движок уже проиндекировал.
// Т.е. для пользователя утилыты это будет Load, а для тех, кто способен читать этот комментарий - это Asquire()

void CFontEditor::SetZoom(float AZoom)
{
	if (FontTexture == NULL && Font == NULL)
		return;
	float OldZoom = Zoom;
	Zoom = AZoom;
	Zoom = Clamp(Zoom, ZOOM_MIN, ZOOM_MAX);
	if (Zoom == OldZoom)
		return;
	Vector2 Temp;
	Vector2 TempWH;
	if (FontTexture != NULL)
		TempWH = Vector2(FontTexture->Width, FontTexture->Height);
	else
		Temp = Vector2(Font->GetSymbolsBBOX().Width(), Font->GetSymbolsBBOX().Height());
	Temp.x = (MousePosition.x - Offset.x) / (TempWH.x * OldZoom);
	Temp.y = (MousePosition.y - Offset.y) / (TempWH.y * OldZoom);
	Temp.ClampTo(V2_ZERO, Vector2(1.0f, 1.0f));
	Offset.x = - Temp.x * (TempWH.x * Zoom) + MousePosition.x;
	Offset.y = - Temp.y * (TempWH.y * Zoom) + MousePosition.y;
}

void CFontEditor::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_g:
			if (State != ES_GRIP_TOOL)
				PreviousState = State;
			State = ES_GRIP_TOOL;
			break;
		case SDLK_EQUALS:
			SetZoom(Zoom + ZOOM_STEP);
			break;
		case SDLK_MINUS:
			SetZoom(Zoom - ZOOM_STEP);
			break;
		}

	}
	else if (AEvent.GetName() == "KeyUp")
	{
		if (AEvent.GetData<Uint16>("Sym") == SDLK_g)
		{
			EEditorState TempState = State;
			State = PreviousState;
			PreviousState = State;
		}
	}
	else if (AEvent.GetName() == "MouseDown")
	{
		Uint8 button = AEvent.GetData<Uint8>("Button");
		switch (button)
		{
		case SDL_BUTTON_WHEELUP:
			SetZoom(Zoom + ZOOM_STEP);
			break;
		case SDL_BUTTON_WHEELDOWN:
			SetZoom(Zoom - ZOOM_STEP);
			break;
		case SDL_BUTTON_LEFT:
			if (MousePosition.x < INTERFACE_OFFSET_X)
				break;
			if (Font != NULL && CornerKind == SCK_NONE)
				for (int i = 0; i < 256; i++)
					if (Font->Boxes[i].Min.x < (MousePosition.x / Zoom - Offset.x / Zoom) && Font->Boxes[i].Max.x > (MousePosition.x / Zoom - Offset.x / Zoom) &&
						Font->Boxes[i].Min.y < (MousePosition.y / Zoom - Offset.y / Zoom) && Font->Boxes[i].Max.y > (MousePosition.y / Zoom - Offset.y / Zoom))
					{
						bool tempbool = (i != CurrentSymbol);
						SetSelectedBoxTo(i);
						if (tempbool)
							return;
						break;
					}				
				{
					if (State == ES_GRIP_TOOL)
						break;
					float Mx = MousePosition.x / Zoom - Offset.x / Zoom;
					float My = MousePosition.y / Zoom - Offset.y / Zoom;
					CornerKind = SCK_NONE;
					if (CBox(SelectionBoxes[0], SelectionBoxes[2]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_LEFT_BOTTOM;
					if (CBox(SelectionBoxes[4], SelectionBoxes[6]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_RIGHT_BOTTOM;
					if (CBox(SelectionBoxes[8], SelectionBoxes[10]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_RIGHT_TOP;
					if (CBox(SelectionBoxes[12], SelectionBoxes[14]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_LEFT_TOP;
					if (CBox(SelectionBoxes[16], SelectionBoxes[18]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_CENTER;
					if (CornerKind != SCK_NONE)
					{
						PreviousState = State;
						State = ES_MOVE_TOOL;
					}
				}
			break;
		case  SDL_BUTTON_RIGHT:
			if (Font == NULL)
				break;
			if (CornerKind != SCK_NONE)
			{
				CornerKind = SCK_NONE;
			}
			break;
		}
	}
}

CFontEditor::CFontEditor()
{
	WindowWidth = CGLWindow::Instance()->GetWidth();
	WindowHeight = CGLWindow::Instance()->GetHeight();
	PRender.lClr		= COLOR_FIRST;
	PRender.sClr		= COLOR_THIRD;
	PRender.pClr		= COLOR_FIRST;
	Zoom				= 2.0f;
	FontTexture			= NULL;
	Font				= NULL;
	CurrentSymbol		= 0;
	Offset.x			= INTERFACE_OFFSET_X + 5;
	Offset.y			= 0.0f;
	edFontTextureName	= NULL;
	edFontname			= NULL;
	edCurrentCymbol		= NULL;
	lblSampleText		= NULL;
	CornerKind			= SCK_NONE;
	State				= ES_NONE;
	PreviousState		= ES_NONE;

	CEventManager::Instance()->Subscribe("KeyUp", this);
	CEventManager::Instance()->Subscribe("KeyDown", this);
	CEventManager::Instance()->Subscribe("MouseDown", this);

	#define BUTTONS_COUNT 8
	const char* ButtonNames[BUTTONS_COUNT] = {"Exit", "Load font", "Load texture", "Test phrase", "Save font", "Expose box", "<-", "->"};
	const CObjectCallback ButtonCallers[BUTTONS_COUNT] = {&ExitFontEditor, &LoadFont, &LoadTexture, &ShowTestPhrase, &SaveFont, &ExposeRect, &GoToPrevChar, &GoToNextChar};

	for (int i = 0; i < BUTTONS_COUNT; i++)
	{
		CButton *temp = CFactory::Instance()->New<CButton>(ButtonNames[i]);
		temp->SetText(static_cast<string>(ButtonNames[i]));
		temp->SetBox(CBox(LEFT_MARGIN, 20 + (BUTTON_HEIGHT + 10) * i, BUTTON_WIDTH, BUTTON_HEIGHT));
		temp->SetCallback(ButtonCallers[i], this);
	}

// 	CButton *temp = dynamic_cast<CButton*>(CGUIManager::Instance()->GetObject("<-"));
// 	temp->aabb = CAABB(LEFT_MARGIN, 20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, BUTTON_WIDTH / 2 - 5, BUTTON_HEIGHT);
//  	temp = dynamic_cast<CButton*>(CGUIManager::Instance()->GetObject("->"));
//  	temp->aabb = CAABB(LEFT_MARGIN + BUTTON_WIDTH / 2 + 10, 20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, BUTTON_WIDTH / 2 - 5, BUTTON_HEIGHT);

	edFontTextureName = CFactory::Instance()->New<CEdit>("edFontTextureName");
	edFontTextureName->SetText(static_cast<string>("Font_font"));
	edFontTextureName->SetBox(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, EDIT_WIDTH, BUTTON_HEIGHT));
//	edFontTextureName->Color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);

	edFontname = CFactory::Instance()->New<CEdit>("edFontname");
	edFontname->SetText(static_cast<string>("iich"));
	edFontname->SetBox(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 1), EDIT_WIDTH, BUTTON_HEIGHT));
	edFontname->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);

	lblSampleText = CFactory::Instance()->New<CLabel>("lblSampleText");
	lblSampleText->SetText(static_cast<string>("The quick brown fox jumps over a lazy dog."));
	lblSampleText->SetBox(CBox(INTERFACE_OFFSET_X + 20, 20 + (BUTTON_HEIGHT+10)*3, 500, 25));
	lblSampleText->Color = COLOR_WHITE;
	lblSampleText->SetVisibility(false);

	lblCharachterSelectedASCIIIndex = CFactory::Instance()->New<CLabel>("lblASCII");
	lblCharachterSelectedASCIIIndex->SetText("ASCII index: " + itos(CurrentSymbol));
	lblCharachterSelectedASCIIIndex->SetBox(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 2), EDIT_WIDTH, BUTTON_HEIGHT));
	lblCharachterSelectedASCIIIndex->Color = COLOR_WHITE;
	lblCharachterSelectedASCIIIndex->SetVisibility(true);

}

void CFontEditor::Render()
{
	GetPosition() = V2_ZERO;
	Color = COLOR_THIRD;
	SetLayer(0);
	SetScaling(1.0f);
	CRenderManager::Instance()->DrawSolidBox(this, CBox(Vector2(.0f, .0f), Vector2(INTERFACE_OFFSET_X, WindowHeight)));

	Offset.x = (int)Offset.x;
	Offset.y = (int)Offset.y;	

// 	if (Font != NULL)
// 		for (int i = 0; i < 256; i++)
// 		{
// 			char a[2];
// 			a[0] = (char)i+32;
// 			a[1] = 0;
// 			if (i == CurrentSymbol)
// 				CFontManager::Instance()->CurrentFont->tClr = RGBAf(0.9f, 0.4f, 0.3f, 0.9f);
// 			CFontManager::Instance()->PrintEx(Font->bbox[i].x1 * Zoom, Font->bbox[i].y1 * Zoom, 0.0f, a);
// 			if (i == CurrentSymbol)
// 				CFontManager::Instance()->CurrentFont->tClr = COLOR_WHITE;
// 		}

//	glScalef(Zoom, Zoom, 1.0f);

	if (FontTexture)
	{
		this->SetScaling(Zoom);
		this->GetPosition() = Offset;
		Color = COLOR_WHITE;
		CRenderManager::Instance()->DrawTexturedBox(this, CBox(V2_ZERO, Vector2(FontTexture->Width, FontTexture->Height)), FontTexture, CBox(V2_ZERO, V2_DIR_RIGHT + V2_DIR_UP).GetVertices());
	}

	SetLayer(-1);
	if (Font)
	{
		for (int i = 0; i < 5; i++)
		{
			Color = COLOR_THIRD;
			if (CornerKind == i)
				Color = RGBAf(0.9f, 0.4f, 0.3f, 0.9f);
			CRenderManager::Instance()->DrawSolidBox(this, CBox(SelectionBoxes[i * 4 + 0], SelectionBoxes[i * 4 + 2]));			
		}
		
		CRenderManager *RenerManager = CRenderManager::Instance();
		for (int i = 0; i < 256; i++)
		{
			Vector2 v0 = Font->Boxes[i].Min;
			Vector2 v1(Font->Boxes[i].Max.x, Font->Boxes[i].Min.y);
			Vector2 v2 = Font->Boxes[i].Max;
			Vector2 v3(Font->Boxes[i].Min.x, Font->Boxes[i].Max.y);
			Color = COLOR_WHITE;
			if (i == CurrentSymbol)
				Color = RGBAf(0.9f, 0.4f, 0.3f, 0.9f);
// 			RenerManager->DrawLine(this, v0, v1);
// 			RenerManager->DrawLine(this, v1, v2);
// 			RenerManager->DrawLine(this, v2, v3);
// 			RenerManager->DrawLine(this, v3, v0);
			RenerManager->DrawLinedBox(this, Font->Boxes[i]);
		}
	}
}

/*void CFontEditor::Update(float dt)
{
	MousePosition = CEngine::Instance()->MousePosition;

	switch (State)
	{
	case ES_GRIP_TOOL:
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		MouseDelta = Vector2(x, -y);
		if (!CBox(0, 0, INTERFACE_OFFSET_X, WindowHeight).Inside(MousePosition) &&  ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
			Offset += MouseDelta;
		break;
	case ES_MOVE_TOOL:
		if (Font == NULL)
			break;
		if (!((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
			break;
		if (MousePosition.x <= INTERFACE_OFFSET_X)
			break;

		int Mx = MousePosition.x / Zoom - Offset.x / Zoom;
		int My = MousePosition.y / Zoom - Offset.y / Zoom;
		int Dx0 = (Font->Boxes[CurrentSymbol].Max.x - Font->Boxes[CurrentSymbol].Min.x) /2;
		int Dx1 = (Font->Boxes[CurrentSymbol].Max.x - Font->Boxes[CurrentSymbol].Min.x) - (Font->Boxes[CurrentSymbol].Max.x - Font->Boxes[CurrentSymbol].Min.x) /2;
		int Dy0 = (Font->Boxes[CurrentSymbol].Max.y - Font->Boxes[CurrentSymbol].Min.y) /2;
		int Dy1 = (Font->Boxes[CurrentSymbol].Max.y - Font->Boxes[CurrentSymbol].Min.y) - (Font->Boxes[CurrentSymbol].Max.y - Font->Boxes[CurrentSymbol].Min.y) /2;

		if (CornerKind == SCK_CENTER)
		{
			Font->Boxes[CurrentSymbol].Min.x = Mx - Dx0;
			Font->Boxes[CurrentSymbol].Max.x = Mx + Dx1;
			Font->Boxes[CurrentSymbol].Min.y = My - Dy0;
			Font->Boxes[CurrentSymbol].Max.y = My + Dy1;
		}

		if (CornerKind == SCK_LEFT_BOTTOM)
		{
			Font->Boxes[CurrentSymbol].Min.x = Mx;
			Font->Boxes[CurrentSymbol].Min.y = My;
		}

		if (CornerKind == SCK_RIGHT_BOTTOM)
		{
			Font->Boxes[CurrentSymbol].Max.x = Mx;
			Font->Boxes[CurrentSymbol].Min.y = My;
		}

		if (CornerKind == SCK_RIGHT_TOP)
		{
			Font->Boxes[CurrentSymbol].Max.x = Mx;
			Font->Boxes[CurrentSymbol].Max.y = My;
		}

		if (CornerKind == SCK_LEFT_TOP)
		{
			Font->Boxes[CurrentSymbol].Min.x = Mx;
			Font->Boxes[CurrentSymbol].Max.y = My;
		}

		if (CornerKind != SCK_NONE)
			SetSelectedBoxTo(CurrentSymbol);
		break;
	}
	
	return;
}*/

CFontEditor::~CFontEditor()
{

}

void CFontEditor::SetSelectedBoxTo(int Index)
{
	if (Font == NULL)
		return;
	CurrentSymbol = Index; // @todo: Add range check
	lblCharachterSelectedASCIIIndex->SetText("ASCII Index: " + itos(CurrentSymbol + 32));

	CBox rect = Font->Boxes[CurrentSymbol];
	Vector2 Vertices[5] = 
	{
		Vector2(rect.Min.x, rect.Min.y),
		Vector2(rect.Max.x, rect.Min.y),
		Vector2(rect.Max.x, rect.Max.y),
		Vector2(rect.Min.x, rect.Max.y),
		Vector2((rect.Min.x + rect.Max.x) * 0.5f, (rect.Min.y + rect.Max.y) * 0.5f),
	};
	for (int i = 0; i < 5; i++)
	{
		SelectionBoxes[i*4 + 0] = Vertices[i] + V2_QuadBinCenter[0] * 2.0f;
		SelectionBoxes[i*4 + 1] = Vertices[i] + V2_QuadBinCenter[1] * 2.0f;
		SelectionBoxes[i*4 + 2] = Vertices[i] + V2_QuadBinCenter[2] * 2.0f;
		SelectionBoxes[i*4 + 3] = Vertices[i] + V2_QuadBinCenter[3] * 2.0f;
	}

}

bool LoadFont(CObject *Caller)
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	FontEditor->Font = CFontManager::Instance()->GetFont(FontEditor->edFontname->GetText());
	if (FontEditor->Font == NULL)
	{
		Log("ERROR", "Font %s not found within data/fonts", FontEditor->edFontname->GetText().c_str());
		return false;
	}
	FontEditor->FontTexture = FontEditor->Font->GetTexture();
	if (FontEditor->FontTexture == NULL)
	{
		Log("ERROR", "Failed to load texture of font '%s'", FontEditor->edFontname->GetText().c_str());
		return false;
	}
	FontEditor->lblSampleText->SetFont(FontEditor->Font);
// 	for (int i = 0; i < 256; i++)
// 	{
// 		CRecti rect = FontEditor->Font->bbox[i];
// 		int x0 = rect.x0, x1 = rect.x1, y0 = rect.y0, y1 = rect.y1;
// 		int hw = (x1 - x0) / 2, hh = (y1 - y0) / 2;
// 
// 		FontEditor->SelectionBoxes[i * 16 + 0].In(x0 - 2 - 0, y0 - 0);  //x0 - 2, y0, 4, hh * 2
// 		FontEditor->SelectionBoxes[i * 16 + 1].In(x0 - 2 + 4, y0 - 0);  //x0 - 2, y0, 4, hh * 2
// 		FontEditor->SelectionBoxes[i * 16 + 2].In(x0 - 2 + 4, y0 + hh*2);  //x0 - 2, y0, 4, hh * 2
// 		FontEditor->SelectionBoxes[i * 16 + 3].In(x0 - 2 - 0, y0 + hh*2);  //x0 - 2, y0, 4, hh * 2
// 
// 		FontEditor->SelectionBoxes[i * 16 + 4].In(x0 - 0, y0 - 2);  //x0, y0 - 2, hw * 2, 4
// 		FontEditor->SelectionBoxes[i * 16 + 5].In(x0 + hw*2, y0 - 2);  //x0, y0 - 2, hw * 2, 4
// 		FontEditor->SelectionBoxes[i * 16 + 6].In(x0 + hw*2, y0 - 2 +4);  //x0, y0 - 2, hw * 2, 4
// 		FontEditor->SelectionBoxes[i * 16 + 7].In(x0 - 0, y0 - 2 + 4);  //x0, y0 - 2, hw * 2, 4
// 
// 		FontEditor->SelectionBoxes[i * 16 + 8].In(x1 - 2 - 0, y0 - 0);  //x1 - 2, y0, 4, hh * 2
// 		FontEditor->SelectionBoxes[i * 16 + 9].In(x1 - 2 + 4, y0 - 0);  //x1 - 2, y0, 4, hh * 2
// 		FontEditor->SelectionBoxes[i * 16 + 10].In(x1 - 2 + 4, y0 + hh*2);  //x1 - 2, y0, 4, hh * 2
// 		FontEditor->SelectionBoxes[i * 16 + 11].In(x1 - 2 - 0, y0 + hh*2);  //x1 - 2, y0, 4, hh * 2
// 
// 		FontEditor->SelectionBoxes[i * 16 + 12].In(x0 - 0, y1 - 2);  //x0, y1 - 2, hw * 2, 4
// 		FontEditor->SelectionBoxes[i * 16 + 13].In(x0 + hw*2, y1 - 2);  //x0, y1 - 2, hw * 2, 4
// 		FontEditor->SelectionBoxes[i * 16 + 14].In(x0 + hw*2, y1 - 2 + 4);  //x0, y1 - 2, hw * 2, 4
// 		FontEditor->SelectionBoxes[i * 16 + 15].In(x0 - 0, y1 - 2 + 4);  //x0, y1 - 2, hw * 2, 4
// 	}
// 	glEnableClientState(GL_VERTEX_ARRAY);
// 	glVertexPointer(2, GL_FLOAT, 0, FontEditor->SelectionBoxes);
	return true;
}

bool SaveFont(CObject *Caller)
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	FontEditor->Font->SetName(FontEditor->edFontname->GetText());
	FontEditor->Font->SetTexture(FontEditor->FontTexture->GetName());
	FontEditor->Font->SaveToFile(FontEditor->Font->GetFilename());
	return true;
}

bool LoadTexture(CObject *Caller) /* Опять же не Load() а Acquire(). */
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	CTexture *TempFontTexture = CTextureManager::Instance()->Get(FontEditor->edFontTextureName->GetText());
	if (TempFontTexture == NULL)
	{
		Log("ERROR", "Font texture %s not found", FontEditor->edFontTextureName->GetText().c_str());
		// Создать объект текст с таймером здесь, выдавать ошибки с его помощью прямо на экран.
		return false;
	}
	FontEditor->FontTexture = TempFontTexture;
	return true;
}

bool ExitFontEditor(CObject *Caller)
{
	CEngine::Instance()->ShutDown();
	return true;
}

bool ShowTestPhrase(CObject *Caller)
{
	CLabel *Lbl = dynamic_cast<CLabel*>(CGUIManager::Instance()->Get("lblSampleText"));
	Lbl->SetVisibility(!Lbl->GetVisibility());
	return true;
}

bool GoToPrevChar(CObject *Caller)
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	FontEditor->SetSelectedBoxTo(FontEditor->CurrentSymbol - 1);
	return true;
}

bool GoToNextChar(CObject *Caller)
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	FontEditor->SetSelectedBoxTo(FontEditor->CurrentSymbol + 1);
	return true;
}

bool ExposeRect(CObject *Caller)
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	FontEditor->Font->Boxes[FontEditor->CurrentSymbol].Min.x = (FontEditor->Font->GetTexture())->Width / 2;
	FontEditor->Font->Boxes[FontEditor->CurrentSymbol].Max.x = (FontEditor->Font->GetTexture())->Width / 2;
	FontEditor->Font->Boxes[FontEditor->CurrentSymbol].Min.y = (FontEditor->Font->GetTexture())->Height / 2;
	FontEditor->Font->Boxes[FontEditor->CurrentSymbol].Max.y = (FontEditor->Font->GetTexture())->Height / 2;
	
	FontEditor->Font->Boxes[FontEditor->CurrentSymbol].Max.x += 20;
	FontEditor->Font->Boxes[FontEditor->CurrentSymbol].Max.y += 20;

	FontEditor->SetSelectedBoxTo(FontEditor->CurrentSymbol);
	return true;
}
