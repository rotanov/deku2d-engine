#ifndef _2DE_PLACEABLE_COMPONENT_H_
#define _2DE_PLACEABLE_COMPONENT_H_

namespace Deku2D
{
	/**
	*	Serves as transform node in transform tree.
	*/
	class CPlaceableComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CPlaceableComponent();

		CPlaceableComponent* Clone(const string &ACloneName = "") const;

		float GetAngle() const;
		const CBox& GetBox() const;
		float GetDepth() const;
		int GetLayer() const;	
		float GetScaling() const;
		const Vector2& GetPosition() const;
		Vector2& GetPosition();

		void SetAngle(float AAngle); //	(Degrees)

		// temp
		void SetBox(const CBox &ABox);

		/**
		*	Layers should be from SOME_NEGATIVE_VALUE to SOME_POSITIVE_VALUE. Layer with greater number is drawn over layer with lower one.
		*	implicitly Depth => [-1; 1]?
		*/
		void SetLayer(int Layer);
		void SetScaling(float AScaling);
		void SetPosition(const Vector2 &APosition);

		CTransformation& GetTransformation();
		const CTransformation& GetTransformation() const;
		void SetTransformation(const CTransformation &ATransformation);

		bool isMirrorHorizontal() const;
		void SetMirrorHorizontal(bool MirrorOrNot);
		bool isMirrorVertical() const;
		void SetMirrorVertical(bool MirrorOrNot);
		bool isIgnoringParentTransform() const;
		void SetIgnoreParentTransform(bool doIgnore);
		void UpdateBox(const CBox& ABox);
		void Deserialize(CXMLNode *AXML);

	private:
		CBox Box;
		
		bool doIgnoreCamera;	// all previous transformations are ignored if true
		bool doMirrorHorizontal;
		bool doMirrorVertical;

		CTransformation Transformation;
	};

}	//	namespace Deku2D

#endif	//	_2DE_PLACEABLE_COMPONENT_H_
