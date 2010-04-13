/**
*	File    : 2de_MathUtils.h
*	Author  : Dreamcatcher
*	Started : 06.11.2007 2:03
*/

/**
* @todo Refactor & rewiev
*/

#ifndef _2DE_MATH_UTILS_H
#define _2DE_MATH_UTILS_H

#include <math.h>
#include <algorithm>
#include <stdexcept>
#include "2de_Core.h"
// If enabled then optimized version of 
// Vector2.Length() used. It costs 5% accuracy.
//#define OPTIMIZE_V2L
const float				epsilon			=	0.00001f;
const double			HI_PI			=	3.1415926535897932;
const double			HI_OOPI			=	0.3183098861837906;
const float				PI				=	3.1415926f;
const float				OOPI			=	0.3183f;
const int				sincostable_dim	=	8192;
static const scalar		deganglem		=	1.0f / (360.0f / (scalar)sincostable_dim);
static const scalar		radanglem		=	1.0f / (PI * 2.0f / (scalar)sincostable_dim);
static const scalar		PI_d180			=	PI / 180.0f;
static const scalar		d180_PI			=	180.0f / PI;

#ifdef USING_OPENGL	// ORLY we need ot here?
	#ifdef USE_SDL_OPENGL
		#include <SDL/SDL_opengl.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif
#endif

class Vector2;
class Matrix2;

void GenSinTable();
scalar fSinr(scalar angle);
scalar fSind(scalar angle);
scalar fSini(int index);
scalar fCosr(scalar angle);
scalar fCosd(scalar angle);
scalar fCosi(int index);

int Random_Int(int min, int max);
float Random_Float(float min, float max);

template<typename T>
__INLINE T Clamp(const T &x , const T &min, const T &max)
{
	return std::min(std::max(x, min), max);
}

__INLINE scalar DegToRad(scalar degree)
{ 
	return (scalar)(degree * PI_d180);
}

__INLINE scalar RadToDeg(scalar radian)
{
	return (scalar)(radian * d180_PI);
}

template<typename T>
__INLINE T Sqr(const T &x)
{
	return x * x;
}

template<typename T>
__INLINE T Cube(const T &x)
{
	return x*x*x;
}

template<typename T>
__INLINE T Abs(const T &x)
{
	return x < 0 ? -x : x;
}

template<>
__INLINE float Abs<float>(const float &x)
{
	*(int *)&x &= 0x7fffffff;
	return x;
}

template<typename T>
__INLINE bool Equal(const T &a, const T &b)
{
	return Abs(a - b) < epsilon;
}

template<typename T>
__INLINE int Sign(const T &x)
{
	return x < 0 ? -1 : 1;
}

template<>
__INLINE int Sign<float>(const float &x)
{	
   if (((int&)x & 0x7FFFFFFF)==0) return 0; // test exponent & mantissa bits: is input zero?
    return (signed((int&)x & 0x80000000) >> 31 ) | 1;
}

// __INLINE float Sign(const float x)
// {
// 	   if (((int&)f & 0x7FFFFFFF)==0) return 0.f; // test exponent & mantissa bits: is input zero?
//     else {
//         float r = 1.0f;
//         (int&)r |= ((int&)f & 0x80000000); // mask sign bit in f, set it in r if necessary
//         return r;
// }
// / fast floating point absolute value.
// 
// inline float abs(float v)
// {
// 	*(int *)&v &= 0x7fffffff;
// 	return v;
// }
// 
// / interpolate between interval [a,b] with t in [0,1].
// 
// inline float lerp(float a, float b, float t)
// {
// 	return a + (b - a) * t;
// }
// 
// / snap floating point number to grid.
// 
// inline float snap(float p, float grid)
// {
// 	return grid ? float( floor((p + grid*0.5f)/grid) * grid) : p;
// }

class Vector2
{
public:
	scalar x, y;

	__INLINE Vector2(scalar Ax = 0.0f, scalar Ay = 0.0f) : x(Ax), y(Ay){}
	
	__INLINE  Vector2 operator +(const Vector2 &V) const
	{ 
		return Vector2(x + V.x, y + V.y);
	}
	__INLINE Vector2 operator -(const Vector2 &V) const
	{ 
		return Vector2(x - V.x, y - V.y);
	}
	__INLINE Vector2 operator *(scalar a) const
	{ 
		return Vector2(x * a, y * a);
	}
	__INLINE Vector2 operator /(scalar a) const
	{
		try
		{
			if(a == 0)
				throw std::runtime_error("Division by zero encountered in Vector2 operator / ");
			scalar t = 1.0f / a;
			return Vector2(x * t, y * t);
		}
		catch(const std::runtime_error& e)
		{
			cout << e.what() << endl;
		}
		return Vector2(); // What we should return if we tried to divide by zero?
	}
	friend __INLINE  Vector2 operator *(scalar k, const Vector2& V)
	{
		return Vector2(V.x * k, V.y * k);
	}
	__INLINE Vector2& operator +=(const Vector2 &V) 
	{
		x += V.x;
		y += V.y;
		return *this;
	}
	__INLINE Vector2& operator -=(const Vector2 &V)
	{
		x -= V.x;
		y -= V.y;
		return *this;
	}
	__INLINE Vector2& operator *=(scalar a)
	{
		x *= a;
		y *= a;
		return *this;
	}
	__INLINE Vector2& operator /=(scalar a)
	{
		return *this = *this / a;
	}

