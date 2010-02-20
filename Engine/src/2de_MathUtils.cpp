#include <math.h>

#include "2de_MathUtils.h"

unsigned int g_seed = 152406923;

int Random_Int(int min, int max)
{
	g_seed = 214013*g_seed + 2531011;
	return min + (g_seed^g_seed>>15) % (max - min + 1);
}

float Random_Float(float min, float max)
{
	g_seed=214013*g_seed+2531011;
	//return min+g_seed*(1.0f/4294967295.0f)*(max-min);
	return min+(g_seed>>16)*(1.0f/65535.0f)*(max-min);
}


Vector2 Vector2::operator * (const Matrix2& M) const
{
	Vector2 T;
	T.x = x * M.e11 + y * M.e12;
	T.y = x * M.e21 + y * M.e22;
	return T;
}

Vector2 Vector2::operator ^ (const Matrix2& M) const
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

void CalcFrustumVertices(scalar fovy, scalar aspect, scalar znear, scalar zfar, Vector3 cpos, Vector3 cat, Vector3 cup, Vector3 v[8])
{
	scalar y1 = znear * tan(fovy*0.5f);
	scalar x1 = y1 * aspect;

	scalar y2 = zfar * tan(fovy*0.5f);
	scalar x2 = y2 * aspect;

	Vector3 nz = cat - cpos, ny = cup, nx;
	nz.Normalise();
	ny.Normalise();
	nx =  nz ^ ny;
	nx.Normalise();

	Vector3 Z,X,Y,nc, fc , Dir;
	Z = cpos - cat;
	Z.Normalise();
	X = Z ^ cup;
	X.Normalise();
	Y = Z ^ X;

	nc = cpos - Z * znear;
	fc = cpos - Z * zfar;

	v[0] = nc - Y * y1 - X * x1;
	v[1] = nc - Y * y1 + X * x1;
	v[2] = nc + Y * y1 + X * x1;
	v[3] = nc + Y * y1 - X * x1;
	
	// compute the 4 corners of the frustum on the far plane
	v[4] = fc - Y * y2 - X * x2;
	v[5] = fc - Y * y2 + X * x2;
	v[6] = fc + Y * y2 + X * x2;
	v[7] = fc + Y * y2 - X * x2;
	
}

int PointsPlaneSide(Vector3 a, Vector3 n,Vector3 offset,Matrix3 R, Vector3 *points, int pnum)
{
	scalar sign = n * (offset + points[0]*R - a);
	for(int i=1; i < pnum; i++)
	{
		scalar t = n * (offset + points[i]*R - a);
		if ((t >= 0 && sign <= 0) || (t <= 0 && sign >= 0)) return 0;
		sign = t;
	}
	return (sign > 0)?1:-1;
	return 0;
}
int PointPlanesSide(Vector3 *a, Vector3 *n,int *iV, Vector3 offset, Matrix3 R, Vector3 point, int fnum, Vector3 &normal, float &depth)
{
	scalar d = 1; 
	scalar min = 99999999;
	for(int i = 0; i < fnum; i++)
	{
		d = n[i] * (offset + point - a[iV[i*3]]); // *R
		if (min > abs(d))
		{
			min = abs(d);
			depth = min;
			normal = n[i];
		}
		if ( d > 0 ) 
			return -1;
	}
	if (d <= 0) return min;
}

int PointPlanesSideEx(Vector3 *a, Vector3 *n, unsigned short *iV, Vector3 offset, Matrix3 R, Vector3 point, int fnum, Vector3 &normal, float &depth, Vector3 scaling)
{
	scalar d = 1; 
	scalar min = 99999999;
	for(int i = 0; i < fnum; i++)
	{
		d = (n[i]*R)  
			* 
			(-offset + point - 
			Vector3(a[iV[i*3]].x*scaling.x,
					a[iV[i*3]].y*scaling.y,
					a[iV[i*3]].z*scaling.z)  
				*R); 
		if (min > abs(d))
		{
			min = abs(d);
			depth = min;
			normal = n[i]*R;
		}
		if ( d > 0 ) 
			return -1;
	}
	if (d <= 0) return min;
}

