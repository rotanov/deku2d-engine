#include "2de_Physics.h"

namespace Deku2d
{
	void CRigidBody::IntegrateVelocity(float dt)
	{
		lv += la * dt;
		av += aa * dt;
	}

	void CRigidBody::IntegratePosition(float dt)
	{
		p += lv * dt;
		a += av * dt;
		a = Clamp(a, 0.0f, PI*2);
		o = Matrix2(a);
	}
}	//	namespace Deku2d