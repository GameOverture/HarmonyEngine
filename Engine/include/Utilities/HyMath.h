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
#include "Scene/AnimFloats/HyTweenFuncs.h"

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
void HySetVec(glm::vec2 &vecOut, const glm::vec2 &srcRef);
void HySetVec(glm::vec2 &vecOut, const glm::vec3 &srcRef);
void HySetVec(glm::vec2 &vecOut, const glm::vec4 &srcRef);
void HySetVec(glm::ivec2 &vecOut, int32 iX, int32 iY);
void HySetVec(glm::ivec2 &vecOut, const glm::ivec2 &srcRef);
void HySetVec(glm::ivec2 &vecOut, const glm::ivec3 &srcRef);
void HySetVec(glm::ivec2 &vecOut, const glm::ivec4 &srcRef);

void HySetVec(glm::vec3 &vecOut, float fX, float fY, float fZ);
void HySetVec(glm::vec3 &vecOut, const glm::vec2 &srcRef);
void HySetVec(glm::vec3 &vecOut, const glm::vec3 &srcRef);
void HySetVec(glm::vec3 &vecOut, const glm::vec4 &srcRef);
void HySetVec(glm::ivec3 &vecOut, int32 iX, int32 iY, int32 iZ);
void HySetVec(glm::ivec3 &vecOut, const glm::ivec2 &srcRef);
void HySetVec(glm::ivec3 &vecOut, const glm::ivec3 &srcRef);
void HySetVec(glm::ivec3 &vecOut, const glm::ivec4 &srcRef);

void HySetVec(glm::vec4 &vecOut, float fX, float fY, float fZ, float fW);
void HySetVec(glm::vec4 &vecOut, const glm::vec2 &srcRef);
void HySetVec(glm::vec4 &vecOut, const glm::vec3 &srcRef);
void HySetVec(glm::vec4 &vecOut, const glm::vec4 &srcRef);
void HySetVec(glm::ivec4 &vecOut, int32 iX, int32 iY, int32 iZ, int32 iW);
void HySetVec(glm::ivec4 &vecOut, const glm::ivec2 &srcRef);
void HySetVec(glm::ivec4 &vecOut, const glm::ivec3 &srcRef);
void HySetVec(glm::ivec4 &vecOut, const glm::ivec4 &srcRef);

template <typename T>
bool HyCompareFloat(T lhs, T rhs)
{
	return trunc(1000. * lhs) == trunc(1000. * rhs);
}

class HyRect
{
	float		m_fHalfWidth;
	float		m_fHalfHeight;
	glm::vec2	m_ptCenter;
	float		m_fRotDegrees; // Rotates around the center of the rect

public:
	HyRect() :
		m_fHalfWidth(0.0f),
		m_fHalfHeight(0.0f),
		m_ptCenter(0.0f),
		m_fRotDegrees(0.0f)
	{ }

	// Creates a rectangle with the bottom left corner at 0,0
	HyRect(float fWidth, float fHeight)
	{
		Set(fWidth, fHeight);
	}
	HyRect(const glm::ivec2 &vSize) // Creates a rectangle with the bottom left corner at 0,0
	{
		Set(static_cast<float>(vSize.x), static_cast<float>(vSize.y));
	}
	HyRect(const glm::vec2 &vSize) // Creates a rectangle with the bottom left corner at 0,0
	{
		Set(vSize.x, vSize.y);
	}
	HyRect(float fX, float fY, float fWidth, float fHeight)
	{
		Set(fX, fY, fWidth, fHeight);
	}
	HyRect(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptCenter, float fRotDegrees = 0.0f)
	{
		Set(fHalfWidth, fHalfHeight, ptCenter, fRotDegrees);
	}

	//std::string ToString() const
	//{
	//	std::ostringstream s;
	//	s << "(X:" <<  << ", Y:" << y << ", W:" << width << ", H:" << height;
	//	if(rot != 0.0f)
	//		s << ", Rot:" << rot;
	//	s << ")";
	//	return s.str();
	//}

	float GetWidth(float fPercent = 1.0f) const
	{
		return (m_fHalfWidth * 2.0f) * fPercent;
	}
	float GetHeight(float fPercent = 1.0f) const
	{
		return (m_fHalfHeight * 2.0f) * fPercent;
	}
	glm::vec2 GetCenter() const
	{
		return m_ptCenter;
	}
	float GetRotation() const
	{
		return m_fRotDegrees;
	}

	void Set(float fWidth, float fHeight)
	{
		m_fHalfWidth = fWidth * 0.5f;
		m_fHalfHeight = fHeight * 0.5f;
		HySetVec(m_ptCenter, m_fHalfWidth, m_fHalfHeight);
		m_fRotDegrees = 0.0f;
	}

