#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

CParticleSystem *ps;
CSprite *Spr;
CHero *Hero;
Vector2 pnts[2];
CPolygon a, b;
CEnemyController *enemyc;
CGUIScheme *guidummy = NULL;
CEdit *Edit = NULL, *Edit1 = NULL;
CButton *Button = NULL;
CCompas compas;

void tempMakeTileSet()
{
	CTileSet ts;
	ts.Info.HorNumTiles = 8;
	ts.Info.VerNumTiles = 2;
	ts.Info.TileHeight = 32;
	ts.Info.TileWidth = 32;

	ts.TextureName = "SnowyTileSet";
	ts.filename = "data\\Tilesets\\TileSet02-Snow01.tls";
	ts.SaveToFile();
}

void tempMakeMap()
{
	CLevelMap m;
	m.filename = "Data\\Levels\\Level02.lvl";
	m.TileSetName = "TileSet02-Snow01";
	m.numCellsHor = 4;
	m.numCellsVer = 4;

	int ind[16] = {5,14,5,15,
				   4,4,4,4,
				   4,4,4,4,
				   4,4,12,4};
	m.Cells = new  CMapCellInfo [16];
	memset(m.Cells, 0, 16*(sizeof(CMapCellInfo)));
	for (int i=0;i<16;i++)
	{
		m.Cells[i].index = ind[i];
		m.Cells[i].z = -0.0f;
		m.Cells[i].interaction = 0;
	}
	m.SaveToFile();
}

