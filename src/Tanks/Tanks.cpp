#include "Tanks.h"


bool CTank::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	if (state == KEY_PRESSED)
	{
		for(int i = 0; i < CONTROLS_COUNT; i++)
			if (key == Controls[i])
			{
				States[i] = true;
				if (i>=0 && i<=3)
				{
					isWalking = true;
					Direction = Directions[i];
				}
			}
		if (key == Controls[akFire])
			FiringTimeout = 0.21f;
	}
	else
	{
		isWalking = false;
		for(int i=0;i<5;i++)
			if (key == Controls[i])
				States[i] = false;
			else 
				if (i>=0 && i<=3 && States[i])
					isWalking = true;
	}
	return true;
}

void CTank::SetPlayerControls(int PlayerIndex)
{
	if (AI != NULL)
		return;
	for(int i =0;i<CONTROLS_COUNT;i++)
		Controls[i] = AR_K_CONTRLS[i][PlayerIndex];
}

bool CTank::Update(scalar dt)
{
	CAABB AABB = CAABB(0, 0, 32, 32);
	if (isWalking)
	{
		Velocity += 0.5f;
		Velocity = clampf(Velocity, 0.0f, 5.0f);
	}
	position += Direction*Velocity;
	Velocity*=0.9f;
	AABB.Offset(position.x, position.y);
	Vector2 BottomLeft = AABB.vMin, TopRight = AABB.vMax, BottomRight = Vector2(AABB.x1, AABB.y0), TopLeft = Vector2(AABB.x0, AABB.y1);
	CAABB Pot1, Pot2;

	switch(Dir2AK(Direction))
	{
	case akLeft:
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(BottomLeft);
		if (Pot1.Intersect(AABB))
			position.x = Pot1.x1;
		if (Pot2.Intersect(AABB))
			position.x = Pot2.x1;
		break;
	case akRight:
		Pot1 = Map->GetCellAABB(TopRight);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			position.x = Pot1.x0 - 32;
		if (Pot2.Intersect(AABB))
			position.x = Pot2.x0 - 32;
		break;
	case akUp:
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(TopRight);
		if (Pot1.Intersect(AABB))
			position.y = Pot1.y0 - 32;
		if (Pot2.Intersect(AABB))
			position.y = Pot2.y0 - 32;
		break;
	case akDown:
		Pot1 = Map->GetCellAABB(BottomLeft);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			position.y = Pot1.y1;
		if (Pot2.Intersect(AABB))
			position.y = Pot2.y1;
		break;
	}

	if (States[akFire])
	{
		FiringTimeout += dt;
		if (FiringTimeout >= FiringInterval && BulletsCount < MAX_BULLETS_PER_TANK)
		{
			FiringTimeout = 0;
			BulletsCount++;
			Bullets[BulletsCount-1].v = Direction*Velocity + Direction*10.0f;
			Bullets[BulletsCount-1].p = GetCenter();
		}
	}
	for (int i = 0;i<BulletsCount;i++)
	{
		Bullets[i].p += Bullets[i].v;
		if (Map->GetCell(Bullets[i].p)->TileIndex == csDestr)
		{
			Map->GetCell(Bullets[i].p)->TileIndex = csFree1;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}
		if (Map->GetCell(Bullets[i].p)->TileIndex == csBlock)
		{
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}


		if ((CAABB(0,0,32,32).Offsetted(Host->GetPlayer(0)->position.x, Host->GetPlayer(0)->position.y)).Inside(Bullets[i].p) && ((string)GetName() != "TankPlayer1"))
		{
			Host->GetPlayer(0)->Health -= 10;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}
		if ((CAABB(0,0,32,32).Offsetted(Host->GetPlayer(1)->position.x, Host->GetPlayer(1)->position.y)).Inside(Bullets[i].p) && ((string)GetName() != "TankPlayer2"))
		{
			Host->GetPlayer(1)->Health -= 10;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}

	}
	return true;
}

bool CTank::Render()
{
	glLoadIdentity();
	scalar Angle = 0;
	if (Direction == V2_DIR_RIGHT)
		Angle = 90;
	if (Direction == V2_DIR_DOWN)
		Angle = 180;
	if (Direction == V2_DIR_LEFT)
		Angle = 270;

	glTranslatef(position.x + 16, position.y + 16, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glTranslatef(-position.x - 16, -position.y - 16, 0.0f);
	Color->glSet();
	glEnable(GL_TEXTURE_2D);
	Tileset->Texture->Bind();
	glBegin(GL_QUADS);
		Tileset->GetCellTC(csTank)[0].glTexCoord();
		glVertex2f(position.x, position.y);

		Tileset->GetCellTC(csTank)[1].glTexCoord();
		glVertex2f(position.x + 32, position.y);

		Tileset->GetCellTC(csTank)[2].glTexCoord();
		glVertex2f(position.x + 32, position.y + 32);

		Tileset->GetCellTC(csTank)[3].glTexCoord();
		glVertex2f(position.x, position.y + 32);
	glEnd();
	glLoadIdentity();

	Tileset->Texture->Bind();
	glColor4f(0.8f, 0.1f, 0.1f, 0.9f);
	glBegin(GL_QUADS);
	for(int i=0;i<BulletsCount;i++)
	{
		Tileset->GetCellTC(csBullet)[0].glTexCoord();
		Bullets[i].p.glVertex();

		Tileset->GetCellTC(csBullet)[1].glTexCoord();
		(Bullets[i].p + Vector2(32,0)).glVertex();

		Tileset->GetCellTC(csBullet)[2].glTexCoord();
		(Bullets[i].p + Vector2(32,32)).glVertex();

		Tileset->GetCellTC(csBullet)[3].glTexCoord();
		(Bullets[i].p + Vector2(0,32)).glVertex();
	}
	glEnd();
	
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);


	glDisable(GL_TEXTURE_2D);
	CPrimitiveRender PRender;
	PRender.plClr = Color;
	PRender.gDrawBBox(Map->GetCellAABB(GetCenter()));
	return true;
}

