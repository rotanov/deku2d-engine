#include "2de_MathUtils.h"

unsigned int g_seed = 152406923;
static float SineTable[SINE_COSINE_TABLE_DIM], CosineTable[SINE_COSINE_TABLE_DIM];

int Random_Int(int min, int max)
{
	g_seed = 214013*g_seed + 2531011;
	return min + (g_seed^g_seed>>15) % (max - min + 1);
}

float Random_Float(float min, float max)
{
	g_seed = 214013 * g_seed + 2531011;
	//return min + g_seed * (1.0f / 4294967295.0f) * (max - min);
	return min + (g_seed >> 16) * (1.0f / 65535.0f) * (max - min);
}

Vector2 Vector2::operator *(const Matrix2& M) const
{
	Vector2 T;
	T.x = x * M.e11 + y * M.e12;
	T.y = x * M.e21 + y * M.e22;
	return T;
}

Vector2 Vector2::operator ^(const Matrix2& M) const
{
	Vector2 T;
	T.x = x * M.e11 + y * M.e21;
	T.y = x * M.e12 + y * M.e22;
	return T;
}

Vector2& Vector2::operator *=(const Matrix2& M)
{
	Vector2 T = *this;
	x = T.x * M.e11 + T.y * M.e12;
	y = T.x * M.e21 + T.y * M.e22;
	return *this;
}

Vector2& Vector2::operator ^=(const Matrix2& M)
{
	Vector2 T = *this;
	x = T.x * M.e11 + T.y * M.e21;
	y = T.x * M.e12 + T.y * M.e22;
	return *this;
}

bool SqareEq(float a, float b, float c, float &t0, float &t1)
{
	float d = b * b - 4.0f * a * c;
	if (d < 0)	return false;
	d = static_cast<float>(sqrt(d));
	float oo2a = 1 / (2.0f * a);
	t0 = (- b + d) * oo2a;
	t1 = (- b - d) * oo2a;
	if (t1 > t0)
	{
		float t = t0;
		t0 = t1;
		t1 = t;
	}
	return true;
}

void GenSinTable()
{
	float a = 0, add = static_cast<float>(PI) * 2 / static_cast<float>(SINE_COSINE_TABLE_DIM);
	for (int i = 0; i < SINE_COSINE_TABLE_DIM; ++i)
	{
		SineTable[i] = static_cast<float>(sin(a));
		CosineTable[i] = static_cast<float>(cos(a));
		a += add;
	}
}

float fSinr(float angle){return SineTable[static_cast<int>(angle * radanglem) % SINE_COSINE_TABLE_DIM];}
float fSind(float angle){return SineTable[static_cast<int>(angle * deganglem) % SINE_COSINE_TABLE_DIM];}
float fSini(int index){return SineTable[index % SINE_COSINE_TABLE_DIM];}
float fCosr(float angle){return CosineTable[static_cast<int>(angle * radanglem) % SINE_COSINE_TABLE_DIM];}
float fCosd(float angle){return CosineTable[static_cast<int>(angle * deganglem) % SINE_COSINE_TABLE_DIM];}
float fCosi(int index){return CosineTable[index % SINE_COSINE_TABLE_DIM];}


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

	for (int j = A.GetVertexCount() - 1, i = 0; i < A.GetVertexCount(); j = i, i ++)
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

	for (int j = B.GetVertexCount() - 1, i = 0; i < B.GetVertexCount(); j = i, i++)
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

	for (int i = 1; i < Polygon.GetVertexCount(); i ++)
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
	return (u0.x - p.x) * (u1.y - p.y) - (u0.y - p.y) * (u1.x - p.x);
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
	if (!IntersectLines(u0, u1, v0, v1, Result))
		return false;
	return true;
}

