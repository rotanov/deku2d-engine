#include "Tileset editor.h"
#include "2de_Engine.h"
#include "2de_LuaUtils.h"
#include "2de_GameUtils.h"

void CTilesetEditor::SetZoom(float AZoom)
{
	if (TilesetTexture == NULL && Tileset == NULL)
		return;
	float OldZoom = Zoom;
	Zoom = AZoom;
	Zoom = Clamp(Zoom, ZOOM_MIN, ZOOM_MAX);
	if (Zoom == OldZoom)
		return;
	Vector2 Temp;
	Vector2 TempWH;
	if (TilesetTexture != NULL)
		TempWH = Vector2(TilesetTexture->Width, TilesetTexture->Height);
// 	else
// 		Temp = Vector2(Font->GetSymbolsBBOX().Width(), Font->GetSymbolsBBOX().Height());
	Temp.x = (MousePosition.x - Offset.x) / (TempWH.x * OldZoom);
	Temp.y = (MousePosition.y - Offset.y) / (TempWH.y * OldZoom);
	Temp.ClampTo(V2_ZERO, Vector2(1.0f, 1.0f));
	Offset.x = - Temp.x * (TempWH.x * Zoom) + MousePosition.x;
	Offset.y = - Temp.y * (TempWH.y * Zoom) + MousePosition.y;
}

void CTilesetEditor::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_g:
			PreviousState = State;
			State = ES_GRIP_TOOL;
			break;
		case SDL_BUTTON_WHEELUP: case SDLK_EQUALS:
			SetZoom(Zoom + ZOOM_STEP);
			break;
		case SDL_BUTTON_WHEELDOWN: case SDLK_MINUS:
			SetZoom(Zoom - ZOOM_STEP);
			break;
		case SDL_BUTTON_LEFT:
			if (MousePosition.x < INTERFACE_OFFSET_X)
				break;
				break;
		case  SDL_BUTTON_RIGHT:
			break;
		}
	}
	else if (AEvent.GetName() == "KeyUp")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_g:
			State = PreviousState;
			break;
		}
	}
}

CTilesetEditor::CTilesetEditor()
{
	WindowWidth = CGLWindow::Instance()->GetWidth();
	WindowHeight = CGLWindow::Instance()->GetHeight();
	PRender.lClr			= COLOR_FIRST;
	PRender.sClr			= COLOR_THIRD;
	PRender.pClr			= COLOR_FIRST;
	Zoom					= 2.0f;
	TilesetTexture			= NULL;
	Tileset					= NULL;
	Offset.x				= INTERFACE_OFFSET_X + 5;
	Offset.y				= 0.0f;
	edTilesetTextureName	= NULL;
	edTilesetName			= NULL;
	State					= ES_NONE;
	PreviousState			= ES_NONE;

	CEventManager::Instance()->Subscribe("KeyDown", this);
	CEventManager::Instance()->Subscribe("KeyUp", this);

#define BUTTONS_COUNT 4
	const char* ButtonNames[BUTTONS_COUNT] = {"Exit", "Load tileset", "Load texture", "Save tileset", };
	const CObjectCallback ButtonCallers[BUTTONS_COUNT] = {&ExitEditor, &LoadTileset, &LoadTexture, &SaveTileset, };

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

	edTilesetTextureName = new CLabeledEdit(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * BUTTONS_COUNT, EDIT_WIDTH, BUTTON_HEIGHT), "Texture:");
	CFactory::Instance()->Add(edTilesetTextureName, "edTilesetTexutreName");
	edTilesetTextureName->SetText(static_cast<string>("verdana9"));
	edTilesetTextureName->Color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);

	edTilesetName = new CLabeledEdit(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 1) + 20, EDIT_WIDTH, BUTTON_HEIGHT), "Tileset:");
	CFactory::Instance()->Add(edTilesetName, "edTilesetName");
	//edTilesetName->aabb = CAABB(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 1), EDIT_WIDTH, BUTTON_HEIGHT);
	edTilesetName->SetText(static_cast<string>("SnowTs"));
	edTilesetName->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);

	edHorNumTiles = new CLabeledEdit(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 2) + 40, EDIT_WIDTH, BUTTON_HEIGHT), "HorNumTiles:");
	CFactory::Instance()->Add(edHorNumTiles, "edHorNumTiles");
	edHorNumTiles->SetText(static_cast<string>("4"));
	edHorNumTiles->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);

	edVerNumTiles = new CLabeledEdit(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 3) + 60, EDIT_WIDTH, BUTTON_HEIGHT), "VerNumTiles:");
	CFactory::Instance()->Add(edVerNumTiles, "edVerNumTiles");
	edVerNumTiles->SetText(static_cast<string>("2"));
	edVerNumTiles->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);

	edTileWidth = new CLabeledEdit(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 4) + 80, EDIT_WIDTH, BUTTON_HEIGHT), "TileWidth:");
	CFactory::Instance()->Add(edTileWidth, "edTileWidth");
	edTileWidth->SetText(static_cast<string>("32"));
	edTileWidth->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);

	edTileHeight = new CLabeledEdit(CBox(LEFT_MARGIN,  20 + (BUTTON_HEIGHT + 10) * (BUTTONS_COUNT + 5) + 100, EDIT_WIDTH, BUTTON_HEIGHT), "TileHeight:");
	CFactory::Instance()->Add(edTileHeight, "edTileHeight");
	edTileHeight->SetText(static_cast<string>("32"));
	edTileHeight->Color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);


