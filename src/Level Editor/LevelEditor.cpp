#include "Ninja.h"
#include "GameUtils.h"

CNinja* Ninja = CNinja::Instance();

#define MIN_ZOOM 4
#define MAX_ZOOM 128

int	GameVTiles =  20; 
int	GameHTiles = 15;
int Zoom = 16;
int ZoomDt = 2;


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

Vector2 TileSelPos = V2Zero;

#define ST_SELECT_TILE		0x00
#define ST_DRAW_TILES		0x01
#define ST_OVERLEFTPANEL	0x02
#define ST_ENTITY_SET		0x03

int State = ST_DRAW_TILES; 


void SetZoom(int _Zoom)
{
	Zoom = clampf(_Zoom, MIN_ZOOM, MAX_ZOOM);
}

int CalcF1(scalar p, scalar size)
{
	return 1;
}


bool ProcessInput(SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_KEYDOWN:
			{

				if (event.key.keysym.sym == SDLK_LSHIFT && MousePos.x > LeftPanel)
				{
					State = ST_SELECT_TILE;
					TileSelPos = Vector2(MousePos.x, MousePos.y);
				}
				if (event.key.keysym.sym == SDLK_LCTRL)
				{
					ZoomDt = 8;
				}
			}
			break;
		case SDL_KEYUP:
			{

				if (event.key.keysym.sym == SDLK_LSHIFT && MousePos.x > LeftPanel)
				{
					if (State == ST_SELECT_TILE)
						State = ST_DRAW_TILES;
					
				}

				if (event.key.keysym.sym == SDLK_LCTRL)
				{
					ZoomDt = 2;
				}


			}
			break;
		case SDL_MOUSEMOTION:
			{
				if (event.motion.x < LeftPanel)
					State = ST_OVERLEFTPANEL;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			{
				if (event.button.button == SDL_BUTTON_WHEELUP)
					SetZoom(Zoom+ZoomDt);
				if (event.button.button == SDL_BUTTON_WHEELDOWN)
					SetZoom(Zoom-ZoomDt);
			}
			break;
	}
	return true;
}

bool Init()
{	
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;
	// Загружаем из конфига всякие разные параметры для редактора
	SetZoom(Ninja->CfgGetInt("DefaultCellSize"));
	GameVTiles	= Ninja->CfgGetInt("GameVTiles");
	GameHTiles	= Ninja->CfgGetInt("GameHTiles");
	LeftPanel	= Ninja->CfgGetInt("LeftPanel");
	BottomPanel	= Ninja->CfgGetInt("BottomPanel");
	Ninja->GetState(STATE_SCREEN_WIDTH, &ScrnWidth);
	Ninja->GetState(STATE_SCREEN_HEIGHT, &ScrnHeight);
	SDL_ShowCursor(0);
	//Ninja->RenderManager.SortByAlpha();
	//Ninja->RenderManager.SortByZ();	
	TileSet = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("Tilesets", "TileSet02-Snow01", CTileSet::NewTileSet));
	gSetBlendingMode();
	Ninja->FontManager->SetCurrentFont("Font");
	CFontManager* fm = CFontManager::Instance("Main");
	f = fm->CurrentFont;
	fm->FreeInst("main");
	Ninja->AddEventFunction(ProcessInput);
	Level.numCellsHor = 20;
	Level.numCellsVer = 10;
	Level.TileSetName = "TileSet02-Snow01";
	Level.Cells = new CMapCellInfo [20*10];
	memset(Level.Cells, 0, 20*10*(sizeof(CMapCellInfo)));
	Level.TileSet = TileSet;
	for (int i=0;i<20*10;i++)
	{
		Level.Cells[i].index = Random_Int(0, 15);
		Level.Cells[i].z = -0.0f;
		Level.Cells[i].interaction = 0;

	}
	Level.GenCells();
	Level.visible = true;
	Level.depth = -0.6f;

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
	CPrimitiveRender p;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_NOONE;
	p.sClr = RGBAf(0.3f, 0.4f, 0.5, 0.7f);
	p.grRectL(V2Zero+Vector2(1,1), Vector2(LeftPanel, ScrnHeight-2));
	p.grRectS(V2Zero+Vector2(1,1), Vector2(LeftPanel, ScrnHeight-2));
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

	if (State == ST_SELECT_TILE)
	{
		glLoadIdentity();
		glTranslatef(TileSelPos.x, TileSelPos.y, 0.0f);
		
		Level.Render();
		TileSet->RenderTileSet();
		Vector2 vt0 = Vector2(((int)(MousePos.x - TileSelPos.x)/(int) TileSet->Info.TileWidth) * TileSet->Info.TileWidth,
						((int)(MousePos.y - TileSelPos.y)/(int) TileSet->Info.TileHeight) * TileSet->Info.TileHeight) + TileSelPos;
		Vector2 vt1 = vt0 + Vector2(TileSet->Info.TileWidth, TileSet->Info.TileHeight);
		p.grRectC(vt0, vt1);
	}


	Vector2 mOffset;
	mOffset.x = (int)Offset.x % (int)Zoom;
	mOffset.y = (int)Offset.y % (int)Zoom;

	if (State != ST_SELECT_TILE)
	{
		p.grRectC(Vector2(vx*Zoom, vy*Zoom)+mOffset,
			Vector2((vx+1)*Zoom, (vy+1)*Zoom)+mOffset);
	}

	p.grCircleL(MousePos, 5.0f);

	glLoadIdentity();
	f->p = Vector2((vx+1)*Zoom, vy*Zoom);
	f->Print(" x:%d y:%d", (int)(vx - CellOffset.x), (int)(vy - CellOffset.y));


	DrawPanels();

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
