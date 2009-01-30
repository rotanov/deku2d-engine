#ifndef GAME_H
#define GAME_H

#include "Ninja.h"
#include "GameUtils.h"
#include "MathUtils.h"

#define GROUND 16

#pragma warning(disable:4244)

class CUnit : public CRenderObject
{
public:
	int Health;
	CSprite sprite;
	CBBox BBox;
	bool dead;
	CLevelMap *map;
	CBBox Gift;
	Vector2 v; //��������

	CUnit ()
	{
		v = Vector2(0.0f, 0.0f);
		Health  = 100;
		Gift = CBBox(274, GROUND, 260 + 102 , GROUND + 59 );
	}
	virtual bool Update(float dt)
	{
		return true;
	}
};

class CEnemy : public CUnit
{
public:
	CEnemy()
	{
		type = T_UPDATABLE | T_COBJECT;
		Moving = Jumping = false;
		EnemyPos = NULL;
	}
	bool Update(float dt);
	void Move(float dm, float dt);
	void MoveInstant(Vector2 pos);
	void Stop();
	void Jump();
	void AssignParticle(CParticle *particle);
	static CObject*	NewEnemy()
	{
		return new CEnemy;
	}
	CParticle *EnemyPos;
protected:
	float deltaM, deltat, deltast;
	bool Moving, Jumping;
};

class CEnemyController : public CObject
{
public:
	CEnemyController()
	{
		type = T_UPDATABLE|T_COBJECT;
		factory = CFactory::Instance();
		spawned = false;
		lt = SDL_GetTicks();
	}
	~CEnemyController()
	{
		factory->FreeInst();
	}
	bool Update(float dt);
	static CObject*	NewController()
	{
		return new CEnemyController;
	}
	void AssignPS(CParticleSystem *_ps)
	{
		ps = _ps;
	}
	CEnemy *SpawnEnemy();
protected:
	CParticleSystem *ps;
	CObjectList enems;
	CFactory *factory;
	bool spawned;
	DWORD lt;
};

void UpdateSnowballs(CParticle *p, float dt);

class CHero : public CUnit
{
public:
	CSprite spra, sprb;
	CParticleSystem *snowballs;
	bool sballlaunched;
	float angle;
	float Snow;
	bool onPlane, relj;

	CHero()
	{
		angle = 0.0f;
		sballlaunched = false;
		type |= T_UPDATABLE;

		Snow = 0;
		onPlane = true;
		BBox = CBBox(0, 0, 56, 48);
		relj = true;


		CNinja *Ninja = CNinja::Instance();
		CFactory  *Factory  = CFactory::Instance();
		snowballs = (CParticleSystem*)Factory->Create(OBJ_PSYSTEM, NULL);
		Factory->FreeInst();

		snowballs->name = "snowballs";
		snowballs->Init();
		((CTexture*)Ninja->TextureManager->GetObject("snwbl"))->Load();
		snowballs->TexID = ((CTexture*)Ninja->TextureManager->GetObject("snwbl"))->GetTexID();

		snowballs->info.sc = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
		snowballs->info.ec = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
		snowballs->visible = true;	
		snowballs->info.plife = -1;
		snowballs->info.emission = 0;
		snowballs->info.startsize = 32;
		snowballs->info.sizevar = 0;

		snowballs->SetUserUpdate(UpdateSnowballs);


		Ninja->FreeInst();
		
	}

	static CObject* NewHero()
	{
		return new CHero;
	}
	bool Update(float dt);
	bool Render();
};






#endif GAME_H