#include "Pong.h"

CPongPlayer::CPongPlayer(EPlayerKind APlayerKind) : PlayerKind(APlayerKind)
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

void CPongPlayer::Render()
{
	CRenderManager::Instance()->DrawSolidBox(this, CBox(Vector2(0, 0), Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT)));
	return;
}

void CPongPlayer::Update(float dt)
{
// 	int dx(0);
// 	int dy(0);
// 	SDL_GetRelativeMouseState(&dx, &dy);


	if (PlayerKind == PLAYER_KIND_ONE)
	{
		if (CEngine::Instance()->keys[SDLK_w])
			Acceleration.y = 3200.0f;
		if (CEngine::Instance()->keys[SDLK_s])
			Acceleration.y = -3200.0f;
		Acceleration.y += 8000.0f * dx;
	}

	if (PlayerKind == PLAYER_KIND_TWO)
	{
		
		if (CEngine::Instance()->keys[SDLK_UP])
			Acceleration.y = 3200.0f;
		if (CEngine::Instance()->keys[SDLK_DOWN])
			Acceleration.y = -3200.0f;
		Acceleration.y += 8000.0f * dy;
	}

	if (Equal(Velocity.y , 0.0f))
		Velocity.y = 0;
	Position += Velocity*dt;
	Velocity += Acceleration*dt;
	Velocity *= 0.9995f;
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

void CPongBall::Iinitialize()
{
	int Section = Random_Int(0, 1);
	float Angle = Section ? Random_Float(0, PI * 0.25f) : Random_Float(PI * 1.75f, PI * 2.0f);
	int Sign = Random_Int(0, 1);
	if (Sign) Angle = PI - Angle;
	Velocity = Vector2(cos(Angle), sin(Angle)) * BallSpeed;
	Position = Vector2(	CGLWindow::Instance()->GetWidth() * 0.5f,
		CGLWindow::Instance()->GetHeight() * 0.5f);
}

CPongBall::CPongBall()
{
	BallSpeed = 250.0f;
	Iinitialize();
}

void CPongBall::Render()
{
	Vector2 tempv = Vector2(PONG_BALL_SIZE, PONG_BALL_SIZE);
	CRenderManager::Instance()->DrawSolidBox(this, CBox(V2_ZERO, tempv));
	return;
}

void CPongBall::Update(float dt)
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

CPongGame::CPongGame()
{
	PlayerTwoScore = PlayerOneScore = 0;
	PlayerOne = new CPongPlayer(PLAYER_KIND_ONE);
	PlayerTwo = new CPongPlayer(PLAYER_KIND_TWO);

	CFactory::Instance()->Add(PlayerOne, "PlayerOne");
	CFactory::Instance()->Add(PlayerTwo, "PlayerTwo");

	Ball = CFactory::Instance()->New<CPongBall>("PongBall");

	//CFontManager::Instance()->SetCurrentFont("Font");
}

CPongGame::~CPongGame()
{
	//CFactory::Instance()->Destroy(PlayerOne); // don't need it..
	//CFactory::Instance()->Destroy(PlayerTwo);
	//CFactory::Instance()->Destroy(Ball);
	//PlayerOne->SetDestroyed();	// Not really
	//PlayerTwo->SetDestroyed();
	//Ball->SetDestroyed();
}

void CPongGame::Render()
{
	// NOTHING
	return;
}

void CPongGame::Update(float dt)
{
	int dx(0);
	int dy(0);
	SDL_GetRelativeMouseState(&dx, &dy);
	PlayerOne->dx = dx;
	PlayerOne->dy = dy;
	PlayerTwo->dx = dx;
	PlayerTwo->dy = dy;

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

// 	if (Ball->Position.x > CGLWindow::Instance()->GetWidth() - PONG_PLAYER_WIDTH - 50)
// 		PlayerTwo->Velocity.y += (- PlayerTwo->Position.y - PONG_PLAYER_HEIGHT*0.5f + Ball->Position.y + PONG_BALL_SIZE * 0.5f)*0.34f;

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
	Ball->BallSpeed = 250.0f + (PlayerOneScore + PlayerTwoScore) * 20.0f;
	return;
}
