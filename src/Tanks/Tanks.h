#ifndef _TANKS_H_
#define _TANKS_H_

#include "Engine.h"

class CTank;
class CTankMap;
class CTankManager;
class CTankAI;

const int ACTION_LEFT	= 0;
const int ACTION_RIGHT	= 1;
const int ACTION_UP		= 2;
const int ACTION_DOWN	= 3;
const int ACTION_FIRE	= 4;
const int ACTION_ITEM	= 5;

const SDLKey P1_LEFT	= SDLK_LEFT;
const SDLKey P1_RIGHT	= SDLK_RIGHT;
const SDLKey P1_UP		= SDLK_UP;
const SDLKey P1_DOWN	= SDLK_DOWN;
const SDLKey P1_FIRE	= SDLK_LCTRL;

const SDLKey P2_LEFT	= SDLK_a;
const SDLKey P2_RIGHT	= SDLK_d;
const SDLKey P2_UP		= SDLK_w;
const SDLKey P2_DOWN	= SDLK_s;
const SDLKey P2_FIRE	= SDLK_f;


const Vector2 TANK_DIR_LEFT		= Vector2(-1.0f, 0.0f);
const Vector2 TANK_DIR_RIGHT	= Vector2(1.0f, 0.0f);
const Vector2 TANK_DIR_UP		= Vector2(0.0f, 1.0f);
const Vector2 TANK_DIR_DOWN		= Vector2(0.0f, -1.0f);
const Vector2 Directions[4] = {TANK_DIR_LEFT, TANK_DIR_RIGHT, TANK_DIR_UP, TANK_DIR_DOWN};
const scalar DEFAULT_TANK_HEALTH = 100;
const scalar DEFAULT_TANK_DAMAGE = 10;
const scalar DEFAULT_TANK_VELOCITY = 5;
#define TANK_TEXTURE_NAME  "Tank3" // Note, case sensitive
#define BULLET_TEXTURE_NAME  "Bullet"

__INLINE int SelDir(Vector2& V)
{
	if (V == TANK_DIR_LEFT)
		return  ACTION_LEFT;
	if (V == TANK_DIR_RIGHT)
		return  ACTION_RIGHT;
	if (V == TANK_DIR_UP)
		return  ACTION_UP;
	if (V == TANK_DIR_DOWN)
		return  ACTION_DOWN;
}

const int MAP_SIZE_X = 20;
const int MAP_SIZE_Y = 15;
const int DEFAULT_CELL_SIZE = 32;

enum TankMapCellState {csFree=0, csTank=1, csBlock=2, csDestr=3};

#define DEFAULT_BLOCK_TEXTURE "Block"
#define DEFAULT_DESTR_TEXTURE "Destr"
#define DEFAULT_FREE_TEXTURE "Free"


class CTank : public CRenderObject, public CUpdateObject
{
public:
	scalar Health;
	scalar Velocity;
	scalar Damage;
	Vector2 Direction;
	CTankMap *Map;
	CTankManager* Host;
	CTankAI* AI;
	CTexture* Texture;
	RGBAf* Color;
//	ETankState state;
	bool States[5]; // left right up down
	bool isAlive;
	bool isFiring;
	bool isWalking;
	CTexture* BulletTexture;
	SDLKey Controls[5];
	CTank(CTankMap* AMap, CTankManager* AHost, CTankAI* AAI) : Map(AMap), Host(AHost), AI(AAI)
	{
		isWalking = false;
		CEngine::Instance()->RenderManager.AddObject(this);
		CEngine::Instance()->UpdateManager.AddObject(this);
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
		isAlive = true;
		isFiring = false;			
		for (int i=0;i<5;i++)
		{
			Controls[i] = SDLKey(0);
			States[i] = false;
		}
//		state = tsIdle;		
		Health = DEFAULT_TANK_HEALTH;
		Velocity = DEFAULT_TANK_VELOCITY;
		Damage = DEFAULT_TANK_DAMAGE;
		int RandomDirection = Random_Int(0, 3);
		Direction = Directions[RandomDirection];
		Texture = CEngine::Instance()->TextureManager->GetTextureByName(TANK_TEXTURE_NAME);  // No! Try to ask host for texture for instance of tank
		BulletTexture = CEngine::Instance()->TextureManager->GetTextureByName(BULLET_TEXTURE_NAME); 
	}
	void Init(RGBAf* AColor, Vector2 APosition)
	{
		Color = AColor;
		Position = APosition;
	}
	CAABB GetAABB();
	Vector2 GetCenter();
	void SetPlayerControls(SDLKey ALeft, SDLKey ARight, SDLKey AUp, SDLKey ADown, SDLKey AFire);
	bool Update(scalar dt);
	bool Render();
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
};

class CTankMap : public CRenderObject, public CUpdateObject
{
public:
	TankMapCellState Cells[MAP_SIZE_Y][MAP_SIZE_X];
	int CellSize;
	CTankManager* Host;
	CTexture *BlockTexture;
	CTexture *FreeTexture;
	CTexture *DestrTexture;
	CTankMap(CTankManager* AHost) : Host(AHost)
	{
		CEngine::Instance()->RenderManager.AddObject(this);
		CEngine::Instance()->UpdateManager.AddObject(this);
		BlockTexture = CEngine::Instance()->TextureManager->GetTextureByName(DEFAULT_BLOCK_TEXTURE);
		DestrTexture = CEngine::Instance()->TextureManager->GetTextureByName(DEFAULT_DESTR_TEXTURE);
		FreeTexture = CEngine::Instance()->TextureManager->GetTextureByName(DEFAULT_FREE_TEXTURE);

		CellSize = DEFAULT_CELL_SIZE;
		for (int i=0;i<MAP_SIZE_X;i++)
			for (int j=0;j<MAP_SIZE_Y;j++)
			{
				if (i==0 || i == MAP_SIZE_X-1 || j==0 || j== MAP_SIZE_Y-1)
					Cells[j][i] = csBlock;
				else
				{
					Cells[j][i] = csFree;
					if (j%2 == 0 && i%2 == 0)
						Cells[j][i] = TankMapCellState(Random_Int(0, 3));
				}
			}
	}

	CAABB GetCellAABB(Vector2& V);
	scalar IsFPTWA(int ADir, Vector2 Position);
	Vector2 GetNewTankLocation();
	bool Render();
	bool Update(scalar dt);
};

class CTankManager : public CList, public CUpdateObject, public CRenderObject
{
public:	
	RGBAf COLOR_P1;
	RGBAf COLOR_P2;

	CTankMap *Map;
	int PlayerCount;
	CTankManager() : PlayerCount(0)
	{
		COLOR_P1 = RGBAf(0.8f, 0.5f, 0.4f, 1.0f);
		COLOR_P2 = RGBAf(0.4f, 0.5f, 0.8f, 1.0f);

		CEngine::Instance()->RenderManager.AddObject(this);
		CEngine::Instance()->UpdateManager.AddObject(this);
		Map = new CTankMap(this);
		AddPlayer();	// First
		AddPlayer();	// Second
	}

	bool Render();
	bool Update(scalar dt);
	void AddPlayer();
	void AddAI();
};

class CTankAI
{
public:
		// Nothing
};


#endif _TANKS_H_