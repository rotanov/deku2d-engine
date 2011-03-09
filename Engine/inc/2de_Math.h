/**
*	created:	2007/11/06 2:03
*	filename: 	2de_Math.h
*	author:		D. Rotanov
*	purpose:	General mathematical routines and classes
*/

#ifndef _2DE_MATH_UTILS_H_
#define _2DE_MATH_UTILS_H_

#include <cmath>
#include <cassert>
#include <stdexcept>

#ifndef _2DE_CORE_H_
#if defined(_MSC_VER)
	#define __INLINE __forceinline
#elif __GNUC__ >= 4
	#define __INLINE __inline __attribute__ ((always_inline))
#else
	#define __INLINE inline
#endif
typedef unsigned char byte;
#endif

// If enabled then optimized version of 
// Vector2.Length() used. It costs 5% accuracy.
// #define OPTIMIZE_V2L
const float	epsilon = 0.00001f;
const float PI = 3.1415926f;
const float PI_D2 = PI / 2.0f;
const float PI2	= PI * 2.0f;
const float OOPI = 0.3183098f;
const int SINE_COSINE_TABLE_DIM	= 8192;
const float deganglem =	static_cast<float>(SINE_COSINE_TABLE_DIM) / 360.0f;
const float radanglem =	static_cast<float>(SINE_COSINE_TABLE_DIM) / PI2;
const float PI_d180 = PI / 180.0f;
const float d180_PI	= 180.0f / PI;
extern unsigned int g_seed;

class Vector2;
class Matrix2;

void GenSinTable();
float fSinr(float angle);
float fSind(float angle);
float fSini(int index);
float fCosr(float angle);
float fCosd(float angle);
float fCosi(int index);

#define Random_Int RandomRange<int>
#define Random_Float RandomRange<float>

template <typename T>
__INLINE T RandomRange(const T &AMin, const T &AMax)
{
	assert(false)
}

template <>
__INLINE int RandomRange<int>(const int &min, const int &max)
{
	assert(max - min + 1);
	g_seed = 214013*g_seed + 2531011;
	return min + (g_seed^g_seed>>15) % (max - min + 1);
}

template <>
__INLINE float RandomRange<float>(const float &min, const float &max)
{
	g_seed = 214013 * g_seed + 2531011;
	//return min + g_seed * (1.0f / 4294967295.0f) * (max - min);
	return min + (g_seed >> 16) * (1.0f / 65535.0f) * (max - min);
}

template<typename T>
__INLINE T Clamp(const T &x , const T &min, const T &max)
{
	return std::min(std::max(x, min), max);
}

__INLINE float DegToRad(float degree)
{ 
	return static_cast<float>(degree * PI_d180);
}

__INLINE float RadToDeg(float radian)
{
	return static_cast<float>(radian * d180_PI);
}

template<typename T>
__INLINE T Sqr(const T &x)
{
	return x * x;
}

template<typename T>
__INLINE T Cube(const T &x)
{
	return x * x * x;
}

template<typename T>
__INLINE T Abs(const T &x)
{
	return x < 0 ? -x : x;
}

__INLINE float Abs(float x)
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
    return (signed((int&)x & 0x80000000) >> 31) | 1;
}

/**
*	Interpolate between interval [a,b] with t in [0,1].	
*/

__INLINE float LinearInterpolate(float a, float b, float t)
{
	return a + (b - a) * t;
}

__INLINE float CosineInterpolate(float a, float b, float t)
{
	return a + (b - a) * (1.0f - cos(t * PI)) * 0.5f;
}

// Here v1 is the "a" point and v2 is the "b" point. v0 is "the point before a" and v3 is "the point after b".
__INLINE float CubicInterpolate(float v0, float v1, float v2, float v3, float t)
{
	float P = (v3 - v2) - (v0 - v1);
	float Q = (v0 - v1) - P;
	float R = v2 - v0;
	float S = v1;
	float x2 = t * t;
	float x3 = x2 * t;
	return P * x3 + Q * x2 + R * t + S;
}

// Also i heard about some Hermite interpolation here http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
// but i think we won't ever need it. And also some keywords: Bezier, Spline, and piecewise Bezier.
// Also-also look here for perlin noise http://freespace.virgin.net/hugo.elias/models/m_perlin.htm and other stuff

// Snap floating point number to grid.
__INLINE float snap(float p, float grid)
{
	return grid ? float(floor((p + grid * 0.5f) / grid) * grid) : p;
}

class Vector2
{
public:
	float x, y;