	__INLINE Vector2 operator -() const
	{
		return Vector2(-x, -y);
	}
	// Cкалярное произведение векторов. / Dot product.
	__INLINE scalar operator *(const Vector2 &V) const
	{
		return x * V.x + y * V.y;
	}
	__INLINE bool operator ==(const Vector2 &V) const
	{
		return Equal(x, V.x) && Equal(y, V.y);
	}
	__INLINE bool operator !=(const Vector2 &V) const
	{
		return !(*this == V);
	}
	Vector2 operator *(const Matrix2 &M) const;
	Vector2 operator ^(const Matrix2 &M) const;
	Vector2& operator *=(const Matrix2 &M);
	Vector2& operator ^=(const Matrix2 &M);
	__INLINE scalar Length() const
	{ 
		#ifdef OPTIMIZE_V2L
			scalar dx, dy;
			dx = Abs(x);
			dy = Abs(y);
			if (dx < dy)  
				return 0.961f * dy + 0.398f * dx;  
			else    
				return 0.961f * dx + 0.398f * dy;
		#else
			return (scalar)sqrt((double)(x * x + y * y));
		#endif
	}
	__INLINE Vector2 GetPerpendicular() const
	{
		return Vector2(-y, x);
	}
	__INLINE Vector2 Normalized() const
	{
		return *this / Length();
	}
	__INLINE scalar Normalize()
	{
		scalar l = Length();
		*this /= l;
		return l;
	}
	__INLINE void ClampTo(const Vector2 &min, const Vector2 &max)
	{
		x = Clamp(x, min.x, max.x);
		y = Clamp(y, min.y, max.y);
	}

#ifdef USING_OPENGL		// WTF is this? OpenGL in Math module?
		__INLINE void glTranslate() const
		{
			glTranslatef(x, y, 0.0f);
		}

		__INLINE void glScale() const
		{
			glScalef(x, y, 1.0f);
		}
		__INLINE void glVertex() const
		{
			glVertex2f(x, y);
		}
		__INLINE void glTexCoord() const
		{
			glTexCoord2f(x, y);
		}
#endif
};

template<typename T, size_t N>
class CCoitus
{
private:
	T Elements[N];
public:
	T& operator [](size_t Index)
	{
		if (Index >= N)
			throw std::runtime_error("CCoitus or CReproductor or CBreeding or CPropagation index out of bounds");
		return Elements[Index];
	}
};

template<size_t N>
class Vector2Array
{
private:
	Vector2 Elements[N];
public:
	Vector2& operator [](size_t Index)
	{
		if (Index >= N)
			throw std::runtime_error("CCoitus or CReproductor or CBreeding or CPropagation index out of bounds");
		return Elements[Index];
	}
};

const Vector2 V2_ZERO		= Vector2(0.0f, 0.0f);
const Vector2 V2_DIR_LEFT	= Vector2(-1.0f, 0.0f);
const Vector2 V2_DIR_RIGHT	= (-V2_DIR_LEFT);
const Vector2 V2_DIR_UP		= Vector2(0.0f, 1.0f);
const Vector2 V2_DIR_DOWN	= (-V2_DIR_UP);

union Vector3{
public:
	struct
	{
		scalar x, y, z;
	};
	struct
	{
		scalar r, g, b;
	};

	__INLINE Vector3(scalar Ax = 0.0f, scalar Ay = 0.0f, scalar Az = 0.0f) : x(Ax), y(Ay), z(Az){}

	__INLINE scalar operator[](int i)
	{
		try
		{
			if (i < 0 || i > 2)
				throw std::runtime_error("Access vector3 components: indx out of bounds");		
		}
		catch(const std::runtime_error& e)
		{
			cout << e.what() << endl;
		}
		return *(&x + i);
	}

	__INLINE Vector3 operator +(const Vector3 &v) const
	{
			return Vector3(x + v.x, y + v.y, z + v.z);
	}

	__INLINE Vector3 operator -(const Vector3 &v) const
	{
			return Vector3(x - v.x, y - v.y, z - v.z);
	}

	__INLINE Vector3 operator +(scalar s) const
	{
			return Vector3(x + s, y + s, z + s);
	}

	__INLINE Vector3 operator -(scalar s) const
	{
			return Vector3(x - s, y - s, z - s);
	}

	__INLINE Vector3 operator *(scalar s) const
	{
		return Vector3(x * s, y * s, z * s);
	}

	__INLINE Vector3 operator /(scalar s) const
	{
		try
		{
			if(s == 0)
				throw std::runtime_error("Division by zero encountered in Vector3 operator / ");
			scalar t = 1.0f / s;
			return Vector3(x * t, y * t, z * t);
		}
		catch(const std::runtime_error& e)
		{
			cout << e.what() << endl;
		}
		return Vector3();
	}

	friend Vector3 operator *(scalar s, const Vector3 &v) 
	{
		return Vector3(v.x * s, v.y * s, v.z * s); 
	}