	void Set(float fX, float fY, float fWidth, float fHeight)
	{
		m_fHalfWidth = fWidth * 0.5f;
		m_fHalfHeight = fHeight * 0.5f;
		HySetVec(m_ptCenter, fX + m_fHalfWidth, fY + m_fHalfHeight);
		m_fRotDegrees = 0.0f;
	}

	void Set(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptCenter, float fRotDegrees)
	{
		m_fHalfWidth = fHalfWidth;
		m_fHalfHeight = fHalfHeight;
		m_ptCenter = ptCenter;
		m_fRotDegrees = fRotDegrees;
	}
};

template<typename T>
struct HyMargins
{
	T left;
	T bottom;
	T right;
	T top;

	HyMargins() : left(0), bottom(0), right(0), top(0)
	{ }

	HyMargins(T tLeft, T tBottom, T tRight, T tTop) : left(tLeft), bottom(tBottom), right(tRight), top(tTop)
	{ }

	HyMargins<T> &operator=(const HyMargins<T> &rhs)
	{
		left = rhs.left;
		bottom = rhs.bottom;
		right = rhs.right;
		top = rhs.top;
		return *this;
	}

	std::string ToString() const
	{
		std::ostringstream s;
		s << "(L:" << left << ", T:" << top << ", R:" << right << ", B:" << bottom << ")";
		return s.str();
	}

	void Set(T tLeft, T tBottom, T tRight, T tTop)
	{
		left = tLeft;
		top = tTop;
		right = tRight;
		bottom = tBottom;
	}
};

struct HyUvCoord
{
	float left;
	float bottom;
	float right;
	float top;

	HyUvCoord() : left(0), bottom(0), right(0), top(0)
	{ }

	HyUvCoord(float fLeft, float fBottom, float fRight, float fTop) : left(fLeft), bottom(fBottom), right(fRight), top(fTop)
	{ }

	HyUvCoord &operator=(const HyUvCoord &rhs)
	{
		left = rhs.left;
		bottom = rhs.bottom;
		right = rhs.right;
		top = rhs.top;
		return *this;
	}

	std::string ToString() const
	{
		std::ostringstream s;
		s << "(L:" << left << ", T:" << top << ", R:" << right << ", B:" << bottom << ")";
		return s.str();
	}

	float Width() const
	{
		return right - left;
	}

	float Height() const
	{
		return abs(bottom - top);
	}

	void Set(float fLeft, float fBottom, float fRight, float fTop)
	{
		left = fLeft;
		top = fTop;
		right = fRight;
		bottom = fBottom;
	}
};

class HyMath
{
public:
	static const float							FloatSlop;

	template <typename T>
	static T Clamp(const T &value, const T &low, const T &high)
	{
		return value < low ? low : (value > high ? high : value);
	}

	template <class T>
	static const T &Min(const T &a, const T &b)
	{
		return (a < b) ? a : b;
	}

	static glm::vec2 Min(const glm::vec2 &a, const glm::vec2 &b)
	{
		return glm::vec2(Min(a.x, b.x), Min(a.y, b.y));
	}

	template <class T>
	static const T &Max(const T &a, const T &b)
	{
		return (a > b) ? a : b;
	}

	static glm::vec2 Max(const glm::vec2 &a, const glm::vec2 &b)
	{
		return glm::vec2(Max(a.x, b.x), Max(a.y, b.y));
	}

	static glm::ivec2 LockAspectRatio(int32 iOldWidth, int32 iOldHeight, int32 iNewWidth, int32 iNewHeight);
	static void InvalidateAABB(b2AABB &aabbOut);
	static bool TestPointAABB(const b2AABB &aabb, const glm::vec2 &pt);
	static bool TestOverlapAABB(const b2AABB &a, const b2AABB &b);

	static glm::vec2 PerpendicularClockwise(const glm::vec2 &vDirVector);
	static glm::ivec2 PerpendicularClockwise(const glm::ivec2 &vDirVector);
	static glm::vec2 PerpendicularCounterClockwise(const glm::vec2 &vDirVector);
	static glm::ivec2 PerpendicularCounterClockwise(const glm::ivec2 &vDirVector);

	static float AngleFromVector(const glm::vec2 &vDirVector); // Caution: Operationally expensive
	static glm::vec2 ClosestPointOnRay(const glm::vec2 &ptRayStart, const glm::vec2 &vNormalizedRayDir, const glm::vec2 &ptTestPoint);

	// Normalizes a value to an arbitrary range. The value wraps when going below min range or above max range.
	static float NormalizeRange(float fValue, float fMin, float fMax);
	static int32 NormalizeRange(int32 iValue, int32 iMin, int32 iMax);

	template<typename TYPE>
	static TYPE Round(TYPE value)
	{
		return round(value);
	}

	// Rounds a given number to the nearest multiple of a specified value
	template<typename TYPE>
	static TYPE RoundToNearest(TYPE value, TYPE multiple)
	{
		return round(value / multiple) * multiple;
	}

