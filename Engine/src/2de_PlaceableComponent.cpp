#include "2de_PlaceableComponent.h"
#include "2de_DebugBox.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CPlaceableComponent

	CPlaceableComponent::CPlaceableComponent() : Box(), doIgnoreCamera(false), doMirrorHorizontal(false), doMirrorVertical(false), Transformation()
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
		Transformation.SetAngle(Angle);
	}

	float CPlaceableComponent::GetAngle() const
	{
		return Transformation.GetAngle();
	}

	void CPlaceableComponent::SetLayer(int Layer)
	{
		Transformation.SetDepth(Layer == 0 ? 0.0f : Layer / 100.0f);	// @todo: get rid of magic number "100.0f"
	}

	float CPlaceableComponent::GetDepth() const
	{
		return Transformation.GetDepth();
	}

	float CPlaceableComponent::GetScaling() const
	{
		return Transformation.GetScaling();
	}

	void CPlaceableComponent::SetScaling(float AScaling)
	{
		Transformation.SetScaling(AScaling);
	}

	int CPlaceableComponent::GetLayer() const
	{
		return Transformation.GetDepth() * 100.0f;
	}

	const Vector2& CPlaceableComponent::GetPosition() const
	{
		return Transformation.GetTranslation();
	}

	Vector2& CPlaceableComponent::GetPosition()
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

	CTransformation& CPlaceableComponent::GetTransformation()
	{
		return Transformation;
	}

	const CTransformation& CPlaceableComponent::GetTransformation() const
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
			Vector2 APosition = Const::Math::V2_ZERO;
			istringstream iss(AXML->GetAttribute("Position"));
			vector<string> tokens;
			copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
			if (tokens.size() == 2)
			{
				APosition.x = from_string<float>(tokens[0]);
				APosition.y = from_string<float>(tokens[1]);
				SetPosition(APosition);
			}
			else
				Log("WARNING", "Incorrect position in prototype");
		}

		if (AXML->HasAttribute("Scaling"))
			SetScaling(from_string<float>(AXML->GetAttribute("Scaling")));

		if (AXML->HasAttribute("Layer"))
			SetLayer(from_string<int>(AXML->GetAttribute("Layer")));

		// TODO: add more..	// Later
	}

	const CBox& CPlaceableComponent::GetBox() const
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
		if (typeid(*this) == typeid(CDebugBoxComponent))
			return;
#if defined(_DEBUG) && !defined(DISABLE_DEBUG_BOXES)
		CDebugBoxComponent *DebugBox = dynamic_cast<CDebugBoxComponent*>(GetChild(0));
		if (NULL != DebugBox)
		{
			if (DebugBox->GetModel() == NULL)
				DebugBox->SetModel(CRenderManager::CreateModelBox(Box.Width(), Box.Height(), MODEL_TYPE_LINES));
			{
				DebugBox->GetModel()->GetVertices()[0] = Vector2(Box.Min.x, Box.Min.y);
				DebugBox->GetModel()->GetVertices()[1] = Vector2(Box.Max.x, Box.Min.y);

				DebugBox->GetModel()->GetVertices()[2] = Vector2(Box.Max.x, Box.Min.y);
				DebugBox->GetModel()->GetVertices()[3] = Vector2(Box.Max.x, Box.Max.y);

				DebugBox->GetModel()->GetVertices()[4] = Vector2(Box.Max.x, Box.Max.y);
				DebugBox->GetModel()->GetVertices()[5] = Vector2(Box.Min.x, Box.Max.y);

				DebugBox->GetModel()->GetVertices()[6] = Vector2(Box.Min.x, Box.Max.y);
				DebugBox->GetModel()->GetVertices()[7] = Vector2(Box.Min.x, Box.Min.y);
			}
		}
#endif
	}

}	//	namespace Deku2D
