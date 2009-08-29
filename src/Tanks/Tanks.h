#ifndef _TANKS_H_
#define _TANKS_H_

#include "Engine.h"

class CTank;
class CTankMap;
class CTankManager;
class CTankAI;

enum EActionKind {akLeft=0, akRight=1, akUp=2, akDown=3, akFire=4, akItem=5};
enum ETanksTileIndex {csFree1=0, csFree2=1, csFree3=2, csFree4=3, csBlock=4, csDestr=5, csTank=6, csBullet=7};

const Vector2	Directions[4] = {DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN};
const scalar	DEFAULT_TANK_HEALTH = 100;
const scalar	DEFAULT_TANK_DAMAGE = 10;
const scalar	DEFAULT_TANK_VELOCITY = 3;
const int		MAX_PLAYERS_COUNT = 2;
const int		MAP_SIZE_X = 20;
const int		MAP_SIZE_Y = 15;
const int		DEFAULT_CELL_SIZE = 32;
const int		CONTROLS_COUNT = 6;
const int		MAX_BULLETS_PER_TANK = 128;
const scalar	DEFAULT_FIRING_INTERVAL = 0.2f;
static RGBAf	COLOR_P1 = RGBAf(0.5f, 0.8f, 0.4f, 1.0f);
static RGBAf	COLOR_P2 = RGBAf(0.9f, 0.9f, 0.4f, 1.0f);
static RGBAf	COLOR_AI = RGBAf(0.9f, 0.5f, 0.4f, 1.0f);

static SDLKey K_LEFT[MAX_PLAYERS_COUNT]	= {SDLK_LEFT,	SDLK_a};
static SDLKey K_RIGHT[MAX_PLAYERS_COUNT]	= {SDLK_RIGHT,	SDLK_d};
static SDLKey K_UP[MAX_PLAYERS_COUNT]	= {SDLK_UP,		SDLK_w};
static SDLKey K_DOWN[MAX_PLAYERS_COUNT]	= {SDLK_DOWN,	SDLK_s};
static SDLKey K_FIRE[MAX_PLAYERS_COUNT]	= {SDLK_RCTRL,	SDLK_f};
static SDLKey K_ITEM[MAX_PLAYERS_COUNT]	= {SDLK_RSHIFT,	SDLK_c};
static const SDLKey *AR_K_CONTRLS[CONTROLS_COUNT] = {K_LEFT, K_RIGHT, K_UP, K_DOWN, K_FIRE, K_ITEM};

#define TANK_TEXTURE_NAME  "Tank3"
#define BULLET_TEXTURE_NAME  "Fire"
#define DEFAULT_BLOCK_TEXTURE "Block"
#define DEFAULT_DESTR_TEXTURE "Destr"
#define DEFAULT_FREE_TEXTURE "Free"

__INLINE int Dir2AK(Vector2& V)
{
	if (V == DIR_LEFT)
		return  akLeft;
	if (V == DIR_RIGHT)
		return  akRight;
	if (V == DIR_UP)
		return  akUp;
	if (V == DIR_DOWN)
		return  akDown;
}

class CTank : public CRenderObject, public CUpdateObject
{
public:
	Vector2 Direction;
	int  Health;
	scalar Velocity;
	scalar Damage;
	//////////////////////////////////////////////////////////////////////////
	CTankMap *Map;
	CTankManager* Host;
	CTankAI* AI;
	CTexture* Texture;
	CTexture* BulletTexture;
	RGBAf* Color;
	//////////////////////////////////////////////////////////////////////////
	bool States[CONTROLS_COUNT];
	bool isAlive;
	bool isFiring;
	bool isWalking;
	SDLKey Controls[6];
	//////////////////////////////////////////////////////////////////////////
	struct  
	{
		Vector2 p, v;
	}Bullets[MAX_BULLETS_PER_TANK];
	
	int BulletsCount;
	scalar FiringInterval;
	scalar FiringTimeout;

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
		Health = DEFAULT_TANK_HEALTH;
		Velocity = DEFAULT_TANK_VELOCITY;
		Damage = DEFAULT_TANK_DAMAGE;
		int RandomDirection = Random_Int(0, 3);
		Direction = Directions[RandomDirection];
		Texture = CEngine::Instance()->TextureManager->GetTextureByName(TANK_TEXTURE_NAME);  // No! Try to ask host for texture for instance of tank
		BulletTexture = CEngine::Instance()->TextureManager->GetTextureByName(BULLET_TEXTURE_NAME); 

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
	bool Update(scalar dt);
	bool Render();
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
						Cells[j][i].TileIndex = csFree1;
				}
			}
	}

	CTankMapCell* GetCell(Vector2& V);
	CAABB GetCellAABB(Vector2& V);
	scalar IsFPTWA(int ADir, Vector2 Position);
	Vector2 GetNewTankLocation();
	bool Render();
	bool Update(scalar dt);
};

class CTankManager : public CList, public CUpdateObject, public CRenderObject
{
public:	
	CTankMap *Map;
	int PlayerCount;
	CTankManager() : PlayerCount(0)
	{


		CEngine::Instance()->RenderManager.AddObject(this);
		CEngine::Instance()->UpdateManager.AddObject(this);
		Map = new CTankMap(this);
		AddPlayer();
		AddPlayer();
		//AddAI();
	}

	CTank* GetPlayer(int PlayerIndex);
	bool Render();
	bool Update(scalar dt);
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