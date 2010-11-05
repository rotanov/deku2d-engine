#include "2de_Update.h"

#include "2de_Engine.h"
#include "2de_GraphicsLow.h"

//////////////////////////////////////////////////////////////////////////
// CUpdatable

// CUpdatable::CUpdatable() : Active(true), Dead(false), Scene(NULL)
// {
// 	SetName("CUpdatable");
// 	PutIntoScene(CSceneManager::Instance()->GetCurrentScene());
// }
// 
// CUpdatable::~CUpdatable()
// {
// 	CUpdateManager::Instance()->Remove(this);
// 
// 	Scene->RemoveUpdatable(this);
// }
// 
// void CUpdatable::SetDead()
// {
// 	Dead = true;
// 	Active = false;
// }
// 
// bool CUpdatable::isDead() const
// {
// 	return Dead;
// }
// 
// void CUpdatable::PutIntoScene(CAbstractScene *AScene)
// {
// 	assert(AScene != NULL);
// 	if (Scene != NULL)
// 		Scene->RemoveUpdatable(this);
// 	Scene = AScene;
// 	Scene->AddUpdatable(this);
// }
// 
// CAbstractScene* CUpdatable::GetScene() const
// {
// 	assert(Scene != NULL);
// 	return Scene;
// }

//////////////////////////////////////////////////////////////////////////
// CUpdateManager

CUpdateManager::CUpdateManager()
{
	SetName("CUpdateManager");
	RootGameObject = CFactory::Instance()->New<CGameObject>("RootGameObject");
}

bool CUpdateManager::UpdateObjects()
{
// 	CUpdatable *data;
// 
// 	for (ManagerIterator it = Objects.begin(); it != Objects.end(); ++it)
// 	{
// 		data = *it;
// 		if (data->isDestroyed())
// 			continue;
// 		if (!CSceneManager::Instance()->InScope(data->GetScene()))
// 			continue;
// 		if (!data->Active)
// 			continue;
// 		// FIXED_DELTA_TIME
// 		/*float dt = 0;
// 		CEngine::Instance()->GetState(CEngine::STATE_DELTA_TIME, &dt);*/
// 		data->Update(CEngine::Instance()->GetDeltaTime()); // @todo: подумать что использоваьт: фиксированную дельту или реальную engine->Getdt()
// 	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CTimerComponent

CTimerComponent::CTimerComponent() : Enabled(false), Interval(0.0f), Accumulated(0.0f)
{
}

CTimerComponent::CTimerComponent(float AInterval) : Enabled(false), Interval(AInterval), Accumulated(0.0f)
{
}

void CTimerComponent::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "EveryFrame")
	{
		Accumulated += CEngine::Instance()->GetDeltaTime();
		if (Accumulated >= Interval)
		{
			CEvent *TimerTickEvent = new CEvent("TimerTick", this);
			TimerTickEvent->SetData("Name", GetName());
			CEventManager::Instance()->TriggerEvent(TimerTickEvent);
			Accumulated = 0.0f;
		}
	}
}

bool CTimerComponent::isEnabled() const
{
	return Enabled;
}

void CTimerComponent::SetEnabled(bool AEnabled)
{
	Enabled = AEnabled;
	if (Enabled)
		CEventManager::Instance()->Subscribe("EveryFrame", this);
	else
		CEventManager::Instance()->Unsubscribe("EveryFrame", this);

	Accumulated = 0.0f;
}

float CTimerComponent::GetInterval() const
{
	return Interval;
}

void CTimerComponent::SetInterval(float AInterval)
{
	Interval = AInterval;
	Accumulated = 0.0f;	// is it needed?.. or make separate Reset(), SetElapsed() or whatever?..
}
