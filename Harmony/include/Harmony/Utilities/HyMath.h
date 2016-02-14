/**************************************************************************
 *	HyMath.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyMath_h__
#define __HyMath_h__

#include "Afx/HyStdAfx.h"

#define HY_PI 3.141592f

#define HyRadToDeg(radian) ((radian) * (180.0f / HY_PI))
#define HyDegToRad(degree) ((degree) * (HY_PI / 180.0f))

// maps unsigned 8 bits/channel to HYCOLOR
#define HYCOLOR_ARGB(a,r,g,b) \
	((uint32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define HYCOLOR_RGBA(r,g,b,a) HYCOLOR_ARGB(a,r,g,b)
#define HYCOLOR_XRGB(r,g,b)   HYCOLOR_ARGB(0xff,r,g,b)

#define HYCOLOR_XYUV(y,u,v)   HYCOLOR_ARGB(0xff,y,u,v)
#define HYCOLOR_AYUV(a,y,u,v) HYCOLOR_ARGB(a,y,u,v)

template <typename T> 
T HyClamp(const T& value, const T& low, const T& high) 
{
	return value < low ? low : (value > high ? high : value); 
}

float Ease_Linear(float a, float b, float t);

//--------------------------------------------------------------------------------------
// Accelerating from zero velocity
//--------------------------------------------------------------------------------------
float Ease_QuadraticIn(float fTime, float fStart, float fDist, float fDur);

//--------------------------------------------------------------------------------------
// Decelerating to zero velocity
//--------------------------------------------------------------------------------------
float Ease_QuadraticOut(float fTime, float fStart, float fDist, float fDur);

//--------------------------------------------------------------------------------------
// Acceleration until halfway, then deceleration
//--------------------------------------------------------------------------------------
float Ease_QuadraticInOut(float fTime, float fStart, float fDist, float fDur);

float Ease_AngleLinear(float angleA, float angleB, int32 spin, float t);

template<typename T>
struct HyRectangle
{
	T x;
	T y;
	T width;
	T height;

	int32 iTag;

	std::string ToString()
	{
		std::ostringstream s;
		s << "(" << x << "," << y << "," << width << "," << height << ")";
		return s.str();
	}

	HyRectangle() : x(0), y(0), width(0), height(0), iTag(0)
	{ }

	HyRectangle(T tX, T tY, T tWidth, T tHeight) : x(tX), y(tY), width(tWidth), height(tHeight), iTag(0)
	{ }
};

#endif /* __HyMath_h__ */
