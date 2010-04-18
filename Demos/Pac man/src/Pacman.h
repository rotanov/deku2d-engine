#ifndef	_PACMAN_H_
#define	_PACMAN_H_

#include "2de_Engine.h"
#include "2de_GameUtils.h"

const size_t LEVEL_WIDTH = 32;
const size_t LEVEL_HEIGHT = 16;
const float DEFAULT_VELOCITY = 100.0f;
const Vector2 DEFAULT_POSITION = Vector2(96.0f, 96.0f);

class CPacmanPlayer : public CUpdateObject
{
public:
	size_t Score;
	Vector2 Position;
	CSprite Sprite;
	CText ScoreText;
	Vector2 Velocity;
	CPacmanPlayer();

	void Update(float dt);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
};

class CPacmanBonus : public CUpdateObject
{
public:
	CPacmanPlayer *Player;
	CSprite Sprite;
	Vector2 Position;
	float Period;
	float Angle;
	CPacmanBonus(Vector2 APosition);
	void Update(float dt);
};

class CPakmanGame :  public CUpdateObject
{
public:
	CLevelMap *Map;
	CTileset *Tiles;
	CPacmanPlayer *Player;
	
	CPakmanGame(CPacmanPlayer *APlayer);
	void Update(float dt);
};

#endif	//	_PACMAN_H_