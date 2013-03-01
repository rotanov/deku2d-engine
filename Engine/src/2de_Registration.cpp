#include "2de_TypeInfo.h"
#include "2de_Object.h"
#include "2de_Vector2.h"
#include "2de_RGBA.h"
#include "2de_Box.h"
#include "2de_Resource.h"
#include "2de_Visitor.h"
#include "2de_PlaceableComponent.h"
#include "2de_RenderableComponent.h"
#include "2de_Sound.h"
#include "2de_Text.h"

namespace Deku2D
{
	D2D_DECLARE_INTEGRAL_TYPE(EModelType)

	D2D_TYPE_INFO_DECLARE(char)
	D2D_TYPE_INFO_DECLARE(unsigned)
	D2D_TYPE_INFO_DECLARE(double)
	D2D_TYPE_INFO_DECLARE(float)
	D2D_TYPE_INFO_DECLARE(int)
	D2D_TYPE_INFO_DECLARE(bool)
	// note: namespace qualification fail: std::string
	D2D_TYPE_INFO_DECLARE(string)

	// note: not sure if it works for enums
	D2D_TYPE_INFO_DECLARE(EModelType)

	D2D_TYPE_INFO_DECLARE(Vector2)
		D2D_DECLARE_PROPERTY_INFO(Vector2, float, X)
		D2D_DECLARE_PROPERTY_INFO(Vector2, float, Y)

	D2D_TYPE_INFO_DECLARE(RGBAf)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, A)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, R)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, G)
		D2D_DECLARE_PROPERTY_INFO(RGBAf, float, B)


	D2D_TYPE_INFO_DECLARE(CBox)
		D2D_DECLARE_PROPERTY_INFO(CBox, Vector2, Min)
		D2D_DECLARE_PROPERTY_INFO(CBox, Vector2, Max)

	D2D_TYPE_INFO_DECLARE_DERIVED(CObject, CNullClass)
		D2D_DECLARE_PROPERTY_INFO(CObject, string, Name)

		D2D_TYPE_INFO_DECLARE_DERIVED(CResource, CObject)
			D2D_DECLARE_PROPERTY_INFO_EX(CResource, string, Filename, SetLoadSource, GetFilename)

			D2D_TYPE_INFO_DECLARE_DERIVED(CFont, CResource)
				D2D_DECLARE_ARRAY_PROPERTY_INFO_EX(CFont, CBox, Boxes, SetBox, GetBox, GetBoxCount)

			D2D_TYPE_INFO_DECLARE_DERIVED(CScript, CResource)
				D2D_DECLARE_PROPERTY_INFO(CScript, string, ScriptText)

			D2D_TYPE_INFO_DECLARE_DERIVED(CModel, CResource)
				// D2D_DECLARE_PROPERTY_INFO(CModel, EModelType, ModelType) потом. Там enum за int конверсия в стринг не считает
				D2D_DECLARE_ARRAY_PROPERTY_INFO_EX(CModel, Vector2, Vertices, SetVertex, GetVertex, GetVertexNumber)

			D2D_TYPE_INFO_DECLARE_DERIVED(CMusic, CResource)

			D2D_TYPE_INFO_DECLARE_DERIVED(CSound, CResource)

		D2D_TYPE_INFO_DECLARE_DERIVED(IVisitableObject, CObject)

			D2D_TYPE_INFO_DECLARE_DERIVED(CGameObject, IVisitableObject)
				D2D_DECLARE_PTR_PROPERTY_INFO(CGameObject, CScript, Script)
				D2D_DECLARE_PTR_ARRAY_PROPERTY_INFO_EX(CGameObject, CGameObject, Children, Attach, GetChild, GetChildCount)

				D2D_TYPE_INFO_DECLARE_DERIVED(CPlaceableComponent, CGameObject)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, float, Angle)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, Vector2, Position)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, Vector2, Scaling)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, CBox, Box)
					D2D_DECLARE_PROPERTY_INFO(CPlaceableComponent, int, Layer)

				D2D_TYPE_INFO_DECLARE_DERIVED(CRenderableComponent, CGameObject)
					D2D_DECLARE_PROPERTY_INFO(CRenderableComponent, RGBAf, Color)
					D2D_DECLARE_PTR_PROPERTY_INFO(CRenderableComponent, CModel, Model)

					D2D_TYPE_INFO_DECLARE_DERIVED(CText, CRenderableComponent)
						D2D_DECLARE_PROPERTY_INFO(CText, string, Text)
						D2D_DECLARE_PTR_PROPERTY_INFO(CText, CFont, Font)

};	// namespace Deku2D