CAABB CTank::GetAABB()
{
	return CAABB(position.x, position.y, position.x + DEFAULT_CELL_SIZE, position.y + DEFAULT_CELL_SIZE);
}

Vector2 CTank::GetCenter()
{
	return (position + Vector2(DEFAULT_CELL_SIZE/2, DEFAULT_CELL_SIZE/2));
}
void CTankManager::AddPlayer()
{
	if (PlayerCount > MAX_PLAYERS_COUNT)
		return;
	CTank *Tank = new CTank(Map, this, NULL);
	char * tmp = new char[16];
	SDL_itoa(PlayerCount+1, tmp, 10);
	Tank->SetName((string)"TankPlayer" + tmp);	
	delete [] tmp;
	Tank->Init(PlayerCount == 0?&COLOR_P1:&COLOR_P2, Map->GetNewTankLocation());
	Tank->SetPlayerControls(PlayerCount);
	AddObject(Tank);
	PlayerCount++;
}

CTank* CTankManager::GetPlayer(int PlayerIndex)
{
	char * tmp = new char[16];
	SDL_itoa(PlayerIndex+1, tmp, 10);
	CTank *res = dynamic_cast<CTank*>(GetObject(((string)"TankPlayer" + tmp).c_str()));;	 
	delete [] tmp;
	return res;
}

void CTankManager::AddAI()
{
	CTank *Tank = NULL;
	CTankAI *AI = new CTankAI(this, Map, Tank);
	Tank = new CTank(Map, this, AI);
	Tank->SetName("Tank");	
	Tank->Init(&COLOR_AI, Map->GetNewTankLocation());
	AddObject(Tank);
	PlayerCount++;
	return;
}

bool CTankManager::Render()
{
	glLoadIdentity();
	gSetBlendingMode();	
	CFont *Font = CEngine::Instance()->FontManager->GetFont("Font");
	Font->tClr = COLOR_P1;
	Font->SetDepth(0.5f);
	Font->Pos.In(10, 460);
	Font->Print("Player1 health: %d", GetPlayer(0)->Health);
	Font->tClr = COLOR_P2;
	Font->Pos.In(490, 460);
	Font->Print("Player2 health: %d", GetPlayer(1)->Health);
	COLOR_WHITE.glSet();
	return true;
}

bool CTankManager::Update(scalar dt)
{
	return true;
}

Vector2 CTankMap::GetNewTankLocation()
{
	for (int i=0;i<MAP_SIZE_X;i++)
		for (int j=0;j<MAP_SIZE_Y;j++)
		{
			if (Cells[j][i].TileIndex == csFree1)
				return Vector2(i*CellSize, j*CellSize);
		}
}

bool CTankMap::Render()
{
	glLoadIdentity();
	glScissor(0, 0, 640, 460);
	gToggleScissor(true);
	Tileset->Texture->Bind();
	glEnable(GL_TEXTURE_2D);
	CellSize = DEFAULT_CELL_SIZE;
	glBegin(GL_QUADS);
	for (int i=0;i<MAP_SIZE_X;i++)
		for (int j=0;j<MAP_SIZE_Y;j++)
		{
			//if (Cells[j][i].TileIndex == csBlock || Cells[j][i].TileIndex  == csDestr || Cells[j][i].TileIndex  == csFree1)
			{
				
					Tileset->GetCellTC(Cells[j][i].TileIndex)[0].glTexCoord();
					(Vector2(i, j)*CellSize).glVertex();
					Tileset->GetCellTC(Cells[j][i].TileIndex)[1].glTexCoord();
					(Vector2(i+1, j)*CellSize).glVertex();
					Tileset->GetCellTC(Cells[j][i].TileIndex)[2].glTexCoord();
					(Vector2(i+1, j+1)*CellSize).glVertex();
					Tileset->GetCellTC(Cells[j][i].TileIndex)[3].glTexCoord();
					(Vector2(i, j+1)*CellSize).glVertex();
				
			}
		}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	gToggleScissor(false);
	return true;
}

bool CTankMap::Update(scalar dt)
{
	return true;
}

CAABB CTankMap::GetCellAABB(Vector2& V)
{
	
	if (!Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize].isFree())
		return CAABB(      (((int)V.x/(int)CellSize)) * CellSize, (((int)V.y/(int)CellSize)) * CellSize, 
			CellSize, CellSize);
	else 
		return CAABB(-1, -1, -1, -1);
}

CTankMapCell* CTankMap::GetCell(Vector2& V)
{
	return &(Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize]);
}
