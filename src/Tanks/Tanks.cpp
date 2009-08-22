#include "Tanks.h"


bool CTank::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	if (state == KEY_PRESSED)
	{
		for(int i=0;i<5;i++)
			if (key == Controls[i])
			{
				States[i] = true;
				if (i>=0 && i<=3)
				{
					isWalking = true;
					Direction = Directions[i];
				}
			}


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

void CTank::SetPlayerControls(SDLKey ALeft, SDLKey ARight, SDLKey AUp, SDLKey ADown, SDLKey AFire)
{
	if (AI != NULL)
		return;
	Controls[0] = ALeft;
	Controls[1] = ARight;
	Controls[2] = AUp;
	Controls[3] = ADown;
	Controls[4] = AFire;
}

bool CTank::Update(scalar dt)
{
	CAABB AABB;
	if (isWalking)
		Position += Direction*Velocity;

		//AABB = Map->IsFPTWA(SelDir(Direction), Position);

	if (States[ACTION_FIRE])
	{
		// Kill it with fire here
		States[ACTION_FIRE] = false;
	}
	return true;
}

bool CTank::Render()
{
	glLoadIdentity();
	scalar Angle = 0;
	if (Direction == TANK_DIR_RIGHT)
		Angle = 90;
	if (Direction == TANK_DIR_DOWN)
		Angle = 180;
	if (Direction == TANK_DIR_LEFT)
		Angle = 270;

	glTranslatef(Position.x + 16, Position.y + 16, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glTranslatef(-Position.x - 16, -Position.y - 16, 0.0f);
	Color->glSet();
	glEnable(GL_TEXTURE_2D);
	Texture->Bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(Position.x, Position.y);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(Position.x + 32, Position.y);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(Position.x + 32, Position.y + 32);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(Position.x, Position.y + 32);
	glEnd();
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
	CPrimitiveRender PRender;
	PRender.plClr = Color;
	PRender.gDrawBBox(Map->GetCellAABB(GetCenter()));
	return true;
}

CAABB CTank::GetAABB()
{
	return CAABB(Position.x, Position.y, Position.x + 32, Position.y + 32);
}

Vector2 CTank::GetCenter()
{
	return (Position + Vector2(16, 16));
}
void CTankManager::AddPlayer()
{
	if (PlayerCount > 2)
		return;
	CTank *Tank = new CTank(Map, this, NULL);
	Tank->Init(PlayerCount==0?&COLOR_P1:&COLOR_P2, Map->GetNewTankLocation());
	if (PlayerCount == 0)
		Tank->SetPlayerControls(P1_LEFT, P1_RIGHT, P1_UP, P1_DOWN, P1_FIRE);
	else
		Tank->SetPlayerControls(P2_LEFT, P2_RIGHT, P2_UP, P2_DOWN, P2_FIRE);
	AddObject(Tank);
	PlayerCount++;
	//Position = Map->GetNewTankLocation(this);
	//	color = Host->GetNewTankColor(this);

}

void CTankManager::AddAI()
{
	/// NOAUI
	return;
}

bool CTankManager::Render()
{
	CEngine::Instance()->FontManager->Print(10, 400, 0.0f, "haha");
	return true;
}

bool CTankManager::Update(scalar dt)
{
	CTank* T1 = NULL, *T2 = NULL;
// 	if (first && first->Data)
// 		T1 = dynamic_cast<CTank*>(first->GetData());
// 	if (first && first->next && first->next->GetData())
// 		T2 = dynamic_cast<CTank*>(first->next->GetData());
// 	if (CAABB(T1->Position.x, T1->Position.y, T1->Position.x + 32, T1->Position.y + 32).Intersect(CAABB(T2->Position.x, T2->Position.y, T2->Position.x + 32, T2->Position.y + 32)))
// 	{
// 		Log("A", "Intersectuion");
// 		T1->Position += T1->Direction*(-T1->Velocity*0.5);
// 		T2->Position += T1->Direction*(T1->Velocity*0.5);
// 	}
	return true;
}

Vector2 CTankMap::GetNewTankLocation()
{
	for (int i=0;i<MAP_SIZE_X;i++)
		for (int j=0;j<MAP_SIZE_Y;j++)
		{
			if (Cells[j][i] == csFree)
				return Vector2(i*CellSize, j*CellSize);
		}

}

bool CTankMap::Render()
{
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	CellSize = DEFAULT_CELL_SIZE;
	for (int i=0;i<MAP_SIZE_X;i++)
		for (int j=0;j<MAP_SIZE_Y;j++)
		{
			if (Cells[j][i] == csBlock || Cells[j][i] == csDestr || Cells[j][i] == csFree)
			{
				if (Cells[j][i] == csBlock)
					BlockTexture->Bind();
				if (Cells[j][i] == csDestr)
					DestrTexture->Bind();
				if (Cells[j][i] == csFree)
					FreeTexture->Bind();
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					(Vector2(i, j)*CellSize).glVertex();
					//glVertex2f(i*CellSize, j*CellSize);

					glTexCoord2f(1.0f, 0.0f);
					glVertex2f((i+1)*CellSize, j*CellSize);

					glTexCoord2f(1.0f, 1.0f);
					glVertex2f((i+1)*CellSize, (j+1)*CellSize);

					glTexCoord2f(0.0f, 1.0f);
					glVertex2f(i*CellSize, (j+1)*CellSize);
				glEnd();
			}
		}
	glDisable(GL_TEXTURE_2D);
	return true;
}

bool CTankMap::Update(scalar dt)
{
	return true;
}

CAABB CTankMap::GetCellAABB(Vector2& V)
{
	return CAABB(      (((int)V.x/(int)CellSize)) * CellSize, (((int)V.y/(int)CellSize)) * CellSize, 
		CellSize, CellSize);
}

scalar CTankMap::IsFPTWA(int ADir, Vector2 Position)
{
	return 0;
}