#include "MathUtils.h"

/*
void M3MtoD3DM(D3DXMATRIX *m,Matrix3 res){
    for (int i=0;i<3;i++)
    {
        m->m[0][i] = res.m[i].x;
        m->m[1][i] = res.m[i].y;
        m->m[2][i] = res.m[i].z;
    }
}
*/

void CalcCameraVertices(scalar fovy, scalar aspect, scalar znear, scalar zfar,
						Vector3 cpos, Vector3 cat, Vector3 cup, Vector3 v[8])
{
	scalar y1 = znear * tan(fovy*0.5f);
	scalar x1 = y1 * aspect;

	scalar y2 = zfar * tan(fovy*0.5f);
	scalar x2 = y2 * aspect;

	/*v[0] = Vector3(-x1,-y1,znear);
	v[1] = Vector3( x1,-y1,znear);
	v[2] = Vector3( x1, y1,znear);
	v[3] = Vector3(-x1, y1,znear);

	v[4] = Vector3(-x2,-y2,zfar);
	v[5] = Vector3( x2,-y2,zfar);
	v[6] = Vector3( x2, y2,zfar);
	v[7] = Vector3(-x2, y2,zfar);*/

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
	
	
//	for(int i=0;i<8;i++)
//		v[i] = v[i] - cpos;



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

int PointPlanesSideEx(Vector3 *a, Vector3 *n,WORD *iV, Vector3 offset, Matrix3 R, Vector3 point, int fnum, Vector3 &normal, float &depth, Vector3 scaling)
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
    
    CalcCameraVertices(fovy, aspect, znear, zfar, cpos, cat, cup, v);
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
// iV is the indices of every triangle, using -1 as separator of faces
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
