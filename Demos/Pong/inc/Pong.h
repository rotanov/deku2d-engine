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

	CPongPlayer(EPlayerKind APlayerKind) : PlayerKind(APlayerKind)
	{
		this->doIgnoreCamera = true;
		Acceleration = Velocity = V2_ZERO;
		int ScreenWidth;
		ScreenWidth = CGLWindow::Instance()->GetWidth();
		switch (PlayerKind)
		{
		case PLAYER_KIND_ONE:
			Position = Vector2(PONG_MARGIN, PONG_MARGIN);
			break;
		case PLAYER_KIND_TWO:
			Position = Vector2(ScreenWidth - PONG_MARGIN - PONG_PLAYER_WIDTH, PONG_MARGIN);
			break;
		}
	}

	void Render()
	{
		CRenderManager::Instance()->DrawSolidBox(this, CBox(Vector2(0, 0), Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT)));
		return;
	}
	void Update(float dt)
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
		Position += Velocity*dt;
		Velocity += Acceleration*dt;
		Velocity *= 0.999f;
		Acceleration = V2_ZERO;

		if (Position.y < 0.0f)
		{
			Position.y = 0.0f;
			Velocity.y = -Velocity.y * 0.5f;
		}

		if (Position.y + PONG_PLAYER_HEIGHT > CGLWindow::Instance()->GetHeight())
		{
			Position.y = CGLWindow::Instance()->GetHeight() - PONG_PLAYER_HEIGHT;
			Velocity.y = -Velocity.y * 0.5f;
		}

		return;
	}
};

class CPongBall : public CRenderable, public CUpdatable
{
public:
	Vector2 Velocity;
	void Iinitialize()
	{
		Velocity = Vector2(Random_Float(-500.0f, 500.0f), Random_Float(-500.0f, 500.0f));
		Position = Vector2(	CGLWindow::Instance()->GetWidth() * 0.5f,
							CGLWindow::Instance()->GetHeight() * 0.5f);
	}
	CPongBall()
	{
		Iinitialize();
	}
	void Render()
	{
		CPrimitiveRender pr;
		pr.doUseGlobalCoordSystem = false;
		pr.sClr = Color;
		pr.grCircleS((Vector2(0, 0)+ Vector2(PONG_BALL_SIZE, PONG_BALL_SIZE))*0.5f, PONG_BALL_SIZE * 0.5f);
		return;
	}
	void Update(float dt)
	{
		Position += Velocity * dt;

		if (Position.y < 0.0f)
		{
			Position.y = 0.0f;
			Velocity.y = -Velocity.y * 0.5f;
		}

		if (Position.y + PONG_BALL_SIZE > CGLWindow::Instance()->GetHeight())
		{
			Position.y = CGLWindow::Instance()->GetHeight() - PONG_BALL_SIZE;
			Velocity.y = -Velocity.y * 0.5f;
		}

		return;
	}
};

class CPongGame : public CRenderable, public CUpdatable
{
public:
	int PlayerOneScore, PlayerTwoScore;
	CPongPlayer *PlayerOne, *PlayerTwo;
	CText PlayerOneScoreText, PlayerTwoScoreText;
	CPongBall *Ball;
	CPongGame()
	{
		PlayerTwoScore = PlayerOneScore = 0;
		PlayerOne = new CPongPlayer(PLAYER_KIND_ONE);
		PlayerTwo = new CPongPlayer(PLAYER_KIND_TWO);

		CFactory::Instance()->Add(PlayerOne, "PlayerOne");
		CFactory::Instance()->Add(PlayerTwo, "PlayerTwo");

		Ball = CFactory::Instance()->New<CPongBall>("PongBall");

		CFontManager::Instance()->SetCurrentFont("Font");
	}
	~CPongGame()
	{
		//CFactory::Instance()->Destroy(PlayerOne); // don't need it..
		//CFactory::Instance()->Destroy(PlayerTwo);
		//CFactory::Instance()->Destroy(Ball);
		//PlayerOne->SetDestroyed();	// Not really
		//PlayerTwo->SetDestroyed();
		//Ball->SetDestroyed();
	}
	void Render()
	{
		// NOTHING
		return;
	}

	void Update(float dt)
	{
		PlayerOneScoreText.Position = Vector2(10.0f, 450.0f);
		PlayerOneScoreText.SetText("Score: " + itos(PlayerOneScore));
		PlayerTwoScoreText.Position = Vector2(CGLWindow::Instance()->GetWidth() - 100.0f, 450.0f);
		PlayerTwoScoreText.SetText("Score: " + itos(PlayerTwoScore));

		CBox BallBox, PlayerOneBox, PlayerTwoBox, *BallCollidedWithThatBox = NULL;
		BallBox = CBox(Ball->Position, Ball->Position + Vector2(PONG_BALL_SIZE, PONG_BALL_SIZE));
		PlayerOneBox = CBox(PlayerOne->Position, PlayerOne->Position + Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
		PlayerTwoBox = CBox(PlayerTwo->Position, PlayerTwo->Position + Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
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
			Ball->Velocity.x = -Ball->Velocity.x + CollideFlagf * Abs(CollidedPlayer->Velocity.y);
			Ball->Velocity.y += Abs(CollidedPlayer->Velocity.y)*0.01f;
		}

		if (Ball->Position.x > CGLWindow::Instance()->GetWidth() - PONG_PLAYER_WIDTH - 50)
			PlayerTwo->Velocity.y += (- PlayerTwo->Position.y - PONG_PLAYER_HEIGHT*0.5f + Ball->Position.y + PONG_BALL_SIZE * 0.5f)*0.34f;

		if (CEngine::Instance()->keys[SDLK_SPACE])
			Ball->Iinitialize();

		if (Ball->Position.x < 0.0f)
		{
			Ball->Iinitialize();
			PlayerTwoScore++;
		}

		if (Ball->Position.x + PONG_BALL_SIZE > CGLWindow::Instance()->GetWidth())
		{
			Ball->Iinitialize();
			PlayerOneScore++;
		}
		return;
	}

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
