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

class CPongPlayer : public CRenderObject, public CUpdateObject
{
	EPlayerKind PlayerKind;
public:
	Vector2 Velocity;
	Vector2 Acceleration;
	CPongPlayer(EPlayerKind APlayerKind) : PlayerKind(APlayerKind)
	{
		Acceleration = Velocity = V2_ZERO;
		int ScreenWidth;
		CEngine::Instance()->GetState(STATE_SCREEN_WIDTH, &ScreenWidth);
		switch (PlayerKind)
		{
		case PLAYER_KIND_ONE:
			position = Vector2(PONG_MARGIN, PONG_MARGIN);
			break;
		case PLAYER_KIND_TWO:
			position = Vector2(ScreenWidth - PONG_MARGIN - PONG_PLAYER_WIDTH, PONG_MARGIN);
			break;
		}
	}
	bool Render()
	{
		CPrimitiveRender pr;
		pr.sClr = RGBAf(1.0f, 1.0f , 1.0f, 1.0f);
		pr.doUseGlobalCoordSystem = false;
		pr.grRectS(Vector2(0, 0), Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
		return true;
	}
	bool Update(float dt)
	{
		if (PlayerKind == PLAYER_KIND_ONE)
		{
			if (CEngine::Instance()->keys[SDLK_UP])
				Acceleration.y = 800.0f;
			if (CEngine::Instance()->keys[SDLK_DOWN])
				Acceleration.y = -800.0f;
		}

		if (PlayerKind == PLAYER_KIND_TWO)
		{
			if (CEngine::Instance()->keys[SDLK_w])
				Acceleration.y = 200.0f;
			if (CEngine::Instance()->keys[SDLK_s])
				Acceleration.y = -200.0f;
		}

		if (Equal(Velocity.y , 0.0f))
			Velocity.y = 0;
		position += Velocity*dt;
		Velocity += Acceleration*dt;
		Velocity *= 0.999f;
		Acceleration = V2_ZERO;

		if (position.y < 0.0f)
		{
			position.y = 0.0f;
			Velocity.y = -Velocity.y * 0.5f;
		}

		if (position.y + PONG_PLAYER_HEIGHT > CGLWindow::Instance()->height)
		{
			position.y = CGLWindow::Instance()->height - PONG_PLAYER_HEIGHT;
			Velocity.y = -Velocity.y * 0.5f;
		}

		return true;	
	}
};

class CPongBall : public CRenderObject, public CUpdateObject
{
public:
	Vector2 Velocity;
	void Iinitialize()
	{
		Velocity = Vector2(Random_Float(-500.0f, 500.0f), Random_Float(-500.0f, 500.0f));
		position = Vector2(CGLWindow::Instance()->width * 0.5f, CGLWindow::Instance()->height * 0.5f);
	}
	CPongBall()
	{
		Iinitialize();
	}
	bool Render()
	{
		CPrimitiveRender pr;
		pr.sClr = RGBAf(1.0f, 1.0f , 1.0f, 1.0f);
		pr.doUseGlobalCoordSystem = false;
		pr.grCircleS((Vector2(0, 0)+ Vector2(PONG_BALL_SIZE, PONG_BALL_SIZE))*0.5f, PONG_BALL_SIZE * 0.5f);
		return true;
	}
	bool Update(float dt)
	{
		position += Velocity * dt;

		if (position.y < 0.0f)
		{
			position.y = 0.0f;
			Velocity.y = -Velocity.y * 0.5f;
		}

		if (position.y + PONG_BALL_SIZE > CGLWindow::Instance()->height)
		{
			position.y = CGLWindow::Instance()->height - PONG_BALL_SIZE;
			Velocity.y = -Velocity.y * 0.5f;
		}

		return true;
	}
};

class CPongGame : public CRenderObject, public CUpdateObject
{
public:
	int PlayerOneScore, PlayerTwoScore;
	CPongPlayer *PlayerOne, *PlayerTwo;
	CPongBall *Ball;
	CPongGame()
	{
		PlayerTwoScore = PlayerOneScore = 0;
		PlayerOne = new CPongPlayer(PLAYER_KIND_ONE);
		PlayerTwo = new CPongPlayer(PLAYER_KIND_TWO);
		Ball = new CPongBall;
		CEngine::Instance()->FontManager->SetCurrentFont("Font");
	}
	~CPongGame()
	{
		SAFE_DELETE(PlayerOne);
		SAFE_DELETE(PlayerTwo);
		SAFE_DELETE(Ball);
	}
	bool Render()
	{
		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		CEngine::Instance()->FontManager->PrintEx(10, 450, 0.0f, "Score: %d", PlayerOneScore);
		CEngine::Instance()->FontManager->PrintEx(1170, 450, 0.0f, "Score: %d", PlayerTwoScore);
		return true;
	}

	bool Update(float dt)
	{
		CAABB BallBox, PlayerOneBox, PlayerTwoBox, *BallCollidedWithThatBox = NULL;
		BallBox = CAABB(Ball->position, Ball->position + Vector2(PONG_BALL_SIZE, PONG_BALL_SIZE));
		PlayerOneBox = CAABB(PlayerOne->position, PlayerOne->position + Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
		PlayerTwoBox = CAABB(PlayerTwo->position, PlayerTwo->position + Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
		CPongPlayer *CollidedPlayer = NULL;

		float CollideFlagf = 0.0f;
		if (BallBox.Intersect(PlayerOneBox))
			CollidedPlayer = PlayerOne,
			BallCollidedWithThatBox = &PlayerOneBox,
			CollideFlagf = 0.5f;
		if (BallBox.Intersect(PlayerTwoBox))
			CollidedPlayer = PlayerTwo,
			BallCollidedWithThatBox = &PlayerTwoBox,
			CollideFlagf = -0.5f;

		if (CollideFlagf != 0.0f)
		{
			Ball->Velocity.x = -Ball->Velocity.x + CollideFlagf * abs(CollidedPlayer->Velocity.y);
			Ball->Velocity.y += abs(CollidedPlayer->Velocity.y)*0.01f;
		}

		if (Ball->position.x > 1100)
			PlayerTwo->Velocity.y += (- PlayerTwo->position.y - PONG_PLAYER_HEIGHT*0.5f + Ball->position.y + PONG_BALL_SIZE * 0.5f)*0.34f;

		if (CEngine::Instance()->keys[SDLK_SPACE])
			Ball->Iinitialize();		

		if (Ball->position.x < 0.0f)
		{
			Ball->Iinitialize();
			PlayerTwoScore++;
		}

		if (Ball->position.x + PONG_BALL_SIZE > CGLWindow::Instance()->width)
		{
			Ball->Iinitialize();
			PlayerOneScore++;
		}
		return true;
	}

};

//////////////////////////////////////////////////////////////////////////

class CPongTitleScreen : public CRenderObject, public CUpdateObject
{
	CPongTitleScreen()
	{

	}
	~CPongTitleScreen()
	{

	}
	bool Render()
	{
		return true;
	}
	bool Update(float dt)
	{
		return true;
	}
};


#endif //_2DE_PONG_H_