#include "2de_Geometry.h"
#include <limits>

namespace Deku2d
{
	//////////////////////////////////////////////////////////////////////////
	// CGeometry

	CGeometry::CGeometry() : Box() {}

	Vector2 CGeometry::SupportMapping(const Vector2 &Direction)
	{
		return Box.Center() + Vector2(	Sign(Direction.x) * Box.Width() * 0.5f,
			Sign(Direction.y) * Box.Height() * 0.5f);
	}

	//////////////////////////////////////////////////////////////////////////
	// CCircle

	CCircle::CCircle(Vector2 APosition, float ARadius) : Position(APosition), Radius(ARadius)
	{
		CalcBox();
	}

	float CCircle::GetRadius() const
	{
		return Radius;
	}

	void CCircle::CalcBox()
	{
		Box.Min = Vector2(Position.x - Radius, Position.y - Radius);
		Box.Min = Vector2(Position.x + Radius, Position.y + Radius);
	}

	//////////////////////////////////////////////////////////////////////////
	// CPolygon

	void CPolygon::Reset(unsigned AVerticesCount)
	{
		delete [] Vertices;
		VerticesCount = AVerticesCount;
		Vertices = new Vector2[AVerticesCount];
	}

	void CPolygon::CalcBox()
	{
		assert(VerticesCount != 0 && Vertices != NULL);

		Box.Min.x = std::numeric_limits<float>::max();
		Box.Min.y = std::numeric_limits<float>::max();
		Box.Max.x = std::numeric_limits<float>::min();
		Box.Max.y = std::numeric_limits<float>::min();

		for (unsigned i = 0; i < VerticesCount; i++)
		{
			Box.Add(Vertices[i]);
		}
	}

	CPolygon::CPolygon(unsigned AVerticesCount /* = 0 */) : VerticesCount(AVerticesCount), Vertices(NULL)
	{
		if (AVerticesCount == 0)
			return;
		Vertices = new Vector2 [VerticesCount];
		memset(Vertices, 0, sizeof(Vertices));
	}

	CPolygon::~CPolygon()
	{
		delete [] Vertices;
	}

	void CPolygon::AddVertex(const Vector2 &Vertex)
	{
		Vector2 *TempVertices = new Vector2 [VerticesCount + 1];
		for (unsigned i = 0; i < VerticesCount; i++)
			TempVertices[i] = Vertices[i];
		TempVertices[VerticesCount++] = Vertex;
		delete [] Vertices;
		Vertices = TempVertices;
	}

	Vector2& CPolygon::operator[](unsigned Index)
	{
		assert(Index >= 0 && Index < VerticesCount);
		return Vertices[Index];
	}

	const Vector2& CPolygon::operator[](unsigned Index) const
	{
		assert(Index >= 0 && Index < VerticesCount);
		return Vertices[Index];
	}

	void CPolygon::RemoveVertex(unsigned Index)
	{
		assert(Index >= 0 && Index < VerticesCount);
		Vector2 *TempVertices = new Vector2 [VerticesCount - 1];
		for (unsigned i = 0; i < VerticesCount; i++)
		{
			if (i > Index)
				TempVertices[i - 1] = Vertices[i];
			if (i < Index)
				TempVertices[i] = Vertices[i];
		}
		delete [] Vertices;
		Vertices = TempVertices;
	}

	unsigned CPolygon::GetVertexCount() const
	{
		return VerticesCount;
	}

	float CPolygon::CalcArea() const
	{
		float result = 0.0f;
		Vector2 p0;
		Vector2 p1;
		for(unsigned i = 0; i < VerticesCount; i++)
		{
			p0 = Vertices[i % VerticesCount];
			p1 = Vertices[(i + 1) % VerticesCount];
			result += (p1.x - p0.x) * 0.5f * (p1.y + p0.y);
		}
		return result;
	}

	void CPolygon::OffsetToCenter() const
	{
		Vector2 p;
		for(unsigned i = 0; i < VerticesCount; i++)
			p += Vertices[i];
		p = p / static_cast<float>(VerticesCount);
		for(unsigned i = 0; i < VerticesCount; i++)
			Vertices[i] -= p;
	}

	CPolygon CPolygon::MakeCircle( float Radius /*= 1.0f*/, unsigned Precision /*= 16*/ )
	{
		CPolygon NewPolygon(Precision);
		float angle = 0.0f;
		float _1dstuff = 1.0f / (static_cast<float>(Precision) * 0.5f);
		for (unsigned i = 0; i < Precision; i++)
		{
			angle = Const::Math::PI * static_cast<float>(i) * _1dstuff;
			NewPolygon[i] = Vector2(cos(angle), sin(angle)) * Radius;
		}
		return NewPolygon;
	}

	CPolygon CPolygon::MakeBox( float Width /*= 1.0f*/, float Height /*= 1.0f*/ )
	{
		float Width_d2 = Width * 0.5f;
		float Height_d2 = Height * 0.5f;
		CPolygon NewPolygon(4);
		NewPolygon.Reset(4);
		NewPolygon[0] = Vector2(-Width_d2, -Height_d2);
		NewPolygon[1] = Vector2(+Width_d2, -Height_d2);
		NewPolygon[2] = Vector2(+Width_d2, +Height_d2);
		NewPolygon[3] = Vector2(-Width_d2, +Height_d2);
		return NewPolygon;
	}

	bool CPolygon::IsSelfIntersects() const
	{
		for (unsigned i = 0; i < VerticesCount; i++)
		{
			unsigned j = (i + 1) % VerticesCount;
			for( unsigned k = i + 1; k < VerticesCount; k++)
			{
				unsigned l = (k + 1) % VerticesCount;
				if (AreSegmentsIntersect(Vertices[i], Vertices[j], Vertices[k], Vertices[l]))
					return true;
			}
		}
		return false;
	}

	bool CPolygon::IsConvex() const
	{
		if (VerticesCount < 4)
			return true;
		unsigned positiveCount = 0;
		unsigned negativeCount = 0;
		unsigned j, k;
		for (unsigned i = 0; i < VerticesCount; i++)
		{
			j = (i + 1) % VerticesCount;
			k = (i + 2) % VerticesCount;
			if (HalfPlaneSign(Vertices[i], Vertices[j], Vertices[k]) > 0)
				positiveCount++;
			else
				negativeCount++;
			if (positiveCount * negativeCount != 0)
				return false;
		}
		return true;
	}

}	//	namespace Deku2d
