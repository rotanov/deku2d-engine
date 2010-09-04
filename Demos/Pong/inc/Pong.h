#ifndef _2DE_PONG_H_
#define _2DE_PONG_H_

#include "2de_Engine.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"

#define PONG_MARGIN 10
#define PONG_PLAYER_WIDTH 24
#define PONG_PLAYER_HEIGHT 96
#define PONG_BALL_SIZE 24

enum EPlayerKind
{
	PLAYER_KIND_ONE,
	PLAYER_KIND_TWO,
	PLAYER_KIND_AI,
};

class CPongPlayer : public CRenderable, public CUpdatable
{
	EPlayerKind PlayerKind;
public:
	Vector2 Velocity;
	Vector2 Acceleration;
	int dx, dy;

	CPongPlayer(EPlayerKind APlayerKind);

	void Render();
	void Update(float dt);
};

class CPongBall : public CRenderable, public CUpdatable
{
public:
	float BallSpeed;
	Vector2 Velocity;
	void Iinitialize();
	CPongBall();
	void Render();
	void Update(float dt);
};

class CPongGame : public CRenderable, public CUpdatable
{
public:
	int PlayerOneScore, PlayerTwoScore;
	CPongPlayer *PlayerOne, *PlayerTwo;
	CText PlayerOneScoreText, PlayerTwoScoreText;
	CPongBall *Ball;
	CPongGame();
	~CPongGame();
	void Render();

	void Update(float dt);

};

//////////////////////////////////////////////////////////////////////////

class CPongTitleScreen : public CRenderable, public CUpdatable
{
	CPongTitleScreen()
	{

	}
	~CPongTitleScreen()
	{

	}
	void Render()
	{
		return;
	}
	void Update(float dt)
	{
		return;
	}
};


#endif //_2DE_PONG_H_
