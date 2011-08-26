#ifndef _2DE_GEOMETRY_H_
#define _2DE_GEOMETRY_H_

#include "2de_Math.h"
#include "2de_Vector2.h"
#include "2de_Box.h"

namespace Deku2d
{
	//////////////////////////////////////////////////////////////////////////
	//	CGeometry
	class CGeometry
	{
	public:
		CGeometry();
		virtual Vector2 SupportMapping(const Vector2 &Direction);
		virtual const CBox& GetBox() const
		{
			return Box;
		}
		virtual void CopyVertices(Vector2 *Destination, unsigned &VerticesCount)
		{
			if (Destination == NULL)
				return;
			Vector2Array<4> Vertices = Box.GetVertices();
			for(VerticesCount = 0; VerticesCount < 4; VerticesCount++)
				Destination[VerticesCount] = Vertices[VerticesCount];
		}
	protected:
		CBox Box;
		virtual void CalcBox(){}
	};

	//////////////////////////////////////////////////////////////////////////
	// CCircle
	class CCircle : public CGeometry
	{
		Vector2 Position;
		float Radius;
	public:
		CCircle(Vector2 APosition, float ARadius);
		float GetRadius() const;
		void CalcBox();
	};

	//////////////////////////////////////////////////////////////////////////
	//CPolygon @todo: decide: should we update BBOX each time, we add or remove vertices
	// or just leave it with possibility to update BBOX each time we need it correct
	class CPolygon : public CGeometry
	{
	public:
		CPolygon(unsigned AVerticesCount = 0);
		~CPolygon();
		void Reset(unsigned AVerticesCount);
		void CalcBox();
		void AddVertex(const Vector2 &Vertex);
		void RemoveVertex(unsigned Index);
		Vector2& operator [](unsigned Index);
		const Vector2& operator [](unsigned Index) const;
		unsigned GetVertexCount() const;
		float CalcArea() const;
		void OffsetToCenter() const;
		// Checks if polygon convex and oriented (either clock- or counter clock- wise)
		// Hope it will be optimized by compiler
		bool IsConvex() const;
		// BRUTAL HARSH CRUCIAL FORCE ALGORITHM Which runs in O(n^2)
		bool IsSelfIntersects() const;
		static CPolygon MakeCircle(float Radius = 1.0f, unsigned Precision = 16);
		static CPolygon MakeBox(float Width = 1.0f, float Height = 1.0f);

		static bool	Collide	(	const CPolygon &A,
			const Vector2 &Apos,
			const Vector2 &Avel,
			const Matrix2 &Aorient,
			const CPolygon &B,
			const Vector2 &Bpos, 
			const Vector2 &Bvel, 
			const Matrix2 &Borient,
			Vector2& n,
			float& depth);
	private:
		unsigned VerticesCount;
		Vector2 *Vertices;
	};

	class CRectangle : public CGeometry
	{
	public:
		// влепить сюда union, когда будешь дома, а не в больничке @todo
		Vector2 v0;	// Lower left 
		Vector2 v1;	// Lower right
		Vector2 v2;	// Upper right
		Vector2 v3;	// Upper left

		CRectangle(const Vector2 &Min, const Vector2 &Max)
		{
			assert(Min.x <= Max.x);
			assert(Min.y <= Max.y);
			v0 = Min;
			v1 = Vector2(Max.x, Min.y);
			v2 = Max;
			v3 = Vector2(Min.x, Max.y);
		}
		CRectangle(const Vector2 &Av0, const Vector2 &Av1, const Vector2 &Av2, const Vector2 &Av3) :
		v0(Av0), v1(Av1), v2(Av2), v3(Av3)
		{}
		CRectangle(const Vector2 &Center, float Width, float Height)
		{
			Vector2 wh_2 = Vector2(Width, Height) * 0.5f;
			v0 = Center - wh_2;
			v1 = Vector2(Center.x + Width * 0.5f, Center.y - Height * 0.5f);
			v2 = Center + wh_2;
			v3 = Vector2(Center.x - Width * 0.5f, Center.y + Height * 0.5f);
		}
		//bool CheckOrienation();
	};

	class CSegment : public CGeometry
	{
	public:
		Vector2 v0, v1;

		CSegment(const Vector2 &Av0, const Vector2 &Av1) : v0(Av0), v1(Av1)
		{
			assert(v0 != v1);
		}
	};

	// @todo: Design below-like family of Collide functions for
	// Point vs. Circle vs. Box vs. Polygon ( total of six )
	// template <typename G1, typename G2>
	// bool tCollide(	const G1 &BodyA,
	// 					const Vector2 &Apos,
	// 					const Vector2 &Avel,
	// 					const Matrix2 &Aorient,
	// 				const G2 &BodyB,
	// 					const Vector2 &Bpos, 
	// 					const Vector2 &Bvel, 
	// 					const Matrix2 &Borient,
	// 				Vector2 &Normal, float &Depth)
	// {
	// 
	// }
	// 
	// template <>
	// __INLINE bool tCollide (	const CPolygon &A,
	// 									const Vector2 &Apos,
	// 									const Vector2 &Avel,
	// 									const Matrix2 &Aorient,
	// 									const CPolygon &B,
	// 									const Vector2 &Bpos, 
	// 									const Vector2 &Bvel, 
	// 									const Matrix2 &Borient,
	// 									Vector2& Normal,
	// 									float& Depth)
	// {
	// 	return CPolygon::Collide(A, Apos, Avel, Aorient, B, Bpos, Bvel, Borient, Normal, Depth);
	// }

}	// namespace Deku2d

#endif