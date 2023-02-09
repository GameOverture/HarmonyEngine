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

//#define HyRadToDeg(radian) ((radian) * (180.0f / HY_PI)) // Use glm::degrees() instead
//#define HyDegToRad(degree) ((degree) * (HY_PI / 180.0f)) // Use glm::radians() instead

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
void HyCopyVec(glm::vec4 &destRef, const glm::vec3 &srcRef);
void HyCopyVec(glm::vec4 &destRef, const glm::vec2 &srcRef);

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

class HyColor
{
	uint8		m_uiA;
	uint8		m_uiR;
	uint8		m_uiG;
	uint8		m_uiB;

public:
	HyColor() :
		m_uiA(0xFF),
		m_uiR(0x00),
		m_uiG(0x00),
		m_uiB(0x00)
	{ }
	//HyColor(uint8 uiRed, uint8 uiGreen, uint8 uiBlue, uint8 uiAlpha = 0xFF) :
	//	m_uiA(uiAlpha),
	//	m_uiR(uiRed),
	//	m_uiG(uiGreen),
	//	m_uiB(uiBlue)
	//{ }
	HyColor(int32 iRed, int32 iGreen, int32 iBlue, int32 iAlpha = 0xFF) :
		m_uiA(iAlpha & 0xFF),
		m_uiR(iRed & 0xFF),
		m_uiG(iGreen & 0xFF),
		m_uiB(iBlue & 0xFF)
	{ }
	HyColor(uint32 uiColor) :
		m_uiA(0xFF),//(uiColor >> 24) & 0xFF), // Safety measure to not accidentally consider 0x0 as opaque black
		m_uiR((uiColor >> 16) & 0xFF),
		m_uiG((uiColor >> 8) & 0xFF),
		m_uiB(uiColor & 0xFF)
	{ }
	HyColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f) :
		m_uiA(static_cast<uint8>(255.0f * HyClamp(fAlpha, 0.0f, 1.0f))),
		m_uiR(static_cast<uint8>(255.0f * HyClamp(fRed, 0.0f, 1.0f))),
		m_uiG(static_cast<uint8>(255.0f * HyClamp(fGreen, 0.0f, 1.0f))),
		m_uiB(static_cast<uint8>(255.0f * HyClamp(fBlue, 0.0f, 1.0f)))
	{ }

	// Returned as uint32 mask: ARGB
	uint32 GetAsUint32() const {
		return (m_uiA << 24) | (m_uiR << 16) | (m_uiG << 8) | m_uiB;
	}

	uint8 GetRed() const {
		return m_uiR;
	}
	float GetRedF() const {
		return m_uiR / 255.0f;
	}

	uint8 GetGreen() const {
		return m_uiG;
	}
	float GetGreenF() const {
		return m_uiG / 255.0f;
	}

	uint8 GetBlue() const {
		return m_uiB;
	}
	float GetBlueF() const {
		return m_uiB / 255.0f;
	}

	uint8 GetAlpha() const {
		return m_uiA;
	}
	float GetAlphaF() const {
		return m_uiA / 255.0f;
	}

	glm::vec4 GetAsVec4() const {
		return glm::vec4(GetRedF(), GetGreenF(), GetBlueF(), GetAlphaF());
	}

	glm::vec3 GetAsVec3() const {
		return glm::vec3(GetRedF(), GetGreenF(), GetBlueF());
	}

	HyColor Lighten() const
	{
		return HyColor((GetAsUint32() & 0xfefefe) << 1);
	}
	HyColor Darken() const
	{
		return HyColor((GetAsUint32() & 0xfefefe) >> 1);
	}
	float Brightness() const
	{
		return sqrtf(0.299f * (m_uiR * m_uiR) + 0.587f * (m_uiG * m_uiG) + 0.114f * (m_uiB * m_uiB));
	}
	bool IsLight() const
	{
		return Brightness() > 127.5f;
	}
	bool IsDark() const
	{
		return Brightness() <= 127.5f;
	}

	static const HyColor White;
	static const HyColor Black;
	static const HyColor Red;
	static const HyColor DarkRed;
	static const HyColor Green;
	static const HyColor DarkGreen;
	static const HyColor Blue;
	static const HyColor DarkBlue;
	static const HyColor Cyan;
	static const HyColor DarkCyan;
	static const HyColor Magenta;
	static const HyColor DarkMagenta;
	static const HyColor Yellow;
	static const HyColor DarkYellow;
	static const HyColor Gray;
	static const HyColor DarkGray;
	static const HyColor LightGray;
	static const HyColor Orange;
	static const HyColor Transparent;
	static const HyColor ContainerPanel;
	static const HyColor ContainerFrame;
	static const HyColor WidgetPanel;
	static const HyColor WidgetFrame;
};
typedef HyColor HyColour;

class HyMath
{
public:
	static glm::ivec2 LockAspectRatio(int32 iOldWidth, int32 iOldHeight, int32 iNewWidth, int32 iNewHeight);
	static void InvalidateAABB(b2AABB &aabbOut);

	static glm::vec2 PerpendicularClockwise(const glm::vec2 &vDirVector);
	static glm::ivec2 PerpendicularClockwise(const glm::ivec2 &vDirVector);
	static glm::vec2 PerpendicularCounterClockwise(const glm::vec2 &vDirVector);
	static glm::ivec2 PerpendicularCounterClockwise(const glm::ivec2 &vDirVector);

	static float AngleFromVector(const glm::vec2 &vDirVector);
	static glm::vec2 ClosestPointOnRay(const glm::vec2 &ptRayStart, const glm::vec2 &vNormalizedRayDir, const glm::vec2 &ptTestPoint);

	// Normalizes a value to an arbitrary range. The value wraps when going below min range or above max range.
	static float NormalizeRange(float fValue, float fMin, float fMax);
	static int32 NormalizeRange(int32 iValue, int32 iMin, int32 iMax);

	// Rounds a given number to the nearest multiple of a specified value
	template<typename TYPE>
	static TYPE RoundToNearest(TYPE value, TYPE multiple)
	{
		return round(value / multiple) * multiple;
	}

	template <typename VEC>
	static int32 HalfSpaceTest(const VEC &ptTestPoint, const VEC &vNormal, const VEC &ptPointOnPlane)
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
};

#endif /* HyMath_h__ */
