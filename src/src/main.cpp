#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

CParticleSystem *ps;
CSprite *Spr;
CHero *Hero;
Vector2 pnts[2];
CEnemyController *enemyc;
CGUIScheme *guidummy = NULL;
CEdit *Edit = NULL;

void tempMakeTileSet()
{
	CTileSet ts;
	ts.Info.HorNumTiles = 16;
	ts.Info.VerNumTiles = 4;
	ts.Info.TileHeight = 16;
	ts.Info.TileWidth = 16;

	ts.TextureName = "DocT";
	ts.filename = "data\\Tilesets\\TileSet01.tls";
	ts.SaveToFile();
}

void tempMakeMap()
{
	CLevelMap m;
	m.filename = "Data\\Levels\\Level01.lvl";
	m.TileSetName = "Dokutsu";
	m.numCellsHor = 4;
	m.numCellsVer = 3;

	int ind[12] = {12,13,44,47,
				   48,48,60,63,
				   52,48,48,48};
	m.Cells = new  CMapCellInfo [12];
	memset(m.Cells, 0, 12*(sizeof(CMapCellInfo)));
	for (int i=0;i<12;i++)
	{
		m.Cells[i].index = ind[i];
		m.Cells[i].z = 0.5f;
		m.Cells[i].interaction = 0;
	}
	m.SaveToFile();
}

bool Init()
{	
	CFactory *Factory = CFactory::Instance();
	guidummy = new CGUIScheme("Data\\main.gui", "Data\\maingui.bmp");
	Factory->Create(OBJ_USER_DEFINED, &(CGUIRenderer::NewRenderer));
	Edit = dynamic_cast<CEdit*>(newWidget("NewEdit", STYLE_OBJEDIT));
	guidummy->CopyWidget(2, Edit);
	FormAddWidget(Edit, NULL);
	Edit->Enabled = true;
	Edit->Left = 10;
	Edit->Top = 10;
	Edit->Height = 100;
	Edit->Width = 100;
	Edit->Visible = true;
	Edit->Caption = "Fuckkk!!";
	Edit->SelStart = 1;
	Edit->SelLength = 3;
	gSetBlendingMode();
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;
	
	

	Hero = dynamic_cast<CHero*>(Factory->Create(OBJ_USER_DEFINED, &(CHero::NewHero)));
	Hero->x = 200;
	Hero->y = 200;//GROUND;
	Hero->z = 0.0f;

	Hero->sprite.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("hero")))->GetTexID();
	Hero->sprite.m_nTextureHeight = 32;
	Hero->sprite.m_nTextureWidth = 32;
	Hero->sprite.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprite.SetAnimation(1);

	Hero->spra.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("heroa")))->GetTexID();
	Hero->spra.m_nTextureHeight = 32;
	Hero->spra.m_nTextureWidth = 32;
	Hero->spra.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->spra.SetAnimation(1);
	Hero->spra.z = 0.0f;
	Hero->sprb.z = 0.0f;
	Hero->z = -0.5f;

	Hero->sprb.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("herob")))->GetTexID();
	Hero->sprb.m_nTextureHeight = 32;
	Hero->sprb.m_nTextureWidth = 32;
	Hero->sprb.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprb.SetAnimation(1);

	ps = dynamic_cast<CParticleSystem*>(Factory->Create(OBJ_PSYSTEM, NULL));
	ps->name = "psys";
	ps->Init();
	ps->info.sc = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	ps->info.ec = RGBAf(1.0f, 1.0f, 1.0f, 0.0f);
	ps->visible = true;	
	pnts[0] = Vector2(0, 480);
	pnts[1] = Vector2(640, 480);
	ps->info.plife = 3;
	ps->info.emission = 500;///*debug 1 */ 3000;
	ps->info.startsize = 8;
	// debug //
	ps->info.sizevar = 8;
	ps->info.isSnow = true; // костыль
	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("sball")))->GetTexID();
	ps->SetGeometry(pnts, 2);

	int i;
	repeat(i, 201)
	{
		ps->Update(0.02f);
	}

	ps = dynamic_cast<CParticleSystem*>(Factory->Create(OBJ_PSYSTEM, NULL));
	ps->name = "enemy_l";
	ps->Init();

	ps->info.sc = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	ps->info.ec = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	ps->visible = true;	
	ps->info.plife = 3000;
	ps->info.MaxParticles = 10;
	ps->info.emission = 0;
	ps->info.startsize = 64;

	enemyc = dynamic_cast<CEnemyController*>(Factory->Create(OBJ_USER_DEFINED, &(CEnemyController::NewController)));
	enemyc->AssignPS(ps);

	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("enemy")))->GetTexID();

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("gift")))->GetTexID();
	Spr->m_nTextureHeight = 64;
	Spr->m_nTextureWidth = 64;
	Spr->AddAnimation(true, 1, 104, 98, 1, 1, 1, 52, 49, 2, 15, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "GftSpr";
	Spr->x = 260; 
	Spr->y = GROUND; 
	Spr->z = 0.9f; 
	Spr->visible = true;

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("bg")))->GetTexID();
	Spr->m_nTextureHeight = 256;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 512, 512, 1, 1, 1, 256, 256, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "BgrSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = -0.9f; 
	Spr->visible = true;

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("gr")))->GetTexID();
	Spr->m_nTextureHeight = 16;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 512, 32, 1, 1, 1, 256, 16, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "grSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = 0.3f; 
	Spr->visible = true;



	
	Ninja->RenderManager.Camera.Assign(&Hero->x, &Hero->y);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LINE_WIDTH);

	tempMakeTileSet();
	tempMakeMap();

	CTileSet *ts;
	ts = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("tileset", "Dokutsu", CTileSet::NewTileSet));
	
	CLevelMap *lm;
	lm = dynamic_cast<CLevelMap*>(Ninja->ResourceManager.LoadResource("levels", "Level01", CLevelMap::NewLevelMap));
	lm->visible = true;
	
	Ninja->RenderManager.SortByZ();
	Ninja->RenderManager.SortByAlpha();

	Ninja->FontManager->SetCurrentFont("FFont");
	Factory->FreeInst();


	return true;
}

bool Draw()
{	
	glLoadIdentity();
	Ninja->FontManager->PrintEx(560, 460, 1, "Fps: %d", Ninja->GetFps());
	return true;
}



int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);
	Ninja->SetState(STATE_USER_INIT, &Init);
	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x0;
}
