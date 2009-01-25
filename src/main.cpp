#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

CParticleSystem *psys, *enemy_l;
CSprite *Spr;
CHero *Hero;
CMap *Map;
//CGUIRenderer *_Dummy;
//CTexture *BackGround, *Gift, *Ground;
Vector2 pnts[2];
CEnemyController *enemyc;

bool Init()
{	
	Ninja->ResourceManager.LoadTextures(Ninja->ResourceListPath);
	Ninja->ResourceManager.LoadResources(Ninja->ResourceListPath);
	CFactory *Factory = CFactory::Instance();
	gSetBlendingMode();

	Hero = (CHero*)Factory->Create(OBJ_USER_DEFINED, &(CHero::NewHero));
	Hero->x = 200;
	Hero->y = 200;//GROUND;
	Hero->z = 0.0f;
	((CTexture*)Ninja->TextureManager->GetObject("hero"))->Load();
	((CTexture*)Ninja->TextureManager->GetObject("heroa"))->Load();
	((CTexture*)Ninja->TextureManager->GetObject("herob"))->Load();
	((CTexture*)Ninja->TextureManager->GetObject("snwbl"))->Load();

	((CTexture*)Ninja->TextureManager->GetObject("bg"))->Load();
	((CTexture*)Ninja->TextureManager->GetObject("gift"))->Load();
	((CTexture*)Ninja->TextureManager->GetObject("gr"))->Load();

	Hero->sprite.m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("hero"))->TexID;
	Hero->sprite.m_nTextureHeight = 32;
	Hero->sprite.m_nTextureWidth = 32;
	Hero->sprite.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprite.SetAnimation(1);

	Hero->spra.m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("heroa"))->TexID;
	Hero->spra.m_nTextureHeight = 32;
	Hero->spra.m_nTextureWidth = 32;
	Hero->spra.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->spra.SetAnimation(1);
	Hero->spra.z = 0.0f;
	Hero->sprb.z = 0.0f;
	Hero->z = -0.5f;

	Hero->sprb.m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("herob"))->TexID;
	Hero->sprb.m_nTextureHeight = 32;
	Hero->sprb.m_nTextureWidth = 32;
	Hero->sprb.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprb.SetAnimation(1);

	psys = (CParticleSystem*)Factory->Create(OBJ_PSYSTEM, NULL);
	psys->name = "psys";
	psys->Init();
	psys->info.sc = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	psys->info.ec = RGBAf(1.0f, 1.0f, 1.0f, .0f);
	psys->visible = true;	
	pnts[0] = Vector2(0, 480);
	pnts[1] = Vector2(640, 480);
	psys->info.plife = 3;
	psys->info.emission = 500;///*debug 1 */ 3000;
	psys->info.startsize = 8;
	// debug //
	psys->info.sizevar = 8;
	psys->info.isSnow = true;
	((CTexture*)Ninja->TextureManager->GetObject("sball"))->Load();
	psys->TexID = ((CTexture*)Ninja->TextureManager->GetObject("sball"))->TexID;
	psys->SetGeometry(pnts, 2);

	enemy_l = (CParticleSystem*)Factory->Create(OBJ_PSYSTEM, NULL);
	enemy_l->name = "enemy_l";
	enemy_l->Init();

	enemy_l->info.sc = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	enemy_l->info.ec = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	enemy_l->visible = true;	
	enemy_l->info.plife = 3000;
	enemy_l->info.MaxParticles = 10;
	enemy_l->info.emission = 0;
	enemy_l->info.startsize = 64;

	enemyc = (CEnemyController*)Factory->Create(OBJ_USER_DEFINED, &(CEnemyController::NewController));
	enemyc->AssignPS(enemy_l);


