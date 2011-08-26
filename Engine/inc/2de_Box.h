#ifndef _2DE_BOX_H_
#define _2DE_BOX_H_

#include "2de_Vector2.h"

namespace Deku2d
{
	/**
	*	CBox - rectangle representation for common needs.	
	*	It is always Axis Aligned.
	*	If you need more general rectangle - use CRentangle geometry representation
	*/
	class CBox
	{
	public:
		Vector2 Min, Max;

		CBox();
		CBox(const Vector2 &AMin, const Vector2 &AMax);
		CBox(float xmin, float ymin, float xmax, float ymax);
		CBox(int x, int y, unsigned width, unsigned height);
		CBox(const Vector2 &Center, float Width, float Height);
		CBox(const Vector2 &Center, float Radius);
		CBox(const Vector2Array<4> &Vertices);
		
		void Add(const Vector2 &Point);
		Vector2 Center();
		float Height() const;
		void Inflate(float x, float y);
		CBox Inflated(float x, float y) const;
		bool Inside(const Vector2 &Point) const;
		bool Inside(const Vector2 &Point, float &MTD) const;
		bool Inside(const Vector2 &Point, float &MTD, Vector2 &n) const;
		bool Intersect(const CBox &box) const;
		void Offset(float dx, float dy);
		void Offset(const Vector2 &Delta);
		CBox Offsetted(float dx, float dy) const;
		CBox Offsetted(const Vector2 &Delta) const;
		bool Outside(const Vector2 &Point, float &MTD, Vector2 &n) const;
		void Union(const CBox &other);
		float Width() const;

		/**
		*	Returns box vertices in such order: Lower-left, Lower-right, Upper-Right, Upper-Left;
		*/
		Vector2Array<4> GetVertices() const;
	};

}	//	namespace Deku2d

#endif	//	_2DE_BOX_H_
