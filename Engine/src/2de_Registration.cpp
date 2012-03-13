#include "2de_RTTI.h"
#include "2de_Object.h"
#include "2de_Visitor.h"
#include "2de_PlaceableComponent.h"
#include "2de_GameObject.h"

namespace Deku2D
{
	D2D_DEFINE_RTTI(CGameObject, IVisitableObject<>)
	D2D_DEFINE_RTTI(CResource, CObject)
	D2D_DEFINE_RTTI(CScript, CResource)
	D2D_DEFINE_RTTI(CObject, CNullClass)
	D2D_DEFINE_RTTI(CPlaceableComponent, CGameObject)
	D2D_DEFINE_CLASS_TEMPLATE_RTTI(IVisitableObject, CObject)

	void CObject::RegisterReflection()
	{
		D2D_REGISTER_PROPERTY(string, Name, CObject)
	}

	void IVisitableObject<>::RegisterReflection()
	{
	}

	void CGameObject::RegisterReflection()
	{
		D2D_REGISTER_PROPERTY(CScript, Script, CGameObject)
	}

	void CPlaceableComponent::RegisterReflection()
	{

	}

	void CResource::RegisterReflection()
	{

	}

	void CScript::RegisterReflection()
	{
	}

};	// namespace Deku2D
