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
		TempWH = Vector2(FontTexture->width, FontTexture->height);
	else
		Temp = Vector2(Font->GetSymbolsBBOX().Width(), Font->GetSymbolsBBOX().Height());
	Temp.x = (MousePosition.x - Offset.x) / (TempWH.x * OldZoom);
	Temp.y = (MousePosition.y - Offset.y) / (TempWH.y * OldZoom);
	Temp.ClampTo(V2_ZERO, Vector2(1.0f, 1.0f));
	Offset.x = - Temp.x * (TempWH.x * Zoom) + MousePosition.x;
	Offset.y = - Temp.y * (TempWH.y * Zoom) + MousePosition.y;
}

bool CFontEditor::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	switch (state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_g:
			PreviousState = State;
			State = ES_GRIP_TOOL;
			break;
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
				for(int i = 0; i < 256; i++)
					if (Font->bbox[i].x0 < (MousePosition.x / Zoom - Offset.x / Zoom) && Font->bbox[i].x1 > (MousePosition.x / Zoom - Offset.x / Zoom) &&
						Font->bbox[i].y0 < (MousePosition.y / Zoom - Offset.y / Zoom) && Font->bbox[i].y1 > (MousePosition.y / Zoom - Offset.y / Zoom))
					{
						bool tempbool = (i != CurrentSymbol);
						SetSelectedBoxTo(i);
						if (tempbool)
							return true;
						break;
					}				
				{
					if (State == ES_GRIP_TOOL)
						break;
					float Mx = MousePosition.x / Zoom - Offset.x / Zoom;
					float My = MousePosition.y / Zoom - Offset.y / Zoom;
					CornerKind = SCK_NONE;
					if (CAABB(SelectionBoxes[0], SelectionBoxes[2]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_LEFT_BOTTOM;
					if (CAABB(SelectionBoxes[4], SelectionBoxes[6]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_RIGHT_BOTTOM;
					if (CAABB(SelectionBoxes[8], SelectionBoxes[10]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_RIGHT_TOP;
					if (CAABB(SelectionBoxes[12], SelectionBoxes[14]).Inside(Vector2(Mx, My)))
						CornerKind = SCK_LEFT_TOP;
					if (CAABB(SelectionBoxes[16], SelectionBoxes[18]).Inside(Vector2(Mx, My)))
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
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_g:
			State = PreviousState;
			break;
		}
		break;
	}
	return true;
}

CFontEditor::CFontEditor()
{
	CEngine::Instance()->GetState(STATE_SCREEN_HEIGHT, &WindowHeight);
	CEngine::Instance()->GetState(STATE_SCREEN_WIDTH, &WindowWidth);
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

	CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);

	#define BUTTONS_COUNT 8
	const char* ButtonNames[BUTTONS_COUNT] = {"Exit", "Load font", "Load texture", "Test phrase", "Save font", "Expose box", "<-", "->"};
	const CObjectCallback ButtonCallers[BUTTONS_COUNT] = {&ExitFontEditor, &LoadFont, &LoadTexture, &ShowTestPhrase, &SaveFont, &ExposeRect, &GoToPrevChar, &GoToNextChar};

	for(int i = 0; i < BUTTONS_COUNT; i++)
	{
		CButton *temp = new CButton();
		temp->aabb = CAABB(LEFT_MARGIN, 20 + (BUTTON_HEIGHT + 10) * i, BUTTON_WIDTH, BUTTON_HEIGHT);
		temp->Text = ButtonNames[i];
		temp->SetCallback(ButtonCallers[i], this);
		temp->SetName(ButtonNames[i]);
	}

// 	CButton *temp = dynamic_cast<CButton*>(CGUIManager::Instance()->GetObject("<-"));
// 	temp->aabb = CAABB(LEFT_MARGIN, 20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, BUTTON_WIDTH / 2 - 5, BUTTON_HEIGHT);
//  	temp = dynamic_cast<CButton*>(CGUIManager::Instance()->GetObject("->"));
//  	temp->aabb = CAABB(LEFT_MARGIN + BUTTON_WIDTH / 2 + 10, 20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, BUTTON_WIDTH / 2 - 5, BUTTON_HEIGHT);

	edFontTextureName = new CEdit();
	edFontTextureName->aabb = CAABB(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontTextureName->Text = "Font";
	edFontTextureName->Color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);

	edFontname = new CEdit();
	edFontname->aabb = CAABB(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 1), EDIT_WIDTH, BUTTON_HEIGHT);
	edFontname->Text = "Font";
	edFontname->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);

	lblSampleText = new CLabel("The quick brown fox jumps over a lazy dog.");
	lblSampleText->aabb = CAABB(INTERFACE_OFFSET_X + 20, 20 + (BUTTON_HEIGHT+10)*3, 500, 25);
	lblSampleText->Color = COLOR_WHITE;
	lblSampleText->SetName("lblSampleText");
	lblSampleText->Visible = false;

	lblCharachterSelectedASCIIIndex = new CLabel("ASCII index: " + itos(CurrentSymbol));
	lblCharachterSelectedASCIIIndex->aabb = CAABB(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 2), EDIT_WIDTH, BUTTON_HEIGHT);
	lblCharachterSelectedASCIIIndex->Color = COLOR_WHITE;
	lblCharachterSelectedASCIIIndex->SetName("lblASCII");
	lblCharachterSelectedASCIIIndex->Visible = true;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, SelectionBoxes);
}

