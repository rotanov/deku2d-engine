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
// 	guidummy = new CGUIScheme("Data\\main.gui", "Data\\maingui.bmp");
// 	Factory->Create(OBJ_USER_DEFINED, &(CGUIRenderer::NewRenderer));
// 		//Edit creation sample
// 		Edit = dynamic_cast<CEdit*>(newWidget("NewEdit", STYLE_OBJEDIT));
// 		//создание едита с именем нью едит
// 		guidummy->CopyWidget(2, Edit);
// 		//заполнение его свойств из уже загруженных эл-тов(по индексу)
// 		FormAddWidget(Edit, NULL);
// 		//добавление едита на форму, если нулл то добавляется на форму окна
// 		//--- Всякий стафф
// 		Edit->Enabled = true;
// 		Edit->Visible = true;
// 		Edit->Left = 10;
// 		Edit->Top = 345;
// 		Edit->Height = 30;
// 		Edit->Width = 400;
// 		Edit->Caption = "Fuck";
// 		Edit->SelStart = 1;
// 		Edit->SelLength = -3;

// 		//Edit creation sample
// 		Edit1 = dynamic_cast<CEdit*>(newWidget("NewEdit1", STYLE_OBJEDIT));
// 		//создание едита с именем нью едит
// 		guidummy->CopyWidget(2, Edit1);
// 		//заполнение его свойств из уже загруженных эл-тов(по индексу)
// 		FormAddWidget(Edit1, NULL);
// 		//добавление едита на форму, если нулл то добавляется на форму окна
// 		//--- Всякий стафф
// 		Edit1->Enabled = true;
// 		Edit1->Visible = true;
// 		Edit1->Left = 10;
// 		Edit1->Top = 380;
// 		Edit1->Height = 30;
// 		Edit1->Width = 400;
// 		Edit1->Caption = "Fuckkk!!rehtyjktyfjnkbrehofhhrohviurebhire";
// 		Edit1->SelStart = 0;
// 		Edit1->SelLength = 0;

// 		//Button creation sample
// 		Button = dynamic_cast<CButton*>(newWidget("NewButton", STYLE_OBJBUTTON));
// 		//создание буттона с именем нью буттон
// 		guidummy->CopyWidget(1, Button);
// 		//заполнение его свойств из уже загруженных эл-тов(по индексу)
// 		FormAddWidget(Button, NULL);
// 		//добавление буттона на форму, если нулл то добавляется на форму окна
// 		//--- Всякий стафф
// 		Button->Enabled = true;
// 		Button->Visible = true;
// 		Button->Left = 415;
// 		Button->Top = 345;
// 		Button->Height = 65;
// 		Button->Width = 200;
// 		Button->Caption = "<--Grats-->";
	gSetBlendingMode();
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;
	
	

	Hero = dynamic_cast<CHero*>(Factory->Create(OBJ_USER_DEFINED, &(CHero::NewHero)));
	Hero->x = 200;
	Hero->y = 200;//GROUND;
	

	Hero->sprite.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Hero_r1")))->GetTexID();
	Hero->sprite.m_nTextureHeight = 32;
	Hero->sprite.m_nTextureWidth = 32;
	Hero->sprite.AddAnimation(true, 1, 28, 24, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprite.SetAnimation(1);

	Hero->spra.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Hero_a1")))->GetTexID();
	Hero->spra.m_nTextureHeight = 32;
	Hero->spra.m_nTextureWidth = 32;
	Hero->spra.AddAnimation(true, 1, 28, 24, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->spra.SetAnimation(1);
	Hero->spra.z = -0.5f;
	Hero->sprb.z = -0.5f;
	Hero->z = -0.5f;

	Hero->sprb.m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Hero_b1")))->GetTexID();
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
	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Particle")))->GetTexID();
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

	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Enemy_r")))->GetTexID();

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Gift_r")))->GetTexID();
	Spr->m_nTextureHeight = 64;
	Spr->m_nTextureWidth = 64;
	Spr->AddAnimation(true, 1, 52, 49, 1, 1, 1, 52, 49, 2, 15, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "GftSpr";
	Spr->x = 260; 
	Spr->y = GROUND; 
	Spr->z = -0.01f; 
	Spr->visible = true;

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("bg_b")))->GetTexID();
	Spr->m_nTextureHeight = 256;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 256, 256, 1, 1, 1, 256, 256, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "BgrSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = -0.9f; 
	Spr->visible = true;

	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("Ground")))->GetTexID();
	Spr->m_nTextureHeight = 16;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 256, 16, 1, 1, 1, 256, 16, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "grSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = -0.3f; 
	Spr->visible = true;


	Spr = dynamic_cast<CSprite*>(Factory->Create(OBJ_SPRITE, NULL));
	Spr->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("bat2_scetch_col2")))->GetTexID();
	Spr->m_nTextureHeight = 64;
	Spr->m_nTextureWidth = 128;
	Spr->AddAnimation(true, 100, 32, 32, 3, 2, 6, 32, 32, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "BatSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = -0.5f; 
	Spr->visible = true;
	
	//Ninja->RenderManager.Camera.Assign(&Hero->x, &Hero->y);
	Ninja->RenderManager.Camera.Assign(&Spr->x, &Spr->y);

	glEnable(GL_DEPTH_TEST); // Dis
	glDisable(GL_LINE_WIDTH);

	tempMakeTileSet();
	tempMakeMap();

	CTileSet *ts;
	//ts = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("tileset", "TileSet01", CTileSet::NewTileSet));
	ts = dynamic_cast<CTileSet*>(Ninja->ResourceManager.LoadResource("Tileset", "TileSet02-Snow01", CTileSet::NewTileSet));
	
	CLevelMap *lm;
	lm = dynamic_cast<CLevelMap*>(Ninja->ResourceManager.LoadResource("Levels", "Level02", CLevelMap::NewLevelMap));
	lm->visible = true;
	lm->z = -0.6f;
	
	Ninja->RenderManager.SortByZ();
	Ninja->RenderManager.SortByAlpha();
	
	

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

// ИДЗ по матрицам >.>
//  	Matrix3 T;
//  	T[0] = Vector3(-6, -2, 7);
//  	T[1] = Vector3(-17, -15, 21);
//  	T[2] = Vector3(26, 17, -30);
// 
// 	float a1 = T.cofac(1, 0, 2, 1);
// 	float a2 = T.cofac(1, 1, 2, 2);
// 	float a3 = T.cofac(1, 0, 2, 2);
// 	Log("A=", "%f", a1);
// 	Log("A=", "%f", a2);
// 	Log("A=", "%f", a3);
// 
 //= T[0][2]*a1 + T[0][0] * a2 - T[0][1] * a3;
// 
// 	Log("Det=", "%f", det);


	MatrixNM T(4, 4);

// 	  	T.e[0][0] = -6;		T.e[0][1] = -2;		T.e[0][2] = 3;
// 	  	T.e[1][0] = -17;	T.e[1][1] = -15;	T.e[1][2] = -9;
// 	  	T.e[2][0] =  26;	T.e[2][1] =	 17;	T.e[2][2] = 4;
// 
// 		T.e[0][0] = -6;		T.e[1][0] = -2;		T.e[2][0] = 3;
// 		T.e[0][1] = -17;	T.e[1][1] = -15;	T.e[2][1] = -9;
// 		T.e[0][2] =  26;	T.e[1][2] =	 17;	T.e[2][2] = 4;


	  	T.e[0][0] = 3;		T.e[0][1] = -3;		T.e[0][2] = -5;		T.e[0][3] = 8;
	  	T.e[1][0] = 1;		T.e[1][1] = 1;		T.e[1][2] = 4;		T.e[1][3] = -6;
	  	T.e[2][0] =  2;		T.e[2][1] = 0;		T.e[2][2] = -7;		T.e[2][3] = 5;
		T.e[3][0] =  -4;	T.e[3][1] = 3;		T.e[3][2] = 5;		T.e[3][3] = -6;

// 		T.e[0][0] = 1;		T.e[0][1] = 1;		T.e[0][2] = 1;		T.e[0][3] = 1;
// 		T.e[1][0] = 2;		T.e[1][1] = 3;		T.e[1][2] = -1;		T.e[1][3] = -2;
// 		T.e[2][0] =  3;		T.e[2][1] = 3;		T.e[2][2] = 7;		T.e[2][3] = 5;
// 		T.e[3][0] =  4;		T.e[3][1] = 4;		T.e[3][2] = 4;		T.e[3][3] = 9;


	float det = T.Determinant(T.n); // Работает. Протестриовано.
	Log("Matrix T det = ", "%f", det);

	return true;
}