	__INLINE Vector2(float Ax = 0.0f, float Ay = 0.0f) : x(Ax), y(Ay){}
	
	__INLINE  Vector2 operator +(const Vector2 &V) const
	{ 
		return Vector2(x + V.x, y + V.y);
	}
	__INLINE Vector2 operator -(const Vector2 &V) const
	{ 
		return Vector2(x - V.x, y - V.y);
	}
	__INLINE Vector2 operator *(float a) const
	{ 
		return Vector2(x * a, y * a);
	}
	__INLINE Vector2 operator /(float a) const
	{
		if (a == 0)
			throw std::runtime_error("Division by zero encountered in Vector2 operator / ");
		float t = 1.0f / a;
		return Vector2(x * t, y * t);
		return Vector2(); // What we should return if we tried to divide by zero? // exception, obviously..
	}
	friend __INLINE  Vector2 operator *(float k, const Vector2 &V)
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
	__INLINE Vector2& operator *=(float a)
	{
		x *= a;
		y *= a;
		return *this;
	}
	__INLINE Vector2& operator /=(float a)
	{
		return *this = *this / a;
	}

	__INLINE Vector2 operator -() const
	{
		return Vector2(-x, -y);
	}
	__INLINE float operator *(const Vector2 &V) const
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
	__INLINE float Length() const
	{ 
		#ifdef OPTIMIZE_V2L
			float dx, dy;
			dx = Abs(x);
			dy = Abs(y);
			if (dx < dy)  
				return 0.961f * dy + 0.398f * dx;  
			else    
				return 0.961f * dx + 0.398f * dy;
		#else
			return static_cast<float>(sqrt(static_cast<double>(x * x + y * y)));
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
	__INLINE float Normalize()
	{
		float l = Length();
		*this /= l;
		return l;
	}
	__INLINE void ClampTo(const Vector2 &min, const Vector2 &max)
	{
		x = Clamp(x, min.x, max.x);
		y = Clamp(y, min.y, max.y);
	}

};

const Vector2 V2_ZERO		= Vector2(0.0f, 0.0f);
const Vector2 V2_DIR_LEFT	= Vector2(-1.0f, 0.0f);
const Vector2 V2_DIR_RIGHT	= (-V2_DIR_LEFT);
const Vector2 V2_DIR_UP		= Vector2(0.0f, 1.0f);
const Vector2 V2_DIR_DOWN	= (-V2_DIR_UP);

template<>
__INLINE Vector2 RandomRange<Vector2>(const Vector2 &AMin, const Vector2 &AMax)
{
	return Vector2
		(
			Random_Float(AMin.x, AMax.x),
			Random_Float(AMin.y, AMax.y)
		);
}


const Vector2 V2_DIRECTIONS[4] = {V2_DIR_LEFT, V2_DIR_DOWN, V2_DIR_RIGHT, V2_DIR_UP,};

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
	Vector2Array(const Vector2 *Vertices)
	{
		for(unsigned int i = 0; i < N; i++)
			Elements[i] = Vertices[i];
	}
	Vector2Array()
	{
		for(unsigned int i = 0; i < N; i++)
			Elements[i] = V2_ZERO;
	}
	Vector2& operator [](size_t Index)
	{
		if (Index >= N)
			throw std::runtime_error("CCoitus or CReproductor or CBreeding or CPropagation index out of bounds");
		return Elements[Index];
	}
	const Vector2& operator [](size_t Index) const
	{
		if (Index >= N)
			throw std::runtime_error("CCoitus or CReproductor or CBreeding or CPropagation index out of bounds");
		return Elements[Index];
	}
};

union Vector3{
public:
	struct
	{
		float x, y, z;
	};
	struct
	{
		float r, g, b;
	};

	__INLINE Vector3(float Ax = 0.0f, float Ay = 0.0f, float Az = 0.0f) : x(Ax), y(Ay), z(Az){}

