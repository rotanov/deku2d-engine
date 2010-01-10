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
	Zoom = clampf(Zoom, ZOOM_MIN, ZOOM_MAX);
	if (Zoom == OldZoom)
		return;
	float TempX = 0.5f, TempY = 0.5f;
	float TempWidth = 0.0f, TempHeight = 0.0f;
	if (FontTexture != NULL)
	{
		TempWidth = FontTexture->width;
		TempHeight= FontTexture->height;
	}
	else
	{
		TempWidth = Font->GetSymbolsBBOX().Width();
		TempHeight = Font->GetSymbolsBBOX().Height();
	}
	TempX = (MousePosition.x - OffsetX) / (TempWidth * OldZoom);
	TempY = (MousePosition.y - OffsetY) / (TempHeight * OldZoom);
	TempX = clampf(TempX, 0.0f, 1.0f);
	TempY = clampf(TempY, 0.0f, 1.0f);
	OffsetX = - TempX * (TempWidth * Zoom) + MousePosition.x;
	OffsetY = - TempY * (TempHeight * Zoom) + MousePosition.y;
}

bool CFontEditor::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	switch (state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_g:
			isGripToolEnabled = true;
			break;
		case SDL_BUTTON_WHEELUP:
			SetZoom(Zoom + ZOOM_STEP);
			break;
		case SDL_BUTTON_WHEELDOWN:
			SetZoom(Zoom - ZOOM_STEP);
			break;
		case SDL_BUTTON_LEFT:
			if (Font != NULL)
				for(int i = 0; i < 256; i++)
					if (Font->bbox[i].x0 < (MousePosition.x / Zoom - OffsetX / Zoom) && Font->bbox[i].x1 > (MousePosition.x / Zoom - OffsetX / Zoom) &&
						Font->bbox[i].y0 < (MousePosition.y / Zoom - OffsetY / Zoom) && Font->bbox[i].y1 > (MousePosition.y / Zoom - OffsetY / Zoom))
					{
						CurrentSymbol = i;
						break;
					}
					break;
		case  SDL_BUTTON_RIGHT:
			if (Font == NULL)
				break;
			float a1, a2, a3, a4;
			a1 = abs(MousePosition.x / Zoom - OffsetX / Zoom - Font->bbox[CurrentSymbol].x0);
			a2 = abs(MousePosition.x / Zoom - OffsetX / Zoom - Font->bbox[CurrentSymbol].x1);
			a3 = abs(MousePosition.y / Zoom - OffsetY / Zoom - Font->bbox[CurrentSymbol].y0);
			a4 = abs(MousePosition.y / Zoom - OffsetY / Zoom - Font->bbox[CurrentSymbol].y1);
			a1 = Min(a1, Min(a2, Min(a3, a4)));
			if (a1 == a2)
				Font->bbox[CurrentSymbol].x1 = MousePosition.x / Zoom - OffsetX / Zoom; else
				if (a1 == a3)
					Font->bbox[CurrentSymbol].y0 = MousePosition.y / Zoom - OffsetY / Zoom; else
					if (a1 == a4)
						Font->bbox[CurrentSymbol].y1 = MousePosition.y / Zoom - OffsetY / Zoom; else
						Font->bbox[CurrentSymbol].x0 = MousePosition.x / Zoom - OffsetX / Zoom;
			break;
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_g:
			//doGripping = false;
			break;
		}
		break;
	}
	return true;
}

CFontEditor::CFontEditor()
{
	isGripToolEnabled = false;
	doGripping = false;
	CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);

	Zoom				= 2.0f;
	FontTexture		= NULL;
	Font				= NULL;
	CurrentSymbol		= 0;
	OffsetX				= INTERFACE_OFFSET_X + 5;
	OffsetY				= 0.0f;
	edFontTextureName	= NULL;
	edFontname			= NULL;
	edCurrentCymbol	= NULL;
	lblSampleText		= NULL;

