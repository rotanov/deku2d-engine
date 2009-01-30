#include "Game.h"

//-------------------------------------------//
//			CTileSet functions				 //
//-------------------------------------------//

bool CTileSet::LoadFromFile(char* filename)
{
	CFile file;
	if (!file.Open(filename, CFILE_READ))
	{
		Log("AHTUNG", "Can't open TileSet file %s", filename);
		return false;
	}
	ImageDataFileName = NULL;
	file.ReadLine(ImageDataFileName);
	
	if (!Data.LoadTexture(ImageDataFileName))
	{
		Log("AHTUNG", "Can't load TilSet %s texture %s", filename, ImageDataFileName);
		return false;
	}

	file.Read(&Info, sizeof(Info));
	if (Info.HorNumTiles*Info.TileWidth > Data.width ||
		Info.VerNumTiles*Info.TileHeight > Data.height)
	{
		Log("AHTUNG", "Inconsistence between TileSet Parameters and Image.");
		return false;
	}
	if (BBox != NULL)
		delete [] BBox;
	file.Read(BBox, sizeof(CBBox)*Info.HorNumTiles*Info.VerNumTiles);

	file.Close();
}

void CTileSet::RenderTileSet()
{
	// encapsulate Opengl calls
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, Data.GetTexID());
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(Info.TileWidth*Info.HorNumTiles, 0);
		glTexCoord2f(1, 1); glVertex2f(Info.TileWidth*Info.HorNumTiles, Info.TileHeight*Info.VerNumTiles);
		glTexCoord2f(0, 1); glVertex2f(0, Info.TileHeight*Info.VerNumTiles);
	glEnd();
}

bool CTileSet::SaveToFile( char *filename )
{
	CFile file;
	if (!file.Open(filename, CFILE_WRITE))
	{
		Log("AHTUNG", "Can't open TileSet file %s", filename);
		return false;
	}
	file.Write(ImageDataFileName, strlen(ImageDataFileName)+1);
	file.Write(&Info, sizeof(Info));
	file.Write(BBox, sizeof(CBBox)*Info.HorNumTiles*Info.VerNumTiles);

	file.Close();
}

CTileSet::CTileSet()
{
	Data.CleanData = true;
	BBox = NULL;
	ImageDataFileName = NULL;
	memset(&Info, 0, sizeof(Info));
}

void CTileSet::SetSettings( byte _TileWidth, byte _TileHeight, int _HorNumTiles, int _VerNumTiles, char *_ImageData )
{
	Info.HorNumTiles = _HorNumTiles;
	Info.VerNumTiles = _VerNumTiles;
	Info.TileWidth = _TileWidth;
	Info.TileHeight = _TileHeight;
	ImageDataFileName = _ImageData;
	if (BBox != NULL)
		delete [] BBox;
	BBox = new CBBox [Info.HorNumTiles*Info.VerNumTiles];
	for(int i=0; i<Info.HorNumTiles*Info.VerNumTiles; i++)
		BBox[i] = CBBox(0, 0, Info.TileHeight, Info.TileWidth);
}

//-------------------------------------------//
//				CMap functions				 //
//-------------------------------------------//
CMap::CMap()
{
	TileSetName = NULL;
	name = "new map";
	mHeight = mWidth = 0;
	TileSet = NULL;
	Cells = NULL;
}

CObject * CMap::NewMap()
{
	return new CMap;
}

bool CMap::Render()
{/*
	glBindTexture(GL_TEXTURE_2D, TileSet->Data.TexID);
	glBegin(GL_QUADS);
	for(byte i=0;i < mHeight;i++)
		for(byte j=0;j < mWidth;j++)
		{
			if (pMap[i*mWidth + j] != 0xFF)
			{	
				glTexCoord2f(
					((float)(pMap[i*mWidth+j]%(TileSet->Data.width/TILE_SIZE))*TILE_SIZE)/(TileSet->Data.width), 
					((float)(pMap[i*mWidth+j]/(TileSet->Data.width/TILE_SIZE))*TILE_SIZE)/(TileSet->Data.height)
							);
				glVertex2f(j*CELL_SIZE, i*CELL_SIZE);
				glTexCoord2f(
					((float)(pMap[i*mWidth+j]%(TileSet->Data.width/TILE_SIZE))*TILE_SIZE + TILE_SIZE)/(TileSet->Data.width), 
					((float)(pMap[i*mWidth+j]/(TileSet->Data.width/TILE_SIZE))*TILE_SIZE)/(TileSet->Data.height)
							);
				glVertex2f(j*CELL_SIZE + CELL_SIZE, i*CELL_SIZE);
				glTexCoord2f(
					((float)(pMap[i*mWidth+j]%(TileSet->Data.width/TILE_SIZE))*TILE_SIZE + TILE_SIZE)/(TileSet->Data.width), 
					((float)(pMap[i*mWidth+j]/(TileSet->Data.width/TILE_SIZE))*TILE_SIZE + TILE_SIZE)/(TileSet->Data.height)
							);
				glVertex2f(j*CELL_SIZE + CELL_SIZE, i*CELL_SIZE + CELL_SIZE);
				glTexCoord2f(
					((float)(pMap[i*mWidth+j]%(TileSet->Data.width/TILE_SIZE))*TILE_SIZE)/(TileSet->Data.width), 
					((float)(pMap[i*mWidth+j]/(TileSet->Data.width/TILE_SIZE))*TILE_SIZE + TILE_SIZE)/(TileSet->Data.height)
							);
				glVertex2f(j*CELL_SIZE, i*CELL_SIZE + CELL_SIZE);	
				
			}
		}
	glEnd();*/
	return true;
}

