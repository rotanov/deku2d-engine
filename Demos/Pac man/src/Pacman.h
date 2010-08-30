#ifndef	_PACMAN_H_
#define	_PACMAN_H_

#include "2de_Engine.h"
#include "2de_GameUtils.h"

const int LEVEL_WIDTH = 32;
const int LEVEL_HEIGHT = 16;
const float DEFAULT_VELOCITY = 100.0f;
const Vector2 DEFAULT_POSITION = Vector2(96.0f, 96.0f);

class CPacmanPlayer : public CUpdatable
{
public:
	size_t Score;
	size_t Damage;
	Vector2 Position;
	CSprite *Sprite;
	CText *ScoreText;
	CText *DamageText;
	Vector2 Velocity;
	CPacmanPlayer();

	void Update(float dt);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
};

class CPacmanBonus : public CUpdatable
{
public:
	CPacmanPlayer *Player;
	Vector2 Position;
	float Period;
	float Angle;
	CPacmanBonus(Vector2 APosition, CSprite *ASprite);
	~CPacmanBonus();
	void Update(float dt);
private:
	CRenderProxy *RenderProxy;
	CSprite *Sprite;
};

enum EDirection
{
	DIRECTION_LEFT,
	DIRECTION_DOWN,
	DIRECTION_RIGHT,
	DIRECTION_UP,
};

class CPacmanEnemy : public CUpdatable
{
public:
	CLevelMap *Map;
	Vector2 Position;
	CPacmanPlayer *Player;
	CSprite *Sprite;
	EDirection Direction;

	CPacmanEnemy(Vector2 APosition);
	void Update(float dt);
};

class CPacmanGame :  public CUpdatable
{
public:
	CLevelMap *Map;
	CTileset *Tiles;
	CPacmanPlayer *Player;
	CSprite *EnemySprite;
	CSprite *BonusSprite;
	CPacmanGame(CPacmanPlayer *APlayer);

	void Update(float dt);
};

#endif	//	_PACMAN_H_
