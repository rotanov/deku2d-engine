#include "Pacman.h"
using namespace Deku2d;

#define NewObject(A) CFactory::Instance()->New<A>("")

CPacmanBonus::CPacmanBonus(Vector2 APosition, CSprite *ASprite) : Position(APosition),
	Angle(rand() % 360), RenderProxy(NULL), Sprite(ASprite)
{
	RenderProxy = new CRenderProxy(Sprite);
	//CFactory::Instance()->Add(RenderProxy);
	RenderProxy->SetPosition(Position);
	RenderProxy->SetLayer(1);
	SetName("Pacman bonus (mushroom)");
}

void CPacmanBonus::Update(float dt)
{
	Period = sin(DegToRad(Angle)) * 16.0f;
	Angle += 10.0f;
	if (Angle > 360.0f)
		Angle = 0.0f;
	RenderProxy->SetPosition(Vector2(ceil(Position.x), ceil(Position.y + Period)));
	CBox AABB = CBox(Player->Position - Vector2(16, 16), Player->Position + Vector2(16, 16));
	CBox AABBself = CBox(Position - Vector2(16, 16), Position + Vector2(16, 16));
	if ((AABB.Intersect(AABBself)))
	{
		Player->Score += 100;
		Player->ScoreText->SetText("Score: " + itos(Player->Score));
		CParticleSystem *Ps = NewObject(CParticleSystem);
		Ps->Init();
		Ps->ColorStart = RGBAf(0.8f, 0.3f, 0.9f, 1.0f);
		Ps->ColorOver = RGBAf(0.3f, 0.3f, 0.9f, 0.0f);
		Ps->SizeStart = 16;
		Ps->SizeOver = 32;
		Ps->Emission = 10;
		Ps->Life = 0.5;
		Ps->Texture = CFactory::Instance()->Get<CTexture>("shroomlittle");
		Ps->Position = Position;
		CFactory::Instance()->Destroy(this);
	}
	return;
}

CPacmanBonus::~CPacmanBonus()
{	
	//CFactory::Instance()->Destroy(RenderProxy);
	delete RenderProxy;
}

CPacmanPlayer::CPacmanPlayer() : Score(0), Damage(0), Velocity(V2_ZERO)
{
	Sprite = CFactory::Instance()->New<CSprite>("Player sprite");
	ScoreText = CFactory::Instance()->New<CText>("Score text");
	DamageText = CFactory::Instance()->New<CText>("Damage text");

	ScoreText->SetText("Score: " + itos(Score));
	DamageText->SetText("Damage: " + itos(Damage));
	DamageText->Color = COLOR_RED;
	SetName("Pac-man player");
	DamageText->SetPosition(Vector2(10.0f, 30.0f));
	ScoreText->SetPosition(Vector2(10.0f, 10.0f));
	ScoreText->SetLayer(10);
	ScoreText->doIgnoreCamera = true;
	DamageText->doIgnoreCamera = true;
	DamageText->SetLayer(10);
	Position = DEFAULT_POSITION;
	Sprite->SetLayer(1);
	Sprite->SetTexture("PacmanFrames");
	Sprite->AddAnimation(true, 50, 32, 32, 4, 2, 7, 32, 32, 0, 0, 0, true);
	Sprite->AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 0, 0, 1, false);
	CEventManager::Instance()->Subscribe("KeyDown", this);
	CRenderManager::Instance()->Camera.Assign(&Position.x, &Position.y);
}

void CPacmanPlayer::Update(float dt)
{
	Sprite->Color += RGBAf(0.01f, 0.01, 0.01f, 0.01f);
	Sprite->SetAnimation(0);
	if (Velocity.Length() < 20.0f)
		Sprite->SetAnimation(1);
	Position += Velocity * dt;
	Velocity *= 0.998f;
	Sprite->SetPosition(Vector2(ceil(Position.x), ceil(Position.y)));
}

void CPacmanPlayer::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_LEFT:
			Velocity.x += -DEFAULT_VELOCITY;
			Velocity.y = 0.0f;
			Sprite->SetAngle(0.0f);
			Sprite->mirror_h = true;
			break;
		case SDLK_RIGHT:
			Velocity.x += DEFAULT_VELOCITY;
			Velocity.y = 0.0f;
			Sprite->SetAngle(0.0f);
			Sprite->mirror_h = false;
			break;
		case SDLK_UP:
			Velocity.y += DEFAULT_VELOCITY;
			Velocity.x = 0.0f;
			Sprite->SetAngle(90.0f);
			Sprite->mirror_h = false;
			break;
		case SDLK_DOWN:
			Velocity.y += -DEFAULT_VELOCITY;
			Velocity.x = 0.0f;
			Sprite->SetAngle(90.0f);
			Sprite->mirror_h = true;
			break;
		}
	}
}