bool CFontEditor::Render()
{

	glLoadIdentity();
	PRender.grRectC(Vector2(.0f, .0f), Vector2(INTERFACE_OFFSET_X, WindowHeight));
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	PRender.grCircleL(MousePosition, 5);
	int fps;
	CEngine::Instance()->GetState(STATE_FPS_COUNT, &fps);
	CFontManager::Instance()->CurrentFont->tClr = COLOR_WHITE;
	CFontManager::Instance()->PrintEx(5, WindowHeight - 20, 0.0f, "FPS: %d", fps);
	glLoadIdentity();

	gToggleScissor(true);
	gScissor(INTERFACE_OFFSET_X, 0, WindowWidth - INTERFACE_OFFSET_X, WindowHeight);
	glLoadIdentity();
	Offset.x = (int)Offset.x;
	Offset.y = (int)Offset.y;
	Offset.glTranslate();

// 	if (Font != NULL)
// 		for(int i=0;i<256;i++)
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

	glScalef(Zoom, Zoom, 1.0f);

	glEnable(GL_TEXTURE_2D);

	if(FontTexture)
	{
		FontTexture->Bind();
		RGBAf(1.0f, 1.0f, 1.0f, 1.0f).glSet();
		glBegin(GL_QUADS);
		glTexCoord2f(.0f, .0f);
		glVertex2f(.0f, .0f);

		glTexCoord2f(1.0f, .0f);
		glVertex2f(FontTexture->width, .0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(FontTexture->width, FontTexture->height);

		glTexCoord2f(.0f, 1.0f);
		glVertex2f(.0f, FontTexture->height);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LINE_WIDTH);
	glLineWidth(1.0f);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4fv(&COLOR_THIRD.r);
	if (Font)
	{

		glBegin(GL_QUADS);
		for(int i=0;i<5;i++)
		{

// 			PRender.grRectS(SelectionBoxes[i * 16 + 0], SelectionBoxes[i * 16 + 3]);
// 			PRender.grRectS(SelectionBoxes[i * 16 + 4], SelectionBoxes[i * 16 + 6]);
// 			PRender.grRectS(SelectionBoxes[i * 16 + 8], SelectionBoxes[i * 16 + 10]);
// 			PRender.grRectS(SelectionBoxes[i * 16 + 12], SelectionBoxes[i * 16 + 15]);
			glColor4fv(&COLOR_THIRD.r);
			if (CornerKind == i)
				glColor4f(0.9f, 0.4f, 0.3f, 0.9f);
			glArrayElement(i*4+0);
			glArrayElement(i*4+1);
			glArrayElement(i*4+2);
			glArrayElement(i*4+3);
// 					glBegin(GL_QUADS);
// 					  SelectionBoxes[i*4+0].glVertex();
// 					  SelectionBoxes[i*4+1].glVertex();
// 					  SelectionBoxes[i*4+2].glVertex();
// 					  SelectionBoxes[i*4+3].glVertex();
// 					glEnd();
			
		}
		glEnd();
		
		glBegin(GL_LINES);
		for(int i=0;i<256;i++)
		{
			if (i == CurrentSymbol)
				RGBAf(0.9f, 0.4f, 0.3f, 0.9f).glSet();
			glVertex2i(Font->bbox[i].x0, Font->bbox[i].y0);
			glVertex2i(Font->bbox[i].x1, Font->bbox[i].y0);

			glVertex2i(Font->bbox[i].x1, Font->bbox[i].y0);
			glVertex2i(Font->bbox[i].x1, Font->bbox[i].y1);

			glVertex2i(Font->bbox[i].x1, Font->bbox[i].y1);
			glVertex2i(Font->bbox[i].x0, Font->bbox[i].y1);

			glVertex2i(Font->bbox[i].x0, Font->bbox[i].y1);
			glVertex2i(Font->bbox[i].x0, Font->bbox[i].y0);
			if (i == CurrentSymbol)
				COLOR_WHITE.glSet();
		}
		glEnd();
	}
	gToggleScissor(false);
	glEnable(GL_DEPTH_TEST);
	return true;
}

bool CFontEditor::Update(float dt)
{
	CEngine::Instance()->GetState(STATE_MOUSE_XY, &MousePosition);

	switch (State)
	{
	case ES_GRIP_TOOL:
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		MouseDelta = Vector2(x, -y);
		if (!CAABB(0, 0, INTERFACE_OFFSET_X, WindowHeight).Inside(MousePosition) &&  ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
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
		int Dx0 = (Font->bbox[CurrentSymbol].x1 - Font->bbox[CurrentSymbol].x0) /2;
		int Dx1 = (Font->bbox[CurrentSymbol].x1 - Font->bbox[CurrentSymbol].x0) - (Font->bbox[CurrentSymbol].x1 - Font->bbox[CurrentSymbol].x0) /2;
		int Dy0 = (Font->bbox[CurrentSymbol].y1 - Font->bbox[CurrentSymbol].y0) /2;
		int Dy1 = (Font->bbox[CurrentSymbol].y1 - Font->bbox[CurrentSymbol].y0) - (Font->bbox[CurrentSymbol].y1 - Font->bbox[CurrentSymbol].y0) /2;

		if (CornerKind == SCK_CENTER)
		{
			Font->bbox[CurrentSymbol].x0 = Mx - Dx0;
			Font->bbox[CurrentSymbol].x1 = Mx + Dx1;
			Font->bbox[CurrentSymbol].y0 = My - Dy0;
			Font->bbox[CurrentSymbol].y1 = My + Dy1;
		}

		if (CornerKind == SCK_LEFT_BOTTOM)
		{
			Font->bbox[CurrentSymbol].x0 = Mx;
			Font->bbox[CurrentSymbol].y0 = My;
		}

		if (CornerKind == SCK_RIGHT_BOTTOM)
		{
			Font->bbox[CurrentSymbol].x1 = Mx;
			Font->bbox[CurrentSymbol].y0 = My;
		}

		if (CornerKind == SCK_RIGHT_TOP)
		{
			Font->bbox[CurrentSymbol].x1 = Mx;
			Font->bbox[CurrentSymbol].y1 = My;
		}

		if (CornerKind == SCK_LEFT_TOP)
		{
			Font->bbox[CurrentSymbol].x0 = Mx;
			Font->bbox[CurrentSymbol].y1 = My;
		}

		if (CornerKind != SCK_NONE)
			SetSelectedBoxTo(CurrentSymbol);
		break;
	}
	
	return true;
}

CFontEditor::~CFontEditor()
{

}

void CFontEditor::SetSelectedBoxTo(int Index)
{
	if (Font == NULL)
		return;
	CurrentSymbol = Index; // TODO: Add range check
	lblCharachterSelectedASCIIIndex->Text = "ASCII Index: " + itos(CurrentSymbol + 32);

	CRecti rect = Font->bbox[CurrentSymbol];
	Vector2 Vertices[5] = 
	{
		Vector2(rect.x0, rect.y0),
		Vector2(rect.x1, rect.y0),
		Vector2(rect.x1, rect.y1),
		Vector2(rect.x0, rect.y1),
		Vector2((rect.x0 + rect.x1) * 0.5f, (rect.y0 + rect.y1) * 0.5f),
	};
	for(int i = 0; i < 5; i++)
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
	FontEditor->Font = CFontManager::Instance()->GetFont(FontEditor->edFontname->Text.c_str());
	if (FontEditor->Font == NULL)
	{
		Log.Log("Error", "Font %s not found within data/fonts", FontEditor->edFontname->Text);
		return false;
	}
	FontEditor->FontTexture = FontEditor->Font->GetTexture();
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
	FontEditor->Font->SetTexture(FontEditor->FontTexture->GetName());
	FontEditor->Font->SaveToFile();
	return true;
}

bool LoadTexture(CObject *Caller) /* Опять же не Load() а Acquire(). */
{
	CFontEditor *FontEditor = dynamic_cast<CFontEditor *>(Caller);
	if (FontEditor == NULL)
		return false;
	CTexture *TempFontTexture = CTextureManager::Instance()->GetTextureByName(FontEditor->edFontTextureName->Text);
	if (TempFontTexture == NULL)
	{
		Log.Log("ERROR", "Font texture %s not found", FontEditor->edFontTextureName->Text);
		// Создать объект текст с таймером здесь, выдавать ошибки с его помощью прямо на экран.
		return false;
	}
	FontEditor->FontTexture = TempFontTexture;
	return true;
}

bool ExitFontEditor(CObject *Caller)
{
	SDL_Event Event;
	Event.type = SDL_QUIT;
	SDL_PushEvent(&Event); // Не так.
	return true;
}

bool ShowTestPhrase(CObject *Caller)
{
	CRenderObject *Lbl = NULL;
	Lbl = dynamic_cast<CRenderObject*>(CGUIManager::Instance()->GetObject("lblSampleText"));
	Lbl->Visible = !Lbl->Visible;
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
	FontEditor->Font->bbox[FontEditor->CurrentSymbol].x0 = (FontEditor->Font->GetTexture())->width / 2;
	FontEditor->Font->bbox[FontEditor->CurrentSymbol].x1 = (FontEditor->Font->GetTexture())->width / 2;
	FontEditor->Font->bbox[FontEditor->CurrentSymbol].y0 = (FontEditor->Font->GetTexture())->height / 2;
	FontEditor->Font->bbox[FontEditor->CurrentSymbol].y1 = (FontEditor->Font->GetTexture())->height / 2;
	
	FontEditor->Font->bbox[FontEditor->CurrentSymbol].x1 += 20;
	FontEditor->Font->bbox[FontEditor->CurrentSymbol].y1 += 20;

	FontEditor->SetSelectedBoxTo(FontEditor->CurrentSymbol);
	return true;
}