// 	lblSampleText = new CLabel("The quick brown fox jumps over a lazy dog.");
// 	lblSampleText->aabb = CAABB(INTERFACE_OFFSET_X + 20, 20 + (BUTTON_HEIGHT+10)*3, 500, 25);
// 	lblSampleText->Color = COLOR_WHITE;
// 	lblSampleText->SetName("lblSampleText");
// 	lblSampleText->Visible = false;

// 	glEnableClientState(GL_VERTEX_ARRAY);
// 	glVertexPointer(2, GL_FLOAT, 0, SelectionBoxes);
// 	edTilesetName->SetFont(CFontManager::Instance()->Get("verdana9"));
// 	edTilesetName->SetStyle()
}

void CTilesetEditor::Render()
{
	Position = V2_ZERO;
	Color = COLOR_THIRD;
	SetLayer(0);
	SetScaling(1.0f);
	CRenderManager::Instance()->DrawSolidBox(this, CBox(Vector2(.0f, .0f), Vector2(INTERFACE_OFFSET_X, WindowHeight)));





	Offset.x = (int)Offset.x;
	Offset.y = (int)Offset.y;
//	Offset.glTranslate();

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

	SetScaling(Zoom);

//	glEnable(GL_TEXTURE_2D);

	if (TilesetTexture)
	{
//		TilesetTexture->Bind();
//		RGBAf(1.0f, 1.0f, 1.0f, 1.0f).glSet();
// 		glBegin(GL_QUADS);
// 		glTexCoord2f(.0f, .0f);
// 		glVertex2f(.0f, .0f);
// 
// 		glTexCoord2f(1.0f, .0f);
// 		glVertex2f(TilesetTexture->Width, .0f);
// 
// 		glTexCoord2f(1.0f, 1.0f);
// 		glVertex2f(TilesetTexture->Width, TilesetTexture->Height);
// 
// 		glTexCoord2f(.0f, 1.0f);
// 		glVertex2f(.0f, TilesetTexture->Height);
// 		glEnd();
	}

	//glDisable(GL_TEXTURE_2D);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (Tileset)
	{
		//Tileset->RenderTileSet();

/*
		glBegin(GL_QUADS);
		for (int i = 0; i < 5; i++)
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
		for (int i = 0; i < 256; i++)
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
*/
	}
	//gToggleScissor(false);
	//glEnable(GL_DEPTH_TEST);
}