bool CMap::LoadFromFile(char* filename)
{
	CFile File;
	CFactory *Factory = CFactory::Instance();
	if (!File.Open(filename, CFILE_READ))
	{
		Log("AHTUNG", "Can't load level %s", filename);
		return false;
	}

	char *tmp = NULL;
	File.ReadLine(tmp);
	name = tmp;
	File.ReadLine(TileSetName);

	TileSet = NULL;
	TileSet = (CTileSet*)Factory->GetObject(TileSetName);
	if (TileSet == NULL)
	{
		TileSet = (CTileSet*)Factory->Create(OBJ_USER_DEFINED, CTileSet::NewTileSet);		
		//TileSet->LoadFromFile(tilesetname);
	}

	File.Read(&mWidth, sizeof(mWidth));
	File.Read(&mHeight, sizeof(mHeight));
	if (Cells != NULL)
		delete [] Cells;
	Cells = new CMapCell [mWidth*mHeight];
	File.Read(Cells, mWidth*mHeight*sizeof(CMapCell));
	File.Close();
	Factory->FreeInst();
	return true;
}


bool CMap::SaveToFile(char *filename)
{
	CFile file;
	if (!file.Open(filename, CFILE_WRITE))
	{
		Log("AHTUNG", "Can't open file %s to save the map", filename);
		return false;
	}
	file.Write((void*)name.data(), strlen(name.data())+1);
	file.Write(TileSetName, strlen(TileSetName)+1);
	file.Write(&mWidth, sizeof(mWidth));
	file.Write(&mHeight, sizeof(mHeight));
	file.Write(Cells, sizeof(CMapCell)*mWidth*mHeight);
	file.Close();
}

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
	CNinja *Ninja  = CNinja::Instance();
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
	Ninja->FreeInst();
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
	CNinja *Ninja = CNinja::Instance();
	CParticleSystem *ps = (CParticleSystem*)Ninja->RenderManager.GetObject("snowballs");
	Ninja->FreeInst();

	CBBox Gift = CBBox(274, GROUND, 260 + 102 , GROUND + 59 );

	Vector2 n;
	float delta;
	if (Gift.Inside(p->p, delta, n))
	{
		p->p += n*delta;
		p->v = Vector2(0,0);
	}

	for (int i = 0; i<ps->info.ParticlesActive; ++i)
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
	CNinja *Ninja = CNinja::Instance();

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

	if (Ninja->keys[SDLK_z])
	{
		if (abs(Snow - 100.0f) > 0.001f)
		{
			CParticleSystem *ps = (CParticleSystem*)Ninja->Factory->GetObject("psys");
			Vector2 n;

			for (int i=0; i< ps->info.ParticlesActive; i++)
			{
				n = (ps->particles[i].p - Vector2(x+C1, y+C2)).Normalized();
				if (((ps->particles[i].p-Vector2(x, y)).Length())<= 200)
					if (DegToRad(tangle + 15) > atan2f(n.y, n.x) && atan2f(n.y, n.x) > DegToRad(tangle - 15))
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


	CParticleSystem *ps = (CParticleSystem*)Ninja->Factory->GetObject("enemy_l");
	for (int i=0; i< ps->info.ParticlesActive; i++)
	{
		CBBox tmp = CBBox(ps->particles[i].p, ps->particles[i].p + Vector2(ps->particles[i].size, ps->particles[i].size));
		if (tmp.Intersect(Gift))
		{
			Health -= ps->particles[i].size / 4.0f;
			ps->particles[i].age = 0xffffff;
		}
	}
	Health= clampf(Health, 0.0f, 100.0f);
	if (Health <= 0.001f)
	{
		((CTexture*)Ninja->TextureManager->GetObject("giftd"))->Load();
		((CSprite*)Ninja->RenderManager.GetObject("GftSpr"))->m_textureID = ((CTexture*)Ninja->TextureManager->GetObject("giftd"))->GetTexID();
	}





	CBBox tmp  = BBox;


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


	Ninja->FreeInst();
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
		for (int i = 0; i < ps->info.ParticlesActive; i++)
		{
			if (((CEnemy*)obj)->EnemyPos == &ps->particles[i])
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			enems.DelObj(ind);
			ind--;
		}
		ind++;
	}
	return 1;
}

CEnemy * CEnemyController::SpawnEnemy()
{
	if (ps->info.ParticlesActive >= ps->info.MaxParticles)
		return NULL;
	CEnemy *enemy = (CEnemy*)factory->Create(OBJ_USER_DEFINED, &(CEnemy::NewEnemy));
	enemy->AssignParticle(ps->CreateParticle());
	bool side = rand()%2;
	enemy->EnemyPos->p.y = GROUND;
	if (side)
		enemy->EnemyPos->p.x = 650;
	else
		enemy->EnemyPos->p.x = -10;
	return enemy;
}

