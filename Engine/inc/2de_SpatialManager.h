#ifndef _2DE_SPATIAL_MANAGER_H_
#define _2DE_SPATIAL_MANAGER_H_

#include "2de_Core.h"
#include "2de_Math.h"

/**
*	CAbstractSpatialManager - abstract data structure, which can support us with list of
*	objects in some region, or pairs of potentially colliding objects.
*	Spatial manager (or even spatial database) must support:
*	a) k-nearest objects query
*	b) region query
*	b) Update the db to reflect changes to objects positions
*/

class CAbstractSpatialManager
{
public:
	virtual void PushBox(CObject *Host, const CBox &Box) = 0;
	virtual void GetObjectsInRegion(const CBox &Region, vector<CObject *> &Destination) = 0;

	/**
	*	GetPCS(): PCS - Potentially Colliding Set
	*	For now it returns pairs of objects to be tested for collision i.e. Destination[0] with Destination[1]
	*	D[2] with D[3] etc.
	*/	
	virtual void GetPCS(const CBox &Region, vector<CObject *> &Destination) = 0;
	virtual void GetKNearestNeighbours(const Vector2 &Position, vector<CObject *> &Destination) = 0;
	virtual void Clear() = 0;
	virtual void Update() = 0;
};

/**
*	Brute-force implementation. Test all pairs, all objects etc.	
*/

class CBruteForceSpatialManager : public CAbstractSpatialManager
{
public:
	virtual void PushBox(CObject *Host, const CBox &Box);
	virtual void GetObjectsInRegion(const CBox &Region, vector<CObject *> &Destination);
	virtual void GetPCS(const CBox &Region, vector<CObject *> &Destination);
	virtual void GetKNearestNeighbours(const Vector2 &Position, vector<CObject *> &Destination);

	virtual void Clear();
	virtual void Update();

private:
	typedef std::vector<std::pair<CObject *, CBox> > DataT;
	DataT Data;	
};

/**
*	blah blah blah
*/

class CUniformGridSpatialManager : public CAbstractSpatialManager
{

};

// @todo: implement and test performance of UniformGrid, HierarchicalGrid, KDTree, BVH, QuadTree


#endif // _2DE_SPATIAL_MANAGER_H_