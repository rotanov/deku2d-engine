#ifndef _2DE_UPDATE_H_
#define _2DE_UPDATE_H_

#include "2de_Core.h"

class CAbstractScene;

/**
* Класс CUpdatable - базовый абстрактный класс для объектов, которые следует обновлять.
*/

class CUpdatable : public virtual CObject
{
public:
	bool Active; // на даннй момент Active и Dead используются для одного и того же.. надо либо оставить что-то одно, либо чётко разделить их назначение..
	CUpdatable();
	~CUpdatable();
	bool isDead() const;
	void SetDead();
	virtual void Update(float dt) = 0;
	void PutIntoScene(CAbstractScene *AScene);
	CAbstractScene* GetScene() const;

private:
	bool Dead;
	CAbstractScene *Scene;
};

/**
* Класс CUpdateManager - менеджер объектов, которые следует обновлять. Такие дела.
*/

class CUpdateManager : public CCommonManager <list<CUpdatable*> >, public CTSingleton <CUpdateManager>
{
public:	
	bool UpdateObjects();

protected:
	CUpdateManager();
	friend class CTSingleton<CUpdateManager>;
};

//////////////////////////////////////////////////////////////////////////
//	class CAbstractAction
class CAbstractAction
{
public:
	virtual void Execute() = 0;
};

//////////////////////////////////////////////////////////////////////////
//CTimeredEvent
template <typename ActionT>
class CTimeredAction : public CUpdatable
{
public:	
	CTimeredAction() : Action(), Life(0.0f), Age(0.0f)
	{

	}
	void SetLife(float ALife)
	{
		Life = ALife;
	}
	void Update(float dt)
	{
		Age += dt;
		if (Age >= Life)
		{
			Action.Execute();
			SetDestroyed();
		}
	}

	ActionT Action;

private:
	float Life;
	float Age;
};

#endif // _2DE_UPDATE_H_
