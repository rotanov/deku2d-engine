#include "Tanks.h"

void CTank::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		for (int i = 0; i < CONTROLS_COUNT; i++)
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
	else if (AEvent.GetName() == "KeyUp")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		isWalking = false;
		for (int i = 0; i < 5; i++)
			if (key == Controls[i])
				States[i] = false;
			else 
				if (i>=0 && i<=3 && States[i])
					isWalking = true;
	}
}

void CTank::SetPlayerControls(int PlayerIndex)
{
	if (AI != NULL)
		return;
	for (int i = 0; i < CONTROLS_COUNT; i++)
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
	GetPosition() += Direction*Velocity;
	Velocity*=0.9f;
	AABB.Offset(GetPosition());
	Vector2 BottomLeft = AABB.Min, TopRight = AABB.Max, BottomRight = Vector2(AABB.Max.x, AABB.Min.y), TopLeft = Vector2(AABB.Min.x, AABB.Max.y);
	CBox Pot1, Pot2;

	switch (Dir2AK(Direction))
	{
	case akLeft:
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(BottomLeft);
		if (Pot1.Intersect(AABB))
			GetPosition().x = Pot1.Max.x;
		if (Pot2.Intersect(AABB))
			GetPosition().x = Pot2.Max.x;
		break;
	case akRight:
		Pot1 = Map->GetCellAABB(TopRight);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			GetPosition().x = Pot1.Min.x - 32;
		if (Pot2.Intersect(AABB))
			GetPosition().x = Pot2.Min.x - 32;
		break;
	case akUp:
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(TopRight);
		if (Pot1.Intersect(AABB))
			GetPosition().y = Pot1.Min.y - 32;
		if (Pot2.Intersect(AABB))
			GetPosition().y = Pot2.Min.y - 32;
		break;
	case akDown:
		Pot1 = Map->GetCellAABB(BottomLeft);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			GetPosition().y = Pot1.Max.y;
		if (Pot2.Intersect(AABB))
			GetPosition().y = Pot2.Max.y;
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
	for (int i = 0; i < BulletsCount; i++)
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


		if ((CBox(0,0,32,32).Offsetted(Host->GetPlayer(0)->GetPosition().x, Host->GetPlayer(0)->GetPosition().y)).Inside(Bullets[i].p) && ((string)GetName() != "TankPlayer1"))
		{
			Host->GetPlayer(0)->Health -= 10;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}
		if ((CBox(0,0,32,32).Offsetted(Host->GetPlayer(1)->GetPosition().x, Host->GetPlayer(1)->GetPosition().y)).Inside(Bullets[i].p) && ((string)GetName() != "TankPlayer2"))
		{
			Host->GetPlayer(1)->Health -= 10;
			std::swap(Bullets[i], Bullets[--BulletsCount]);
		}
	}
}

void CTank::Render()
{
//	glLoadIdentity();
	float Angle = 0;
	if (Direction == V2_DIR_RIGHT)
		Angle = 90;
	if (Direction == V2_DIR_DOWN)
		Angle = 180;
	if (Direction == V2_DIR_LEFT)
		Angle = 270;

// 	glTranslatef(Position.x + 16, Position.y + 16, 0.0f);
// 	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
// 	glTranslatef(-Position.x - 16, -Position.y - 16, 0.0f);
	SetLayer(2);
	RenderManager->DrawTexturedBox(this, CBox(V2_ZERO, Vector2(32.0f, 32.0f)), Tileset->GetTexture(), Tileset->GetCellTC(csTank));

//Draw bullets
//Tileset->GetTexture()	
// 	for (int i = 0; i < BulletsCount; i++)
// 	{
// 		Tileset->GetCellTC(csBullet)[0].glTexCoord();
// 		Bullets[i].p.glVertex();
// 
// 		Tileset->GetCellTC(csBullet)[1].glTexCoord();
// 		(Bullets[i].p + Vector2(32,0)).glVertex();
// 
// 		Tileset->GetCellTC(csBullet)[2].glTexCoord();
// 		(Bullets[i].p + Vector2(32,32)).glVertex();
// 
// 		Tileset->GetCellTC(csBullet)[3].glTexCoord();
// 		(Bullets[i].p + Vector2(0,32)).glVertex();
// 	}

	
	return;
}

CBox CTank::GetAABB()
{
	return CBox(GetPosition().x, GetPosition().y, GetPosition().x + DEFAULT_CELL_SIZE, GetPosition().y + DEFAULT_CELL_SIZE);
}

Vector2 CTank::GetCenter()
{
	return (GetPosition() + Vector2(DEFAULT_CELL_SIZE/2, DEFAULT_CELL_SIZE/2));
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
//	glLoadIdentity();
//	gSetBlendingMode();	
	CFont *Font = FontManager->GetFont("Font");
	return;
}

void CTankManager::Update(float dt)
{
	PlOneHealth.SetText("Player1 health: %d" + itos(GetPlayer(0)->Health));
	PlTwoHealth.SetText("Player2 health: %d" + itos(GetPlayer(1)->Health));
	return;
}

Vector2 CTankMap::GetNewTankLocation()
{
	for (int i = 0; i < MAP_SIZE_X; i++)
		for (int j = 0; j < MAP_SIZE_Y; j++)
		{
			if (Cells[j][i].TileIndex == csFree1)
				return Vector2(i*CellSize, j*CellSize);
		}

	return Vector2(); // just to prevent warning..
}

void CTankMap::Render()
{
//	glLoadIdentity();
	CellSize = DEFAULT_CELL_SIZE;

	for (int i = 0; i < MAP_SIZE_X; i++)
		for (int j = 0; j < MAP_SIZE_Y; j++)
		{
			//if (Cells[j][i].TileIndex == csBlock || Cells[j][i].TileIndex  == csDestr || Cells[j][i].TileIndex  == csFree1)
			{
				RenderManager->DrawTexturedBox(this, CBox(Vector2(i, j) * CellSize, Vector2(i+1, j+1) * CellSize),
					Tileset->GetTexture(), Tileset->GetCellTC(Cells[j][i].TileIndex));
			}
		}

//	glDisable(GL_TEXTURE_2D);
	return;
}

void CTankMap::Update(float dt)
{
	return;
}

CBox CTankMap::GetCellAABB(Vector2 V)
{
	
	if (!Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize].isFree())
		return CBox((((int)V.x/(int)CellSize)) * CellSize, (((int)V.y/(int)CellSize)) * CellSize,
			CellSize, CellSize);
	else 
		return CBox(-1, -1, -1, -1);
}

CTankMapCell* CTankMap::GetCell(Vector2& V)
{
	return &(Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize]);
}
