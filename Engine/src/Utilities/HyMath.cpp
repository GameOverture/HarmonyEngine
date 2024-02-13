/**************************************************************************
 *	HyMath.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"

const HyColor HyColor::White(0xffffff);
const HyColor HyColor::Black(0x000000);
const HyColor HyColor::Red(0xff0000);
const HyColor HyColor::DarkRed(0x800000);
const HyColor HyColor::Green(0xff00);
const HyColor HyColor::DarkGreen(0x008000);
const HyColor HyColor::Blue(0xff);
const HyColor HyColor::DarkBlue(0x000080);
const HyColor HyColor::Cyan(0xffff);
const HyColor HyColor::DarkCyan(0x008080);
const HyColor HyColor::Magenta(0xff00ff);
const HyColor HyColor::DarkMagenta(0x800080);
const HyColor HyColor::Yellow(0xffff00);
const HyColor HyColor::DarkYellow(0x808000);
const HyColor HyColor::Gray(0xa0a0a4);
const HyColor HyColor::DarkGray(0x808080);
const HyColor HyColor::LightGray(0xc0c0c0);
const HyColor HyColor::Orange(0xFA5A0A);
const HyColor HyColor::Brown(0x802600);
const HyColor HyColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
const HyColor HyColor::ContainerPanel(0x2D2D2D);
const HyColor HyColor::ContainerFrame(0x202020);// 0x3F3F46);
const HyColor HyColor::WidgetPanel(0x3F3F46);
const HyColor HyColor::WidgetFrame(0x777777);

void HySetVec(glm::vec2 &vecOut, float fX, float fY)
{
	vecOut.x = fX; vecOut.y = fY;
}

void HySetVec(glm::vec2 &vecOut, const glm::vec2 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::vec2 &vecOut, const glm::vec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::vec2 &vecOut, const glm::vec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::ivec2 &vecOut, int32 iX, int32 iY)
{
	vecOut.x = iX; vecOut.y = iY;
}

void HySetVec(glm::ivec2 &vecOut, const glm::ivec2 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::ivec2 &vecOut, const glm::ivec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::ivec2 &vecOut, const glm::ivec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::vec3 &vecOut, float fX, float fY, float fZ)
{
	vecOut.x = fX; vecOut.y = fY; vecOut.z = fZ;
}

void HySetVec(glm::vec3 &vecOut, const glm::vec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0.0f;
}

void HySetVec(glm::vec3 &vecOut, const glm::vec3 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::vec3 &vecOut, const glm::vec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
}

void HySetVec(glm::ivec3 &vecOut, int32 iX, int32 iY, int32 iZ)
{
	vecOut.x = iX; vecOut.y = iY; vecOut.z = iZ;
}

void HySetVec(glm::ivec3 &vecOut, const glm::ivec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0;
}

void HySetVec(glm::ivec3 &vecOut, const glm::ivec3 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::ivec3 &vecOut, const glm::ivec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
}

void HySetVec(glm::vec4 &vecOut, float fX, float fY, float fZ, float fW)
{
	vecOut.x = fX; vecOut.y = fY; vecOut.z = fZ; vecOut.w = fW;
}

void HySetVec(glm::vec4 &vecOut, const glm::vec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0.0f;
	vecOut.w = 0.0f;
}

void HySetVec(glm::vec4 &vecOut, const glm::vec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
	vecOut.w = 0.0f;
}

void HySetVec(glm::vec4 &vecOut, const glm::vec4 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::ivec4 &vecOut, int32 iX, int32 iY, int32 iZ, int32 iW)
{
	vecOut.x = iX; vecOut.y = iY; vecOut.z = iZ; vecOut.w = iW;
}

void HySetVec(glm::ivec4 &vecOut, const glm::ivec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0;
	vecOut.w = 0;
}

void HySetVec(glm::ivec4 &vecOut, const glm::ivec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
	vecOut.w = 0;
}

void HySetVec(glm::ivec4 &vecOut, const glm::ivec4 &srcRef)
{
	vecOut = srcRef;
}

glm::ivec2 HyMath::LockAspectRatio(int32 iOldWidth, int32 iOldHeight, int32 iNewWidth, int32 iNewHeight)
{
	// Preserve old aspect ratio
	float fLockedAspectRatio = static_cast<float>(iOldWidth) / static_cast<float>(iOldHeight);
	float fScaledAspectRatio = static_cast<float>(iNewWidth) / static_cast<float>(iNewHeight);

	// Determine whether width or height is our constraint, then calculate the return size
	glm::ivec2 vReturnSize(iNewWidth, iNewHeight);
	if(fScaledAspectRatio < fLockedAspectRatio) // Width constraint (calculate height, based on provided width)
	{
		vReturnSize.y = (iNewWidth * iOldHeight) / iOldWidth;
		vReturnSize.x = iNewWidth;
	}
	else if(fScaledAspectRatio > fLockedAspectRatio) // Height constraint (calculate width, based on provided height)
	{
		vReturnSize.x = iNewHeight * iOldWidth / iOldHeight;
		vReturnSize.y = iNewHeight;
	}

	return vReturnSize;
}

/*static*/ void HyMath::InvalidateAABB(b2AABB &aabbOut)
{
	aabbOut.lowerBound.Set(1.0f, 1.0f);
	aabbOut.upperBound.Set(-1.0f, -1.0f);
}

