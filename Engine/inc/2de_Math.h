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
#include <vector>

#ifndef __INLINE
	#if defined(_MSC_VER)
		#define __INLINE __forceinline
	#elif __GNUC__ >= 4
		#define __INLINE __inline __attribute__ ((always_inline))
	#else
		#define __INLINE inline
	#endif
#endif

#if defined(min)
	#undef min
#endif

#if defined(max)
	#undef max
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
extern unsigned g_seed;

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
	assert(false);
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
__INLINE float Snap(float p, float grid)
{
	return grid ? static_cast<float>(floor((p + grid * 0.5f) / grid) * grid) : p;
}

class Vector2
{
public:
	float x, y;

	__INLINE Vector2(float Ax = 0.0f, float Ay = 0.0f) : x(Ax), y(Ay){}

	__INLINE float operator[](int i)
	{
		assert(i >= 0 && i <= 1);
		return *(&x + i);
	}
	
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
		assert(a != 0.0f);
		float t = 1.0f / a;
		return Vector2(x * t, y * t);
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
		assert(l != 0.0f);
		*this /= l;
		return l;
	}
};

const Vector2 V2_ZERO		= Vector2(0.0f, 0.0f);
const Vector2 V2_DIR_LEFT	= Vector2(-1.0f, 0.0f);
const Vector2 V2_DIR_RIGHT	= (-V2_DIR_LEFT);
const Vector2 V2_DIR_UP		= Vector2(0.0f, 1.0f);
const Vector2 V2_DIR_DOWN	= (-V2_DIR_UP);
const Vector2 V2_DIRECTIONS[4] = {V2_DIR_LEFT, V2_DIR_DOWN, V2_DIR_RIGHT, V2_DIR_UP,};

template<>
__INLINE Vector2 RandomRange<Vector2>(const Vector2 &AMin, const Vector2 &AMax)
{
	return Vector2
		(
			Random_Float(AMin.x, AMax.x),
			Random_Float(AMin.y, AMax.y)
		);
}

template<>
__INLINE Vector2 Clamp(const Vector2 &x, const Vector2 &min, const Vector2 &max)
{
	Vector2 result;
	result.x = Clamp(x.x, min.x, max.x);
	result.y = Clamp(x.y, min.y, max.y);
}

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
		for(unsigned i = 0; i < N; i++)
			Elements[i] = Vertices[i];
	}
	Vector2Array()
	{
		for(unsigned i = 0; i < N; i++)
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

class Vector3
{
public:
	float x, y, z;

	__INLINE Vector3(float Ax = 0.0f, float Ay = 0.0f, float Az = 0.0f) : x(Ax), y(Ay), z(Az){}

	__INLINE float operator[](int i)
	{
		assert(i >= 0 && i <= 2);
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
		assert(s != 0.0f);
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
		// to be implemented properly
		assert(false);
		return *this;
	}

	float Normalize()
	{
		float l = Length();
		assert(l != 0.0f);
		(*this) *= (1.0f / l);
		return l;
	}

	Vector3 Normalized()
	{
		float l = Length();
		assert(l != 0.0f);
		Vector3 t = (*this) * (1.0f / l);
		return t;
	}

	float ComputeNormal(const Vector3& V0, const Vector3& V1, const Vector3& V2)
	{
		Vector3 E = V1 - V0;
		Vector3 F = V2 - V1;
		(*this)  = E ^ F;
		return (*this).Normalize();
	}
};

template<>
__INLINE Vector3 Clamp(const Vector3 &x, const Vector3 &min, const Vector3 &max)
{
	Vector3 result;
	result.x = Clamp(x.x, min.x, max.x);
	result.y = Clamp(x.y, min.y, max.y);
	result.z = Clamp(x.z, min.z, max.z);
}

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

		__INLINE T& operator [](unsigned i)
		{
			assert(i < 4);
			return *(&r + i);
		}

		__INLINE T operator [](unsigned i) const
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

	__INLINE float& operator [](unsigned i)
	{
		assert(i < 4);
		return *(&r + i);
	}

	__INLINE float operator [](unsigned i) const
	{
		assert(i < 4);
		return *(&r + i);
	}

};

template <>
class RGBA<unsigned char>
{
public:
	unsigned char r, g, b, a;
	RGBA(unsigned char Ar = 0, unsigned char Ag = 0, unsigned char Ab = 0, unsigned char Aa = 255) : r(Ar), g(Ag),	b(Ab), a(Aa) {}
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

	__INLINE unsigned char& operator [](unsigned i)
	{
		assert(i < 4);
		return *(&r + i);
	}

	__INLINE unsigned char operator [](unsigned i) const
	{
		assert(i < 4);
		return *(&r + i);
	}

};

typedef RGBA<float> RGBAf;
typedef RGBA<unsigned char> RGBAub;

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

template <unsigned COLUMNS, unsigned ROWS>
class Matrix
{
public:
	float e[ROWS][COLUMNS];

};

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

/**
*	CalcConvexHull does exactly how it named.
*	Taken from http://www.e-maxx.ru/algo/convex_hull_graham and slightly 
*	modified in order to fit naming conventions and interfaces
*/
__INLINE void CalcConvexHull (std::vector<Vector2> &a);

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
	CPolygon(unsigned AVerticesCount);
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
template <typename G1, typename G2>
bool tCollide(	const G1 &BodyA,
					const Vector2 &Apos,
					const Vector2 &Avel,
					const Matrix2 &Aorient,
				const G2 &BodyB,
					const Vector2 &Bpos, 
					const Vector2 &Bvel, 
					const Matrix2 &Borient,
				Vector2 &Normal, float &Depth)
{

}

template <>
__INLINE bool tCollide (	const CPolygon &A,
									const Vector2 &Apos,
									const Vector2 &Avel,
									const Matrix2 &Aorient,
									const CPolygon &B,
									const Vector2 &Bpos, 
									const Vector2 &Bvel, 
									const Matrix2 &Borient,
									Vector2& Normal,
									float& Depth)
{
	return CPolygon::Collide(A, Apos, Avel, Aorient, B, Bpos, Bvel, Borient, Normal, Depth);
}


#endif // _2DE_MATH_UTILS_H_