CPacmanGame::CPacmanGame(CPacmanPlayer *APlayer) : Player(APlayer)
{
	EnemySprite = CFactory::Instance()->New<CSprite>("Enemy sprite");
	BonusSprite = CFactory::Instance()->New<CSprite>("Bonus sprite");

	BonusSprite->SetVisibility(false);
	BonusSprite->AddAnimation(true, 50, 32, 32, 4, 2, 6, 32, 32, 0, 0, 0, true);
	BonusSprite->SetTexture("PacmanBonus");


	Tiles = CFactory::Instance()->Get<CTileset>("PacManTileset");
	Tiles->CheckLoad();
	Map = new CLevelMap(LEVEL_WIDTH, LEVEL_HEIGHT, "PacManTileset", "Pac man map");
	CFactory::Instance()->Add(Map, "Pac man map");
	Map->SetLayer(0);
	Map->SetScaling(2.0f);
	//	Map.TileSet = Tiles;
	for (int i = 0; i < LEVEL_WIDTH; i++)
		for (int j = 0; j < LEVEL_HEIGHT; j++)
			if ((i*j == 0 || i == LEVEL_WIDTH - 1 || j == LEVEL_HEIGHT - 1))
				Map->GetMapCell(i, j)->index = 1;
			else
			{
				int Factor;
				Factor = rand() % 100;
				Map->GetMapCell(i, j)->index = Factor < 25 ? 1 : 0;
				if (Factor >= 25 && (i != 1 || j != 1))
				{
					if (rand() % 10 < 2)
					{
						CPacmanBonus *Bonus = new CPacmanBonus(Vector2(i * 64 + 32, j * 64 + 32), BonusSprite);
						Bonus->Player = Player;
						CFactory::Instance()->Add(Bonus);
					}
					else if (rand() % 20 < 2)
					{
						CPacmanEnemy *Enemy = new CPacmanEnemy(Vector2(i * 64 + 32, j * 64 + 32));
						Enemy->Player = Player;
						Enemy->Map = Map;
						CFactory::Instance()->Add(Enemy);
					}
				}
			}

			Map->GenCells();
			//CSoundMixer::Instance()->SetMusicVolume(100);
}


void CPacmanGame::Update(float dt)
{
	if (Player == NULL)
		return;

	CBox AABB = CBox(Player->Position - Vector2(16, 16), Player->Position + Vector2(16, 16));

	Vector2 BottomLeft = AABB.Min,
		TopRight = AABB.Max,
		BottomRight = Vector2(AABB.Max.x, AABB.Min.y),
		TopLeft = Vector2(AABB.Min.x, AABB.Max.y);
	CBox Pot1, Pot2;
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

	switch (Direction)
	{
	case 0: // left
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(BottomLeft);
		if (Pot1.Intersect(AABB))
		{
			Player->Velocity.x = 0.0f;
			Player->Position.x = Pot1.Max.x + 16;
		}
		if (Pot2.Intersect(AABB))
		{
			Player->Velocity.x = 0.0f;
			Player->Position.x = Pot2.Max.x + 16;
		}
		break;
	case 1:	// right
		Pot1 = Map->GetCellAABB(TopRight);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
		{
			Player->Velocity.x = 0.0f;
			Player->Position.x = Pot1.Min.x - 16;
		}
		if (Pot2.Intersect(AABB))
		{
			Player->Velocity.x = 0.0f;
			Player->Position.x = Pot2.Min.x - 16;
		}
		break;
	case 2: // up
		Pot1 = Map->GetCellAABB(TopLeft);
		Pot2 = Map->GetCellAABB(TopRight);
		if (Pot1.Intersect(AABB))
		{
			Player->Velocity.y = 0.0f;
			Player->Position.y = Pot1.Min.y - 16;
		}
		if (Pot2.Intersect(AABB))
		{
			Player->Velocity.y = 0.0f;
			Player->Position.y = Pot2.Min.y - 16;
		}
		break;
	case 3: // down
		Pot1 = Map->GetCellAABB(BottomLeft);
		Pot2 = Map->GetCellAABB(BottomRight);
		if (Pot1.Intersect(AABB))
		{
			Player->Velocity.y = 0.0f;
			Player->Position.y = Pot1.Max.y + 16;
		}
		if (Pot2.Intersect(AABB))
		{
			Player->Velocity.y = 0.0f;
			Player->Position.y = Pot2.Max.y + 16;
		}
		break;
	}
}

CPacmanEnemy::CPacmanEnemy(Vector2 APosition) : Position(APosition), Player(NULL),
Direction(static_cast<EDirection>(rand()%4))
{
	Sprite = CFactory::Instance()->New<CSprite>("PacmanEnemy sprite " + itos(GetID()));
	SetName("Pacman enemy");
	Sprite->SetLayer(1);
	Sprite->SetTexture("PacmanEnemy");
	Sprite->AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 0, 0, 0, false);
	Sprite->AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 32, 0, 1, false);
	Sprite->AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 32, 32, 2, false);
	Sprite->AddAnimation(false, 0, 32, 32, 1, 1, 1, 32, 32, 0, 32, 3, false);
	Sprite->SetAnimation(Direction);
	Sprite->SetPosition(Position);
}

void CPacmanEnemy::Update(float dt)
{
	if (Map->GetMapCell(Position + V2_DIRECTIONS[Direction] * 32)->index == 1 || rand()%100 < 2)
	{
		Direction = static_cast<EDirection>((Direction + 1) % 4);
	}
	Position = Position + V2_DIRECTIONS[Direction] * 128 * dt;
	CBox AABB = CBox(Player->Position - Vector2(16, 16), Player->Position + Vector2(16, 16));
	CBox AABBself = CBox(Position - Vector2(16, 16), Position + Vector2(16, 16));
	if ((AABB.Intersect(AABBself)) && Player->Sprite->Color.g >= 0.9f)
	{
		Player->Damage += 32;
		Player->DamageText->SetText("Damage: " + itos(Player->Damage));
		Player->Sprite->Color = COLOR_RED;
		CParticleSystem *Ps = NewObject(CParticleSystem);
		Ps->Init();
		Ps->ColorStart = COLOR_RED;
		Ps->ColorOver = COLOR_RED;
		Ps->SizeStart = 4;
		Ps->SizeOver = 4;
		Ps->SizeVariability = 2;
		Ps->Emission = 10;
		Ps->Life = 0.5;
		Ps->Texture = CFactory::Instance()->Get<CTexture>("ParticlePacmanBlood");
		Ps->PtrPosition = &Player->Position;
	}
	Sprite->SetPosition(Position);
}
