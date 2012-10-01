#ifndef _2DE_RGBA_H_
#define _2DE_RGBA_H_

#include "2de_Math.h"

namespace Deku2D
{
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

		float GetA() const { return a; }
		float GetR() const { return r; }
		float GetG() const { return g; }
		float GetB() const { return b; }

		void SetA(const float& _a) { a = _a; }
		void SetR(const float& _r) { r = _r; }
		void SetG(const float& _g) { g = _g; }
		void SetB(const float& _b) { b = _b; }

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
			return RGBA
				(
				static_cast<unsigned char>(r * rhs),
				static_cast<unsigned char>(g * rhs),
				static_cast<unsigned char>(b * rhs),
				static_cast<unsigned char>(a * rhs)
				);
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
			this->r = static_cast<unsigned char>(this->r * rhs);
			this->g = static_cast<unsigned char>(this->g * rhs);
			this->b = static_cast<unsigned char>(this->b * rhs);
			this->a = static_cast<unsigned char>(this->a * rhs);
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

}	//	namespace Deku2D

#endif	//	_2DE_RGBA_H_