//	_testenemy = (CEnemy*)Factory->Create(OBJ_USER_DEFINED, &(CEnemy::NewEnemy));
//	_testenemy->AssignParticle(enemy_l->CreateParticle());
//	_testenemy->MoveInstant(Vector2(100.0f, GROUND));
//	_testenemy->Move(300.0f, 6.0f);

	((CTexture*)Ninja->TextureManager->GetObject("enemy"))->Load();
	enemy_l->TexID = ((CTexture*)Ninja->TextureManager->GetObject("enemy"))->TexID;

	Spr = (CSprite*)Factory->Create(OBJ_SPRITE, NULL);
	Spr->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("gift"))->TexID;
	Spr->m_nTextureHeight = 64;
	Spr->m_nTextureWidth = 64;
	Spr->AddAnimation(true, 1, 104, 98, 1, 1, 1, 52, 49, 2, 15, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "GftSpr";
	Spr->x = 260; 
	Spr->y = GROUND; 
	Spr->z = 0.9f; 
	Spr->visible = true;

	Spr = (CSprite*)Factory->Create(OBJ_SPRITE, NULL);
	Spr->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("bg"))->TexID;
	Spr->m_nTextureHeight = 256;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 512, 512, 1, 1, 1, 256, 256, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "BgrSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = -0.9f; 
	Spr->visible = true;

	Spr = (CSprite*)Factory->Create(OBJ_SPRITE, NULL);
	Spr->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("gr"))->TexID;
	Spr->m_nTextureHeight = 16;
	Spr->m_nTextureWidth = 256;
	Spr->AddAnimation(true, 1, 512, 32, 1, 1, 1, 256, 16, 0, 0, 1, true);
	Spr->SetAnimation(1);
	Spr->name = "grSpr";
	Spr->x = 0; 
	Spr->y = 0; 
	Spr->z = 0.3f; 
	Spr->visible = true;

	int i;
	repeat(i, 201)
	{
		psys->Update(0.02f);
	}

	
	//Map = (CMap*)Factory->Create(OBJ_USER_DEFINED, &(CMap::NewMap));	
	//_Dummy = (CGUIRenderer*)Factory->Create(OBJ_USER_DEFINED, &(CGUIRenderer::NewRenderer));
	//CGUIScheme *sch = new CGUIScheme("data\\main.gui", "data\\maingui.bmp");
	//CForm *frm = (CForm*)addForm("data\\test.df");
	Ninja->RenderManager.Camera.Assign(&Hero->x, &Hero->y);

	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_LINE_WIDTH);
	
	//Ninja->RenderManager.SortByZ();
	//Ninja->RenderManager.SortByAlpha();

	Ninja->FontManager->SetCurrentFont("FFont");
	Factory->FreeInst();
	return true;
}

bool Draw()
{	
	
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	Ninja->FontManager->PrintEx(560, 460, 1, "Fps: %d", Ninja->GetFps());
	
// 	glTranslatef(0.0f, 0.0f, -0.9f);
// 	gSetColor(Vector4(1,1,1,1));
// 	glBindTexture(GL_TEXTURE_2D, BackGround->TexID);
// 	glBegin(GL_QUADS);
// 		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
// 		glTexCoord2f(2.0f, 0.0f); glVertex2f(1024, 0);
// 		glTexCoord2f(2.0f, 1.0f); glVertex2f(1024, 512);
// 		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, 512);
// 	glEnd();
// 
// 	glTranslatef(0.0f, 0.0f, 1.0f);
// 	gSetColor(Vector4(1,1,1,1));
// 	glBindTexture(GL_TEXTURE_2D, Ground->TexID);
// 	glBegin(GL_QUADS);
// 		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
// 		glTexCoord2f(2.0f, 0.0f); glVertex2f(1024, 0);
// 		glTexCoord2f(2.0f, 1.0f); glVertex2f(1024, 32);
// 		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, 32);
// 	glEnd();

	//gSetColor(Vector4(1,1,1,1));
	//glBindTexture(GL_TEXTURE_2D, Gift->TexID);
	//glBegin(GL_QUADS);
	//glTexCoord2f(0.0f, 0.0f); glVertex2f(260, 24);
	//glTexCoord2f(1.0f, 0.0f); glVertex2f(260+128, 24);
	//glTexCoord2f(1.0f, 1.0f); glVertex2f(260+128,24+ 128);
	//glTexCoord2f(0.0f, 1.0f); glVertex2f(260, 24 + 128);
	//glEnd();


	return true;
}

bool Update()
{
	return true;
}

int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_UPDATE_FUNC, &Update);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);
	Ninja->SetState(STATE_USER_INIT, &Init);
	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x0;
}