bool Draw()
{	
	//glLoadIdentity();
	Ninja->FontManager->PrintEx(560, 460, 1, "Fps: %d", Ninja->GetFps());
//	char buff[1024];
//	memset(buff, 0, sizeof(buff));
//	sprintf(buff, "%d %d %d %d", Edit->KeyState, SDL_GetTicks() - Edit->KeyTime, Edit->Shift, Edit->SelLength);
//	Button->Caption = buff;

	static Vector2 apos(200, 300);
	static Vector2 bpos(300, 300);
	static Vector2 norm;
	static float depth;

	a.Collide(&a, apos, Vector2(0.0f, 0.0f), Matrix2(0.0f),
				&b, bpos, Vector2(0.0f, 0.0f), Matrix2(0.0f), norm, depth);
	
	

	if (Ninja->keys[SDLK_SPACE])
	{
		apos -= norm*depth/2.0f;
		bpos += norm*depth/2.0f;
		depth = 0;
	}

	gRenderPolygon(&apos, &Matrix2(), &a, &Vector4(0.9f, 0.7f, 0.7f, 0.5f));
	gRenderPolygon(&bpos, &Matrix2(), &b, &Vector4(0.6f, 0.9f, 0.6f, 0.6f));
	gRenderSegment(&bpos, &(bpos + norm*depth), &Vector4(1.0f, 1.0f, 1.0f, 0.8f));

	if (Ninja->keys[SDLK_j])
		apos.x -= 5;
	if (Ninja->keys[SDLK_l])
		apos.x += 5;
	if (Ninja->keys[SDLK_i])
		apos.y += 5;
	if (Ninja->keys[SDLK_k])
		apos.y -= 5;

	CRenderObject *tmp = dynamic_cast<CRenderObject*>(Ninja->RenderManager.GetObject("BatSpr"));
	if (Ninja->keys[SDLK_w])
		tmp->y += 10;
	if (Ninja->keys[SDLK_s])
		tmp->y -= 10;

	if (Ninja->keys[SDLK_d])
		tmp->x += 10;
	if (Ninja->keys[SDLK_a])
		tmp->x -= 10;

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
