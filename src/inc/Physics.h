#ifndef PHYSICS_H
#define PHYSICS_H

#include "CoreUtils.h"
#include "MathUtils.h"

class CPhysicalObject : public CUpdateObject
{
public:
	Vector2 p;
	Vector2 v;

	CGeometry G;	// �������������� ������������� ����� �������	
	
	CBBox *box;

	CPhysicalObject(){};
	virtual ~CPhysicalObject(){};
};

class CPhysicsManager : public CObjectList
{
//	... 
};


#endif PHYSICS_H