/*static*/ bool HyMath::TestPointAABB(const b2AABB &aabb, const glm::vec2 &pt)
{
	return (aabb.IsValid() &&
		pt.x >= aabb.lowerBound.x && pt.y >= aabb.lowerBound.y &&
		pt.x <= aabb.upperBound.x && pt.y <= aabb.upperBound.y);
}

/*static*/ glm::vec2 HyMath::PerpendicularClockwise(const glm::vec2 &vDirVector)
{
	return glm::vec2(vDirVector.y, -vDirVector.x);
}

/*static*/ glm::ivec2 HyMath::PerpendicularClockwise(const glm::ivec2 &vDirVector)
{
	return glm::ivec2(vDirVector.y, -vDirVector.x);
}

/*static*/ glm::vec2 HyMath::PerpendicularCounterClockwise(const glm::vec2 &vDirVector)
{
	return glm::vec2(-vDirVector.y, vDirVector.x);
}

/*static*/ glm::ivec2 HyMath::PerpendicularCounterClockwise(const glm::ivec2 &vDirVector)
{
	return glm::ivec2(-vDirVector.y, vDirVector.x);
}

/*static*/ float HyMath::AngleFromVector(const glm::vec2 &vDirVector)
{
	glm::vec2 vNormalizedDirVector = glm::normalize(vDirVector);
	return glm::degrees(atan2(vNormalizedDirVector.y, vNormalizedDirVector.x));
}

/*static*/ glm::vec2 HyMath::ClosestPointOnRay(const glm::vec2 &ptRayStart, const glm::vec2 &vNormalizedRayDir, const glm::vec2 &ptTestPoint)
{
	// Get the vector from the start of the ray to the test point
	glm::vec2 v = ptTestPoint - ptRayStart;

	// Get the projection of v onto the ray direction
	float t = glm::dot(v, vNormalizedRayDir);

	// If the projection is negative, the closest point is the start of the ray
	if(t < 0.0f)
		return ptRayStart;

	// Otherwise, the closest point is along the ray
	return ptRayStart + t * vNormalizedRayDir;
}

/*static*/ float HyMath::NormalizeRange(float fValue, float fMin, float fMax)
{
	float fWidth = fMax - fMin;
	if(fWidth == 0.0f)
		return fMin;

	float fOffsetValue = fValue - fMin;
	return (fOffsetValue - (floor(fOffsetValue / fWidth) * fWidth)) + fMin;	// + fMin to reset back to start of original range
}

/*static*/ int32 HyMath::NormalizeRange(int32 iValue, int32 iMin, int32 iMax)
{
	int32 iWidth = iMax - iMin;
	if(iWidth == 0)
		return iMin;

	int32 iOffsetValue = iValue - iMin;
	return (iOffsetValue - ((iOffsetValue / iWidth) * iWidth)) + iMin;	// + iMin to reset back to start of original range
}

/*static*/ float HyMath::TweenProgress(float fStart, float fEnd, float fElaspedTime, float fFullDuration, HyTweenFunc fpTweenFunc /*= HyTween::Linear*/)
{
	fElaspedTime = HyMath::Clamp(fElaspedTime, 0.0f, fFullDuration);
	return fStart + (fEnd - fStart) * fpTweenFunc(fElaspedTime / fFullDuration);
}