scalar FindMTD(Vector3 a, Vector3 n,Vector3 offset,Matrix3 R, Vector3 *points, int pnum)
{
	scalar sign = n * (offset + points[0]*R - a);
	for(int i=1; i < pnum; i++)
	{
		scalar t = n * (offset + points[i]*R - a);
		if (t < sign)
		sign = t;
	}
	return sign;
}

 bool CullBox(Vector3 _min, Vector3 _max,Vector3 pos, Vector3 scaling, Matrix3 R,
                        scalar fovy, scalar aspect, scalar znear, scalar zfar,
                        Vector3 cpos, Vector3 cat, Vector3 cup)
{
    Vector3 v[8];
    Vector3 p[8];
    _min.x*=scaling.x;
    _min.y*=scaling.y;
    _min.z*=scaling.z;
    _max.x*=scaling.x;
    _max.y*=scaling.y;
    _max.z*=scaling.z;
    p[0] = _min;
    p[1] = Vector3(_max.x, _min.y, _min.z);
    p[2] = Vector3(_max.x, _max.y, _min.z);
    p[3] = Vector3(_min.x, _max.y, _min.z);
    p[4] = Vector3(_min.x, _min.y, _max.z);
    p[5] = Vector3(_max.x, _min.y, _max.z);
    p[6] = Vector3(_max.x, _max.y, _max.z);
    p[7] = Vector3(_min.x, _max.y, _max.z);
    
    CalcFrustumVertices(fovy, aspect, znear, zfar, cpos, cat, cup, v);
    scalar sign[6];
    sign[0] = PointsPlaneSide(v[0], CalcNorm(v[0], v[1], v[2]), pos, //Vector3(0,0,0)
                                R ,p, 8);
    sign[1] = PointsPlaneSide(v[1], CalcNorm(v[1], v[5], v[6]), pos, 
                                R ,p, 8);
    sign[2] = PointsPlaneSide(v[0], CalcNorm(v[0], v[3], v[7]), pos, 
                                R ,p, 8);
    sign[3] = PointsPlaneSide(v[0], CalcNorm(v[0], v[4], v[5]), pos, 
                                R ,p, 8);
    sign[4] = PointsPlaneSide(v[3], CalcNorm(v[3], v[7], v[6]), pos, 
                                R ,p, 8);
    sign[5] = PointsPlaneSide(v[6], CalcNorm(v[6], v[7], v[4]), pos, 
                                R ,p, 8);
    if (sign[0]<0) return 0;
    if (sign[1]<0) return 0;
    if (sign[2]<0) return 0;
    if (sign[3]<0) return 0;
    if (sign[4]>0) return 0;
    if (sign[5]>0) return 0;
    return 1;
}

#define X 0
#define Y 1
#define Z 2

#define EPSILON (double) 0.00000001 
#define SIGN(x) \
    ((double) x<=EPSILON) ? (((double) x < -EPSILON) ? -1 : 0) : 1

/* These macros are used to determine where is the point in the tetrahedron */
#define OUTSIDE 0
#define EDGE1 1
#define EDGE2 2
#define EDGE3 3
#define FACE1 1
#define FACE2 2
#define FACE3 3
#define INSIDE 4

#define DETER(p1,p2,p3) \
     p1[X]*( p2[Y]*p3[Z] - p2[Z]*p3[Y] ) - \
     p1[Y]*( p2[X]*p3[Z] - p2[Z]*p3[X] ) + \
     p1[Z]*( p2[X]*p3[Y] - p2[Y]*p3[X] )

#define SIGDETER(p1,p2,p3) \
     SIGN(DETER(p1,p2,p3))

