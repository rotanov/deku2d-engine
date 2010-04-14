#ifndef _TANKS_H_
#define _TANKS_H_

#include "2de_Engine.h"



class CTank;
class CTankMap;
class CTankManager;
class CTankAI;

enum EActionKind {akLeft=0, akRight=1, akUp=2, akDown=3, akFire=4, akItem=5};
enum ETanksTileIndex {csFree1=7, csFree2=6, csFree3=3, csFree4=2, csBlock=5, csDestr=1, csTank=4, csBullet=0};

const Vector2	Directions[4] = {V2_DIR_LEFT, V2_DIR_RIGHT, V2_DIR_UP, V2_DIR_DOWN};
const float	DEFAULT_TANK_HEALTH = 100;
const float	DEFAULT_TANK_DAMAGE = 10;
const float	DEFAULT_TANK_VELOCITY = 3;
const int		MAX_PLAYERS_COUNT = 2;
const int		MAP_SIZE_X = 20;
const int		MAP_SIZE_Y = 15;
const int		DEFAULT_CELL_SIZE = 32;
const int		CONTROLS_COUNT = 6;
const int		MAX_BULLETS_PER_TANK = 128;
const float	DEFAULT_FIRING_INTERVAL = 0.2f;

static RGBAf	COLOR_P1 = RGBAf(0.5f, 0.8f, 0.4f, 1.0f);
static RGBAf	COLOR_P2 = RGBAf(0.9f, 0.9f, 0.4f, 1.0f);
static RGBAf	COLOR_AI = RGBAf(0.9f, 0.5f, 0.4f, 1.0f);

static SDLKey K_LEFT[MAX_PLAYERS_COUNT]		= {SDLK_LEFT,	SDLK_a};
static SDLKey K_RIGHT[MAX_PLAYERS_COUNT]	= {SDLK_RIGHT,	SDLK_d};
static SDLKey K_UP[MAX_PLAYERS_COUNT]		= {SDLK_UP,		SDLK_w};
static SDLKey K_DOWN[MAX_PLAYERS_COUNT]		= {SDLK_DOWN,	SDLK_s};
static SDLKey K_FIRE[MAX_PLAYERS_COUNT]		= {SDLK_RCTRL,	SDLK_f};
static SDLKey K_ITEM[MAX_PLAYERS_COUNT]		= {SDLK_RSHIFT,	SDLK_c};
static const SDLKey *AR_K_CONTRLS[CONTROLS_COUNT] = {K_LEFT, K_RIGHT, K_UP, K_DOWN, K_FIRE, K_ITEM};

__INLINE int Dir2AK(Vector2& V) // Direction to action kind
{
	for (int i=0;i<4;i++)
		if (V == Directions[i])
			return EActionKind(i);
}

class CTank : public CRenderObject, public CUpdateObject
{
public:
	struct Bullet {
		Vector2 p, v;
	};
	Vector2			Direction;
	int				Health;
	float			Velocity;
	float			Damage;
	//////////////////////////////////////////////////////////////////////////
	CTankMap*		Map;
	CTankManager*	Host;
	CTankAI*		AI;
	CTileset*		Tileset;
	RGBAf*			Color;
	//////////////////////////////////////////////////////////////////////////
	bool			States[CONTROLS_COUNT];
	bool			isAlive;
	bool			isFiring;
	bool			isWalking;
	SDLKey			Controls[6];
	//////////////////////////////////////////////////////////////////////////
	Bullet			Bullets[MAX_BULLETS_PER_TANK];
	
	int				BulletsCount;
	float			FiringInterval;
	float			FiringTimeout;

	CTank(CTankMap* AMap, CTankManager* AHost, CTankAI* AAI) : Map(AMap), Host(AHost), AI(AAI)
	{
		isWalking = false;
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
		Tileset = CTileSetManager::Instance()->GetTileset("TanksTileset");
		isAlive = true;
		isFiring = false;			
		for (int i=0;i<5;i++)
		{
			Controls[i] = SDLKey(0);
			States[i] = false;
		}
		Health = DEFAULT_TANK_HEALTH;
		Velocity = DEFAULT_TANK_VELOCITY;
		Damage = DEFAULT_TANK_DAMAGE;
		int RandomDirection = Random_Int(0, 3);
		Direction = Directions[RandomDirection];

		BulletsCount = 0;

		FiringInterval = DEFAULT_FIRING_INTERVAL;
		FiringTimeout = 0.0f;
	}
	void Init(RGBAf* AColor, Vector2 APosition)
	{
		Color = AColor;
		Position = APosition;
	}
	CAABB GetAABB();
	Vector2 GetCenter();
	void SetPlayerControls(int PlayerIndex);
	void Update(float dt);
	void Render();
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
};

class CTankMapCell
{
public:
	ETanksTileIndex TileIndex;
	bool isFree()
	{
		return (TileIndex != csBlock && TileIndex != csDestr);
	}
};

class CTankMap : public CRenderObject, public CUpdateObject
{
public:
	CTankMapCell Cells[MAP_SIZE_Y][MAP_SIZE_X];
	int CellSize;
	CTankManager* Host;
	CTileset* Tileset;
	CTankMap(CTankManager* AHost) : Host(AHost)
	{
		Tileset = CTileSetManager::Instance()->GetTileset("TanksTileset");

		CellSize = DEFAULT_CELL_SIZE;
		for (int i=0;i<MAP_SIZE_X;i++)
			for (int j=0;j<MAP_SIZE_Y;j++)
			{
				if (i==0 || i == MAP_SIZE_X-1 || j==0 || j== MAP_SIZE_Y-1)
					Cells[j][i].TileIndex = csBlock;
				else
				{	
					int prob = ETanksTileIndex(Random_Int(0, 10));
					if (prob == 0)
						Cells[j][i].TileIndex = csBlock;
					else
					if (prob == 1)
						Cells[j][i].TileIndex = csDestr;
					else
					{
						int freeind = Random_Int(0,20);
						if (freeind == 0)
							Cells[j][i].TileIndex = csFree1; else
							if (freeind == 1)
								Cells[j][i].TileIndex = csFree4; else
								if (freeind == 2)
									Cells[j][i].TileIndex = csFree3; else
									
										Cells[j][i].TileIndex = csFree2; 

					}
				}
			}
	}

	CTankMapCell* GetCell(Vector2& V);
	CAABB GetCellAABB(Vector2 V);
	float IsFPTWA(int ADir, Vector2 Position);
	Vector2 GetNewTankLocation();
	void Render();
	void Update(float dt);
};

class CTankManager : public CList, public CUpdateObject, public CRenderObject
{
public:	
	CTankMap *Map;
	int PlayerCount;
	CTankManager() : PlayerCount(0)
	{
		Map = new CTankMap(this);
		AddPlayer();
		AddPlayer();
		//AddAI();
	}

	CTank* GetPlayer(int PlayerIndex);
	void Render();
	void Update(float dt);
	void AddPlayer();
	void AddAI();
};

class CTankAI
{
public:
	CTankManager *Host;
	CTankMap *Map;
	CTank* Tank;
	CTankAI(CTankManager *AHost, CTankMap *AMap, CTank *ATank):Host(AHost), Map(AMap), Tank(ATank){}
};


#endif _TANKS_H_