	// Round off decimal to nearest hundredth
	template<typename TYPE>
	static TYPE RoundToHundredth(TYPE value)
	{
		return round(value * 100) / 100;
	}

	// Round off decimal to nearest thousandth
	template<typename TYPE>
	static TYPE RoundToThousandth(TYPE value)
	{
		return round(value * 1000) / 1000;
	}

	static glm::vec2 RoundVec(const glm::vec2 &vectorOut)
	{
		return glm::vec2(round(vectorOut.x), round(vectorOut.y));
	}
	static glm::vec3 RoundVec(const glm::vec3 &vectorOut)
	{
		return glm::vec3(round(vectorOut.x), round(vectorOut.y), round(vectorOut.z));
	}
	static glm::vec4 RoundVec(const glm::vec4 &vectorOut)
	{
		return glm::vec4(round(vectorOut.x), round(vectorOut.y), round(vectorOut.z), round(vectorOut.w));
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

	static float TweenProgress(float fStart, float fEnd, float fElaspedTime, float fFullDuration, HyTweenFunc fpTweenFunc = HyTween::Linear);
};

// 24-bit color
class HyColor
{
	uint8		m_uiR;
	uint8		m_uiG;
	uint8		m_uiB;

public:
	HyColor() :
		m_uiR(0x00),
		m_uiG(0x00),
		m_uiB(0x00)
	{ }
	HyColor(int32 iRed, int32 iGreen, int32 iBlue) :
		m_uiR(iRed & 0xFF),
		m_uiG(iGreen & 0xFF),
		m_uiB(iBlue & 0xFF)
	{ }
	HyColor(uint32 uiHexCode) : // 24-bit hex code color as 0xRRGGBB - does not initialize alpha channel (which is set to 0xFF)
		m_uiR((uiHexCode >> 16) & 0xFF),
		m_uiG((uiHexCode >> 8) & 0xFF),
		m_uiB(uiHexCode & 0xFF)
	{ }
	HyColor(float fRed, float fGreen, float fBlue) :
		m_uiR(static_cast<uint8>(255.0f * HyMath::Clamp(fRed, 0.0f, 1.0f))),
		m_uiG(static_cast<uint8>(255.0f * HyMath::Clamp(fGreen, 0.0f, 1.0f))),
		m_uiB(static_cast<uint8>(255.0f * HyMath::Clamp(fBlue, 0.0f, 1.0f)))
	{ }

	bool operator==(const HyColor &rhs)
	{
		return m_uiR == rhs.m_uiR && m_uiG == rhs.m_uiG && m_uiB == rhs.m_uiB;
	}

	bool operator!=(const HyColor &rhs)
	{
		return !(*this == rhs);
	}

	uint32 GetAsHexCode() const {
#if defined(HY_ENDIAN_LITTLE)
		return (m_uiR << 16) | (m_uiG << 8) | m_uiB; // Returned as uint32 mask: as 0xRRGGBB
#else
		return (m_uiB << 16) | (m_uiG << 8) | m_uiR; // Returned as uint32 mask: as 0xRRGGBB
#endif
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

	glm::vec4 GetAsVec4() const {
		return glm::vec4(GetRedF(), GetGreenF(), GetBlueF(), 1.0f);
	}
	glm::vec3 GetAsVec3() const {
		return glm::vec3(GetRedF(), GetGreenF(), GetBlueF());
	}

	HyColor Shade(int32 iPercent) const
	{
		int32 R = m_uiR;
		int32 G = m_uiG;
		int32 B = m_uiB;

		if(R == 0)
			R = 64;
		if(G == 0)
			G = 64;
		if(B == 0)
			B = 64;

		R = R * (100 + iPercent) / 100;
		G = G * (100 + iPercent) / 100;
		B = B * (100 + iPercent) / 100;

		R = HyMath::Clamp(R, 0, 255);
		G = HyMath::Clamp(G, 0, 255);
		B = HyMath::Clamp(B, 0, 255);

		return HyColor(R, G, B);
	}
	HyColor Lighten() const
	{
		return Shade(40);
	}
	HyColor Darken() const
	{
		return Shade(-40);
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
	static const HyColor Pink;
	static const HyColor Green;
	static const HyColor DarkGreen;
	static const HyColor Blue;
	static const HyColor DarkBlue;
	static const HyColor Cyan;
	static const HyColor DarkCyan; // Teal
	static const HyColor Magenta;
	static const HyColor Purple;
	static const HyColor Yellow;
	static const HyColor Gold;
	static const HyColor DarkYellow;
	static const HyColor Gray;
	static const HyColor DarkGray;
	static const HyColor LightGray;
	static const HyColor Orange;
	static const HyColor Brown;
	static const HyColor PanelContainer;
	static const HyColor FrameContainer;
	static const HyColor PanelWidget;
	static const HyColor FrameWidget;
	static const HyColor _InternalUse;
};
typedef HyColor HyColour;

#endif /* HyMath_h__ */
