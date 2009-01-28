#ifndef PHYSICS_H
#define PHYSICS_H

#include "CoreUtils.h"
#include "MathUtils.h"

class CPhysicalObject
{
public:
	Vector2 p;
	Vector2 v;
	//... some physical stuff here
	CRecti *bbox;
	//CColisionStruct *Coll;
	CPhysicalObject *next;

	// methods
	CPhysicalObject(){};
	virtual ~CPhysicalObject(){};
};

class CPhysicsManager
{
//	... 
};


#endif PHYSICS_H