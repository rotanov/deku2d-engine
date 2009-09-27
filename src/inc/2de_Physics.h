#ifndef PHYSICS_H
#define PHYSICS_H

#include "2de_Core.h"
#include "2de_MathUtils.h"

class CPhysicalObject : public CUpdateObject
{
public:
	CAABB *box;
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
	scalar	a;
	Matrix2 o;

	scalar	invMass;
	scalar	inertiaMoment;

	Vector2 lv;
	Vector2 la;

	scalar	av;
	scalar	aa;
	
	void IntegrateVelocity(float dt)
	{
		lv += la * dt;
		av += aa * dt; 
	}

	void IntegratePosition(float dt)
	{
		p += lv * dt;
		a += av * dt;
		a = clampf(a, 0.0f, PI*2);
		o = Matrix2(a);
	}

	void ApplyForce(const Vector2 force, const Vector2 point) //применить силу к точке
	{
		
	}
	void ApplyForce2(const Vector2 linearForce, const Vector2 torque) //применить силу и момент сил
	{

	}
};

class CPhysicsManager : public CList
{
//	... 
};


#endif PHYSICS_H