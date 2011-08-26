#include "2de_Math.h"

#include <cstring>
#include <cstdio>
#include <limits>
#include <algorithm>

#include "2de_Geometry.h"
#include "2de_Matrix2.h"

namespace Deku2d
{
	unsigned g_seed = 152406923;
	//unsigned g_seed = time(NULL);

	static float SineTable[Const::Math::SINE_COSINE_TABLE_DIM], CosineTable[Const::Math::SINE_COSINE_TABLE_DIM];

	bool SqareEq(float a, float b, float c, float &t0, float &t1)
	{
		float d = b * b - 4.0f * a * c;
		if (d < 0.0f)
			return false;
		d = static_cast<float>(sqrt(d));
		float oo2a = 1.0f / (2.0f * a);
		t0 = (- b + d) * oo2a;
		t1 = (- b - d) * oo2a;
		if (t1 > t0)
			std::swap(t0, t1);
		return true;
	}

	void GenSinTable()
	{
		float a = 0, add = static_cast<float>(Const::Math::PI) * 2 / static_cast<float>(Const::Math::SINE_COSINE_TABLE_DIM);
		for (int i = 0; i < Const::Math::SINE_COSINE_TABLE_DIM; ++i)
		{
			SineTable[i] = static_cast<float>(sin(a));
			CosineTable[i] = static_cast<float>(cos(a));
			a += add;
		}
	}

	float fSinr(float angle){return SineTable[static_cast<int>(angle * Const::Math::radanglem) % Const::Math::SINE_COSINE_TABLE_DIM];}
	float fSind(float angle){return SineTable[static_cast<int>(angle * Const::Math::deganglem) % Const::Math::SINE_COSINE_TABLE_DIM];}
	float fSini(int index){return SineTable[index % Const::Math::SINE_COSINE_TABLE_DIM];}
	float fCosr(float angle){return CosineTable[static_cast<int>(angle * Const::Math::radanglem) % Const::Math::SINE_COSINE_TABLE_DIM];}
	float fCosd(float angle){return CosineTable[static_cast<int>(angle * Const::Math::deganglem) % Const::Math::SINE_COSINE_TABLE_DIM];}
	float fCosi(int index){return CosineTable[index % Const::Math::SINE_COSINE_TABLE_DIM];}


	// Polygons collision stuff will be here

	void GetInterval(const CPolygon &Polygon, const Vector2 &xAxis, float &min, float &max);
	bool FindMTD(Vector2* xAxis, float* taxis, int iNumAxes, Vector2& N, float& t);

	bool IntervalIntersect(const CPolygon &A,
						   const CPolygon &B,
						   const Vector2 &xAxis, 
						   const Vector2 &xOffset,
						   const Vector2 &xVel,
						   const Matrix2 &xOrient,
						   float &taxis,
						   float tmax);

	bool CPolygon::Collide(const CPolygon &A,
						   const Vector2 &Apos,
						   const Vector2 &Avel,
						   const Matrix2 &Aorient,
						   const CPolygon &B,
						   const Vector2 &Bpos,
						   const Vector2 &Bvel,
						   const Matrix2 &Borient,
						   Vector2 &n,
						   float &depth)
	{
		Matrix2 xOrient = Aorient ^ Borient;
		Vector2 xOffset = (Apos - Bpos) ^ Borient;
		Vector2 xVel    = (Avel - Bvel) ^ Borient;

		Vector2 xAxis[64]; // note : a maximum of 32 vertices per poly is supported
		float  taxis[64];
		int    iNumAxes=0;

		float fVel2 = xVel * xVel;

		if (fVel2 > 0.00001f)
		{
			xAxis[iNumAxes] = Vector2(-xVel.y, xVel.x);

			if (!IntervalIntersect(	A, 
				B, 
				xAxis[iNumAxes], 
				xOffset, xVel, xOrient,
				taxis[iNumAxes], depth))
			{
				return false;
			}
			iNumAxes++;
		}

		for (unsigned j = A.GetVertexCount() - 1, i = 0; i < A.GetVertexCount(); j = i, i ++)
		{
			Vector2 E0 = A[j];
			Vector2 E1 = A[i];
			Vector2 E  = E1 - E0;
			xAxis[iNumAxes] = Vector2(-E.y, E.x) * xOrient;

			if (!IntervalIntersect(A, B, xAxis[iNumAxes], xOffset, xVel,
				xOrient, taxis[iNumAxes], depth))
			{
				return false;
			}

			iNumAxes++;
		}

		for (unsigned j = B.GetVertexCount() - 1, i = 0; i < B.GetVertexCount(); j = i, i++)
		{
			Vector2 E0 = B[j];
			Vector2 E1 = B[i];
			Vector2 E  = E1 - E0;
			xAxis[iNumAxes] = Vector2(-E.y, E.x);

			if (!IntervalIntersect(A, B, xAxis[iNumAxes], xOffset, xVel, xOrient,
				taxis[iNumAxes], depth))
			{
				return false;
			}

			iNumAxes++;	
		}


		if (B.GetVertexCount() == 2)
		{
			Vector2 E  = B[1] - B[0];
			xAxis[iNumAxes] = E;

			if (!IntervalIntersect(A, B, xAxis[iNumAxes], xOffset, xVel, xOrient,
				taxis[iNumAxes], depth))
			{
				return false;
			}

			iNumAxes++;
		}

		if (A.GetVertexCount() == 2)
		{
			Vector2 E  = A[1] - A[0];
			xAxis[iNumAxes] = E * xOrient;

			if (!IntervalIntersect(	A, B, xAxis[iNumAxes], xOffset, xVel, xOrient,
				taxis[iNumAxes], depth))
			{
				return false;
			}

			iNumAxes++;
		}

		if (!FindMTD(xAxis, taxis, iNumAxes, n, depth))
			return false;

		if (n * xOffset < 0.0f)
			n = -n;

		n *= Borient;

		return true;
	}