	__INLINE Vector3& operator +=(const Vector3 &v) 
	{ 
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	__INLINE Vector3& operator -=(const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	__INLINE Vector3& operator *=(scalar s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	__INLINE Vector3& operator /=(scalar s)
	{
		return *this = *this / s;
	}

	__INLINE Vector3 operator -() const
	{
		return Vector3(-x, -y, -z);
	}

	__INLINE scalar operator *(const Vector3 &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	__INLINE Vector3 operator ^(const Vector3 &v) const
	{
		return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	__INLINE scalar Length() const
	{
		return (scalar)sqrt((double)(x * x + y * y + z * z));
	}

	Vector3 RotateAroundAxis(const Vector3& P, const Vector3& D, float angle)
	{
		//rotate around the vector parallel to (u,v,w) that passes through (a,b,c) // OMFG
		float axx,axy,axz,ax1;
		float ayx,ayy,ayz,ay1;
		float azx,azy,azz,az1;

		float u,v,w;
		float u2,v2,w2;
		float a,b,c;

		float sa,ca;

		sa=sin(angle);
		ca=cos(angle);

		u = D.x;
		v = D.y;
		w = D.z;												//< OMG OMG OMG OMG OMG OMG OMG OMG OMG OMG OMG OMG >
																//			\/	\/	\/ \/ \/ \/ \/ \/
		u2 = u*u;
		v2 = v*v;
		w2 = w*w;

		a = P.x;
		b = P.y;
		c = P.z;

		axx = u2+(v2+w2)*ca;		axy = u*v*(1-ca)-w*sa;	axz = u*w*(1-ca)+v*sa;	ax1 = a*(v2+w2)-u*(b*v+c*w)+(u*(b*v+c*w)-a*(v2+w2))*ca+(b*w-c*v)*sa;
		ayx = u*v*(1-ca)+w*sa;		ayy = v2+(u2+w2)*ca;	ayz = v*w*(1-ca)-u*sa;	ay1 = b*(u2+w2)-v*(a*u+c*w)+(v*(a*u+c*w)-b*(u2+w2))*ca+(c*u-a*w)*sa;
		azx = u*w*(1-ca)-v*sa;		azy = v*w*(1-ca)+u*sa;	azz = w2+(u2+v2)*ca;	az1 = c*(u2+v2)-w*(a*u+b*v)+(w*(a*u+b*v)-c*(u2+v2))*ca+(a*v-b*u)*sa;

		Vector3 W;
		W.x = axx * x + axy * y + axz * z + ax1;
		W.y = ayx * x + ayy * y + ayz * z + ay1;
		W.z = azx * x + azy * y + azz * z + az1;

		*this = W;

		return *this;
	}

	scalar Normalise() 
	{
		scalar l = Length();
		if ( l == 0.0f )
			return 0.0f;
		(*this) *= ( 1.0f / l );	
		return l; 
	}

	Vector3 Normalized() 
	{
		scalar l = Length();
		if ( l == 0.0f )
			return Vector3();
		Vector3 t = (*this) * ( 1.0f / l );	 
		return t;
	}

	scalar ComputeNormal(const Vector3& V0, const Vector3& V1, const Vector3& V2)
	{
		Vector3 E = V1 - V0;// E -= V0;
		Vector3 F = V2 - V1;// F -= V1;
		(*this)  = E ^ F;		
		return (*this).Normalise();
	}

	__INLINE void ClampTo(const Vector3 &min, const Vector3 &max)
	{
		x = Clamp(x, min.x, max.x);
		y = Clamp(y, min.y, max.y);
		z = Clamp(z, min.z, max.z);
	}



	void Rotate(Vector3 angle)
	{
		Vector3	sina, cosa, temp;

		sina.x = sin(angle.x);
		cosa.x = cos(angle.x);
		sina.y = sin(angle.y);
		cosa.y = cos(angle.y);
		sina.z = sin(angle.z);
		cosa.z = cos(angle.z);

		temp.x = (x * cosa.z + y * sina.z);
		temp.y = (y * cosa.z - x * sina.z);

		x = temp.x;
		y = temp.y;

		temp.z = (z * cosa.x + y * sina.x);
		temp.y = (y * cosa.x - z * sina.x);

		z = temp.z;
		y = temp.y;

		temp.x = (x * cosa.y + z * sina.y);
		temp.z = (z * cosa.y - x * sina.y);

		x = temp.x;
		z = temp.z;
	}
	// if that vector is normal so this function will return Euler angle to axes

	__INLINE Vector3 GetEulerAnglesFromNormal(Vector3 n)
	{
		Vector3 xyz_rot;
		xyz_rot.x= -atan2(n.y, n.z) + PI  /2.0f;
		xyz_rot.y= atan2(n.x, n.z);
		xyz_rot.z= -atan2(n.x, n.y);
		return xyz_rot;
	}

	__INLINE float AngleBeetweenVectors(Vector3 a, Vector3 b)
	{
		// TODO: Check if length of a || b == 0.0f
		return acos((a*b)/(a.Length()*b.Length()));
	}
};

typedef Vector3 RGBf;

class CAABB
{
public:
	Vector2 vMin, vMax;

	CAABB()
	{
		vMax = vMin = V2_ZERO;
	}
	CAABB(const Vector2 &_min, const Vector2 &_max) : vMin(_min), vMax(_max){}
	CAABB(scalar xmin, scalar ymin, scalar xmax, scalar ymax)
	{
		vMin.x = xmin;
		vMin.y = ymin;
		vMax.x = xmax;
		vMax.y = ymax;
	}
	// Если используется для кнопок и прочей еботы в экранных координатах, то этот конструктор удобнее для человеческого существа  // ОМГ, кто это писал?
	CAABB(int x, int y, int width, int height)  
	{
		vMin.x = x;
		vMin.y = y;
		vMax.x = x + width;
		vMax.y = y + height;
	}


	void Add(const Vector2 &point)
	{
		if (point.x > vMax.x)
			vMax.x = point.x;
		if (point.x < vMin.x)
			vMin.x = point.x;
		if (point.y > vMax.y)
			vMax.y = point.y;
		if (point.y > vMin.y)
			vMin.y = point.y;
	}

	void Offset(scalar dx, scalar dy)
	{
		vMin.x += dx;
		vMax.x += dx;
		vMin.y += dy;
		vMax.y += dy;
	}

	CAABB Offsetted(scalar dx, scalar dy) const
	{
		CAABB tmp;
		tmp.vMin.x = vMin.x + dx;
		tmp.vMax.x = vMax.x + dx;
		tmp.vMin.y = vMin.y + dy;
		tmp.vMax.y = vMax.y + dy;
		return tmp;
	}

	void Add(scalar x, scalar y)
	{
		Add(Vector2(x, y));
	}

	void Inflate(scalar x, scalar y)
	{
		vMin.x -=x;
		vMax.x +=x;
		vMin.y -=y;
		vMax.y +=y;		
	}

	CAABB Inflated(scalar x, scalar y) const
	{
		CAABB Result(vMin.x-x, vMin.y-y, vMax.x+x, vMax.y+y);
		return Result;
	}

	bool Inside(const Vector2 &point) const
	{
		if (point.x >= vMax.x || point.x <= vMin.x)
			return false;
		if (point.y >= vMax.y || point.y <= vMin.y)
			return false;
		return true;
	}

	bool Inside(const Vector2 &point, scalar &MTD) const // MTD - is minimal translation distance
	{
		if (point.x >= vMax.x || point.x <= vMin.x)
			return false;
		if (point.y >= vMax.y || point.y <= vMin.y)
			return false;
		
		//DistanceToLine
		scalar d1, d2, d3, d4;
		d1 =  vMax.x - point.x;
		d2 =  vMax.y - point.y;
		d3 = -vMin.x + point.x;
		d4 = -vMin.y + point.y;
		MTD = std::min(std::min(d1, d2), std::min(d3, d4));
		return true;
	}

	bool Inside(const Vector2 &point, scalar &MTD, Vector2 &n) const // MTD - is minimal translation distance // Ага блядь, одинаковые комменты рулят. OMG, там ещё третий внизу.
	{
		if (point.x >= vMax.x || point.x <= vMin.x)
			return false;
		if (point.y >= vMax.y || point.y <= vMin.y)
			return false;

		//DistanceToLine
		scalar d1, d2, d3, d4;		
		d1 =  vMax.x - point.x;
		d2 =  vMax.y - point.y;
		d3 = -vMin.x + point.x;
		d4 = -vMin.y + point.y;
		MTD = std::min(std::min(d1, d2), std::min(d3, d4));

		if (MTD == d1)
			n = Vector2(1.0f, 0.0f);
		if (MTD == d2)
			n = Vector2(0.0f, 1.0f);
		if (MTD == d3)
			n = Vector2(-11.0f, 0.0f);
		if (MTD == d4)
			n = Vector2(0.0f, -1.0f);

		return true;
	}

	bool Outside(const Vector2 &point, scalar &MTD, Vector2 &n) const // MTD - is minimal translation distance
	{
		if (point.x < vMax.x && point.x > vMin.x && point.y < vMax.y || point.y > vMin.y)
			return false;

		//DistanceToLine
		scalar d1, d2, d3, d4;		
		d1 = -vMax.x + point.x;
		d2 = -vMax.y + point.y;
		d3 =  vMin.x - point.x;
		d4 =  vMin.y - point.y;
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

	bool Intersect(CAABB box) const
	{
		if (box.vMin.x >= vMax.x)
			return false;
		if (box.vMin.y >= vMax.y)
			return false;
		if (box.vMax.x <= vMin.x)
			return false;
		if (box.vMax.y <= vMin.y)
			return false;		

		return true;
	}

	void Union(CAABB other)
	{
		Add(other.vMax);
		Add(other.vMin);
	}

	scalar Width() const
	{
		return vMax.x - vMin.x;
	}

	scalar Height() const
	{
		return vMax.y - vMin.y;
	}
};

class Matrix3
{
public:
	Vector3 m[3];
	
	Matrix3()
	{
		m[0] = Vector3();
		m[1] = Vector3();
		m[2] = Vector3();
	}

	Matrix3(scalar xx, scalar xy, scalar xz,
			  scalar yx, scalar yy, scalar yz,
			  scalar zx, scalar zy, scalar zz)
	{
		m[0] = Vector3(xx, xy, xz);
		m[1] = Vector3(yx, yy, yz);
		m[2] = Vector3(zx, zy, zz);
	}

	
	
	Matrix3(Vector3 c0, Vector3 c1, Vector3 c2) //fill the columns
	{
		m[0] = c0;
		m[1] = c1;
		m[2] = c2;
	}

	__INLINE Vector3 & operator [](int i) 
	{ return m[i];	} 

	__INLINE Matrix3 &operator += ( Matrix3 other)
	{
		m[0] += other.m[0];
		m[1] += other.m[1];
		m[2] += other.m[2];
		return (*this);
	}
	__INLINE Matrix3 &operator -= ( Matrix3 other)
	{
		m[0] -= other.m[0];
		m[1] -= other.m[1];
		m[2] -= other.m[2];
		return (*this);
	}
	__INLINE Matrix3 &operator *= ( scalar other)
	{
		m[0] *= other;
		m[1] *= other;
		m[2] *= other;
		return (*this);
	}
	__INLINE Matrix3 &operator /= ( scalar other)
	{
		if (Equal(other, 0.0f))
			m[0] = m[1] = m[2] = Vector3();			
		else
		{
			scalar t = 1.0f/other;
			m[0] *= t;
			m[1] *= t;
			m[2] *= t;
		}
		return (*this);
	}

	__INLINE friend Matrix3 operator*(const Matrix3& m1, const Matrix3& m2)
	{
		return Matrix3(
			m2.m[0]*m1.m[0], m2.m[1]*m1.m[0], m2.m[2]*m1.m[0],
			m2.m[0]*m1.m[1], m2.m[1]*m1.m[1], m2.m[2]*m1.m[1],
			m2.m[0]*m1.m[2], m2.m[1]*m1.m[2], m2.m[2]*m1.m[2]);
	}
	__INLINE Matrix3 operator+(Matrix3 m)
	{
		return (*this) += m;
	}

	__INLINE friend Vector3 operator *(const Vector3& V, const Matrix3& M)
	{
		Vector3 t;
		t.x = M.m[0]*V;
		t.y = M.m[1]*V;
		t.z = M.m[2]*V;
		return t;
	}

	__INLINE friend Vector3 operator *(const Matrix3& M, const Vector3& V)
	{
		Vector3 t;
		t.x = M.m[0]*V;
		t.y = M.m[1]*V;
		t.z = M.m[2]*V;
		return t;
	}

	__INLINE Matrix3 operator-(Matrix3 m)
	{
		return (*this) -= m;
	}

	Matrix3 Transpose() const 
	{
		return Matrix3(m[0].x, m[1].x, m[2].x,
						 m[0].y, m[1].y, m[2].y,
						 m[0].z, m[1].z, m[2].z);
	}

	scalar cofac(int r1, int c1, int r2, int c2) 
		{			
			return (m[r1][c1] * m[r2][c2] - m[r1][c2] * m[r2][c1]);
		}

	__INLINE Matrix3 inverse() 
	{
		Vector3 co = Vector3(cofac(1, 1, 2, 2), cofac(1, 2, 2, 0), cofac(1, 0, 2, 1));
		scalar det = m[0]*co;
		if (det == 0.0f)
			return Matrix3(0, 0, 0, 0 ,0, 0, 0 ,0, 0);
		scalar s = 1.0f / det;
		return Matrix3(co.x * s, cofac(0, 2, 2, 1) * s, cofac(0, 1, 1, 2) * s,
						 co.y * s, cofac(0, 0, 2, 2) * s, cofac(0, 2, 1, 0) * s,
						 co.z * s, cofac(0, 1, 2, 0) * s, cofac(0, 0, 1, 1) * s);

		return Matrix3(co.x * s, co.y * s, co.z * s,
						 cofac(0, 2, 2, 1) * s, cofac(0, 0, 2, 2) * s, cofac(0, 1, 2, 0) * s,
						 cofac(0, 1, 1, 2) * s, cofac(0, 2, 1, 0) * s, cofac(0, 0, 1, 1) * s);

	}




};

class Matrix4
{
public:
		scalar	e11, e12, e13, e14,
				e21, e22, e23, e24,
				e31, e32, e33, e34,
				e41, e42, e43, e44;
};

class Matrix2
{
public:
		float e11;
		float e12;
		float e21;
		float e22;

	Matrix2(float _e11, float _e12, float _e21, float _e22) : e11(_e11), e12(_e12), e21(_e21), e22(_e22)
	{}

	Matrix2()
	{}

	Matrix2(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);

		e11 = c; e12 = s;
		e21 =-s; e22 = c;
	}

// 	float  operator()(int i, int j) const 
// 	{
// 		return e[i][j];
// 	}
// 
// 	float& operator()(int i, int j)       
// 	{
// 		return e[i][j]; 
// 	}

// 	const Vector2& operator[](int i) const
// 	{
// 		return reinterpret_cast<const Vector2&>(e[i][0]);
// 	}
// 
// 	Vector2& operator[](int i)
// 	{
// 		return reinterpret_cast<Vector2&>(e[i][0]);
// 	}		

	static Matrix2 Identity()
	{
		static const Matrix2 T(1.0f, 0.0f, 0.0f, 1.0f);

		return T;
	}

	static Matrix2 Zer0()
	{
		static const Matrix2 T(0.0f, 0.0f, 0.0f, 0.0f);

		return T;
	}


	Matrix2 Tranpose()
	{
		std::swap(e12, e21);		
		return *this;
	}

	Matrix2 Tranposed() const
	{
		Matrix2 T;

		T.e11 = e11;
		T.e21 = e12;
		T.e12 = e21;
		T.e22 = e22;

		return T;
	}

	__INLINE Matrix2 operator - () const
	{
		Matrix2 T;
		T.e11 = -e11;
		T.e12 = -e12;
		T.e21 = -e21;
		T.e22 = -e22;
		return T;
	}

	Matrix2 operator * (const Matrix2& M) const 
	{
		Matrix2 T;

		T.e11 = e11 * M.e11 + e12 * M.e21;
		T.e21 = e21 * M.e11 + e22 * M.e21;
		T.e12 = e11 * M.e12 + e12 * M.e22;
		T.e22 = e21 * M.e12 + e22 * M.e22;

		return T;
	}

	Matrix2 operator ^ (const Matrix2& M) const 
	{
		Matrix2 T;

		T.e11 = e11 * M.e11 + e12 * M.e12;
		T.e21 = e21 * M.e11 + e22 * M.e12;
		T.e12 = e11 * M.e21 + e12 * M.e22;
		T.e22 = e21 * M.e21 + e22 * M.e22;

		return T;
	}

	__INLINE Matrix2 operator * (float s) const
	{
		Matrix2 T;

		T.e11 = e11 * s;
		T.e21 = e21 * s;
		T.e12 = e12 * s;
		T.e22 = e22 * s;

		return T;
	}

	__INLINE Matrix2 operator + (const Matrix2 &M) const
	{
		Matrix2 T;
		T.e11 = e11 + M.e11;
		T.e12 = e12 + M.e12;
		T.e21 = e21 + M.e21;
		T.e22 = e22 + M.e22;
		return T;
	}

	__INLINE Matrix2 operator - (const Matrix2 &M) const
	{
		Matrix2 T;
		T.e11 = e11 - M.e11;
		T.e12 = e12 - M.e12;
		T.e21 = e21 - M.e21;
		T.e22 = e22 - M.e22;
		return T;
	}

	__INLINE scalar Determinant()
	{
		return e11 * e22 - e12 * e21;
	}
};

union Vector4
{
	struct
	{
		scalar x, y, z, w;
	};
	struct
	{
		scalar r, g, b, a;
	};

	__INLINE scalar operator[](int i)
	{
		if (i<0 || i> 3)
			return 0.0f;
		return *(&x + i);
	}


	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
		{}
	Vector4(scalar Ax, scalar Ay, scalar Az, scalar Aw) : x(Ax), y(Ay), z(Az), w(Aw)
		{}
	Vector4(const Vector3 &v) : x(v.x), y(v.y), z(v.z), w(1.0f)
		{};
	//void operator=(const Vector4 &q){x=q.x;y=q.y;z=q.z;w=q.w;}
	__INLINE Vector4 operator+(Vector4 q){return Vector4( x + q.x , y + q.y, z + q.z, w + q.w );}
	__INLINE Vector4 operator-(Vector4 q){return Vector4( x - q.x , y - q.y, z - q.z, w - q.w );}
	__INLINE Vector4 operator*(scalar s){return Vector4(x * s, y * s, z * s, w * s);}
	__INLINE Vector4 operator=(const Vector4& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = V.w;
		return *this;
	}

	__INLINE Vector4 operator/(scalar s)
	{
		if (s == 0.0f)
			return Vector4();
		scalar is = 1.0f/s; 
		return (*this)*is;
	}
	__INLINE Vector4 operator+=(Vector4 q){(*this) = (*this)+q; return *this;}
	__INLINE Vector4 operator-=(Vector4 q){(*this) = (*this)-q; return *this;}
	scalar Norm()const{return x * x + y * y + z * z + w * w;}
	scalar Length(  ){return sqrt(x * x + y * y + z * z + w * w );}	
	Vector4 Conjugate(){ return Vector4(-x,-y,-z,w);}
	Vector4 Identity()
	{
		//TODO: Danger chek division by zero
		return (*this).Conjugate()/(*this).Norm();
	};
	scalar InnerProduct(const Vector4& q){return x*q.x+y*q.y+z*q.z+w*q.w;}

		__INLINE Vector4 operator*(const Vector4 &q)
	{
		Vector4 result;
		result.x = w * q.x + x * q.w + y * q.z - z * q.y;
		result.y = w * q.y - x * q.z + y * q.w + z * q.x;
		result.z = w * q.z + x * q.y - y * q.x + z * q.w;
		result.w = w * q.w - x * q.x - y * q.y - z * q.z;

		return result;
	}
		
	__INLINE void AxisAngle(Vector3& axis, scalar& angle)const
	{
		scalar vl = (scalar)sqrt( x*x + y*y + z*z );
		if( vl > 0.0001f )
		{
			scalar ivl = 1.0f/vl;
			axis = Vector3( x*ivl, y*ivl, z*ivl );
			if( w < 0 )
				angle = 2.0f*(scalar)atan2(-vl, -w); //-PI,0 
			else
				angle = 2.0f*(scalar)atan2( vl,  w); //0,PI 
		}
		else
		{
			axis = Vector3(0,0,0);
			angle = 0;
		}
    }


	void Rotate(scalar amount, scalar xAxis, scalar yAxis, scalar zAxis)
	{
		if((xAxis != 0 && xAxis != 1) ||
			(yAxis != 0 && yAxis != 1) ||
			(zAxis != 0 && zAxis != 1))
		{
			scalar length = (scalar)sqrt(xAxis * xAxis + yAxis * yAxis + zAxis * zAxis);
			if (length == 0.0f)
				return;
			xAxis /= length; yAxis /= length; zAxis /= length;
		}
		scalar angle = DegToRad(amount);
		scalar sine = (scalar)sin(angle / 2.0f);
		x = xAxis * sine;
		y = yAxis * sine;
		z = zAxis * sine;
		w = (scalar)cos(angle / 2.0f);
		//TODO: division by zero check
		scalar length = 1 / (scalar)sqrt(x * x + y * y + z * z + w * w);
		x *= length;
		y *= length;
		z *= length;
	}

	__INLINE void toMatrix3( Matrix3& m  )const
	{
		scalar wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
		scalar s  = 2.0f/(*this).Norm();  
		x2 = x * s;    y2 = y * s;    z2 = z * s;
		xx = x * x2;   xy = x * y2;   xz = x * z2;
		yy = y * y2;   yz = y * z2;   zz = z * z2;
		wx = w * x2;   wy = w * y2;   wz = w * z2;

		m.m[0].x = 1.0f - (yy + zz);
		m.m[1].x = xy - wz;
		m.m[2].x = xz + wy;
	
		m.m[0].y = xy + wz;
		m.m[1].y = 1.0f - (xx + zz);
		m.m[2].y = yz - wx;
	
		m.m[0].z = xz - wy;
		m.m[1].z = yz + wx;
		m.m[2].z = 1.0f - (xx + yy);
    }
#ifdef USING_OPENGL
		__INLINE void glSet()
		{
			glColor4fv(&x);
		}
#endif
};

typedef Vector4 Quaternion;
typedef Vector4 RGBAf;

// <чятик, чятик (радость)>

/**
/	я не совсем понимаю, зачем это делать именно так? разве нам нужны для 
/	цвета векторные операции? да нет, не нужны.. зато очень нужна возможность
/	конвертировать RGBAub в RGBAf, например.. и как это сделаешь теперь, если это
/	всего-лишь тайпдеф от вектора.. а ведь я щас как раз делаю CGUIStyle и заебусь
/	высчитывать флоаты для дефолтных цветов...
*/

/**
/	Ты ошибаешься и нам как раз таки нужны векторные операции для цвета.
/	Фишку с конвертацией можно прокрутить унаследовава от Vector4. Опять же,
/	Я хочу ещё работу с HSV добавить.
*/

/**
/	ну вот если унаследовать, тогда другой разговор - это будет хорошо..
/	но не тайпдефить же..
*/

// </чятик, чятик (радость)>

/**
*	MatrixNM - произвольная матрица. M строк, N столбцов
*/ 

class MatrixNM
{
public:
	int n, m;
	scalar **e;

	MatrixNM():n(0), m(0), e(NULL){}
	MatrixNM(int _n, int _m) : n(_n), m(_m)
	{
		e = new scalar* [m];
		for(int i=0; i< m; e[i++] = new scalar [n]);
	}

	float  operator()(int i, int j) const 
	{
		return e[i][j];
	}
	float& operator()(int i, int j)       
	{
		return e[i][j]; 
	}

	void SwapC(int c0, int c1)
	{
		for(int i=0; i < m; ++i)
			std::swap(e[i][c0], e[i][c1]);
	}

	void SwapR(int r0, int r1)
	{
		for(int i=0; i < n; ++i)
			std::swap(e[r0][i], e[r1][i]);
	}

	scalar Determinant(int k)
	{
		if (n != m)
			return -0.0f;
		if (k == 2)
		{
			return e[0][0]*e[1][1] - e[0][1]*e[1][0];
		}
		scalar r = 0, t = 0;
		for(int i = 0; i< k ; i++)
		{
			if (i%2 == 0) t = 1.0f; else t = -1.0f;
			t *= e[0][i];
			for (int j = i; j < k-1; j++)
				SwapC(j, j+1);
			for (int j = 0; j < k-1; j++)
				SwapR(j, j+1);
			r += t*Determinant(k-1); 
			for (int j = k-1; j > 0; j--)
				SwapR(j, j-1);
			for (int j = k-1; j > i; j--)
				SwapC(j, j-1);
			t = 0.0f;
		}
		return r;
	}

	__INLINE MatrixNM operator *(const MatrixNM& M) const
	{
		if (n != M.m)
			return *this;
		MatrixNM R = MatrixNM(M.n, m);
		for(int i = 0; i < m; i++)
			for(int j = 0; j < M.n; j++)
			{
				R.e[i][j] = 0;
				for(int k = 0; k < n ; k++)
				{
					R.e[i][j] += e[i][k]*M.e[k][j];
				}
			}
		return R;
	}

	__INLINE MatrixNM operator * (scalar s) const
	{
		MatrixNM R = *this;
		for(int i =0; i < R.n; ++i)
			for(int j = 0; j < R.m; ++j)
				R.e[i][j] *= s;
		return R;
	}
};

__INLINE Vector3 CalcNorm(const Vector3 v1,const Vector3 v2,const Vector3 v3)
{
   Vector3 t = (v3-v2)^(v2-v1);
   t.Normalise();
   return t;
}

/**
*	SqareEq(...) Решает квдратное уравнение.
*	Корни - t0 и t1
*	a b c - соответствующие коэффициенты квадратного уравнения.
*/

bool SqareEq( scalar a, scalar b, scalar c, scalar &t0, scalar &t1);

/**
*	CalcFrustumVertices(...) - функция вычисляет координаты вершин
*	усеченной пирамиды, образуемой пирамидой камеры и 2мя плоскостями отсечения.
*/

void CalcFrustumVertices(scalar fovy, scalar aspect, scalar znear, scalar zfar, Vector3 cpos, Vector3 cat,Vector3 cup, Vector3 v[8]);

/**
*	PointsPlaneSide(...) - фунция проверяет лежат ли все точки
*	по одну сторону от плоскост или нет. если нет то возвращает 0
*	если да - то знак полуплоскости
*/

int PointsPlaneSide(Vector3 a, Vector3 n,Vector3 offset,Matrix3 R, Vector3 *points, int pnum);
int PointPlanesSide(Vector3 *a, Vector3 *n,int *iV, Vector3 offset, Matrix3 R, Vector3 point, int fnum, Vector3 &normal, float &depth);
int PointPlanesSideEx(Vector3 *a, Vector3 *n, unsigned short *iV, Vector3 offset, Matrix3 R, Vector3 point,int  fnum, Vector3 &normal, float &depth, Vector3 scaling);


scalar FindMTD(Vector3 a, Vector3 n,Vector3 offset,Matrix3 R, Vector3 *points, int pnum);
bool CullBox(Vector3 _min, Vector3 _max, Vector3 pos, Vector3 scaling, Matrix3 R,
				scalar fovy, scalar aspect, scalar znear, scalar zfar,
				Vector3 cpos, Vector3 cat, Vector3 cup);

// непонятная функция. походу я её откуда-то рипанул, надо разобраться
int inclusion (Vector3 *p, int *iV,  int nVert,  int nFaces,  Vector3 q);

/**
/	HalfPlaneSign - определяет знак полуплоскости точки x относительно прямой, лежащей на отрезке [u0, u1].
/	Если точка лежит на прямой, то результат 0.0f
*/
scalar HalfPlaneSign(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

/**
/	DistanceToLine - определяет расстояние со знаком от точки до прямой, лежащей на отрезке.
*/
scalar DistanceToLine(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

/**
/	DistanceToSegment - определяет расстояние со знаком от точки до отрезка.
/	TODO: Возможно есть более быстрый способ определять знак расстояниея в случаях конца отрезка.
	TODO: Возможно стоит возвращать ещё и найденный случай, т.е. первая точка отрезка, вторая или сам отрезок.
*/
scalar DistanceToSegment(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

/**
/	IntersectLines - определяет точку пересечения прямых, лежащих на отрезках, если они пересекаются.
*/
bool IntersectLines(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result);

/**
/	IntersectSegments - определяет точку пересечения отрезков, если они пересекаются.
/	TODO: есть подозрения на рассмотрение случая совпадения прямых.
*/
bool IntersectSegments(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result);	// Feel the difference.

//////////////////////////////////////////////////////////////////////////
//	Geometry

class CGeometry
{
public:
	int type;			// Определять тип одним интом как-то не очень хорошо. Да и вообще, чего это тип, я уже и забыл.
	CAABB Box;
	CGeometry();
	virtual void CalcBBOX(){}
};

class CCircle : public CGeometry
{
	Vector2 Position;
	scalar Radius;
	CCircle(Vector2 APosition, scalar ARadius) : Position(APosition), Radius(ARadius)
	{
		CalcBBOX();
	}
	void CalcBBOX()
	{
		Box.vMin = Vector2(Position.x - Radius, Position.y - Radius);
		Box.vMin = Vector2(Position.x + Radius, Position.y + Radius);
	}
};	

class CPolygon : public CGeometry
{
public:
	int numV;		// кол-во вершин
	Vector2 *V;		// указатель на массив вершин // И кто их создаёт? Кто их удаляет? Где они хранятся? Можно ли их легко добавлять, удалять? Кто вообще так пишет классы? Стоит ли это вынести в отдельный класс? Кто решит все эти вопросы? Зачем эта строка такая длинная? Что я курил на этих выходных?
	CPolygon(): numV(0), V(NULL){ }
	CPolygon(int _numV);
	void Reset(int _numV);
	void CalcBBOX();

	static bool	Collide	(const CPolygon* A, const Vector2& Apos, const Vector2& Avel, const Matrix2& Aorient,
						const CPolygon* B, const Vector2& Bpos, const Vector2& Bvel, const Matrix2& Borient,
						Vector2& n, float& depth);

};

class CRectangle : public CGeometry
{

};

class CSegment : public CGeometry  // Are we really need it?
{

};


#endif // _2DE_MATH_UTILS_H
