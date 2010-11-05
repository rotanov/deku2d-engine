#ifndef _2DE_PHYSICS_H_
#define _2DE_PHYSICS_H_

#include "2de_Core.h"
#include "2de_Math.h"
#include "2de_Update.h"

class CPhysicalObject /*: public CUpdatable*/
{
public:
	CBox *box;
	CPhysicalObject(){};
	virtual ~CPhysicalObject(){};
};

class CStaticBody : public CPhysicalObject
{
public:
	Vector2 p;
};

class CDynamicBody : public CPhysicalObject
{
public:
	Vector2 v;
};

class CRigidBody : public CPhysicalObject
{
public:
	CGeometry G;	// Геометрическая интерпретация формы объекта	

	Vector2 p;
	float	a;
	Matrix2 o;

	float	invMass;
	float	inertiaMoment;

	Vector2 lv;
	Vector2 la;

	float	av;
	float	aa;
	
	void IntegrateVelocity(float dt);

	void IntegratePosition(float dt);

	void ApplyForce(const Vector2 force, const Vector2 point) //применить силу к точке
	{
		
	}
	void ApplyForce2(const Vector2 linearForce, const Vector2 torque) //применить силу и момент сил
	{

	}
};

class CPhysicsManager // ... 
{
//	... 
};


#endif // _2DE_PHYSICS_H_
