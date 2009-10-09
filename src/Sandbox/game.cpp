#include "Game.h"


bool CHero::Render()
{
	int C1, C2 = 23;
	float fangle = angle;
	if (spra.mirror_h)
	{
		C1 = 33;
		fangle *= -1.0f;
	}
	else
		C1 = 23;
	
	glTranslatef(C1, C2, 0);
	glRotatef(fangle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-C1, -C2, 0);
	sprb.Render();
	glTranslatef(C1, C2, 0);
	glRotatef(-fangle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-C1, -C2, 0);
	spra.Render();

	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glTranslatef(0.0f, 50.0f, 0.0f);
	glBegin(GL_QUADS);
		// Health meter
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(010.0f, 400.0f);
		glVertex2f(114.0f, 400.0f);
		glVertex2f(114.0f, 416.0f);
		glVertex2f(010.0f, 416.0f);

		glColor3f(0.9f, 0.5f, 0.5f);
		glVertex2f(012.0f, 402.0f);
		glVertex2f(12.0f + Health, 402.0f);
		glVertex2f(12.0f + Health, 414.0f);
		glVertex2f(012.0f, 414.0f);

		//Snow meter
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(010.0f, 380.0f);
		glVertex2f(114.0f, 380.0f);
		glVertex2f(114.0f, 396.0f);
		glVertex2f(010.0f, 396.0f);

		glColor3f(0.5f, 0.5f, 0.9f);
		glVertex2f(012.0f, 382.0f);
		glVertex2f(12.0f + Snow, 382.0f);
		glVertex2f(12.0f + Snow, 394.0f);
		glVertex2f(012.0f, 394.0f);

	glEnd();
	CEngine *Ninja  = CEngine::Instance();
	gSetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	if (Health <= 0.001f)
	{	
		glLoadIdentity();
		glScalef(3.0f, 3.0f, 1.0f);
		Ninja->FontManager->PrintEx(220, 220,1,  "Present was eaten. Game Over.");
	}
	Ninja->FontManager->PrintEx(15, 399, 1, "Health");
	Ninja->FontManager->PrintEx(15, 379, 1, "Snow");
	if (Snow == 100)
		Ninja->FontManager->PrintEx(65, 379, 1, "ready");
	return true;
}

void UpdateSnowballs( CParticle *p, float dt )
{
	p->p += dt*p->v;
	p->v *= 0.98;
	p->v.y -= 10;
	if (p->p.y < 16)
	{
		p->v.y = 0;
		p->p.y = 0;
	}
	CEngine *Ninja = CEngine::Instance();
	CParticleSystem *ps = dynamic_cast<CParticleSystem*>(Ninja->RenderManager.GetObject("snowballs"));

	CAABB Gift = CAABB(274, GROUND, 260 + 51 , GROUND + 29 );

	Vector2 n;
	float delta;
	if (Gift.Inside(p->p, delta, n))
	{
		p->p += n*delta;
		p->v = Vector2(0,0);
	}

	for (int i = 0; i<ps->Info.ParticlesActive; ++i)
	{
		if (&(ps->particles[i]) == p)
			break;
		if ((ps->particles[i].p-p->p).Length() <= 16)
		{
			n = (ps->particles[i].p-p->p).Normalized();		// Collision resolving: just change pos of particles along penetration normal; by half of MTD each;
			p->p -= n*8;
			ps->particles[i].p += n*8;
		}
	}
}