bool Init()
{	
	CFactory *Factory = CFactory::Instance();
 	guidummy = new CGUIScheme("Data\\main.gui", "Data\\maingui.bmp");
 	Factory->Create(OBJ_USER_DEFINED, &(CGUIRenderer::NewRenderer));
 		//Edit creation sample
 		Edit = dynamic_cast<CEdit*>(newWidget("NewEdit", STYLE_OBJEDIT));
 		//создание едита с именем нью едит
 		guidummy->CopyWidget(2, Edit);
 		//заполнение его свойств из уже загруженных эл-тов(по индексу)
 		FormAddWidget(Edit, NULL);
 		//добавление едита на форму, если нулл то добавляется на форму окна
 		//--- Всякий стафф
 		Edit->Enabled = true;
 		Edit->Visible = true;
 		Edit->Left = 10;
 		Edit->Top = 345;
 		Edit->Height = 30;
 		Edit->Width = 400;
 		Edit->Caption = "Fuck";
 		Edit->SelStart = 1;
 		Edit->SelLength = 0;

 		//Edit creation sample
 		Edit1 = dynamic_cast<CEdit*>(newWidget("NewEdit1", STYLE_OBJEDIT));
 		//создание едита с именем нью едит
 		guidummy->CopyWidget(2, Edit1);
 		//заполнение его свойств из уже загруженных эл-тов(по индексу)
 		FormAddWidget(Edit1, NULL);
 		//добавление едита на форму, если нулл то добавляется на форму окна
 		//--- Всякий стафф
 		Edit1->Enabled = true;
 		Edit1->Visible = true;
 		Edit1->Left = 10;
 		Edit1->Top = 380;
 		Edit1->Height = 30;
 		Edit1->Width = 400;
 		Edit1->Caption = "Fuckkk!!rehtyjktyfjnkbrehofhhrohviurebhire";
 		Edit1->SelStart = 0;
 		Edit1->SelLength = 0;

 		//Button creation sample
 		Button = dynamic_cast<CButton*>(newWidget("NewButton", STYLE_OBJBUTTON));
 		//создание буттона с именем нью буттон
 		guidummy->CopyWidget(1, Button);
 		//заполнение его свойств из уже загруженных эл-тов(по индексу)
 		FormAddWidget(Button, NULL);
 		//добавление буттона на форму, если нулл то добавляется на форму окна
 		//--- Всякий стафф
 		Button->Enabled = true;
 		Button->Visible = true;
 		Button->Left = 415;
 		Button->Top = 345;
 		Button->Height = 65;
 		Button->Width = 200;
 		Button->Caption = "<--Grats-->";

		Edit->SetFont("Font");
		Edit1->SetFont("Font");
		Button->SetFont("Font");
	gSetBlendingMode();
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;
	
	

	Hero = dynamic_cast<CHero*>(Factory->Create(OBJ_USER_DEFINED, &(CHero::NewHero)));
	Hero->p.In(200.0f, 200.0f);
	

	Hero->sprite.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Hero_r1")))->GetTexID();
	Hero->sprite.m_nTextureHeight = 32;
	Hero->sprite.m_nTextureWidth = 32;
	Hero->sprite.AddAnimation(true, 1, 28, 24, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprite.SetAnimation(1);

	Hero->spra.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Hero_a1")))->GetTexID();
	Hero->spra.m_nTextureHeight = 32;
	Hero->spra.m_nTextureWidth = 32;
	Hero->spra.AddAnimation(true, 1, 28, 24, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->spra.SetAnimation(1);
	Hero->spra.depth = -0.5f;
	Hero->sprb.depth = -0.5f;
	Hero->depth = -0.5f;

	Hero->sprb.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Hero_b1")))->GetTexID();
	Hero->sprb.m_nTextureHeight = 32;
	Hero->sprb.m_nTextureWidth = 32;
	Hero->sprb.AddAnimation(true, 1, 28, 24, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprb.SetAnimation(1);

	ps = dynamic_cast<CParticleSystem*>(Factory->Create(OBJ_PSYSTEM, NULL));
	ps->name = "psys";
	ps->Init();
	ps->info.sc = RGBAf(1.0f, 0.5f, 0.5f, 1.0f);
	ps->info.ec = RGBAf(0.5f, 0.5f, 1.0f, 0.5f);
	ps->visible = true;	
	pnts[0] = Vector2(0, 480);
	pnts[1] = Vector2(640, 480);
	ps->info.plife = 3;
	ps->info.emission = 500;///*debug 1 */ 3000;
	ps->info.startsize = 32;
	// debug //
	ps->info.sizevar = 8;
	ps->info.isSnow = true; // костыль
	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Particle")))->GetTexID();
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

	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Enemy_r")))->GetTexID();

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Gift_r")))->GetTexID();
	Spr->m_nTextureHeight = 64;
	Spr->m_nTextureWidth = 64;
	Spr->AddAnimation(true, 1, 52, 49, 1, 1, 1, 52, 49, 2, 15, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "GftSpr";
	Spr->p.In(260.0f, GROUND);
	Spr->depth = -0.01f; 
	Spr->visible = true;

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("bg_b")))->GetTexID();
	Spr->m_nTextureHeight = 256;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 256, 256, 1, 1, 1, 256, 256, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "BgrSpr";
	Spr->p = V2Zero;
	Spr->depth = -0.9f; 
	Spr->visible = true;

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Ground")))->GetTexID();
	Spr->m_nTextureHeight = 16;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 256, 16, 1, 1, 1, 256, 16, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "grSpr";
	Spr->p = V2Zero;
	Spr->depth = -0.3f; 
	Spr->visible = true;


	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("bat2_scetch_col2")))->GetTexID();
	Spr->m_nTextureHeight = 64;
	Spr->m_nTextureWidth = 128;
	Spr->AddAnimation(true, 100, 32, 32, 3, 2, 6, 32, 32, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "BatSpr";
	Spr->p = V2Zero;
	Spr->depth = -0.5f; 
	Spr->visible = true;
	
	//Ninja->RenderManager.Camera.Assign(&Hero->x, &Hero->y);
	Ninja->RenderManager.Camera.Assign(&Spr->p.x, &Spr->p.y);

	glEnable(GL_DEPTH_TEST); // Dis
	glDisable(GL_LINE_WIDTH);

	//tempMakeTileSet();
	//tempMakeMap();

	CTileSet *ts;
	//ts = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("tileset", "TileSet01", CTileSet::NewTileSet));
	ts = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("Tileset", "TileSet02-Snow01", CTileSet::NewTileSet));
	
// 	CLevelMap *lm;
// 	lm = dynamic_cast<CLevelMap*>(Ninja->ResourceManager.LoadResource("Levels", "Level02", CLevelMap::NewLevelMap));
// 	lm->visible = true;
// 	lm->depth = -0.6f;
	
	Ninja->RenderManager.SortByAlpha();
	Ninja->RenderManager.SortByZ();
	
	
	

	Ninja->FontManager->SetCurrentFont("Font");
	Factory->FreeInst();
	
	a = CPolygon::CPolygon(5);
	a.V[0] = Vector2(-90, 0);
	a.V[1] = Vector2(-50, -90);
	a.V[2] = Vector2(50, -90);
	a.V[3] = Vector2(90, 0);
	a.V[4] = Vector2(0, 60);

	b = CPolygon::CPolygon(6);
	b.V[0] = Vector2(-40, 50);
	b.V[1] = Vector2(-40, -50);
	b.V[2] = Vector2(0, -80);
	b.V[3] = Vector2(40, -50);
	b.V[4] = Vector2(40, 50);
	b.V[5] = Vector2(0, 80);


	return true;
}