	void GetInterval(const CPolygon &Polygon, const Vector2& xAxis, float& min, float& max)
	{
		min = max = (Polygon[0] * xAxis);

		for (unsigned i = 1; i < Polygon.GetVertexCount(); i ++)
		{
			float d = (Polygon[i] * xAxis);
			if (d < min) min = d; else if (d > max) max = d;
		}
	}

	bool IntervalIntersect(const CPolygon &A,
						   const CPolygon &B,
						   const Vector2 &xAxis, 
						   const Vector2 &xOffset,
						   const Vector2 &xVel,
						   const Matrix2 &xOrient,
						   float &taxis,
						   float tmax)
	{
		float min0, max0;
		float min1, max1;
		GetInterval(A, xAxis ^ xOrient, min0, max0);
		GetInterval(B, xAxis, min1, max1);

		float h = xOffset * xAxis;
		min0 += h;
		max0 += h;

		float d0 = min0 - max1;
		float d1 = min1 - max0;

		if (d0 > 0.0f || d1 > 0.0f)
		{
			float v = xVel * xAxis;

			if (fabs(v) < 0.0000001f)
				return false;

			float t0 =-d0 / v;
			float t1 = d1 / v;

			if (t0 > t1) { float temp = t0; t0 = t1; t1 = temp; }
			taxis  = (t0 > 0.0f)? t0 : t1;

			if (taxis < 0.0f || taxis > tmax)
				return false;

			return true;
		}
		else
		{
			taxis = (d0 > d1)? d0 : d1;
			return true;
		}
	}
	bool FindMTD(Vector2* xAxis, float* taxis, int iNumAxes, Vector2& N, float& t)
	{
		int mini = -1;
		t = 0.0f;
		for (int i = 0; i < iNumAxes; i ++)
		{	
			if (taxis[i] > 0)
			{
				if (taxis[i] > t)
				{
					mini = i;
					t = taxis[i];
					N = xAxis[i];
					N.Normalize();
				}
			}
		}

		if (mini != -1)
			return true;

		mini = -1;
		for (int i = 0; i < iNumAxes; i ++)
		{
			float n = xAxis[i].Normalize();
			taxis[i] /= n;

			if (taxis[i] > t || mini == -1)
			{
				mini = i;
				t = taxis[i];
				N = xAxis[i];
			}
		}

		if (mini == -1)
			printf("Error\n");

		return (mini != -1);
	}

	float HalfPlaneSign(const Vector2 &u0, const Vector2 &u1, const Vector2 &p)	// Кстати, это площадь тругольника на этих трёх точках. // Или параллелограма.
	{
		return	(u0.x - p.x) * (u1.y - p.y) - 
				(u0.y - p.y) * (u1.x - p.x);
	}

	bool IntersectLines(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result)
	{
		float a1 = u1.y - u0.y;
		float b1 = u0.x - u1.x;
		float a2 = v1.y - v0.y;
		float b2 = v0.x - v1.x;
		Matrix2 deltaMatrix(a1, b1, a2, b2);
		float deltaDet = deltaMatrix.Determinant();
		if (Equal(deltaDet, 0.0f))
			return false;	// Прямые параллельны, т.е. a1b2 - a2b1 == 0; Кстати, условие перпендикулярности: a1a2 == -b1b2;
							// @todo А что, если прямые совпадают?
		float c1 = u1.y * u0.x - u1.x * u0.y;	//a1 * u0.x + b1 * u0.y;
		float c2 = v1.y * v0.x - v1.x * v0.y;	//a2 * v0.x + b2 * v0.y;
		Result = Vector2(Matrix2(c1, b1, c2, b2).Determinant() / deltaDet,
						 Matrix2(a1, c1, a2, c2).Determinant() / deltaDet);
		return true;
	}

