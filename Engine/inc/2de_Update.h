#ifndef _2DE_UPDATE_H_
#define _2DE_UPDATE_H_

#include "2de_Core.h"

class CAbstractScene;	// Separate header, may be?

/*
	interesting part of interface:
		bool Active;
		bool isDead() const;
		void SetDead();
		virtual void Update(float dt) = 0;
		void PutIntoScene(CAbstractScene *AScene);
		CAbstractScene* GetScene() const;
		CAbstractScene *Scene;
		bool Dead;	
*/

class CUpdateManager : public CTSingleton <CUpdateManager>
{
public:	
	bool UpdateObjects();
	CGameObject *RootGameObject;

protected:
	CUpdateManager();
	friend class CTSingleton<CUpdateManager>;
};

class CTimerComponent : public CGameObject
{
public:
	CTimerComponent();
	CTimerComponent(float AInterval);

	void ProcessEvent(const CEvent &AEvent);

	bool isEnabled() const;
	void SetEnabled(bool AEnabled);

	float GetInterval() const;
	void SetInterval(float AInterval);

private:
	bool Enabled;
	float Interval;
	float Accumulated;

};

#endif // _2DE_UPDATE_H_