#define SIGN3D(p1,p2,p3,p4) \
     (p1[X]*((p2[Y]*p3[Z]) + (p2[Z]*p4[Y]) + (p3[Y]*p4[Z]) - \
     (p3[Z]*p4[Y]) - (p4[Z]*p2[Y]) - (p2[Z]*p3[Y]) ) ) - \
     (p2[X]*((p1[Y]*p3[Z]) + (p1[Z]*p4[Y]) + (p3[Y]*p4[Z]) - \
     (p3[Z]*p4[Y]) - (p1[Z]*p3[Y]) - (p4[Z]*p1[Y]) ) ) + \
     (p3[X]*((p1[Y]*p2[Z]) + (p2[Y]*p4[Z]) + (p1[Z]*p4[Y])- \
     (p2[Z]*p4[Y]) - (p4[Z]*p1[Y]) - (p1[Z]*p2[Y]) ) ) - \
     (p4[X]*((p1[Y]*p2[Z]) + (p2[Y]*p3[Z]) + (p1[Z]*p3[Y]) - \
     (p2[Z]*p3[Y]) - (p3[Z]*p1[Y]) - (p1[Z]*p2[Y]) ) )

#define SIGN3D2(p1,p2,p3,p4) \
      SIGN(SIGN3D(p1,p2,p3,p4))
int pointOriginalTetrahedron (Vector3 q, Vector3 v1, Vector3 v2, Vector3 v3) {

int s1,s2,s3,s4;

s1 = -SIGDETER(q,v1,v2); 
s2 = -SIGDETER(q,v2,v3);
if ((s1!=0) && (s2!=0) && (s1!=s2)) 
    return OUTSIDE;
s3=-SIGDETER(q,v3,v1);
s4= SIGN3D2(q,v1,v2,v3);
if ( (s1!=0) && (s2!=0) && (s3!=0) && (s4!=0)&& ( (s1!=s2) || (s2!=s3) || (s1!=s3) || (s1!=s4) || (s2!=s4) || (s3!=s4)) ) 
    return OUTSIDE;
if ( (s1==s2) && (s2==s3) && (s3==s4) && (s4==s1) )
    return INSIDE;
if ( (s1==0) && (s2==s3) )
    return FACE1;
if ( (s2==0) && (s1==s3) )
    return FACE2;
if ( (s3==0) && (s2==s1) )
    return FACE3;
if ( (s1==0) && (s2==0) )
    return EDGE1;
if ( (s2==0) && (s3==0) )
    return EDGE2;
if ( (s3==0) && (s1==0) )
    return EDGE3;
return OUTSIDE;
}
// -------------------------------------------------------- 
int inclusion (Vector3 *p, int *iV,  int nVert,  int nFaces,  Vector3 q) {
// P is the set of vertices of the mesh
// nFaces is the number of triangles of the mesh
// iV is the indices of every triangle, using -1 as seAposrator of faces
// q is the point to study

long int j=0; 
static int state; 
int incl=0; 
long int v=0;
int s;
//short int *vertPos,*vertNeg;// This arrays must be created once for every mesh
// If we are going to use the same mesh, the arrays must be static but initialized to 0
static short int * vertPos=(short int *) calloc (nVert,sizeof (short int));
static short int * vertNeg=(short int *)  calloc (nVert,sizeof (short int));
memset(vertPos, 0, sizeof(short int)*nVert);
memset(vertNeg, 0, sizeof(short int)*nVert);

while ( j < nFaces ) {
      /* First, we study the position of point q respect to the tetrahedron of face j */
      state = pointOriginalTetrahedron (q, (p[iV[v]]), (p[iV[v+1]]), (p[iV[v+2]]) );
      if (state==INSIDE) incl+=2*SIGDETER((p[iV[v]]),(p[iV[v+1]]),(p[iV[v+2]]));
       else if (state!=OUTSIDE) { /* The point lies on an original face or an original edge */
                s=SIGDETER((p[iV[v]]),(p[iV[v+1]]),(p[iV[v+2]]));
                 if (s>0) {
                      if (!vertPos[iV[v]+state-1]) {
                          incl+=2*s;
                          /* Mark the vertex as visited */
                          vertPos[iV[v]+state-1]=true;
                     }
                 }
                 else {
                      if (!vertNeg[iV[v]+state-1]) {
                          incl+=2*s;
                          /* Mark the vertex as visited */
                          vertNeg[iV[v]+state-1]=true;
                     }
                }
           }
           j++;
          v+=3; // Study the next triangle of the mesh
      } // while
      if (incl>0)  return INSIDE; 
       return OUTSIDE;
}