bool Draw()
{	
	glLoadIdentity();
	Ninja->FontManager->PrintEx(560, 460, 1, "Fps: %d", Ninja->GetFps());

	static Vector2 apos(200, 300);
	static Vector2 bpos(300, 300);
	static Vector2 norm;
	static float depth;
	
	static int angle = 0;

	if (!a.Collide(&a, apos, Vector2(0.0f, 0.0f), Matrix2(DegToRad(angle)),
				&b, bpos, Vector2(0.0f, 0.0f), Matrix2(0.0f), norm, depth))
	{
		norm = Vector2(0.0f, 0.0f);
		depth = 0.0f;
	}
	
	

	if (Ninja->keys[SDLK_SPACE])
	{
		apos -= norm*depth/2.0f;
		bpos += norm*depth/2.0f;
		depth = 0;
	}
	
	CPrimitiveRender pr;
	pr.doUseCurrentCoordSystem = false;
	pr.lClr = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pr.sClr = Vector4(0.9f, 0.7f, 0.7f, 0.5f);
	pr.BlendingOption = PRM_RNDR_OPT_BLEND_ONE+1;
	pr.LineStippleEnabled = true;

	pr.grPolyC(apos, angle, &a);
	pr.grPolyC(bpos, 0.0f, &b);
	pr.grArrowC(Vector2(100, 100), Vector2(400, 300));
	pr.grSegment(bpos, (bpos + norm*depth));
	angle += 1;
	if (angle > 360.0f) angle -= 360;

	if (Ninja->keys[SDLK_j])
		apos.x -= 5;
	if (Ninja->keys[SDLK_l])
		apos.x += 5;
	if (Ninja->keys[SDLK_i])
		apos.y += 5;
	if (Ninja->keys[SDLK_k])
		apos.y -= 5;

	CRenderObject *tmp = dynamic_cast<CRenderObject*>(Ninja->RenderManager.GetObjectByName("BatSpr"));
	if (Ninja->keys[SDLK_w])
		tmp->p.y += 10;
	if (Ninja->keys[SDLK_s])
		tmp->p.y -= 10;

	if (Ninja->keys[SDLK_d])
		tmp->p.x += 10;
	if (Ninja->keys[SDLK_a])
		tmp->p.x -= 10;

	return true;
}

CPrimitiveRender p;

bool Init2()
{
	p.Angle = 0.0f;
	p.lClr = RGBAf(0.4f, 0.5f, 0.7f, 0.9f);
	p.sClr = RGBAf(0.5f, 0.7f, 0.4f, 0.9f);
	p.pClr = RGBAf(0.7f, 0.5f, 0.4f, 0.9f);
	p.psize = 10.0f;
	p.lwidth = 4.0f;
	return true;
}
int k = 0;

bool Draw2()
{
	glLoadIdentity();
	glTranslatef(640 - 25 - 4, 480 - 25 - 4, 0.0f);
	if(k < 5)
		p.Angle += k;
	else
		p.Angle -= k;
	
	if(k > 5) k--;
	else k++;
	p.grInYan(V2Zero, 25);
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
