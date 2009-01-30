#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

CParticleSystem *ps;
CSprite *Spr;
CHero *Hero;
CMap *Map;
Vector2 pnts[2];
CEnemyController *enemyc;

bool Init()
{	
	gSetBlendingMode();
	if (!Ninja->ResourceManager.OpenResourceList(Ninja->ResourceListPath)) 
		return false;
	if (!Ninja->ResourceManager.LoadResources())
		return false;
	
	CFactory *Factory = CFactory::Instance();
	

	Hero = (CHero*)Factory->Create(OBJ_USER_DEFINED, &(CHero::NewHero));
	Hero->x = 200;
	Hero->y = 200;//GROUND;
	Hero->z = 0.0f;

	Hero->sprite.m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("hero"))->GetTexID();
	Hero->sprite.m_nTextureHeight = 32;
	Hero->sprite.m_nTextureWidth = 32;
	Hero->sprite.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprite.SetAnimation(1);

	Hero->spra.m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("heroa"))->GetTexID();
	Hero->spra.m_nTextureHeight = 32;
	Hero->spra.m_nTextureWidth = 32;
	Hero->spra.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->spra.SetAnimation(1);
	Hero->spra.z = 0.0f;
	Hero->sprb.z = 0.0f;
	Hero->z = -0.5f;

	Hero->sprb.m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("herob"))->GetTexID();
	Hero->sprb.m_nTextureHeight = 32;
	Hero->sprb.m_nTextureWidth = 32;
	Hero->sprb.AddAnimation(true, 1, 56, 48, 1, 1, 1, 28, 24, 2, 7, 1, true);
	Hero->sprb.SetAnimation(1);

	ps = (CParticleSystem*)Factory->Create(OBJ_PSYSTEM, NULL);
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
	ps->TexID = ((CTexture*)Ninja->TextureManager->GetObject("sball"))->GetTexID();
	ps->SetGeometry(pnts, 2);

	int i;
	repeat(i, 201)
	{
		ps->Update(0.02f);
	}

	ps = (CParticleSystem*)Factory->Create(OBJ_PSYSTEM, NULL);
	ps->name = "enemy_l";
	ps->Init();

	ps->info.sc = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	ps->info.ec = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	ps->visible = true;	
	ps->info.plife = 3000;
	ps->info.MaxParticles = 10;
	ps->info.emission = 0;
	ps->info.startsize = 64;

	enemyc = (CEnemyController*)Factory->Create(OBJ_USER_DEFINED, &(CEnemyController::NewController));
	enemyc->AssignPS(ps);

	ps->TexID = ((CTexture*)Ninja->TextureManager->GetObject("enemy"))->GetTexID();

	Spr = (CSprite*)Factory->Create(OBJ_SPRITE, NULL);
	Spr->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("gift"))->GetTexID();
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
	Spr->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("bg"))->GetTexID();
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
	Spr->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("gr"))->GetTexID();
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
