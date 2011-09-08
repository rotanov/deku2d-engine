#ifndef _2DE_RENDERABLE_COMPONENT_H_
#define _2DE_RENDERABLE_COMPONENT_H_

#include "2de_GameObject.h"

namespace Deku2D
{
	/**
	* CRenderableComponent - AOP way replacement for CRenderable. // these historical notes are very very funny, but we need to write the actual documentation, not the tales of the past..
	*/

	class CRenderableComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CTransformation WorldTransform;

		CRenderableComponent(CModel *AModel = NULL);
		virtual ~CRenderableComponent();

		CRenderableComponent* Clone(const string &ACloneName = "") const;

		const CBox& GetBox() const;
		void SetBox(const CBox& ABox);
		bool GetVisibility() const;
		virtual void SetVisibility(bool AVisible);
		const RGBAf& GetColor() const;
		RGBAf& GetColor();
		void SetColor(const RGBAf &AColor);
		EBlendingMode GetBlendingMode() const;
		void SetBlendingMode(EBlendingMode ABlendingMode);
		CModel* GetModel() const;
		void SetModel(CModel *AModel);
		CRenderConfig& GetConfiguration();
		const CRenderConfig& GetConfiguration() const;
		void SetConfiguration(const CRenderConfig &AConfiguraton);
		void Deserialize(CXMLNode *AXML);

		CBox Box;
		void UpdateBox(const CBox& ABox);

	protected:
		CRenderableComponent(const CRenderableComponent &ARenderableComponent);

		CResourceRefCounter<CModel> Model;

	private:
		CRenderConfig Configuration;
		bool Visible;	
	};

}	//	namespace Deku2D

#endif	//	_2DE_RENDERABLE_COMPONENT_H_