bool CHero::Update( float dt )
{
	CEngine *Ninja = CEngine::Instance();

	if (Ninja->keys[SDLK_LEFT])
	{			
		if (onPlane)
			v.x -= 20;
		else
			v.x -= 2;
		spra.mirror_h = true;
		sprb.mirror_h = true;
	}
	if (Ninja->keys[SDLK_RIGHT])
	{
		if (onPlane)
			v.x += 20;
		else
			v.x += 2;
		spra.mirror_h = false;
		sprb.mirror_h = false;
	}
	angle = clampf(angle, -45.0f, 45);
	float tangle = angle;
	if (sprite.mirror_h)
	{
		tangle = 180.0f - angle;
		if (angle < 0)
			tangle -= 360.0f;
	}
	if (Ninja->keys[SDLK_DOWN])
		angle -= 7;
	if (Ninja->keys[SDLK_UP])
		angle += 7;

	if (Ninja->keys[SDLK_a])
		if (onPlane && relj)
		{
			v.y += 250;
			onPlane = false;
			relj = false;
		}else;
	else
	{
		relj = true;
	}

	int C1, C2 = 23;
	if (spra.mirror_h)
		C1 = 33;
	else
		C1 = 23;
	//if something wrong simply replace tangle*(C1!=33) + (180 - tangle)*(C1==33) with tangle
	if (spra.mirror_h)
		tangle = 180 - tangle;

	if (Ninja->keys[SDLK_z])
	{
		if (abs(Snow - 100.0f) > 0.001f)
		{
			CParticleSystem *ps = dynamic_cast<CParticleSystem*>(Ninja->Factory->GetObject("psys"));
			Vector2 n;
//					if (DegToRad(tangle + 15) > atan2f(n.y, n.x) && atan2f(n.y, n.x) > DegToRad(tangle - 15))
			for (int i=0; i< ps->Info.ParticlesActive; i++)
			{
				n = (ps->particles[i].p - Vector2(x+C1, y+C2)).Normalized();
				if (((ps->particles[i].p-Vector2(x, y)).Length())<= 200)
					if (DegToRad(tangle+ 15) > atan2f(n.y, n.x) && atan2f(n.y, n.x) > DegToRad(tangle - 15))
					{
						ps->particles[i].p += (Vector2(x, y)-ps->particles[i].p+Vector2(32, 32))/((ps->particles[i].p-Vector2(x, y)).Length()*0.15f);
						if (((ps->particles[i].p-Vector2(x+C1, y+C2)).Length())<= 35)
						{
							Snow += 1;
							ps->particles[i].age = 0xffffff;
						}

						Snow = clampf(Snow, 0.0f, 100.0f);
					}
			}
		}
		else
		{
			CParticle *prt = snowballs->CreateParticle();
			prt->v = Vector2(cos(DegToRad(tangle)), sin(DegToRad(tangle)))*500.01f;

			Vector2 pos = Vector2(cos(DegToRad(tangle)), sin(DegToRad(tangle)))*32;
			pos += Vector2(C1-16+x, C2-16+y);

			prt->p = pos;
			sballlaunched = true;
			Snow = 0.0f;
		}

	}

	if (spra.mirror_h)
		tangle = - 180 - tangle;


	CParticleSystem *ps = dynamic_cast<CParticleSystem*>(Ninja->Factory->GetObject("enemy_l"));
	for (int i=0; i< ps->Info.ParticlesActive; i++)
	{
		CAABB tmp = CAABB(ps->particles[i].p, ps->particles[i].p + Vector2(ps->particles[i].size, ps->particles[i].size));
		if (tmp.Intersect(Gift))
		{
			Health -= ps->particles[i].size / 4.0f;
			ps->particles[i].age = 0xffffff;
		}
	}
	Health= clampf(Health, 0.0f, 100.0f);
// 	if (Health <= 0.001f)
// 	{
// 		(dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("giftd")));
// 		(dynamic_cast<CSprite*>(Ninja->RenderManager.GetObject("GftSpr")))->m_textureID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObject("giftd")))->GetTexID();
// 	}





	CAABB tmp  = BBox;


	x += v.x*dt;
	tmp.Offset(x, y);
	if (tmp.Intersect(Gift))
	{	
		if (v.x > 0)
			x = Gift.vMin.x  - 56;
		else
			x = Gift.vMax.x;
		v.x = -v.x*0.1f;
	}


	tmp = BBox;
	y += v.y*dt;
	tmp.Offset(x, y);
	if (tmp.Intersect(Gift))
	{
		y = GROUND+60;
		v.y = 0;
		onPlane = true;
	}



	if (onPlane)
		v.x *= 0.92f;
	else
		v.x *= 0.999f;

	//		if(Gift.)
	if (y <= GROUND)
	{
		y = GROUND;
		v.y = 0 ;
		onPlane = true;
	}
	else
		v.y -= 10;



	return true;

}

void CEnemy::Move( float dm, float dt )
{
	Moving = true;
//	deltaM = dm/dt;
	EnemyPos->v.x = dm/dt;
	deltat = dt;
	deltast = 0;
}

bool CEnemy::Update( float dt )
{
	if (EnemyPos == NULL)
		return 1;
	EnemyPos->age = 1;
	if (Moving)
	{
		if (dt + deltast > deltat)
		{
			EnemyPos->v.x = 0;
			Moving = false;
		}
		deltast += dt;
	}
	return 1;
}

void CEnemy::MoveInstant( Vector2 pos )
{
	EnemyPos->p = pos;
}

void CEnemy::AssignParticle( CParticle *particle )
{
	EnemyPos = particle;
	EnemyPos->v = Vector2(0, 0);
}

bool CEnemyController::Update( float dt )
{
	if (SDL_GetTicks() - lt >=3000)
	{
		//spawning enemies
		spawned = true;
		CEnemy *enemy = SpawnEnemy();
		if (enemy != NULL)
		{
			//setting right direction
			enemy->Move((float)(enemy->Gift.vMax.x + enemy->Gift.vMin.x)/2 - enemy->EnemyPos->p.x, 4);
			enems.AddObject(enemy);
			lt = SDL_GetTicks();
		}
//		enemy->Move()
	}
	else
		spawned = false;
	CObject *obj;
	enems.Reset();
	int ind = 0;
	while (enems.Enum(obj))
	{
		bool flag = false;
		for (int i = 0; i < ps->Info.ParticlesActive; i++)
		{
			if ((dynamic_cast<CEnemy*>(obj))->EnemyPos == &ps->particles[i])
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			enems.DelObject(ind);
			ind--;
		}
		ind++;
	}
	return 1;
}

CEnemy * CEnemyController::SpawnEnemy()
{
	if (ps->Info.ParticlesActive >= ps->Info.MaxParticles)
		return NULL;
	CEnemy *enemy = dynamic_cast<CEnemy*>(factory->Create(OBJ_USER_DEFINED, &(CEnemy::NewEnemy)));
	enemy->AssignParticle(ps->CreateParticle());
	bool side = rand()%2;
	enemy->EnemyPos->p.y = GROUND;
	if (side)
		enemy->EnemyPos->p.x = 650;
	else
		enemy->EnemyPos->p.x = -10;
	return enemy;
}