bool IntersectCircles(const Vector2 &p0, const float r0, const Vector2 &p1, const float r1, Vector2 &Normal, float &Depth)
{
	Vector2 p0p1 = p1 - p0;
	if ((Sqr(p0p1.x) + Sqr(p0p1.y)) >= Sqr(r0 + r1))
		return false;
	Normal = p0p1.Normalized();
	Depth = Abs(r1 - r0);
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

//////////////////////////////////////////////////////////////////////////
// CAABB

CBox::CBox() : Min(V2_ZERO), Max(V2_ZERO)
{

}

CBox::CBox(const Vector2 &AMin, const Vector2 &AMax) : Min(AMin), Max(AMax)
{

}

CBox::CBox(float xmin, float ymin, float xmax, float ymax) : Min(xmin, ymin), Max(xmax, ymax)
{

}

CBox::CBox(int x, int y, unsigned int width, unsigned int height) : Min(x, y), Max(x + width, y + height)
{

}

void CBox::Add(const Vector2 &point)
{
	if (point.x > Max.x)
		Max.x = point.x;
	if (point.x < Min.x)
		Min.x = point.x;
	if (point.y > Max.y)
		Max.y = point.y;
	if (point.y > Min.y)
		Min.y = point.y;
}

void CBox::Add(float x, float y)
{
	Add(Vector2(x, y));
}
void CBox::Offset(float dx, float dy)
{
	Min.x += dx;
	Max.x += dx;
	Min.y += dy;
	Max.y += dy;
}

CBox CBox::Offsetted(float dx, float dy) const
{
	CBox tmp;
	tmp.Min.x = Min.x + dx;
	tmp.Max.x = Max.x + dx;
	tmp.Min.y = Min.y + dy;
	tmp.Max.y = Max.y + dy;
	return tmp;
}

void CBox::Inflate(float x, float y)
{
	Min.x -=x;
	Max.x +=x;
	Min.y -=y;
	Max.y +=y;
}

CBox CBox::Inflated(float x, float y) const
{
	CBox Result(Min.x-x, Min.y-y, Max.x+x, Max.y+y);
	return Result;
}

bool CBox::Inside(const Vector2 &point) const
{
	if (point.x >= Max.x || point.x <= Min.x)
		return false;
	if (point.y >= Max.y || point.y <= Min.y)
		return false;
	return true;
}

bool CBox::Inside(const Vector2 &point, float &MTD) const /* MTD - is minimal translation distance */
{
	if (point.x >= Max.x || point.x <= Min.x)
		return false;
	if (point.y >= Max.y || point.y <= Min.y)
		return false;

	//DistanceToLine
	float d1, d2, d3, d4;
	d1 =  Max.x - point.x;
	d2 =  Max.y - point.y;
	d3 = -Min.x + point.x;
	d4 = -Min.y + point.y;
	MTD = std::min(std::min(d1, d2), std::min(d3, d4));
	return true;
}

bool CBox::Inside(const Vector2 &point, float &MTD, Vector2 &n) const /* MTD - is minimal translation distance // Ага блядь, одинаковые комменты рулят. OMG, там ещё третий внизу. */
{
	if (point.x >= Max.x || point.x <= Min.x)
		return false;
	if (point.y >= Max.y || point.y <= Min.y)
		return false;

	//DistanceToLine
	float d1, d2, d3, d4;
	d1 =  Max.x - point.x;
	d2 =  Max.y - point.y;
	d3 = -Min.x + point.x;
	d4 = -Min.y + point.y;
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

bool CBox::Outside(const Vector2 &point, float &MTD, Vector2 &n) const /* MTD - is minimal translation distance */
{
	// FIXME: warning: suggest parentheses around && within || // i was too lazy to parse this logic expression..
	if (point.x < Max.x && point.x > Min.x && point.y < Max.y || point.y > Min.y)
		return false;

	//DistanceToLine
	float d1, d2, d3, d4;
	d1 = -Max.x + point.x;
	d2 = -Max.y + point.y;
	d3 =  Min.x - point.x;
	d4 =  Min.y - point.y;
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

//////////////////////////////////////////////////////////////////////////
// CGeometry

CGeometry::CGeometry()
{
	Box  = CBox(0.0f, 0.0f, 0.0f, 0.0f);
}

//////////////////////////////////////////////////////////////////////////
// CCircle

CCircle::CCircle(Vector2 APosition, float ARadius) : Position(APosition), Radius(ARadius)
{
	CalcBBOX();
}

//////////////////////////////////////////////////////////////////////////
// CPolygon

void CPolygon::Reset(unsigned int AVerticesCount)
{
	SAFE_DELETE_ARRAY(Vertices);
	Vertices = new Vector2 [VerticesCount];
}

void CPolygon::CalcBBOX()
{
	assert(VerticesCount != 0 && Vertices != NULL);
	Box.Min.x = 0xffffff; // @todo: replace by something lookubg more normal
	Box.Min.y = 0xffffff;
	Box.Max.x = -0xffffff;
	Box.Max.x = -0xffffff;
	for (int i = 0; i < VerticesCount; i++)
	{
		Box.Add(Vertices[i]);
	}
}

CPolygon::CPolygon(unsigned int AVerticesCount) : VerticesCount(AVerticesCount)
{
	Vertices = new Vector2 [VerticesCount];
	memset(Vertices, 0, sizeof(Vertices));
}

CPolygon::CPolygon() : VerticesCount(0), Vertices(NULL)
{

}

CPolygon::~CPolygon()
{
	SAFE_DELETE_ARRAY(Vertices);
}

void CPolygon::AddVertex(const Vector2 &Vertex)
{
	Vector2 *TempVertices = new Vector2 [VerticesCount + 1];
	for (unsigned int i = 0; i < VerticesCount; i++)
		TempVertices[i] = Vertices[i];
	TempVertices[VerticesCount++] = Vertex;
	SAFE_DELETE_ARRAY(Vertices);
	Vertices = TempVertices;
}

Vector2& CPolygon::operator[](unsigned int Index)
{
	assert(Index >= 0 && Index < VerticesCount);
	return Vertices[Index];
}

const Vector2& CPolygon::operator[](unsigned int Index) const
{
	assert(Index >= 0 && Index < VerticesCount);
	return Vertices[Index];
}

void CPolygon::RemoveVertex(unsigned int Index)
{
	assert(Index >= 0 && Index < VerticesCount);
	Vector2 *TempVertices = new Vector2 [VerticesCount - 1];
	for (unsigned int i = 0; i < VerticesCount; i++)
	{
		if (i > Index)
			TempVertices[i - 1] = Vertices[i];
		if (i < Index)
			TempVertices[i] = Vertices[i];
	}
	SAFE_DELETE_ARRAY(Vertices);
	Vertices = TempVertices;
}

unsigned int CPolygon::GetVertexCount() const
{
	return VerticesCount;
}
