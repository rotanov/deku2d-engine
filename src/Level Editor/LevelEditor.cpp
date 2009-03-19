#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

float Zoom = 16.0f;
#define GAME_HTILES 20 
#define GAME_VTILES 15
CTileSet *TileSet = NULL;
Vector2 Mp;
CFont* f;
Vector2 oMp = V2Zero, Offset = V2Zero;
Vector2 CellOffset;

void DrawGrid()
{
	int n, m;
	Ninja->GetState(STATE_SCREEN_WIDTH, &n);
	Ninja->GetState(STATE_SCREEN_HEIGHT, &m);
	(n /= Zoom)++;
	(m /= Zoom)++;
	glLoadIdentity();
	CPrimitiveRender p;
	glTranslatef((int)Offset.x % (int)Zoom,(int)Offset.y % (int)Zoom, 0.0f);
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
	for (int i = (int)CellOffset.x; i <= n+2; i+=GAME_HTILES)
	{
		p.grSegment(Vector2(i*Zoom, -2*Zoom), Vector2(i*Zoom, (m+2)*Zoom));
	}
	for (int i = (int)CellOffset.y; i <= m+2; i+=GAME_VTILES)
	{
		p.grSegment(Vector2(-2*Zoom, i*Zoom), Vector2((n+2)*Zoom, i*Zoom));
	}


}

bool Init()
{	
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;

	Zoom = atoi((Ninja->Config.First->Get("DefaultCellSize"))->GetValue());
	
	SDL_ShowCursor(0);
	Ninja->RenderManager.SortByAlpha();
	Ninja->RenderManager.SortByZ();
	TileSet = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("Tilesets", "TileSet02-Snow01", CTileSet::NewTileSet));

	gSetBlendingMode();

	Ninja->FontManager->SetCurrentFont("Font");
	CFontManager* fm = CFontManager::Instance("Main");
	f = fm->CurrentFont;
	fm->FreeInst("main");
	return true;
}

bool Draw()
{	
// 	gToggleScissor(true);
// 	gScissor(256.0f, 0.0f, 1000-128, 768);
	DrawGrid();
/*	gToggleScissor(false);*/

	Ninja->GetState(STATE_MOUSE_XY, &Mp);
	CPrimitiveRender p;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_ONE;
	p.doUseCurrentCoordSystem = true;
	p.lClr = RGBAf(0.1f, 0.7f, 0.3f, 0.7f);
	p.pClr = RGBAf(0.1f, 0.7f, 0.3f, 0.7f);
	p.sClr = RGBAf(0.0f, 0.0f, 0.0f, 0.0f);
	int vx = ((int)Mp.x/(int)Zoom), vy = ((int)Mp.y/(int)Zoom);

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

	if (Ninja->keys[SDLK_g])
	{
		Offset += Mp - oMp;
	}
	CellOffset.x = (int)Offset.x / (int)Zoom;
	CellOffset.y = (int)Offset.y / (int)Zoom;

	oMp = Mp;

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
