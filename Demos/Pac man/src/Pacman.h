#ifndef	_PACMAN_H_
#define	_PACMAN_H_

#include "2de_Engine.h"
#include "2de_GameUtils.h"

const float DEF_VEL = 100.0f;

class CPacmanPlayer : public CRenderObject, public CUpdateObject
{
public:
	CSprite Sprite;
	Vector2 Velocity;
	CPacmanPlayer() : Velocity(V2_ZERO)
	{
		Position = Vector2(96.0f, 96.0f);
		Sprite.SetTexture("PacmanFrames");
		Sprite.AddAnimation(true, 50, 32, 32, 4, 2, 7, 32, 32, 0, 0, 0, true);
		Sprite.AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 0, 0, 1, false);
		Sprite.SetAngle(90.0f);
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
	}
	bool Render()
	{
		return true;
	}
	bool Update(float dt)
	{
		Sprite.SetAnimation(0);
		if (Velocity.Length() < 10.0f)
			Sprite.SetAnimation(1);
		Position += Velocity * dt;
		Velocity *= 0.999f;
		Sprite.Position = Position;
		return true;
	}
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		if (state == KEY_PRESSED)
		{
			switch(key)
			{
			case SDLK_LEFT:
				Velocity.x = -DEF_VEL;
				Sprite.SetAngle(0.0f);
				Sprite.mirror_h = true;
				break;
			case SDLK_RIGHT:
				Velocity.x = DEF_VEL;
				Sprite.SetAngle(0.0f);
				Sprite.mirror_h = false;
				break;
			case SDLK_UP:
				Velocity.y = DEF_VEL;
				Sprite.SetAngle(90.0f);
				Sprite.mirror_h = false;
				break;
			case SDLK_DOWN:
				Velocity.y = -DEF_VEL;
				Sprite.SetAngle(90.0f);
				Sprite.mirror_h = true;
				break;

			}
		}
		return true;
	}

};

class CPakmanGame : public CRenderObject, public CUpdateObject
{
public:
	CLevelMap *Map;
	CTileset *Tiles;
	CPacmanPlayer *Player;
	
	CPakmanGame()
	{
		Player = NULL;
		Tiles = CFactory::Instance()->Get<CTileset>("PacManTileset");
		Tiles->CheckLoad();
		Map = new CLevelMap(10, 10, "PacManTileset", "Pacman map");
		//	Map.TileSet = Tiles;
		for(int i = 0; i < 10; i++)
			for(int j = 0; j < 10; j++)
				if ((i*j == 0 || i == 9 || j == 9))
					Map->Cells[Map->_Cell(i, j)].index = 1;
				else
					Map->Cells[Map->_Cell(i, j)].index = rand() % 100 < 25 ? 1 : 0;

		Map->GenCells();
		CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("PacMan"));
		
	}

	void SetPlayer(CPacmanPlayer *APlayer)
	{
		Player = APlayer;
	}
	bool Render()
	{
		glLoadIdentity();
		//glClearColor(0.3f, 0.4f, 0.3f, 1.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		COLOR_WHITE.glSet();
		gSetBlendingMode();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glEnable(GL_TEXTURE_2D);
		Tiles->GetTexture()->Bind();
		glBegin(GL_QUADS);
		for(int j = 0; j < 10; j++)
			for(int i = 0; i < 10; i++)
			{
				(Tiles->GetCellTC(Map->Cells[Map->_Cell(j, i)].index)[0]).glTexCoord();
				(Vector2(i, j)*64).glVertex();
				Tiles->GetCellTC(Map->Cells[Map->_Cell(j, i)].index)[1].glTexCoord();
				(Vector2(i+1, j)*64).glVertex();
				Tiles->GetCellTC(Map->Cells[Map->_Cell(j, i)].index)[2].glTexCoord();
				(Vector2(i+1, j+1)*64).glVertex();
				Tiles->GetCellTC(Map->Cells[Map->_Cell(j, i)].index)[3].glTexCoord();
				(Vector2(i, j+1)*64).glVertex();
			}
			glEnd();

			if (Player)
			{
				glDisable(GL_TEXTURE_2D);
				CAABB Someaabb = GetCellAABB(Player->Position);
				glBegin(GL_LINES);
				Someaabb.vMin.glVertex();
				Someaabb.vMax.glVertex();
				glEnd();

				CAABB AABB = CAABB(Player->Position - Vector2(16, 16), Player->Position + Vector2(16, 16));

				Vector2 BottomLeft = AABB.vMin,
						TopRight = AABB.vMax,
						BottomRight = Vector2(AABB.vMax.x, AABB.vMin.y),
						TopLeft = Vector2(AABB.vMin.x, AABB.vMax.y);
				CAABB Pot1, Pot2;

				int Direction = -10;
				if (Abs(Player->Velocity.x) > Abs(Player->Velocity.y))
				{
					if (Player->Velocity.x >= 0.0f)
						Direction = 1;
					else
						Direction = 0;
				}
				else
				{
					if (Player->Velocity.y >= 0)
						Direction = 2;
					else
						Direction = 3;

				}

	switch(Direction)
	{
	case 0: // left
		Pot1 = GetCellAABB(TopLeft);
		Pot2 = GetCellAABB(BottomLeft);
		if (Pot1.Intersect(AABB))
			Player->Position.x = Pot1.vMax.x + 17;
		if (Pot2.Intersect(AABB))
			Player->Position.x = Pot2.vMax.x + 17;
		break;
	case 1:	// right
		Pot1 = GetCellAABB(TopRight);
		Pot2 = GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			Player->Position.x = Pot1.vMin.x - 17;
		if (Pot2.Intersect(AABB))
			Player->Position.x = Pot2.vMin.x - 17;
		break;
	case 2: // up
		Pot1 = GetCellAABB(TopLeft);
		Pot2 = GetCellAABB(TopRight);
		if (Pot1.Intersect(AABB))
			Player->Position.y = Pot1.vMin.y - 17;
		if (Pot2.Intersect(AABB))
			Player->Position.y = Pot2.vMin.y - 17;
		break;
	case 3: // down
		Pot1 = GetCellAABB(BottomLeft);
		Pot2 = GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
			Player->Position.y = Pot1.vMax.y + 17;
		if (Pot2.Intersect(AABB))
			Player->Position.y = Pot2.vMax.y + 17;
		break;
	}



			}

			return true;
	}

	bool Update(float dt)
	{
		if (Player == NULL)
			return true;

		return true;
	}

	CAABB GetCellAABB(Vector2 V)
	{
			
		if (Map->Cells[Map->_Cell((int)V.y/(int)64, (int)V.x/(int)64)].index != 0)
			return CAABB(      (((int)V.x/(int)64)) * 64, (((int)V.y/(int)64)) * 64, 
			64, 64);
		else 
			return CAABB(-1, -1, -1, -1);
	}

// 	CTankMapCell* GetCell(Vector2& V)
// 	{
// 		return &(Cells[(int)V.y/(int)CellSize][(int)V.x/(int)CellSize]);
// 	}

};

#endif	//	_PACMAN_H_