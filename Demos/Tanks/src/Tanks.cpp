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

void CTank::Update(float dt)
{
	CBox AABB = CBox(0, 0, 32, 32);
	if (isWalking)
	{
		Velocity += 0.5f;
		Velocity = Clamp(Velocity, 0.0f, 5.0f);
	}
	Position += Direction*Velocity;
	Velocity*=0.9f;
	AABB.Offset(Position.x, Position.y);
	Vector2 BottomLeft = AABB.Min, TopRight = AABB.Max, BottomRight = Vector2(AABB.Max.x, AABB.Min.y), TopLeft = Vector2(AABB.Min.x, AABB.Max.y);
	CBox Pot1, Pot2;

	switch(Dir2AK(Direction))
	{
	case akLeft:
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(BottomLeft);
		if (Pot1.Intersect(AABB))
			Position.x = Pot1.Max.x;
		if (Pot2.Intersect(AABB))
			Position.x = Pot2.Max.x;
		break;
	case akRight:
		Pot1 = Map->GetCellAABB(TopRight);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			Position.x = Pot1.Min.x - 32;
		if (Pot2.Intersect(AABB))
			Position.x = Pot2.Min.x - 32;
		break;
	case akUp:
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(TopRight);
		if (Pot1.Intersect(AABB))
			Position.y = Pot1.Min.y - 32;
		if (Pot2.Intersect(AABB))
			Position.y = Pot2.Min.y - 32;
		break;
	case akDown:
		Pot1 = Map->GetCellAABB(BottomLeft);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			Position.y = Pot1.Max.y;
		if (Pot2.Intersect(AABB))
			Position.y = Pot2.Max.y;
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


		if ((CBox(0,0,32,32).Offsetted(Host->GetPlayer(0)->Position.x, Host->GetPlayer(0)->Position.y)).Inside(Bullets[i].p) && ((string)GetName() != "TankPlayer1"))
		{
			Host->GetPlayer(0)->Health -= 10;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}
		if ((CBox(0,0,32,32).Offsetted(Host->GetPlayer(1)->Position.x, Host->GetPlayer(1)->Position.y)).Inside(Bullets[i].p) && ((string)GetName() != "TankPlayer2"))
		{
			Host->GetPlayer(1)->Health -= 10;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}
	}
}

void CTank::Render()
{
	glLoadIdentity();
	float Angle = 0;
	if (Direction == V2_DIR_RIGHT)
		Angle = 90;
	if (Direction == V2_DIR_DOWN)
		Angle = 180;
	if (Direction == V2_DIR_LEFT)
		Angle = 270;

	glTranslatef(Position.x + 16, Position.y + 16, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glTranslatef(-Position.x - 16, -Position.y - 16, 0.0f);
	Color->glSet();
	glEnable(GL_TEXTURE_2D);
	Tileset->GetTexture()->Bind();
	glBegin(GL_QUADS);
		Tileset->GetCellTC(csTank)[0].glTexCoord();
		glVertex2f(Position.x, Position.y);

		Tileset->GetCellTC(csTank)[1].glTexCoord();
		glVertex2f(Position.x + 32, Position.y);

		Tileset->GetCellTC(csTank)[2].glTexCoord();
		glVertex2f(Position.x + 32, Position.y + 32);

		Tileset->GetCellTC(csTank)[3].glTexCoord();
		glVertex2f(Position.x, Position.y + 32);
	glEnd();
	glLoadIdentity();

	Tileset->GetTexture()->Bind();
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
	return;
}

CBox CTank::GetAABB()
{
	return CBox(Position.x, Position.y, Position.x + DEFAULT_CELL_SIZE, Position.y + DEFAULT_CELL_SIZE);
}

Vector2 CTank::GetCenter()
{
	return (Position + Vector2(DEFAULT_CELL_SIZE/2, DEFAULT_CELL_SIZE/2));
}
void CTankManager::AddPlayer()
{
	if (PlayerCount > MAX_PLAYERS_COUNT)
		return;
	CTank *Tank = new CTank(Map, this, NULL);
	//char * tmp = new char[16];
	//SDL_itoa(, tmp, 10);
	Tank->SetName((string)"TankPlayer" + itos(PlayerCount+1));	
	//delete [] tmp;
	Tank->Init(PlayerCount == 0?&COLOR_P1:&COLOR_P2, Map->GetNewTankLocation());
	Tank->SetPlayerControls(PlayerCount);
	Add(Tank);
	PlayerCount++;
}

CTank* CTankManager::GetPlayer(int PlayerIndex)
{
	//char * tmp = new char[16];
	//SDL_itoa(PlayerIndex+1, tmp, 10);
	CTank *res = dynamic_cast<CTank*>(Get(((string)"TankPlayer" + itos(PlayerIndex+1)).c_str()));
	//delete [] tmp;
	return res;
}

void CTankManager::AddAI()
{
	CTank *Tank = NULL;
	CTankAI *AI = new CTankAI(this, Map, Tank);
	Tank = new CTank(Map, this, AI);
	Tank->SetName("Tank");	
	Tank->Init(&COLOR_AI, Map->GetNewTankLocation());
	Add(Tank);
	PlayerCount++;
	return;
}

void CTankManager::Render()
{
	glLoadIdentity();
	gSetBlendingMode();	
	CFont *Font = CFontManager::Instance()->GetFont("Font");
// 	Font->tClr = COLOR_P1;
// 	Font->SetDepth(0.5f);
// 	Font->Pos = Vector2(10, 460);
// 	Font->Print("Player1 health: %d", GetPlayer(0)->Health);
// 	Font->tClr = COLOR_P2;
// 	Font->Pos = Vector2(490, 460);
// 	Font->Print("Player2 health: %d", GetPlayer(1)->Health);
	COLOR_WHITE.glSet();
	return;
}

void CTankManager::Update(float dt)
{
	return;
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

void CTankMap::Render()
{
	glLoadIdentity();
	glScissor(0, 0, 640, 460);
	gToggleScissor(true);
	Tileset->GetTexture()->Bind();
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
	return;
}

void CTankMap::Update(float dt)
{
	return;
}

CBox CTankMap::GetCellAABB(Vector2 V)
{
	
	if (!Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize].isFree())
		return CBox(      (((int)V.x/(int)CellSize)) * CellSize, (((int)V.y/(int)CellSize)) * CellSize, 
			CellSize, CellSize);
	else 
		return CBox(-1, -1, -1, -1);
}

CTankMapCell* CTankMap::GetCell(Vector2& V)
{
	return &(Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize]);
}