	__INLINE float operator[](int i)
	{
			if (i < 0 || i > 2)
				throw std::runtime_error("Access vector3 components: indx out of bounds");		
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

	__INLINE Vector3 operator +(float s) const
	{
			return Vector3(x + s, y + s, z + s);
	}

	__INLINE Vector3 operator -(float s) const
	{
			return Vector3(x - s, y - s, z - s);
	}

	__INLINE Vector3 operator *(float s) const
	{
		return Vector3(x * s, y * s, z * s);
	}

	__INLINE Vector3 operator /(float s) const
	{
		if (s == 0)
			throw std::runtime_error("Division by zero encountered in Vector3 operator / ");
		float t = 1.0f / s;
		return Vector3(x * t, y * t, z * t);
	}

	friend Vector3 operator *(float s, const Vector3 &v)
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

	__INLINE Vector3& operator *=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	__INLINE Vector3& operator /=(float s)
	{
		return *this = *this / s;
	}

	__INLINE Vector3 operator -() const
	{
		return Vector3(-x, -y, -z);
	}

	__INLINE float operator *(const Vector3 &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	__INLINE Vector3 operator ^(const Vector3 &v) const
	{
		return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	__INLINE float Length() const
	{
		return static_cast<float>(sqrt(static_cast<double>(x * x + y * y + z * z)));
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

	float Normalise()
	{
		float l = Length();
		if (l == 0.0f)
			return 0.0f;
		(*this) *= (1.0f / l);
		return l;
	}

	Vector3 Normalized()
	{
		float l = Length();
		if (l == 0.0f)
			return Vector3();
		Vector3 t = (*this) * (1.0f / l);
		return t;
	}

	float ComputeNormal(const Vector3& V0, const Vector3& V1, const Vector3& V2)
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
		// @todo: Check if length of a || b == 0.0f
		return acos((a*b)/(a.Length()*b.Length()));
	}
};

typedef Vector3 RGBf;

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

	Matrix3(float xx, float xy, float xz,
			  float yx, float yy, float yz,
			  float zx, float zy, float zz)
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

	__INLINE Matrix3 &operator += (Matrix3 other)
	{
		m[0] += other.m[0];
		m[1] += other.m[1];
		m[2] += other.m[2];
		return (*this);
	}
	__INLINE Matrix3 &operator -= (Matrix3 other)
	{
		m[0] -= other.m[0];
		m[1] -= other.m[1];
		m[2] -= other.m[2];
		return (*this);
	}
	__INLINE Matrix3 &operator *= (float other)
	{
		m[0] *= other;
		m[1] *= other;
		m[2] *= other;
		return (*this);
	}
	__INLINE Matrix3 &operator /= (float other)
	{
		if (Equal(other, 0.0f))
			m[0] = m[1] = m[2] = Vector3();			
		else
		{
			float t = 1.0f/other;
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

	float cofac(int r1, int c1, int r2, int c2)
		{			
			return (m[r1][c1] * m[r2][c2] - m[r1][c2] * m[r2][c1]);
		}

	__INLINE Matrix3 inverse()
	{
		Vector3 co = Vector3(cofac(1, 1, 2, 2), cofac(1, 2, 2, 0), cofac(1, 0, 2, 1));
		float det = m[0]*co;
		if (det == 0.0f)
			return Matrix3(0, 0, 0, 0 ,0, 0, 0 ,0, 0);
		float s = 1.0f / det;
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
		float	e11, e12, e13, e14,
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

	__INLINE Matrix2 Inverted() const
	{
		Matrix2 temp = *this;
		temp = Matrix2(e22, -e12, -e21, e11) * (1.0f / Determinant());
		return temp;
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

	__INLINE float Determinant() const
	{
		return e11 * e22 - e12 * e21;
	}
};

class Vector4
{
public:
	float x, y, z, w;

	__INLINE float operator[](int i)
	{
		if (i<0 || i> 3)
			return 0.0f;
		return *(&x + i);
	}


	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
		{}
	Vector4(float Ax, float Ay, float Az, float Aw) : x(Ax), y(Ay), z(Az), w(Aw)
		{}
	Vector4(const Vector3 &v) : x(v.x), y(v.y), z(v.z), w(1.0f)
		{};
	//void operator=(const Vector4 &q){x=q.x;y=q.y;z=q.z;w=q.w;}
	__INLINE Vector4 operator+(Vector4 q){return Vector4(x + q.x , y + q.y, z + q.z, w + q.w);}
	__INLINE Vector4 operator-(Vector4 q){return Vector4(x - q.x , y - q.y, z - q.z, w - q.w);}
	__INLINE Vector4 operator*(float s){return Vector4(x * s, y * s, z * s, w * s);}
	__INLINE Vector4 operator=(const Vector4& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = V.w;
		return *this;
	}

	__INLINE Vector4 operator/(float s)
	{
		if (s == 0.0f)
			return Vector4();
		float is = 1.0f/s;
		return (*this)*is;
	}
	__INLINE Vector4 operator+=(Vector4 q){(*this) = (*this)+q; return *this;}
	__INLINE Vector4 operator-=(Vector4 q){(*this) = (*this)-q; return *this;}
	float Norm()const{return x * x + y * y + z * z + w * w;}
	float Length(){return sqrt(x * x + y * y + z * z + w * w);}	
	Vector4 Conjugate(){ return Vector4(-x,-y,-z,w);}
	Vector4 Identity()
	{
		//@todo: Danger chek division by zero
		return (*this).Conjugate()/(*this).Norm();
	};
	float InnerProduct(const Vector4& q){return x*q.x+y*q.y+z*q.z+w*q.w;}

		__INLINE Vector4 operator*(const Vector4 &q)
	{
		Vector4 result;
		result.x = w * q.x + x * q.w + y * q.z - z * q.y;
		result.y = w * q.y - x * q.z + y * q.w + z * q.x;
		result.z = w * q.z + x * q.y - y * q.x + z * q.w;
		result.w = w * q.w - x * q.x - y * q.y - z * q.z;

		return result;
	}
		
	__INLINE void AxisAngle(Vector3& axis, float& angle) const
	{
		float vl = static_cast<float>(sqrt(x*x + y*y + z*z));
		if (vl > 0.0001f)
		{
			float ivl = 1.0f/vl;
			axis = Vector3(x*ivl, y*ivl, z*ivl);
			if (w < 0)
				angle = 2.0f * static_cast<float>(atan2(-vl, -w)); //-PI,0 
			else
				angle = 2.0f * static_cast<float>(atan2(vl,  w)); //0,PI 
		}
		else
		{
			axis = Vector3(0,0,0);
			angle = 0;
		}
    }


	void Rotate(float amount, float xAxis, float yAxis, float zAxis)
	{
		if ((xAxis != 0 && xAxis != 1) ||
			(yAxis != 0 && yAxis != 1) ||
			(zAxis != 0 && zAxis != 1))
		{
			float length = static_cast<float>(sqrt(xAxis * xAxis + yAxis * yAxis + zAxis * zAxis));
			if (length == 0.0f)
				return;
			xAxis /= length; yAxis /= length; zAxis /= length;
		}
		float angle = DegToRad(amount);
		float sine = static_cast<float>(sin(angle / 2.0f));
		x = xAxis * sine;
		y = yAxis * sine;
		z = zAxis * sine;
		w = static_cast<float>(cos(angle / 2.0f));
		//@todo: division by zero check
		float length = 1 / static_cast<float>(sqrt(x * x + y * y + z * z + w * w));
		x *= length;
		y *= length;
		z *= length;
	}

	__INLINE void toMatrix3(Matrix3 &m)const
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
		float s  = 2.0f/(*this).Norm();  
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


};

template <>
__INLINE Vector4 RandomRange<Vector4>(const Vector4 &AMin, const Vector4 &AMax)
{
	return Vector4
		(
			RandomRange(AMin.x, AMax.x),
			RandomRange(AMin.y, AMax.y),
			RandomRange(AMin.z, AMax.z),
			RandomRange(AMin.w, AMax.w)
		);
}

typedef Vector4 Quaternion;

/**
*	Despite Vector4 (aka Quaternion) and RGBA have much in common they
*	have some difference and I decided to get rid of union and put them into
*	separate classes without common base class. That's it.
*/
template <typename T>
class RGBA
{
public:
		T r, g, b, a;

		__INLINE RGBA& operator =(const RGBA& rhs)
		{
			r = rhs.r;
			g = rhs.g;
			b = rhs.b;
			a = rhs.a;
			return *this;
		}

		__INLINE RGBA operator +(const RGBA &rhs) const
		{			
			return RGBA(r + rhs.r , g + rhs.g, b + rhs.b, a + rhs.a);
		}

		__INLINE RGBA operator -(const RGBA &rhs) const
		{
			return RGBA(r - rhs.r , g - rhs.g, b - rhs.b, a - rhs.a);
		}

		__INLINE RGBA operator *(float rhs) const
		{
			return RGBA(r * rhs, g * rhs, b * rhs, a * rhs);
		}

		__INLINE RGBA operator /(float rhs) const
		{
			float rhs_1 = 1.0f / rhs;
			return (*this) * rhs_1;
		}

		__INLINE RGBA& operator +=(const RGBA &rhs)
		{
			r += rhs.r;
			g += rhs.g;
			b += rhs.b;
			a += rhs.a;
			return *this;
		}

		__INLINE RGBA& operator -=(const RGBA &rhs)
		{
			r -= rhs.r;
			g -= rhs.g;
			b -= rhs.b;
			a -= rhs.a;
			return *this;
		}

		__INLINE RGBA& operator *=(float rhs)
		{
			this->r *= rhs;
			this->g *= rhs;
			this->b *= rhs;
			this->a *= rhs;
			return *this;
		}

		__INLINE RGBA& operator /=(float rhs)
		{
			float rhs_1 = 1.0f / rhs;
			return (*this) *= rhs_1;
		}

		__INLINE bool operator ==(const RGBA &rhs) const
		{
			return Equal(r, rhs.r) && Equal(g, rhs.g) && Equal(b, rhs.b) && Equal(a, rhs.a);
		}

		__INLINE bool operator !=(const RGBA &rhs) const
		{
			return !((*this) == rhs);
		}

		__INLINE T& operator [](unsigned int i)
		{
			assert(i < 4);
			return *(&r + i);
		}

		__INLINE T operator [](unsigned int i) const
		{
			assert(i < 4);
			return *(&r + i);
		}
};

template <>
class RGBA<float>
{
public:
	float r, g, b, a;
	RGBA(float Ar = 0.0f, float Ag = 0.0f, float Ab = 0.0f, float Aa = 1.0f) : r(Ar), g(Ag),	b(Ab), a(Aa) {}
	__INLINE RGBA& operator =(const RGBA& rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	__INLINE RGBA operator +(const RGBA &rhs) const
	{			
		return RGBA(r + rhs.r , g + rhs.g, b + rhs.b, a + rhs.a);
	}

	__INLINE RGBA operator -(const RGBA &rhs) const
	{
		return RGBA(r - rhs.r , g - rhs.g, b - rhs.b, a - rhs.a);
	}

	__INLINE RGBA operator *(float rhs) const
	{
		return RGBA(r * rhs, g * rhs, b * rhs, a * rhs);
	}

	__INLINE RGBA operator /(float rhs) const
	{
		float rhs_1 = 1.0f / rhs;
		return (*this) * rhs_1;
	}

	__INLINE RGBA& operator +=(const RGBA &rhs)
	{
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		a += rhs.a;
		return *this;
	}

	__INLINE RGBA& operator -=(const RGBA &rhs)
	{
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		a -= rhs.a;
		return *this;
	}

	__INLINE RGBA& operator *=(float rhs)
	{
		this->r *= rhs;
		this->g *= rhs;
		this->b *= rhs;
		this->a *= rhs;
		return *this;
	}

	__INLINE RGBA& operator /=(float rhs)
	{
		float rhs_1 = 1.0f / rhs;
		return (*this) *= rhs_1;
	}

	__INLINE bool operator ==(const RGBA &rhs) const
	{
		return Equal(r, rhs.r) && Equal(g, rhs.g) && Equal(b, rhs.b) && Equal(a, rhs.a);
	}

	__INLINE bool operator !=(const RGBA &rhs) const
	{
		return !((*this) == rhs);
	}

	__INLINE float& operator [](unsigned int i)
	{
		assert(i < 4);
		return *(&r + i);
	}

	__INLINE float operator [](unsigned int i) const
	{
		assert(i < 4);
		return *(&r + i);
	}

};

template <>
class RGBA<byte>
{
public:
	byte r, g, b, a;
	RGBA(byte Ar = 0, byte Ag = 0, byte Ab = 0, byte Aa = 255) : r(Ar), g(Ag),	b(Ab), a(Aa) {}
	__INLINE RGBA& operator =(const RGBA& rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	__INLINE RGBA operator +(const RGBA &rhs) const
	{			
		return RGBA(r + rhs.r , g + rhs.g, b + rhs.b, a + rhs.a);
	}

	__INLINE RGBA operator -(const RGBA &rhs) const
	{
		return RGBA(r - rhs.r , g - rhs.g, b - rhs.b, a - rhs.a);
	}

	__INLINE RGBA operator *(float rhs) const
	{
		return RGBA(r * rhs, g * rhs, b * rhs, a * rhs);
	}

	__INLINE RGBA operator /(float rhs) const
	{
		float rhs_1 = 1.0f / rhs;
		return (*this) * rhs_1;
	}

	__INLINE RGBA& operator +=(const RGBA &rhs)
	{
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		a += rhs.a;
		return *this;
	}

	__INLINE RGBA& operator -=(const RGBA &rhs)
	{
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		a -= rhs.a;
		return *this;
	}

	__INLINE RGBA& operator *=(float rhs)
	{
		this->r *= rhs;
		this->g *= rhs;
		this->b *= rhs;
		this->a *= rhs;
		return *this;
	}

	__INLINE RGBA& operator /=(float rhs)
	{
		float rhs_1 = 1.0f / rhs;
		return (*this) *= rhs_1;
	}

	__INLINE bool operator ==(const RGBA &rhs) const
	{
		return Equal(r, rhs.r) && Equal(g, rhs.g) && Equal(b, rhs.b) && Equal(a, rhs.a);
	}

	__INLINE bool operator !=(const RGBA &rhs) const
	{
		return !((*this) == rhs);
	}

	__INLINE byte& operator [](unsigned int i)
	{
		assert(i < 4);
		return *(&r + i);
	}

	__INLINE byte operator [](unsigned int i) const
	{
		assert(i < 4);
		return *(&r + i);
	}

};

typedef RGBA<float> RGBAf;
typedef RGBA<byte> RGBAub;

template <typename T >
__INLINE RGBA<T> RandomRange(const RGBA<T> &AMin, const RGBA<T> &AMax)
{
	return RGBA<T>
		(
			RandomRange(AMin.r, AMax.r),
			RandomRange(AMin.g, AMax.g),
			RandomRange(AMin.b, AMax.b),
			RandomRange(AMin.a, AMax.a)
		);
}

/*
template <>
RGBAf RandomRange<RGBAf>(const RGBAf &AMin, const RGBAf &AMax)
{
	return RGBAf
		(
			Random_Float(AMin.r, AMax.r),
			Random_Float(AMin.g, AMax.g),
			Random_Float(AMin.b, AMax.b),
			Random_Float(AMin.a, AMax.a)
		);
}

template <>
RGBAub RandomRange<RGBAub>(const RGBAub &AMin, const RGBAub &AMax)
{
	return RGBAub
		(
			Random_Int(AMin.r, AMax.r),
			Random_Int(AMin.g, AMax.g),
			Random_Int(AMin.b, AMax.b),
			Random_Int(AMin.a, AMax.a)
		);
}
*/

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
	CBox(int x, int y, unsigned int width, unsigned int height);
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


/**
*	SqareEq(...) Решает квдратное уравнение.
*	Корни - t0 и t1
*	a b c - соответствующие коэффициенты квадратного уравнения.
*/

bool SqareEq(float a, float b, float c, float &t0, float &t1);

/**
*	HalfPlaneSign - определяет знак полуплоскости точки x относительно прямой, лежащей на отрезке [u0, u1].
*	Если точка лежит на прямой, то результат 0.0f
*/

float HalfPlaneSign(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

/**
*	DistanceToLine - определяет расстояние со знаком от точки до прямой, лежащей на отрезке.
*/

float DistanceToLine(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

/**
*	DistanceToSegment - определяет расстояние со знаком от точки до отрезка.
*	@todo: Возможно есть более быстрый способ определять знак расстояниея в случаях конца отрезка.
*	@todo: Возможно стоит возвращать ещё и найденный случай, т.е. первая точка отрезка, вторая или сам отрезок.
*/

float DistanceToSegment(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

/**
*	IntersectLines - определяет точку пересечения прямых, лежащих на отрезках, если они пересекаются.
*/

bool IntersectLines(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result);

/**
*	IntersectSegments(): returns intersection point if segments intersect.
*	Notice, that it is segments, not infinite lines.
*	@todo: check out coincident lines case.
*/

bool IntersectSegments(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result);

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
	virtual void CopyVertices(Vector2 *Destination, unsigned int &VerticesCount)
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
	CCircle(Vector2 APosition, float ARadius);
	void CalcBox()
	{
		Box.Min = Vector2(Position.x - Radius, Position.y - Radius);
		Box.Min = Vector2(Position.x + Radius, Position.y + Radius);
	}
};

//////////////////////////////////////////////////////////////////////////
//CPolygon @todo: decide: should we update BBOX each time, we add or remove vertices
// or just leave it with possibility to update BBOX each time we need it correct

class CPolygon : public CGeometry
{
public:
	CPolygon();
	CPolygon(unsigned int AVerticesCount);
	~CPolygon();
	void Reset(unsigned int AVerticesCount);
	void CalcBox();
	void AddVertex(const Vector2 &Vertex);
	void RemoveVertex(unsigned int Index);
	Vector2& operator [](unsigned int Index);
	const Vector2& operator [](unsigned int Index) const;
	unsigned int GetVertexCount() const;

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
	unsigned int VerticesCount;
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

#endif // _2DE_MATH_UTILS_H_
