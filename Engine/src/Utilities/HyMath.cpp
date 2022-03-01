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
const HyColor HyColor::Orange(0xFFA500);
const HyColor HyColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
const HyColor HyColor::ContainerPanel(0x252526);
const HyColor HyColor::ContainerFrame(0x3F3F46);
const HyColor HyColor::WidgetPanel(0x3F3F46);
const HyColor HyColor::WidgetFrame(0x555555);

void HySetVec(glm::vec2 &vecOut, float fX, float fY)
{
	vecOut.x = fX; vecOut.y = fY;
}

void HySetVec(glm::vec3 &vecOut, float fX, float fY, float fZ)
{
	vecOut.x = fX; vecOut.y = fY; vecOut.z = fZ;
}

void HySetVec(glm::vec4 &vecOut, float fX, float fY, float fZ, float fW)
{
	vecOut.x = fX; vecOut.y = fY; vecOut.z = fZ; vecOut.w = fW;
}

void HySetVec(glm::ivec2 &vecOut, int32 iX, int32 iY)
{
	vecOut.x = iX; vecOut.y = iY;
}

void HySetVec(glm::ivec3 &vecOut, int32 iX, int32 iY, int32 iZ)
{
	vecOut.x = iX; vecOut.y = iY; vecOut.z = iZ;
}

void HySetVec(glm::ivec4 &vecOut, int32 iX, int32 iY, int32 iZ, int32 iW)
{
	vecOut.x = iX; vecOut.y = iY; vecOut.z = iZ; vecOut.w = iW;
}

void HyCopyVec(glm::vec2 &destRef, const glm::vec2 &srcRef)
{
	destRef = srcRef;
}

void HyCopyVec(glm::vec3 &destRef, const glm::vec3 &srcRef)
{
	destRef = srcRef;
}

void HyCopyVec(glm::vec2 &destRef, const glm::vec3 &srcRef)
{
	destRef.x = srcRef.x;
	destRef.y = srcRef.y;
}

void HyCopyVec(glm::vec3 &destRef, const glm::vec2 &srcRef)
{
	destRef.x = srcRef.x;
	destRef.y = srcRef.y;
}

bool HyTestPointAABB(const b2AABB &aabb, const glm::vec2 &pt)
{
	return (aabb.IsValid() &&
			pt.x >= aabb.lowerBound.x && pt.y >= aabb.lowerBound.y &&
			pt.x <= aabb.upperBound.x && pt.y <= aabb.upperBound.y);
}

float HyEase_Linear(float a, float b, float t)
{
	return ((b-a)*t)+a;
}

//--------------------------------------------------------------------------------------
// Accelerating from zero velocity
//--------------------------------------------------------------------------------------
float HyEase_QuadraticIn(float fTime, float fStart, float fDist, float fDur)
{
	fTime /= fDur;
	return fDist * fTime * fTime + fStart;
}

//--------------------------------------------------------------------------------------
// Decelerating to zero velocity
//--------------------------------------------------------------------------------------
float HyEase_QuadraticOut(float fTime, float fStart, float fDist, float fDur)
{
	fTime /= fDur;
	return -fDist * fTime*(fTime-2) + fStart;
}

//--------------------------------------------------------------------------------------
// Acceleration until halfway, then deceleration
//--------------------------------------------------------------------------------------
float HyEase_QuadraticInOut(float fTime, float fStart, float fDist, float fDur)
{
	fTime /= fDur/2;

	if (fTime < 1) 
		return fDist/2*fTime*fTime + fStart;

	fTime--;

	return -fDist/2 * (fTime*(fTime-2) - 1) + fStart;
}

float HyEase_AngleLinear(float angleA, float angleB, int32 spin, float t)
{
	if(spin==0)
	{
		return angleA;
	}
	if(spin>0)
	{
		if((angleB-angleA)<0)
		{
			angleB+=360;
		}
	}
	else if(spin<0)
	{
		if((angleB-angleA)>0)
		{    
			angleB-=360;
		}
	}

	return HyEase_Linear(angleA,angleB,t);
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