void CTilesetEditor::Update(float dt)
{
	MousePosition = CEngine::Instance()->MousePos;

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
		if (Tileset == NULL)
			break;
		if (!((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
			break;
		if (MousePosition.x <= INTERFACE_OFFSET_X)
			break;

// 		int Mx = MousePosition.x / Zoom - Offset.x / Zoom;
// 		int My = MousePosition.y / Zoom - Offset.y / Zoom;
// 		int Dx0 = (Font->bbox[CurrentSymbol].x1 - Font->bbox[CurrentSymbol].x0) /2;
// 		int Dx1 = (Font->bbox[CurrentSymbol].x1 - Font->bbox[CurrentSymbol].x0) - (Font->bbox[CurrentSymbol].x1 - Font->bbox[CurrentSymbol].x0) /2;
// 		int Dy0 = (Font->bbox[CurrentSymbol].y1 - Font->bbox[CurrentSymbol].y0) /2;
// 		int Dy1 = (Font->bbox[CurrentSymbol].y1 - Font->bbox[CurrentSymbol].y0) - (Font->bbox[CurrentSymbol].y1 - Font->bbox[CurrentSymbol].y0) /2;
// 

		break;
	}
}

CTilesetEditor::~CTilesetEditor()
{

}

bool LoadTileset(CObject *Caller)
{
	CTilesetEditor *TilesetEditor = dynamic_cast<CTilesetEditor *>(Caller);
	if (TilesetEditor == NULL)
		return false;
	TilesetEditor->Tileset = CTileSetManager::Instance()->GetTileset(TilesetEditor->edTilesetName->GetText());
	if (TilesetEditor->Tileset == NULL)
	{
		Log("Error", "Tileset %s not found within data/fonts", TilesetEditor->edTilesetName->GetText().c_str());
		return false;
	}
	TilesetEditor->TilesetTexture = TilesetEditor->Tileset->GetTexture();
	TilesetEditor->edHorNumTiles->SetText(itos(TilesetEditor->Tileset->HorNumTiles));
	TilesetEditor->edVerNumTiles->SetText(itos(TilesetEditor->Tileset->VerNumTiles));
	TilesetEditor->edTileWidth->SetText(itos(TilesetEditor->Tileset->TileWidth));
	TilesetEditor->edTileHeight->SetText(itos(TilesetEditor->Tileset->TileHeight));
	return true;
}

bool SaveTileset(CObject *Caller)
{
	CTilesetEditor *TilesetEditor = dynamic_cast<CTilesetEditor *>(Caller);
	if (TilesetEditor == NULL)
		return false;
	if (TilesetEditor->Tileset == NULL)
		TilesetEditor->Tileset = CFactory::Instance()->New<CTileset>(TilesetEditor->edTilesetName->GetText());
	TilesetEditor->Tileset->SetName(TilesetEditor->edTilesetName->GetText());
	TilesetEditor->Tileset->SetTexture(TilesetEditor->TilesetTexture->GetName());
	TilesetEditor->Tileset->SetSettings(stoi(TilesetEditor->edTileWidth->GetText()), stoi(TilesetEditor->edTileHeight->GetText()),
	stoi(TilesetEditor->edHorNumTiles->GetText()), stoi(TilesetEditor->edVerNumTiles->GetText()));


	TilesetEditor->Tileset->SaveToFile(TilesetEditor->Tileset->GetFilename());
	return true;
}

bool LoadTexture(CObject *Caller) /* Опять же не Load() а Acquire(). */
{
	CTilesetEditor *TilesetEditor = dynamic_cast<CTilesetEditor *>(Caller);
	if (TilesetEditor == NULL)
		return false;
	CTexture *TempFontTexture = CTextureManager::Instance()->Get(TilesetEditor->edTilesetTextureName->GetText());
	if (TempFontTexture == NULL)
	{
		Log("ERROR", "Font texture %s not found", TilesetEditor->edTilesetTextureName->GetText().c_str());
		// Создать объект текст с таймером здесь, выдавать ошибки с его помощью прямо на экран.
		return false;
	}
	TilesetEditor->TilesetTexture = TempFontTexture;
	return true;
}

bool ExitEditor(CObject *Caller)
{
	SDL_Event Event;
	Event.type = SDL_QUIT;
	SDL_PushEvent(&Event); // Не так.
	return true;
}
