#include "2de_Update.h"

#include "2de_Engine.h"
#include "2de_GraphicsLow.h"

//////////////////////////////////////////////////////////////////////////
// CUpdatable

CUpdatable::CUpdatable() : Active(true), Dead(false), Scene(NULL)
{
	SetName("CUpdatable");
	PutIntoScene(CSceneManager::Instance()->GetCurrentScene());
}

CUpdatable::~CUpdatable()
{
	if (!Managed) // one if-check is better than useless list-search..
		CUpdateManager::Instance()->Remove(GetID());
}

void CUpdatable::SetDead()
{
	Dead = true;
	Active = false;
}

bool CUpdatable::isDead() const
{
	return Dead;
}

void CUpdatable::PutIntoScene(CAbstractScene *AScene)
{
	assert(AScene != NULL);
	if (Scene != NULL)
		Scene->RemoveUpdatable(this);
	Scene = AScene;
	Scene->AddUpdatable(this);
}

CAbstractScene* CUpdatable::GetScene() const
{
	assert(Scene != NULL);
	return Scene;
}

//////////////////////////////////////////////////////////////////////////
// CUpdateManager

CUpdateManager::CUpdateManager()
{
	SetName("CUpdateManager");
}

bool CUpdateManager::UpdateObjects()
{
	ManagerContainer toDelete;	
	CEngine *engine = CEngine::Instance();
	for(ManagerIterator it = Objects.begin(); it != Objects.end(); ++it)
	{
		CUpdatable *data = (*it);
		if (data->isDestroyed())
		{
			toDelete.push_back(data);
			continue;
		}
		if (!CSceneManager::Instance()->InScope(data->GetScene()))
			continue;
		if (!data->Active)
			continue;
		// FIXED_DELTA_TIME
		float dt = 0;
		CEngine::Instance()->GetState(CEngine::STATE_DELTA_TIME, &dt);
		data->Update(dt); // @todo: подумать что использоваьт: фиксированную дельту или реальную engine->Getdt()
	}
	for(ManagerIterator i = toDelete.begin(); i != toDelete.end(); ++i)
	{
		Objects.remove(*i);
		CObject::DecRefCount(*i);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CTimeredAction