bool SqareEq( float a, float b, float c, float &t0, float &t1)
{
	float d = b * b - 4.0f * a * c;
	if ( d < 0)	return false;
	d = (float)sqrt( d);
	float oo2a = 1 / ( 2.0f * a);
	t0 = (- b + d) * oo2a;
	t1 = (- b - d) * oo2a;
	if ( t1 > t0)
	{
		float t = t0;
		t0 = t1;
		t1 = t;
	}
	return true;	
}

static float sintable[sincostable_dim], costable[sincostable_dim];

 void GenSinTable()
{
	float a = 0, add = (float)PI*2 / (float)sincostable_dim;
	for (int i=0; i<sincostable_dim; ++i){
		sintable[i] = (float)sin(a);
		costable[i] = (float)cos(a);
		a += add;
	}
}

float fSinr(float angle){return sintable[(int)(angle*radanglem)%sincostable_dim];}
float fSind(float angle){return sintable[(int)(angle*deganglem)%sincostable_dim];}
float fSini(int index){return sintable[index%sincostable_dim];}
float fCosr(float angle){return costable[(int)(angle*radanglem)%sincostable_dim];}
float fCosd(float angle){return costable[(int)(angle*deganglem)%sincostable_dim];}
float fCosi(int index){return costable[index%sincostable_dim];}



//////////////////////////////////////////////////////////////////////////
//	Geometry

CGeometry::CGeometry()
{
	Box  = CAABB(0.0f, 0.0f, 0.0f, 0.0f);
	type = 0;
}



void CPolygon::Reset( int _numV )
{
	if (V != NULL)
		delete [] V;
	V = new Vector2 [numV];
	memset(V, 0, sizeof(V[0])*numV);
}

void CPolygon::CalcBBOX()
{
	if (numV == 0)
		return;
	if (V == NULL)
		return;
	Box.vMin.x = 0xffffff;
	Box.vMin.y = 0xffffff;
	Box.vMax.x = -0xffffff;
	Box.vMax.x = -0xffffff;
	for(int i = 0; i< numV; i++)
	{
		Box.Add(V[i]);
	}
}

CPolygon::CPolygon( int _numV ) :numV(_numV)
{
	V = new Vector2 [numV];
	memset(V, 0, sizeof(V[0])*numV);
}

// Polygons collision stuff will be here

void GetInterval(const Vector2 *axVertices, int iNumVertices, 
				 const Vector2& xAxis, float& min, float& max);

bool FindMTD(Vector2* xAxis, float* taxis, int iNumAxes, Vector2& N, float& t);

bool IntervalIntersect(const CPolygon* A,
					   const CPolygon* B,
					   const Vector2& xAxis, 
					   const Vector2& xOffset, const Vector2& xVel, const Matrix2& xOrient,
					   float& taxis, float tmax);

