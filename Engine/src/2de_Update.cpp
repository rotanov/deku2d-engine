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
	//RootGameObject = CFactory::Instance()->New<CPlaceableComponent>("RootGameObject");
	RootGameObject = dynamic_cast<CPlaceableComponent *>(CFactory::Instance()->CreateByName("PlaceableComponent", "Root"));
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

