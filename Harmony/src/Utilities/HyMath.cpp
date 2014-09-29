/**************************************************************************
 *	HyMath.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Utilities/HyMath.h"

float Ease_Linear(float a, float b, float t)
{
	return ((b-a)*t)+a;
}

//--------------------------------------------------------------------------------------
// Accelerating from zero velocity
//--------------------------------------------------------------------------------------
float Ease_QuadraticIn(float fTime, float fStart, float fDist, float fDur)
{
	fTime /= fDur;
	return fDist * fTime * fTime + fStart;
}

//--------------------------------------------------------------------------------------
// Decelerating to zero velocity
//--------------------------------------------------------------------------------------
float Ease_QuadraticOut(float fTime, float fStart, float fDist, float fDur)
{
	fTime /= fDur;
	return -fDist * fTime*(fTime-2) + fStart;
}

//--------------------------------------------------------------------------------------
// Acceleration until halfway, then deceleration
//--------------------------------------------------------------------------------------
float Ease_QuadraticInOut(float fTime, float fStart, float fDist, float fDur)
{
	fTime /= fDur/2;

	if (fTime < 1) 
		return fDist/2*fTime*fTime + fStart;

	fTime--;

	return -fDist/2 * (fTime*(fTime-2) - 1) + fStart;
}

float Ease_AngleLinear(float angleA, float angleB, int32 spin, float t)
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

	return Ease_Linear(angleA,angleB,t);
}

