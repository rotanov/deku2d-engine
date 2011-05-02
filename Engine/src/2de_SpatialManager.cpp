#include "2de_SpatialManager.h"
#include "2de_Components.h"
#include "2de_Engine.h"

void CBruteForceSpatialManager::PushBox(CObject *Host, const CBox &Box)
{
	Data.push_back(std::pair<CObject *, CBox>(Host, Box));
}

void CBruteForceSpatialManager::GetObjectsInRegion(const CBox &Region, vector<CObject *> &Destination)
{
	for (DataT::iterator i = Data.begin(); i != Data.end(); ++i)
		if (i->second.Intersect(Region))
			Destination.push_back(i->first);
}

void CBruteForceSpatialManager::GetPCS(const CBox &Region, vector<CObject *> &Destination)
{
	for (DataT::iterator i = Data.begin(); i != Data.end() - 1; ++i)
		for (DataT::iterator j = i + 1; j != Data.end(); ++j)
			if (Region.Intersect(i->second) || Region.Intersect(j->second) && i->second.Intersect(j->second))
				Destination.push_back(i->first), Destination.push_back(j->first);
}

void CBruteForceSpatialManager::Clear()
{
	Data.clear();
}

void CBruteForceSpatialManager::Update()
{
	// <strike> @todo: nothing </strike>
	for (DataT::iterator i = Data.begin(); i != Data.end(); ++i)
	{
// 		CGameObject *gameObject = dynamic_cast<CGameObject*>(i->first);
// 		if (!gameObject)
// 			continue;
// 		CPlaceableComponent * placing = dynamic_cast<CPlaceableComponent*>(gameObject->GetParent());
// 		if (!placing)
// 			continue;
		/*
		CTransformation tempTransform = placing->GetTransformation();
		tempTransform.SetTranslation(tempTransform.GetTranslation() + Vector2(0, -9) * CEngine::Instance()->GetDeltaTime());
		placing->SetTransformation(tempTransform);
		*/
	}
}

void CBruteForceSpatialManager::GetKNearestNeighbours(const Vector2 &Position, vector<CObject *> &Destination)
{
	// later
}