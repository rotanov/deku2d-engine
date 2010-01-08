#include "2de_Engine.h"
#include "2de_GameUtils.h"

CEngine* Ninja = CEngine::Instance();

#define MIN_ZOOM 4
#define MAX_ZOOM 128

int	GameVTiles = 20; 
int	GameHTiles = 15;
int Zoom = 16;
int ZoomDt = 2;


int LeftPanel = 196;
int BottomPanel = 196;

CTileset *TileSet = NULL;
CLevelMap Level;
CFont* f;

int ScrnWidth;
int ScrnHeight;

int CurrTileIndex = 0;
Vector2 MousePos;
Vector2 oMp = V2_ZERO;
Vector2 Offset = V2_ZERO;
Vector2 CellOffset;

Vector2 TileSelPos = V2_ZERO;

#define ST_SELECT_TILE		0x00
#define ST_DRAW_TILES		0x01
#define ST_OVERLEFTPANEL	0x02
#define ST_ENTITY_SET		0x03

int State = ST_DRAW_TILES; 

struct CCellNode
{
	CMapCellInfo info;
	CCellNode *next;
	CCellNode()
	{
		next = NULL;
	}
};

CCellNode root;

void AddTile(int TileIndex, int x, int y)
{
	CCellNode* next = &root;
	while (next->next)
		next = next->next;
	next->next = new CCellNode;
	next = next->next;
	next->info.z = 0.0f;
	next->info.tc = TileSet->GetCellTC(TileIndex);
	Vector2 ji = Vector2(x, y);
	scalar w = TileSet->Info.TileWidth;
	scalar h = TileSet->Info.TileHeight;
	for(int k = 0; k < 4; k++)
	{
		next->info.pos[k] = (ji + V2_QuadBin[k]);
		next->info.pos[k].x *= w;
		next->info.pos[k].y *= h;
	}
}

void RenderTileList()
{
	CCellNode *next = root.next;
	glLoadIdentity();
	glTranslatef(Offset.x, Offset.y, 0.0f);
	glScalef((float)Zoom / (float)TileSet->Info.TileWidth , (float)Zoom / (float)TileSet->Info.TileHeight , 0.0f);
	glPushAttrib(GL_TEXTURE_BIT | GL_BLEND);
	glEnable(GL_TEXTURE);
	//glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	TileSet->Texture->Bind();
	glBegin(GL_QUADS);
	CMapCellInfo *t;
	while (next)
	{
		t = &next->info;

		glTexCoord2f(t->tc[0].x, t->tc[0].y);
		glVertex3f(t->pos[0].x,	t->pos[0].y, t->z);

		glTexCoord2f(t->tc[1].x, t->tc[1].y); 
		glVertex3f(t->pos[1].x,	t->pos[1].y, t->z);

		glTexCoord2f(t->tc[2].x, t->tc[2].y); 
		glVertex3f(t->pos[2].x,	t->pos[2].y, t->z);

		glTexCoord2f(t->tc[3].x, t->tc[3].y); 
		glVertex3f(t->pos[3].x,	t->pos[3].y, t->z);
		next = next->next;
	}
	glEnd();
	glPopAttrib();

}


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
				else
					if (State == ST_OVERLEFTPANEL)
						State = ST_DRAW_TILES;
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
		case SDL_MOUSEBUTTONDOWN:
			{
				if (event.button.button == SDL_BUTTON_LEFT && State == ST_DRAW_TILES)
				{
					AddTile(CurrTileIndex, (MousePos.x-Offset.x) / Zoom, (MousePos.y-Offset.y) / Zoom);
				}
				if (event.button.button == SDL_BUTTON_LEFT && State == ST_SELECT_TILE)
				{
					CurrTileIndex = ((int)(MousePos.y - TileSelPos.y)/(int)TileSet->Info.TileHeight)*TileSet->Info.HorNumTiles + (int)(MousePos.x - TileSelPos.x)/(int)TileSet->Info.TileWidth; 
				}
			}
			break;
	}
	return true;
}

bool Init()
{	
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
	TileSet = dynamic_cast<CTileset*>(Ninja->ResourceManager.LoadResource("Tilesets", "TileSet02-Snow01", CTileset::NewTileset));
	gSetBlendingMode();
	Ninja->FontManager->SetCurrentFont("Font");
	CFontManager* fm = CFontManager::Instance();
	f = fm->CurrentFont;
	Ninja->AddEventFunction(ProcessInput);
	Level.numCellsHor = 20;
	Level.numCellsVer = 10;
	//Level.TileSet->name = "TileSet02-Snow01";
	Level.Cells = new CMapCellInfo [20*10];
	memset(Level.Cells, 0, 20*10*(sizeof(CMapCellInfo)));
	Level.TileSet = TileSet;
	for (int i=0;i<20*10;i++)
	{
		Level.Cells[i].index = Random_Int(2, 2);
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
	p.doUseGlobalCoordSystem = false;
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
	p.BlendingOption = PRM_RNDR_OPT_BLEND_ONE;
	p.sClr = RGBAf(0.3f, 0.4f, 0.5, 0.7f);
	p.grRectL(V2_ZERO+Vector2(1,1), Vector2(LeftPanel, ScrnHeight-2));
	p.grRectS(V2_ZERO+Vector2(1,1), Vector2(LeftPanel, ScrnHeight-2));
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
	RenderTileList();


	gToggleScissor(false);


	
	
	CPrimitiveRender p;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_ONE;
	p.doUseGlobalCoordSystem = true;
	p.lClr = RGBAf(0.1f, 0.7f, 0.3f, 0.7f);
	p.pClr = RGBAf(0.1f, 0.7f, 0.3f, 0.7f);
	p.sClr = RGBAf(0.0f, 0.0f, 0.0f, 0.0f);
	int vx = ((int)MousePos.x/(int)Zoom), vy = ((int)MousePos.y/(int)Zoom);

	if (State == ST_SELECT_TILE)
	{
		glLoadIdentity();
		glTranslatef(TileSelPos.x, TileSelPos.y, 0.0f);
		
		//Level.Render();
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
	f->Pos = Vector2((vx+1)*Zoom, vy*Zoom);
	f->Print(" x:%d y:%d", (int)(vx - CellOffset.x), (int)(vy - CellOffset.y));


	DrawPanels();

	oMp = MousePos;

	return true;
}
int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(STATE_CONFIG_NAME, "LevelEditor.xml");
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);

	Ninja->Run();

	return 0x0;
}
