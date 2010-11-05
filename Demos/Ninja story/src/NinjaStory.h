#ifndef _NINJA_STORY_H_
#define _NINJA_STORY_H_

#include "2de_Engine.h"

class CLifeBarBase /*:*/ /*public CRenderable, *//*public CUpdatable*/
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

class CDamageIndicator/* : public CUpdatable*/
{
private:
	CText DamageInfo;	 // aggregate or inherit @todo: think & meditate

};

class CEnemySpawner // This thing will spawn our base enemies. Maybe this is don't really needed at all.
{

};

// Some particle effects

class CEnemyBase /*: public CUnitBase*/
{

};

class CNinjaUnit /*: public CUnitBase*/
{

};

class CPlayer/* : public CUpdatable*/	// We will be playing for CNinja, 
	// but such design should make it possible to play for any Enemy or someone with implemented interface for player
{
public:
	void Update(float dt)
	{

	}
};

// I just drafted some classes from my paper-class-concepts

// ... and finally


class CNinjaStoryGame/* : public CUpdatable*/
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

#endif // _NINJA_STORY_H_
