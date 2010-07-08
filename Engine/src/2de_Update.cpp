#include "2de_Update.h"

#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CUpdateObject

CUpdateObject::CUpdateObject() : Active(true), Dead(false)
{
	SetName("CUpdateObject");
	CUpdateManager::Instance()->Add(this);
}

CUpdateObject::~CUpdateObject()
{
	CUpdateManager::Instance()->Remove(GetID());
}

void CUpdateObject::SetDead()
{
	Dead = true;
	Active = false;
}

bool CUpdateObject::isDead() const
{
	return Dead;
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
		CUpdateObject *data = (*it);
		if (!data->Active)
			continue;
		if (data->isDestroyed())
		{
			toDelete.push_back(data);
			continue;
		}
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