	bool IntersectSegments(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result)
	{
		if (HalfPlaneSign(u0, u1, v0) * HalfPlaneSign(u0, u1, v1) > 0)
			return false;
		if (HalfPlaneSign(v0, v1, u0) * HalfPlaneSign(v0, v1, u1) > 0)
			return false;
		// In the "IntersectLines" lies check if lines are parallel, but at that point we're already know that they're not
		if (!IntersectLines(u0, u1, v0, v1, Result))
			return false;
		return true;
	}

	bool AreSegmentsIntersect( const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1 )
	{
		if (HalfPlaneSign(u0, u1, v0) * HalfPlaneSign(u0, u1, v1) > 0)
			return false;
		if (HalfPlaneSign(v0, v1, u0) * HalfPlaneSign(v0, v1, u1) > 0)
			return false;
		return true;
	}

	bool IntersectCircles(const Vector2 &p0, const float r0, const Vector2 &p1, const float r1, Vector2 &Normal, float &Depth)
	{
		Vector2 p0p1 = p1 - p0;
		if ((Sqr(p0p1.x) + Sqr(p0p1.y)) >= Sqr(r0 + r1))
			return false;
		Normal = p0p1.Normalized();
		Depth = std::fabs(r1 - r0);
		return true;
	}

	bool PointInsidePolygon();

	float DistanceToLine(const Vector2 &u0, const Vector2 &u1, const Vector2 &p)
	{
		return HalfPlaneSign(u0, u1, p) / (u0 - u1).Length();
	}

	float DistanceToSegment(const Vector2 &u0, const Vector2 &u1, const Vector2 &p)
	{
		Vector2 v = u1 - u0;
		Vector2 w = p - u0;

		float c1 = w * v;
		if (c1 <= 0)
			return (p - u0).Length() * Sign(HalfPlaneSign(u0, u1, p)); // Мы же хотим получить расстояние со знаком даже если это расстояние до концов отрезка.

		float c2 = v * v;
		if (c2 <= c1)
			return (p - u1).Length() * Sign(HalfPlaneSign(u0, u1, p));

		return HalfPlaneSign(u0, u1, p) / (u0 - u1).Length();
	}

	__INLINE void CalcConvexHull(std::vector<Vector2> &a )
	{
		class CalcConvexHullHelper
		{
		public:
			static __INLINE bool cmp (Vector2 a, Vector2 b)
			{
				return a.x < b.x || a.x == b.x && a.y < b.y;
			}

			/**
			* cw and cww is some strange way to Implement HalfPlaneSign() with
			* devilish copy-paste and having 3 instead of 2 multiplications
			* @todo: replace by HalfPlaneSign
			*/
			static __INLINE bool cw (Vector2 a, Vector2 b, Vector2 c)
			{
				return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y) < 0;
			}

			static __INLINE bool ccw (Vector2 a, Vector2 b, Vector2 c)
			{
				return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y) > 0;
			}
		};

		if (a.size() == 1)  return;
		sort (a.begin(), a.end(), &CalcConvexHullHelper::cmp);
		Vector2 p1 = a[0],  p2 = a.back();
		std::vector<Vector2> up, down;
		up.push_back (p1);
		down.push_back (p1);
		for (size_t i=1; i<a.size(); ++i)
		{
			if (i==a.size()-1 || CalcConvexHullHelper::cw (p1, a[i], p2))
			{
				while (up.size()>=2 && !CalcConvexHullHelper::cw (up[up.size()-2], up[up.size()-1], a[i]))
					up.pop_back();
				up.push_back (a[i]);
			}
			if (i==a.size()-1 || CalcConvexHullHelper::ccw (p1, a[i], p2))
			{
				while (down.size()>=2 && !CalcConvexHullHelper::ccw (down[down.size()-2], down[down.size()-1], a[i]))
					down.pop_back();
				down.push_back (a[i]);
			}
		}
		a.clear();
		for (size_t i=0; i<up.size(); ++i)
			a.push_back (up[i]);
		for (size_t i=down.size()-2; i>0; --i)
			a.push_back (down[i]);
	}

}	//	namespace Deku2d