#define BUTTONS_COUNT 4
	const char* ButtonNames[BUTTONS_COUNT] = {"Load font", "Load texture", "Exit", "Test phrase",};
	const CObjectCallback ButtonCallers[BUTTONS_COUNT] = {&LoadFont, &LoadTexture, &ExitFontEditor, &ShowTestPhrase};

	for(int i = 0; i < BUTTONS_COUNT; i++)
	{
		CButton *temp = (new CButton(CAABB(LEFT_MARGIN, 20 + (BUTTON_HEIGHT+10)*i, BUTTON_WIDTH, BUTTON_HEIGHT),
		ButtonNames[i],
		RGBAf(0.4f, 0.4f, 0.4f, 1.0f)));	
		temp->SetCallback(ButtonCallers[i], this);
	}

	edFontTextureName = new CEdit();
	edFontTextureName->aabb = CAABB(LEFT_MARGIN, 350, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontTextureName->Text = "Font";
	edFontTextureName->color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);

	edFontname = new CEdit();
	edFontname->aabb = CAABB(LEFT_MARGIN, 500, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontname->Text = "Font";
	edFontname->color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);
	edFontname->color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);
	edFontname->SetParent(CGUIManager::Instance());

	lblSampleText = new CLabel("The quick brown fox jumps over a lazy dog.");
	lblSampleText->aabb = CAABB(INTERFACE_OFFSET_X + 20, 20 + (BUTTON_HEIGHT+10)*3, 500, 25);
	lblSampleText->color = COLOR_WHITE;
	lblSampleText->SetName("lblSampleText");
}

bool CFontEditor::Render()
{
	CEngine::Instance()->GetState(STATE_SCREEN_HEIGHT, &wheight);
	CEngine::Instance()->GetState(STATE_SCREEN_WIDTH, &wwidth);

	CPrimitiveRender PRender;
	PRender.lClr = COLOR_FIRST;
	PRender.sClr = COLOR_THIRD;
	PRender.pClr = COLOR_FIRST;
	glLoadIdentity();
	PRender.grRectC(Vector2(.0f, .0f), Vector2(INTERFACE_OFFSET_X, wheight));
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	PRender.grCircleL(MousePosition, 5);
	int fps;
	CEngine::Instance()->GetState(STATE_FPS_COUNT, &fps);
	CEngine::Instance()->FontManager->CurrentFont->tClr = COLOR_WHITE;
	CEngine::Instance()->FontManager->PrintEx(10, 400, 0.0f, "FPS: %d", fps);
	glLoadIdentity();

	gToggleScissor(true);
	gScissor(INTERFACE_OFFSET_X, 0, wwidth - INTERFACE_OFFSET_X, wheight);
	glLoadIdentity();
	glTranslatef(OffsetX, OffsetY, .0f);
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
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	if (Font)
	{
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
	return true;
}

bool CFontEditor::Update(float dt)
{
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	MouseDelta = Vector2(x, -y);
	CEngine::Instance()->GetState(STATE_MOUSE_XY, &MousePosition);
	if (isGripToolEnabled && !CAABB(0, 0, INTERFACE_OFFSET_X, wheight).Inside(MousePosition) &&  ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
	{
		OffsetX += MouseDelta.x;
		OffsetY += MouseDelta.y;
	}
	return true;
}

bool LoadFont(CObject *Caller)
{
	dynamic_cast<CFontEditor*>(Caller)->Font = CFontManager::Instance()->GetFont((dynamic_cast<CFontEditor*>(Caller))->edFontname->Text.c_str());
	if (dynamic_cast<CFontEditor*>(Caller)->Font == NULL)
	{
		Log.Log("Error", "Font %s not found within data/fonts", dynamic_cast<CFontEditor*>(Caller)->edFontname->Text);
		return false;
	}
	dynamic_cast<CFontEditor*>(Caller)->FontTexture = dynamic_cast<CFontEditor*>(Caller)->Font->GetTexture();
	dynamic_cast<CFontEditor*>(Caller)->lblSampleText->SetFont(dynamic_cast<CFontEditor*>(Caller)->Font);
	return true;
}

bool SaveFont(CObject *Caller)
{
	// Написать код сохранения изменений, сделаных в шрифте, в файле здесь.
	return true;
}

bool LoadTexture(CObject *Caller) /* Опять же не Load() а Acquire(). */
{
	dynamic_cast<CFontEditor*>(Caller)->FontTexture = CTextureManager::Instance()->GetTextureByName(dynamic_cast<CFontEditor*>(Caller)->edFontTextureName->Text);
	if (dynamic_cast<CFontEditor*>(Caller)->FontTexture == NULL)
	{
#ifdef _WIN32
		MessageBox(0, "Texture not found", "Error", MB_ICONERROR | MB_OK);
#endif
		return false;
	}
	return true;
}

bool ExitFontEditor(CObject *Caller)
{
	SDLGLExit(0);
	return true;
}

bool ShowTestPhrase(CObject *Caller)
{
	CRenderObject *Lbl = NULL;
	Lbl = dynamic_cast<CRenderObject*>(CGUIManager::Instance()->GetObject("lblSampleText"));
	Lbl->visible = !Lbl->visible;
	return true;
}