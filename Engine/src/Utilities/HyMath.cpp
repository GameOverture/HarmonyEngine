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

