#include "2de_Box.h"

namespace Deku2d
{
	//////////////////////////////////////////////////////////////////////////
	// CBox
	CBox::CBox() : Min(Const::Math::V2_ZERO), Max(Const::Math::V2_ZERO) {}

	CBox::CBox(const Vector2 &AMin, const Vector2 &AMax) : Min(AMin), Max(AMax)
	{
		//assert(Min.x <= Max.x && Min.y <= Max.y);
	}

	CBox::CBox(float xmin, float ymin, float xmax, float ymax) : Min(xmin, ymin), Max(xmax, ymax)
	{
		//assert(xmin <= xmax && ymin <= ymax);
	}

	CBox::CBox(int x, int y, unsigned width, unsigned height) : 
	Min(static_cast<float>(x), static_cast<float>(y)),
		Max(static_cast<float>(x + width), static_cast<float>(y + height)) {}

	CBox::CBox(const Vector2 &Center, float Width, float Height) : Min(Center), Max(Center)
	{
		assert(Width >= 0.0f && Height >= 0.0f);
		Vector2 tempWHd2 = Vector2(Width, Height) * 0.5f;
		Min -= tempWHd2, Max += tempWHd2;
	}

	CBox::CBox(const Vector2 &Center, float Radius) : Min(Center), Max(Center)
	{
		assert(Radius >= 0.0f);
		Vector2 tempRRd2 = Vector2(Radius, Radius) * 0.5f;
		Min -= tempRRd2, Max += tempRRd2;
	}

	CBox::CBox(const Vector2Array<4> &Vertices) : Min(Vertices[0]), Max(Vertices[2])
	{
		assert(Min.x <= Max.x && Min.y <= Max.y);
	}

	void CBox::Add(const Vector2 &Point)
	{
		if (Point.x > Max.x)
			Max.x = Point.x;
		if (Point.x < Min.x)
			Min.x = Point.x;
		if (Point.y > Max.y)
			Max.y = Point.y;
		if (Point.y < Min.y)
			Min.y = Point.y;
	}

	void CBox::Offset(float dx, float dy)
	{
		Offset(Vector2(dx, dy));	
	}

	void CBox::Offset(const Vector2 &Delta)
	{
		Min += Delta, Max += Delta;
	}
	CBox CBox::Offsetted(float dx, float dy) const
	{
		return Offsetted(Vector2(dx, dy));
	}

	CBox CBox::Offsetted(const Vector2 &Delta) const
	{
		return CBox(Min + Delta, Max + Delta);	
	}
	void CBox::Inflate(float dx, float dy)
	{
		Vector2 Delta(dx, dy);
		Min -= Delta, Max += Delta;
	}

	CBox CBox::Inflated(float dx, float dy) const
	{
		Vector2 Delta(dx, dy);
		return CBox(Min - Delta, Max + Delta);	
	}

	bool CBox::Inside(const Vector2 &Point) const
	{
		if (Point.x >= Max.x || Point.x <= Min.x || Point.y >= Max.y || Point.y <= Min.y)
			return false;
		return true;
	}

	bool CBox::Inside(const Vector2 &Point, float &MTD) const
	{
		if (Point.x >= Max.x || Point.x <= Min.x || Point.y >= Max.y || Point.y <= Min.y)
			return false;
		float d1, d2, d3, d4;
		d1 =  Max.x - Point.x;
		d2 =  Max.y - Point.y;
		d3 = -Min.x + Point.x;
		d4 = -Min.y + Point.y;
		MTD = std::min(std::min(d1, d2), std::min(d3, d4));
		return true;
	}

	bool CBox::Inside(const Vector2 &Point, float &MTD, Vector2 &n) const
	{
		if (Point.x >= Max.x || Point.x <= Min.x || Point.y >= Max.y || Point.y <= Min.y)
			return false;

		//DistanceToLine
		float d1, d2, d3, d4;
		d1 =  Max.x - Point.x;
		d2 =  Max.y - Point.y;
		d3 = -Min.x + Point.x;
		d4 = -Min.y + Point.y;
		MTD = std::min(std::min(d1, d2), std::min(d3, d4));

		if (MTD == d1)
			n = Vector2(1.0f, 0.0f);
		if (MTD == d2)
			n = Vector2(0.0f, 1.0f);
		if (MTD == d3)
			n = Vector2(-1.0f, 0.0f);
		if (MTD == d4)
			n = Vector2(0.0f, -1.0f);

		return true;
	}

	bool CBox::Outside(const Vector2 &Point, float &MTD, Vector2 &n) const
	{
		if (Inside(Point))
			return false;

		//DistanceToLine
		float d1, d2, d3, d4;
		d1 = -Max.x + Point.x;
		d2 = -Max.y + Point.y;
		d3 =  Min.x - Point.x;
		d4 =  Min.y - Point.y;
		MTD = std::min(std::min(d1, d2), std::min(d3, d4));

		if (MTD == d1)
			n = Vector2(1.0f, 0.0f);
		if (MTD == d2)
			n = Vector2(0.0f, 1.0f);
		if (MTD == d3)
			n = Vector2(-1.0f, 0.0f);
		if (MTD == d4)
			n = Vector2(0.0f, -1.0f);

		return true;
	}

	bool CBox::Intersect(const CBox &box) const
	{
		if (box.Min.x >= Max.x)
			return false;
		if (box.Min.y >= Max.y)
			return false;
		if (box.Max.x <= Min.x)
			return false;
		if (box.Max.y <= Min.y)
			return false;

		return true;
	}

	void CBox::Union(const CBox &other)
	{
		Add(other.Max);
		Add(other.Min);
	}

	float CBox::Width() const
	{
		return Max.x - Min.x;
	}

	float CBox::Height() const
	{
		return Max.y - Min.y;
	}

	Vector2Array<4> CBox::GetVertices() const
	{
		Vector2Array<4> Result;
		Result[0] = Min;
		Result[1] = Vector2(Max.x, Min.y);
		Result[2] = Max;
		Result[3] = Vector2(Min.x, Max.y);;
		return Result;
	}

	Vector2 CBox::Center()
	{
		return (Min + Max) * 0.5f;
	}

}	//	namespace Deku2d
