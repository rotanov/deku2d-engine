#include "2de_PlaceableComponent.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CPlaceableComponent
	CPlaceableComponent::CPlaceableComponent() : doIgnoreCamera(false), doMirrorHorizontal(false), doMirrorVertical(false)
	{
		ClassName = "PlaceableComponent";
	}

	CPlaceableComponent* CPlaceableComponent::Clone(const string &ACloneName /*= ""*/) const
	{
		return CloneHelper(new CPlaceableComponent(*this), ACloneName);
	}

	void CPlaceableComponent::SetAngle(float AAngle)
	{
		float Angle = AAngle;
		if (std::fabs(AAngle) > 360.0f)
			Angle = AAngle - Sign(AAngle)*((static_cast<int>(AAngle) / 360) * 360.0f); 
		if (Angle < 0.0f)
			Angle = 360.0f + Angle;
		Transformation.SetRotation(Angle);
	}

	float CPlaceableComponent::GetAngle() const
	{
		return Transformation.GetRotation();
	}

	void CPlaceableComponent::SetLayer(int Layer)
	{
		Transformation.SetDepth(Layer == 0 ? 0.0f : Layer / 100.0f);	// @todo: get rid of magic number "100.0f"
	}

	float CPlaceableComponent::GetDepth() const
	{
		return Transformation.GetDepth();
	}

	const Vector2 CPlaceableComponent::GetScaling() const
	{
		return Transformation.GetScaling();
	}

	void CPlaceableComponent::SetScaling(const Vector2 &AScaling)
	{
		Transformation.SetScaling(AScaling);
	}

	int CPlaceableComponent::GetLayer() const
	{
		return Transformation.GetDepth() * 100.0f;
	}

	const Vector2 CPlaceableComponent::GetPosition() const
	{
		return Transformation.GetTranslation();
	}

	void CPlaceableComponent::SetPosition(const Vector2 &APosition)
	{
		Transformation.SetTranslation(APosition);
	}

	void CPlaceableComponent::SetTransformation(const CTransformation &ATransformation)
	{
		Transformation = ATransformation;
	}

	CTransformation CPlaceableComponent::GetTransformation()
	{
		return Transformation;
	}

	const CTransformation CPlaceableComponent::GetTransformation() const
	{
		return Transformation;
	}

	bool CPlaceableComponent::isMirrorVertical() const
	{
		return doMirrorVertical;
	}

	void CPlaceableComponent::SetMirrorVertical(bool MirrorOrNot)
	{
		doMirrorHorizontal = MirrorOrNot;
	}

	bool CPlaceableComponent::isMirrorHorizontal() const
	{
		return doMirrorHorizontal;
	}

	void CPlaceableComponent::SetMirrorHorizontal(bool MirrorOrNot)
	{
		doMirrorHorizontal = MirrorOrNot;
	}

	bool CPlaceableComponent::isIgnoringParentTransform() const
	{
		return doIgnoreCamera;
	}

	void CPlaceableComponent::SetIgnoreParentTransform(bool doIgnore)
	{
		doIgnoreCamera = doIgnore;
	}

	void CPlaceableComponent::Deserialize(CXMLNode *AXML)
	{
		CGameObject::Deserialize(AXML);

		if (AXML->HasAttribute("Angle"))
			SetAngle(from_string<float>(AXML->GetAttribute("Angle")));

		if (AXML->HasAttribute("Position"))
		{
			SetPosition(Vector2(GetNFloatTokensFromString(AXML->GetAttribute("Position"), 2)));
			//Log("WARNING", "Incorrect position in prototype");
		}

		if (AXML->HasAttribute("Scaling"))
		{
			SetScaling(Vector2(GetNFloatTokensFromString(AXML->GetAttribute("Scaling"), 2)));
			//Log("WARNING", "Incorrect position in prototype");			
		}

		if (AXML->HasAttribute("Layer"))
			SetLayer(from_string<int>(AXML->GetAttribute("Layer")));

		// TODO: add more..	// Later
	}

	const CBox CPlaceableComponent::GetBox() const
	{
		return Box;
	}

	void CPlaceableComponent::SetBox(const CBox &ABox)
	{
		Box = ABox;
	}

	void CPlaceableComponent::UpdateBox(const CBox& ABox)
	{
		Box.Union(ABox);
	}

}	//	namespace Deku2D