bool CPolygon::Collide(const CPolygon* A, const Vector2& Apos, const Vector2& Avel, const Matrix2& Aorient,
					   const CPolygon* B, const Vector2& Bpos, const Vector2& Bvel, const Matrix2& Borient,
					   Vector2& n, float& depth)
{
	if (!A || !B)
		return false;

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

	for(int j = A->numV-1, i = 0; i < A->numV; j = i, i ++)
	{
		Vector2 E0 = A->V[j];
		Vector2 E1 = A->V[i];
		Vector2 E  = E1 - E0;
		xAxis[iNumAxes] = Vector2(-E.y, E.x) * xOrient;

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

	for(int j = B->numV-1, i = 0; i < B->numV; j = i, i ++)
	{
		Vector2 E0 = B->V[j];
		Vector2 E1 = B->V[i];
		Vector2 E  = E1 - E0;
		xAxis[iNumAxes] = Vector2(-E.y, E.x);

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

	if (B->numV == 2)
	{
		Vector2 E  = B->V[1] - B->V[0];
		xAxis[iNumAxes] = E;

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

	if (A->numV == 2)
	{
		Vector2 E  = A->V[1] - A->V[0];
		xAxis[iNumAxes] = E * xOrient;

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

	if (!FindMTD(xAxis, taxis, iNumAxes, n, depth))
		return false;

	if (n * xOffset < 0.0f)
		n = -n;

	n *= Borient;

	return true;
}


void GetInterval(const Vector2 *axVertices, int iNumVertices, const Vector2& xAxis, float& min, float& max)
{
	min = max = (axVertices[0] * xAxis);

	for(int i = 1; i < iNumVertices; i ++)
	{
		float d = (axVertices[i] * xAxis);
		if (d < min) min = d; else if (d > max) max = d;
	}
}

bool IntervalIntersect(const CPolygon* A,
					   const CPolygon* B,
					   const Vector2& xAxis, 
					   const Vector2& xOffset, const Vector2& xVel, const Matrix2& xOrient,
					   float& taxis, float tmax)
{
	float min0, max0;
	float min1, max1;
	GetInterval(A->V, A->numV, xAxis ^ xOrient, min0, max0);
	GetInterval(B->V, B->numV, xAxis, min1, max1);

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
	for(int i = 0; i < iNumAxes; i ++)
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
	for(int i = 0; i < iNumAxes; i ++)
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

scalar HalfPlaneSign(const Vector2 &u0, const Vector2 &u1, const Vector2 &p)	// Кстати, это площадь тругольника на этих трёх точках. // Или параллелограма.
{
	return (u0.x - p.x) * (u1.y - p.y) - (u0.y - p.y) * (u1.x - p.x);
}

bool IntersectLines(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result)
{
	scalar a1 = u1.y - u0.y;
	scalar b1 = u0.x - u1.x; 
	scalar a2 = v1.y - v0.y;
	scalar b2 = v0.x - v1.x;
	Matrix2 deltaMatrix(a1, b1, a2, b2);
	scalar deltaDet = deltaMatrix.Determinant();
	if (Equal(deltaDet, 0.0f))
		return false;	// Прямые параллельны, т.е. a1b2 - a2b1 == 0; Кстати, условие перпендикулярности: a1a2 == -b1b2;
	scalar c1 = u1.y * u0.x - u1.x * u0.y;	//a1 * u0.x + b1 * u0.y;
	scalar c2 = v1.y * v0.x - v1.x * v0.y;	//a2 * v0.x + b2 * v0.y;
	Result = Vector2(Matrix2(c1, b1, c2, b2).Determinant() / deltaDet, Matrix2(a1, c1, a2, c2).Determinant() / deltaDet);
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

bool IntersectCircles(const Vector2 &p0, const scalar r0, const Vector2 &p1, const scalar r1, Vector2 &Normal, scalar &Depth)
{
	Vector2 p0p1 = p1 - p0;
	if ((Sqr(p0p1.x) + Sqr(p0p1.y)) >= Sqr(r0 + r1))
		return false;
	Normal = p0p1.Normalized();
	Depth = abs(r1 - r0);
	return true;
}

bool PointInsidePolygon();

scalar DistanceToLine(const Vector2 &u0, const Vector2 &u1, const Vector2 &p)
{
	return HalfPlaneSign(u0, u1, p) / (u0 - u1).Length();
}

scalar DistanceToSegment(const Vector2 &u0, const Vector2 &u1, const Vector2 &p)
{
	Vector2 v = u1 - u0;
	Vector2 w = p - u0;

	scalar c1 = w * v;
	if (c1 <= 0)
		return (p - u0).Length() * Sign(HalfPlaneSign(u0, u1, p)); // Мы же хотим получить расстояние со знаком даже если это расстояние до концов отрезка.

	scalar c2 = v * v;
	if (c2 <= c1)
		return (p - u1).Length() * Sign(HalfPlaneSign(u0, u1, p));

	return HalfPlaneSign(u0, u1, p) / (u0 - u1).Length();
}