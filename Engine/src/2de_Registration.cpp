#include "2de_TypeInfo.h"
#include "2de_Object.h"
#include "2de_Vector2.h"
#include "2de_RGBA.h"
#include "2de_Box.h"
#include "2de_Resource.h"
#include "2de_Visitor.h"
#include "2de_PlaceableComponent.h"
#include "2de_RenderableComponent.h"

namespace Deku2D
{
	D2D_DECLARE_INTEGRAL_TYPE(EModelType)

	D2D_DECL_TYPE_INFO(char)
	D2D_DECL_TYPE_INFO(unsigned)
	D2D_DECL_TYPE_INFO(double)
	D2D_DECL_TYPE_INFO(float)
	D2D_DECL_TYPE_INFO(int)
	D2D_DECL_TYPE_INFO(bool)
	D2D_DECL_TYPE_INFO(string)	// note namespace qualification fail: std::string

	// don't know if it works for enums
	D2D_DECL_TYPE_INFO(EModelType)

	D2D_DECL_TYPE_INFO(Vector2)
		D2D_DECLARE_PROPERTY_INFO(Vector2, float, X)
		D2D_DECLARE_PROPERTY_INFO(Vector2, float, Y)

	D2D_DECL_TYPE_INFO(RGBAf)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, A)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, R)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, G)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, B)


	D2D_DECL_TYPE_INFO(CBox)
		D2D_DECLARE_PROPERTY_INFO(CBox, Vector2, Min)
		D2D_DECLARE_PROPERTY_INFO(CBox, Vector2, Max)

	D2D_DECL_DERIVED_TYPE_INFO(CObject, CNullClass)
		D2D_DECLARE_PROPERTY_INFO(CObject, string, Name)

		D2D_DECL_DERIVED_TYPE_INFO(CResource, CObject)
			D2D_DECLARE_PROPERTY_INFO_EX(CResource, string, Filename, SetLoadSource, GetFilename)

			D2D_DECL_DERIVED_TYPE_INFO(CFont, CResource)
				D2D_DECLARE_ARRAY_PROPERTY_INFO_EX(CFont, CBox, Boxex, SetBox, GetBox, GetBoxCount)

			D2D_DECL_DERIVED_TYPE_INFO(CScript, CResource)
				D2D_DECLARE_PROPERTY_INFO(CScript, string, ScriptText)

			D2D_DECL_DERIVED_TYPE_INFO(CModel, CResource)
				// D2D_DECLARE_PROPERTY_INFO(CModel, EModelType, ModelType) потом. Там enum за int конверсия в стринг не считает
				D2D_DECLARE_ARRAY_PROPERTY_INFO_EX(CModel, Vector2, Vertices, SetVertex, GetVertex, GetVertexNumber)

		D2D_DECL_DERIVED_TYPE_INFO(IVisitableObject, CObject)

			D2D_DECL_DERIVED_TYPE_INFO(CGameObject, IVisitableObject)
				D2D_DECLARE_PTR_PROPERTY_INFO(CGameObject, CScript, Script)
				D2D_DECLARE_PTR_ARRAY_PROPERTY_INFO_EX(CGameObject, CGameObject, Children, Attach, GetChild, GetChildCount)

				D2D_DECL_DERIVED_TYPE_INFO(CPlaceableComponent, CGameObject)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, float, Angle)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, Vector2, Position)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, Vector2, Scaling)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, CBox, Box)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, int, Layer)

				D2D_DECL_DERIVED_TYPE_INFO(CRenderableComponent, CGameObject)
					D2D_DECLARE_PROPERTY_INFO(CRenderableComponent, RGBAf, Color)
					D2D_DECLARE_PTR_PROPERTY_INFO(CRenderableComponent, CModel, Model)

					D2D_DECL_DERIVED_TYPE_INFO(CText, CRenderableComponent)
						D2D_DECLARE_PROPERTY_INFO(CText, string, Text)
						D2D_DECLARE_PTR_PROPERTY_INFO(CText, CFont, Font)

};	// namespace Deku2D
