/**************************************************************************
 *	HyMath.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyMath_h__
#define HyMath_h__

#include "Afx/HyStdAfx.h"

#define HY_PI 3.141592f

//#define HyRadToDeg(radian) ((radian) * (180.0f / HY_PI))
//#define HyDegToRad(degree) ((degree) * (HY_PI / 180.0f))

// maps unsigned 8 bits/channel to HYCOLOR
#define HYCOLOR_ARGB(a,r,g,b) \
	((uint32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define HYCOLOR_RGBA(r,g,b,a) HYCOLOR_ARGB(a,r,g,b)
#define HYCOLOR_XRGB(r,g,b)   HYCOLOR_ARGB(0xff,r,g,b)

#define HYCOLOR_XYUV(y,u,v)   HYCOLOR_ARGB(0xff,y,u,v)
#define HYCOLOR_AYUV(a,y,u,v) HYCOLOR_ARGB(a,y,u,v)

void HySetVec(glm::vec2 &vecOut, float fX, float fY);
void HySetVec(glm::vec3 &vecOut, float fX, float fY, float fZ);
void HySetVec(glm::vec4 &vecOut, float fX, float fY, float fZ, float fW);
void HySetVec(glm::ivec2 &vecOut, int32 iX, int32 iY);
void HySetVec(glm::ivec3 &vecOut, int32 iX, int32 iY, int32 iZ);
void HySetVec(glm::ivec4 &vecOut, int32 iX, int32 iY, int32 iZ, int32 iW);

void HyCopyVec(glm::vec2 &destRef, const glm::vec2 &srcRef);
void HyCopyVec(glm::vec3 &destRef, const glm::vec3 &srcRef);
void HyCopyVec(glm::vec2 &destRef, const glm::vec3 &srcRef);
void HyCopyVec(glm::vec3 &destRef, const glm::vec2 &srcRef);

template <typename VEC>
int32 HyHalfSpaceTest(const VEC &ptTestPoint, const VEC &vNormal, const VEC &ptPointOnPlane)
{
	// Calculate a vector from the point on the plane to our test point
	VEC vTemp(ptTestPoint - ptPointOnPlane);

	// Calculate the distance: dot product of the new vector with the plane's normal
	float fDist = glm::dot(vTemp, vNormal);

	float fEpsilon = std::numeric_limits<float>::epsilon();
	if(fDist > fEpsilon)
	{
		// ptTestPoint is in front of the plane
		return 1;
	}
	else if(fDist < -fEpsilon)
	{
		// ptTestPoint is behind the plane
		return -1;
	}

	// If neither of these were true, then ptTestPoint is on the plane
	return 0;
}

bool HyTestPointAABB(const b2AABB &aabb, const glm::vec2 &pt);

template <typename T>
bool HyCompareFloat(T lhs, T rhs)
{
	return trunc(1000. * lhs) == trunc(1000. * rhs);
}

template <typename T>
T HyRound(T num)
{
	return (num - floor(num) >= 0.5) ? ceil(num) : floor(num);
}

template <typename T>
T HyClamp(const T& value, const T& low, const T& high) 
{
	return value < low ? low : (value > high ? high : value); 
}

template <class T>
const T &HyMin(const T &a, const T &b)
{
	return (a < b) ? a : b;
}

template <class T>
const T &HyMax(const T &a, const T &b)
{
	return (a > b) ? a : b;
}

float HyEase_Linear(float a, float b, float t);

//--------------------------------------------------------------------------------------
// Accelerating from zero velocity
//--------------------------------------------------------------------------------------
float HyEase_QuadraticIn(float fTime, float fStart, float fDist, float fDur);

//--------------------------------------------------------------------------------------
// Decelerating to zero velocity
//--------------------------------------------------------------------------------------
float HyEase_QuadraticOut(float fTime, float fStart, float fDist, float fDur);

//--------------------------------------------------------------------------------------
// Acceleration until halfway, then deceleration
//--------------------------------------------------------------------------------------
float HyEase_QuadraticInOut(float fTime, float fStart, float fDist, float fDur);

// 
float HyEase_AngleLinear(float angleA, float angleB, int32 spin, float t);

template<typename T>
struct HyRectangle
{
	T left;
	T bottom;
	T right;
	T top;

	int32 iTag;

	HyRectangle() : left(0), bottom(0), right(0), top(0), iTag(0)
	{ }

	HyRectangle(T tLeft, T tBottom, T tRight, T tTop) : left(tLeft), bottom(tBottom), right(tRight), top(tTop), iTag(0)
	{ }

	std::string ToString() const
	{
		std::ostringstream s;
		s << "(L:" << left << ", T:" << top << ", R:" << right << ", B:" << bottom << ")";
		return s.str();
	}

	T Width() const
	{
		return right - left;
	}

	T Height() const
	{
		return abs(bottom - top);
	}

	void Set(T tLeft, T tBottom, T tRight, T tTop)
	{
		left = tLeft;
		top = tTop;
		right = tRight;
		bottom = tBottom;
	}

	// Does not compare tags
	bool operator==(const HyRectangle<T> &rhs) const
	{
		return (0 == memcmp(this, &rhs, sizeof(T) * 4));
	}
};

template<typename T>
struct HyScreenRect
{
	T x;
	T y;
	T width;
	T height;

	int32 iTag;

	HyScreenRect() : x(0), y(0), width(0), height(0), iTag(0)
	{ }

	HyScreenRect(T tX, T tY, T tWidth, T tHeight) : x(tX), y(tY), width(tWidth), height(tHeight), iTag(0)
	{ }

	std::string ToString() const
	{
		std::ostringstream s;
		s << "(X:" << x << ", Y:" << y << ", W:" << width << ", H:" << height << ")";
		return s.str();
	}

	// Does not compare tags
	bool operator==(const HyScreenRect<T> &rhs) const
	{
		return (0 == memcmp(this, &rhs, sizeof(T) * 4));
	}
};

class HyMath
{
public:
	static glm::ivec2 LockAspectRatio(int32 iOldWidth, int32 iOldHeight, int32 iNewWidth, int32 iNewHeight);
	static void InvalidateAABB(b2AABB &aabbOut);
};

#endif /* HyMath_h__ */
