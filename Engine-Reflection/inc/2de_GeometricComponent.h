#ifndef _2DE_GEOMETRIC_COMPONENT_H_
#define _2DE_GEOMETRIC_COMPONENT_H_

#include "2de_GameObject.h"

namespace Deku2D
{
	/**
	 *	CGeometricComponent - class of component that represents geometrical interpretation of some object
	 *	It has to have some connection with something. Obvious "something" in this case is a connection with
	 *	corresponding physical component. But there are some questions:
	 *		a) Is there some other, non-physical component?
	 *		b) How to support this kind of relation?
	 */
	class CGeometricComponent : public CGameObject	// "Geometric" or "Geometrical"?
	{
		D2D_DECLARE_VISITABLE()

	public:
		CGeometricComponent() : Box(0, 0, 0, 0)
		{
			ClassName = "GeometricComponent";
		}

		// Danger: When we use it as arg to DrawSolidBox() then it apply scaling two times. @todo: fix this <--
		// why const CBox? it's a copy of CBox object, not a reference, why should it be const?..	
		const CBox GetBox() const
		{
			CBox TempBox = Box;
	//		TempBox.Min *= GetScaling();	// FUCK FUCK FUCK
	//		TempBox.Max *= GetScaling();
	//		TempBox.Offset(GetPosition());
			//Box.RotateByAngle(Angle);
			return TempBox;
		}

		virtual void SetBox(const CBox &ABox)
		{
			Box = ABox;
		}

		float Width()
		{
			return Box.Width();
		}

		float Height()
		{
			return Box.Height();
		}
		

	private:
		CBox Box; //	Axis Aligned Bounding Box for culling
	};

}	//	namespace Deku2D

#endif	//	_2DE_GEOMETRIC_COMPONENT_H_
