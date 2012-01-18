#include "2de_RTTI.h"
#include "2de_Object.h"
#include "2de_Visitor.h"
#include "2de_PlaceableComponent.h"
#include "2de_GameObject.h"

namespace Deku2D
{
	D2D_DEFINE_RTTI(CGameObject, IVisitableObject<>)
	D2D_DEFINE_RTTI(CObject, CNullClass)
	D2D_DEFINE_RTTI(CPlaceableComponent, CGameObject)
	D2D_DEFINE_CLASS_TEMPLATE_RTTI(IVisitableObject, CObject)

	void CObject::RegisterReflection()
	{
		REGISTER_PROPERTY(string, Name, CObject)
	}

	void IVisitableObject<>::RegisterReflection()
	{
	}

	void CGameObject::RegisterReflection()
	{
	}

	void CPlaceableComponent::RegisterReflection()
	{

	}

};	// namespace Deku2D
