#ifndef	_PACMAN_H_
#define	_PACMAN_H_

#include "2de_Engine.h"
#include "2de_GameUtils.h"

const size_t LEVEL_WIDTH = 32;
const size_t LEVEL_HEIGHT = 16;
const float DEFAULT_VELOCITY = 100.0f;
const Vector2 DEFAULT_POSITION = Vector2(96.0f, 96.0f);

class CPacmanBonus : public CUpdateObject
{
public:
	CSprite Sprite;
	Vector2 Position;
	float Period;
	float Angle;
	CPacmanBonus(Vector2 APosition) : Position(APosition), Angle(rand() % 360)
	{
		Sprite.Position = Vector2(ceil(Position.x), ceil(Position.y));
		Sprite.SetTexture("PacmanBonus");
		Sprite.AddAnimation(true, 50, 32, 32, 4, 2, 6, 32, 32, 0, 0, 0, true);
	}
	bool Update(float dt)
	{
		Period = sin(DegToRad(Angle)) * 16.0f;
		Angle += 10.0f;
		if (Angle > 360.0f)
			Angle = 0.0f;
		Sprite.Position = Vector2(ceil(Position.x), ceil(Position.y + Period));
		return true;
	}
};

class CPacmanPlayer : public CUpdateObject
{
public:
	Vector2 Position;
	CSprite Sprite;
	Vector2 Velocity;
	CPacmanPlayer() : Velocity(V2_ZERO)
	{
		SetName("Pac-man player");
		Position = DEFAULT_POSITION;
		Sprite.SetTexture("PacmanFrames");
		Sprite.AddAnimation(true, 50, 32, 32, 4, 2, 7, 32, 32, 0, 0, 0, true);
		Sprite.AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 0, 0, 1, false);
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
		CRenderManager::Instance()->Camera.Assign(&Position.x, &Position.y);
	}

	bool Update(float dt)
	{
		Sprite.SetAnimation(0);
		if (Velocity.Length() < 20.0f)
			Sprite.SetAnimation(1);
		Position += Velocity * dt;
		Velocity *= 0.998f;
		Sprite.Position = Vector2(ceil(Position.x), ceil(Position.y));
		return true;
		
	}
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		if (state == KEY_PRESSED)
		{
			switch(key)
			{
			case SDLK_LEFT:
				Velocity.x += -DEFAULT_VELOCITY;
				Velocity.y = 0.0f;
				Sprite.SetAngle(0.0f);
				Sprite.mirror_h = true;
				break;
			case SDLK_RIGHT:
				Velocity.x += DEFAULT_VELOCITY;
				Velocity.y = 0.0f;
				Sprite.SetAngle(0.0f);
				Sprite.mirror_h = false;
				break;
			case SDLK_UP:
				Velocity.y += DEFAULT_VELOCITY;
				Velocity.x = 0.0f;
				Sprite.SetAngle(90.0f);
				Sprite.mirror_h = false;
				break;
			case SDLK_DOWN:
				Velocity.y += -DEFAULT_VELOCITY;
				Velocity.x = 0.0f;
				Sprite.SetAngle(90.0f);
				Sprite.mirror_h = true;
				break;
			}
		}
		return true;
	}
};

class CPakmanGame :  public CUpdateObject
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
		Map = new CLevelMap(LEVEL_WIDTH, LEVEL_HEIGHT, "PacManTileset", "Pacman map");
		Map->Scaling = 2.0f;
		//	Map.TileSet = Tiles;
		for(int i = 0; i < LEVEL_WIDTH; i++)
			for(int j = 0; j < LEVEL_HEIGHT; j++)
				if ((i*j == 0 || i == LEVEL_WIDTH - 1 || j == LEVEL_HEIGHT - 1))
					Map->Cells[Map->_Cell(i, j)].index = 1;
				else
				{
					int Factor;
					Factor = rand() % 100;
					Map->Cells[Map->_Cell(i, j)].index = Factor < 25 ? 1 : 0;
					if (Factor >= 25)
						if (rand()%10 < 2)
							new CPacmanBonus(Vector2(i*64 + 32, j*64 + 32));
				}

		Map->GenCells();
		CSoundMixer::Instance()->SetMusicVolume(50);
		CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("PacMan"));

		
	}

	void SetPlayer(CPacmanPlayer *APlayer)
	{
		Player = APlayer;
	}

	bool Update(float dt)
	{
		if (Player == NULL)
			return true;

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
			{
				Player->Velocity.x = 0.0f;
				Player->Position.x = Pot1.vMax.x + 16;
			}
			if (Pot2.Intersect(AABB))
			{
				Player->Velocity.x = 0.0f;
				Player->Position.x = Pot2.vMax.x + 16;
			}
			break;
		case 1:	// right
			Pot1 = GetCellAABB(TopRight);
			Pot2 = GetCellAABB(BottomRight);
			if (Pot1.Intersect(AABB))
			{
				Player->Velocity.x = 0.0f;
				Player->Position.x = Pot1.vMin.x - 16;
			}
			if (Pot2.Intersect(AABB))
			{
				Player->Velocity.x = 0.0f;
				Player->Position.x = Pot2.vMin.x - 16;
			}
			break;
		case 2: // up
			Pot1 = GetCellAABB(TopLeft);
			Pot2 = GetCellAABB(TopRight);
			if (Pot1.Intersect(AABB))
			{
				Player->Velocity.y = 0.0f;
				Player->Position.y = Pot1.vMin.y - 16;
			}
			if (Pot2.Intersect(AABB))
			{
				Player->Velocity.y = 0.0f;
				Player->Position.y = Pot2.vMin.y - 16;
			}
			break;
		case 3: // down
			Pot1 = GetCellAABB(BottomLeft);
			Pot2 = GetCellAABB(BottomRight);
			if (Pot1.Intersect(AABB))
			{
				Player->Velocity.y = 0.0f;
				Player->Position.y = Pot1.vMax.y + 16;
			}
			if (Pot2.Intersect(AABB))
			{
				Player->Velocity.y = 0.0f;
				Player->Position.y = Pot2.vMax.y + 16;
			}
			break;
		}
		return true;
	}

	CAABB GetCellAABB(const Vector2 &V)
	{
			
		if (Map->Cells[Map->_Cell((int)V.x/(int)64, (int)V.y/(int)64 )].index != 0)
			return CAABB( (((int)V.x/(int)64)) * 64,
						  (((int)V.y/(int)64)) * 64, 
							64, 64);
		else 
			return CAABB(-1, -1, -1, -1);
	}
};

#endif	//	_PACMAN_H_