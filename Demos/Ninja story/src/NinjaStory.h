#ifndef _NINJA_STORY_H_
#define _NINJA_STORY_H_

#include "2de_Engine.h"

class CLifeBarBase : public CRenderable, public CUpdatable
{
public:
	void Render()
	{
		// Draw life bar
	}
	void Update(float dt)
	{
		// Update lifebar props here;
	}
private:
	int Health;
};

class CPlayerLifeBar : public CLifeBarBase
{
public:
	// This is special life bar for our player
};

class CDamageIndicator : public CUpdatable
{
private:
	CText DamageInfo;	 // aggregate or inherit @todo: think & meditate

};

class CEnemySpawner // This thing will spawn our base enemies. Maybe this is don't really needed at all.
{

};

// Some particle effects

class CEnemyBase : public CUnitBase
{

};

class CNinjaUnit : public CUnitBase
{

};

class CPlayer : public CUpdatable	// We will be playing for CNinja, 
	// but such design should make it possible to play for any Enemy or someone with implemented interface for player
{
public:
	void Update(float dt)
	{

	}
};

// I just drafted some classes from my paper-class-concepts

// ... and finally


class CNinjaStoryGame : public CUpdatable
{
public: 
	CNinjaStoryGame()
	{
		CSprite *Sprite = CFactory::Instance()->New<CSprite>("BlackmanSprite");
		Sprite->LoadFromFile("");
	}
	void Update(float dt)
	{
	}
};

class CBouncingText : public CUpdatable
{
public:
	CBouncingText() : Text(NULL), t0(0.0f, 0.0f),
		t1(0.0f, 0.0f),
		Life(4.0f), Age(0.0f)
	{
		Text = new CText;
		Text->Color = RGBAf(0.1f, 0.7f, 0.2f, 0.4f);
		float temp = Random_Float(0.0f, PI);
		Vector2 mouse = CEngine::Instance()->MousePos;
		t0 = mouse;
		t1 = Vector2(cos(temp) * 400.0f, sin(temp) * 400.0f) + t0;
		delta = (t1 - t0) / Life;
		Text->SetAngle(Random_Float(0.0f, 360.0f));
	}
	~CBouncingText()
	{
		delete Text;
	}
	void SetText(const string &AText)
	{
		Text->SetText(AText);
	}
	void Update(float dt)
	{		
		Text->SetAngle(Clamp(Text->GetAngle() + dt * 100.0f, 0.0f, 360.0f));
		Age += dt;
		t0 += delta * dt * cos((Age * PI) / (2.0f * Life));
		Text->Position = t0;
		if (Age >= Life)
		{
			CFactory::Instance()->Destroy(this);
		}
	}
private:
	CText *Text;
	Vector2 t0, t1, delta;
	float Life;
	float Age;
};

class CTemporaryBouncingTextTest : public CUpdatable
{
public:
	void Update(float dt)
	{
		// moved from global Update
		static float age = 1.0f;
		age += dt;
		if (CEngine::Instance()->keys[SDLK_SPACE] && age >= 0.001f)
		{
			CBouncingText *newbt = CFactory::Instance()->New<CBouncingText>(""); //new CBouncingText();
			newbt->SetText(itos(Random_Int(1, 999)) + "dmg");
			age = 0.0f;
		}
	}
};

#endif // _NINJA_STORY_H_
