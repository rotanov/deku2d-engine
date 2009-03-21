#include "Ninja.h"
#include "GameUtils.h"

CNinja* Ninja = CNinja::Instance();

int	GameVTiles =  20; 
int	GameHTiles = 15;
int Zoom = 16;
int LeftPanel = 196;
int BottomPanel = 196;

CTileSet *TileSet = NULL;
CLevelMap Level;
CFont* f;

int ScrnWidth;
int ScrnHeight;

Vector2 MousePos;
Vector2 oMp = V2Zero;
Vector2 Offset = V2Zero;
Vector2 CellOffset;

#define ST_SELECT_TILE		0x00
#define ST_DRAW_TILES		0x01
#define ST_OVERLEFTPANEL	0x02
#define ST_ENTITY_SET		0x03

int State = 0x01; // Текущее состояние, т.е. что мы сейчас делаем.

bool Init()
{	
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;

	// Загружаем из конфига всякие разные параметры для редактора
	Zoom		= atoi((Ninja->Config.First->Get("DefaultCellSize"))->GetValue());
	GameVTiles	= atoi((Ninja->Config.First->Get("GameVTiles"))->GetValue());
	GameHTiles	= atoi((Ninja->Config.First->Get("GameHTiles"))->GetValue());
	LeftPanel	= atoi((Ninja->Config.First->Get("LeftPanel"))->GetValue());
	BottomPanel	= atoi((Ninja->Config.First->Get("BottomPanel"))->GetValue());

	Ninja->GetState(STATE_SCREEN_WIDTH, &ScrnWidth);
	Ninja->GetState(STATE_SCREEN_HEIGHT, &ScrnHeight);
	SDL_ShowCursor(0);

	Ninja->RenderManager.SortByAlpha();
	Ninja->RenderManager.SortByZ();
	
	TileSet = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("Tilesets", "TileSet02-Snow01", CTileSet::NewTileSet));

	gSetBlendingMode();

	Ninja->FontManager->SetCurrentFont("Font");
	CFontManager* fm = CFontManager::Instance("Main");
	f = fm->CurrentFont;
	fm->FreeInst("main");

	//Level.
		return true;
}


void CheckMouse()
{

}

void DrawGrid()
{
	int n = ScrnWidth;
	int m = ScrnHeight;
	(n /= Zoom)++;
	(m /= Zoom)++;
	glLoadIdentity();
	CPrimitiveRender p;
	glTranslatef((int)Offset.x % Zoom,(int)Offset.y % Zoom, 0.0f);
	p.doUseCurrentCoordSystem = false;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_ONE;
	p.lClr = RGBAf(1.0f, 1.0f, 1.0f, 0.6f);
	p.pClr = RGBAf(1.0f, 1.0f, 1.0f, 0.6f);
	p.lwidth = 0.2f;

	for (int i = -2; i <= n+2; i ++)
	{
		p.grSegment(Vector2(i*Zoom, -2*Zoom), Vector2(i*Zoom, (m+2)*Zoom));
	}
	for (int i = -2; i <= m+2; i ++)
	{
		p.grSegment(Vector2(-2*Zoom, i*Zoom), Vector2((n+2)*Zoom, i*Zoom));
	}

	p.lClr = RGBAf(0.4f, 0.1f, 0.8f, 0.6f);
	p.lwidth = 3.0f;
	for (int i = (int)CellOffset.x; i <= n+2; i+=GameHTiles)
	{
		p.grSegment(Vector2(i*Zoom, -2*Zoom), Vector2(i*Zoom, (m+2)*Zoom));
	}
	for (int i = (int)CellOffset.y; i <= m+2; i+=GameVTiles)
	{
		p.grSegment(Vector2(-2*Zoom, i*Zoom), Vector2((n+2)*Zoom, i*Zoom));
	}
}

void DrawCursor()
{
	
}

void DrawPanels()
{
	
}



bool Draw()
{	
	Ninja->GetState(STATE_MOUSE_XY, &MousePos);
	if (Ninja->keys[SDLK_g])
	{
		Offset += MousePos - oMp;
	}

	CellOffset.x = (int)Offset.x / (int)Zoom;
	CellOffset.y = (int)Offset.y / (int)Zoom;

 	gToggleScissor(true);
 	gScissor(LeftPanel, 0.0f, ScrnWidth-LeftPanel, ScrnHeight);
	DrawGrid();
	gToggleScissor(false);


	
	CPrimitiveRender p;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_ONE;
	p.doUseCurrentCoordSystem = true;
	p.lClr = RGBAf(0.1f, 0.7f, 0.3f, 0.7f);
	p.pClr = RGBAf(0.1f, 0.7f, 0.3f, 0.7f);
	p.sClr = RGBAf(0.0f, 0.0f, 0.0f, 0.0f);
	int vx = ((int)MousePos.x/(int)Zoom), vy = ((int)MousePos.y/(int)Zoom);

	if (Ninja->keys[SDLK_LSHIFT])
	{
		glLoadIdentity();
		glTranslatef(vx*Zoom, vy*Zoom, 0.0f);
		TileSet->RenderTileSet();
	}
	
	Vector2 mOffset;
	mOffset.x = (int)Offset.x % (int)Zoom;
	mOffset.y = (int)Offset.y % (int)Zoom;
	p.grRectC(Vector2(vx*Zoom, vy*Zoom)+mOffset,
			  Vector2((vx+1)*Zoom, (vy+1)*Zoom)+mOffset);



	glLoadIdentity();
	f->p = Vector2((vx+1)*Zoom, vy*Zoom);
	f->Print(" x:%d y:%d", (int)(vx - CellOffset.x), (int)(vy - CellOffset.y));


	oMp = MousePos;

	return true;
}
int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "LevelEditor.xml");
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);
	Ninja->SetState(STATE_USER_INIT, &Init);

	Ninja->Run();
	Ninja->FreeInst();

	return 0x0;
}
