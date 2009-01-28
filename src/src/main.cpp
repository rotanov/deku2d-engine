#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

CParticleSystem *psys, *enemy_l;
CSprite *Spr;
CHero *Hero;
CMap *Map;
Vector2 pnts[2];
CEnemyController *enemyc;

bool Init()
{	
	Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath);
	Ninja->ResourceManager.LoadResources();
	CFactory *Factory = CFactory::Instance();
	gSetBlendingMode();

	Hero = (CHero*)Factory->Create(OBJ_USER_DEFINED, &(CHero::NewHero));
	Hero->x = 200;
	Hero->y = 200;//GROUND;
	Hero->z = 0.0f;

	
 	CTextureManager *TxMan = Ninja->TextureManager;
	TxMan->LoadAllTextures(); //MemoryWarning;
	TxMan->LoadTextureByName("hero");
	TxMan->LoadTextureByName("heroa");
	TxMan->LoadTextureByName("herob");
	TxMan->LoadTextureByName("snwbl");

	TxMan->LoadTextureByName("bg");
	TxMan->LoadTextureByName("gift");
	TxMan->LoadTextureByName("gr");


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
	psys->info.ec = RGBAf(1.0f, 1.0f, 1.0f, 0.0f);
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

	
	Ninja->RenderManager.Camera.Assign(&Hero->x, &Hero->y);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LINE_WIDTH);
	
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
