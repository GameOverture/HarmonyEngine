/**************************************************************************
 *	HyTweenFuncs.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyTweenFuncs.h"

/*static*/ float HyTween::PI = atan(1.0f)*4.0f;
/*static*/ float HyTween::PI2 = (atan(1.0f)*4.0f) / 2.0f;
/*static*/ float HyTween::B1 = 1.0f / 2.75f;
/*static*/ float HyTween::B2 = 2.0f / 2.75f;
/*static*/ float HyTween::B3 = 1.5f / 2.75f;
/*static*/ float HyTween::B4 = 2.5f / 2.75f;
/*static*/ float HyTween::B5 = 2.25f / 2.75f;
/*static*/ float HyTween::B6 = 2.625f / 2.75f;

/*static*/ float HyTween::Linear(float fRatio)
{
	return fRatio;
}

/*static*/ float HyTween::QuadIn(float fRatio)
{
	return fRatio * fRatio;
}

/*static*/ float HyTween::QuadOut(float fRatio)
{
	return fRatio * (2 - fRatio);
}

/*static*/ float HyTween::QuadInOut(float fRatio)
{
	return fRatio < 0.5 ? 2.0f * fRatio * fRatio : 1.0f - pow(-2.0f * fRatio + 2.0f, 2.0f) / 2.0f;
	//return fRatio <= 0.5f ? fRatio * fRatio * 2.0f : 1.0f - (--fRatio) * fRatio * 2.0f;
}

/*static*/ float HyTween::CubeIn(float fRatio)
{
	return fRatio * fRatio * fRatio;
}

/*static*/ float HyTween::CubeOut(float fRatio)
{
	return 1.0f - pow(1.0f - fRatio, 3.0f);
	//return 1.0f + (--fRatio) * fRatio * fRatio;
}

/*static*/ float HyTween::CubeInOut(float fRatio)
{
	return fRatio < 0.5f ? 4.0f * fRatio * fRatio * fRatio : 1.0f - pow(-2.0f * fRatio + 2.0f, 3.0f) / 2.0f;
	//return fRatio <= 0.5f ? fRatio * fRatio * fRatio * 4.0f : 1.0f + (--fRatio) * fRatio * fRatio * 4.0f;
}

/*static*/ float HyTween::QuartIn(float fRatio)
{
	return fRatio * fRatio * fRatio * fRatio;
}

/*static*/ float HyTween::QuartOut(float fRatio)
{
	return 1.0f - pow(1.0f - fRatio, 4.0f);
	//return 1.0f - (--fRatio) * fRatio * fRatio * fRatio;
}

/*static*/ float HyTween::QuartInOut(float fRatio)
{
	return fRatio < 0.5f ? 8.0f * fRatio * fRatio * fRatio * fRatio : 1.0f - pow(-2.0f * fRatio + 2.0f, 4.0f) / 2.0f;
	//return fRatio <= 0.5f ? fRatio * fRatio * fRatio * fRatio * 8.0f : (1.0f - (fRatio = fRatio * 2.0f - 2.0f) * fRatio * fRatio * fRatio) / 2.0f + 0.5f;
}

/*static*/ float HyTween::QuintIn(float fRatio)
{
	return fRatio * fRatio * fRatio * fRatio * fRatio;
}

/*static*/ float HyTween::QuintOut(float fRatio)
{
	return 1.0f - pow(1.0f - fRatio, 5.0f);
	//return (fRatio = fRatio - 1.0f) * fRatio * fRatio * fRatio * fRatio + 1.0f;
}

/*static*/ float HyTween::QuintInOut(float fRatio)
{
	return fRatio < 0.5f ? 16.0f * fRatio * fRatio * fRatio * fRatio * fRatio : 1.0f - pow(-2.0f * fRatio + 2.0f, 5.0f) / 2.0f;
	//return ((fRatio *= 2.0f) < 1.0f) ? (fRatio * fRatio * fRatio * fRatio * fRatio) / 2.0f : ((fRatio -= 2.0f) * fRatio * fRatio * fRatio * fRatio + 2.0f) / 2.0f;
}

/*static*/ float HyTween::SineIn(float fRatio)
{
	return 1.0f - cos(PI2 * fRatio);
}

/*static*/ float HyTween::SineOut(float fRatio)
{
	return sin(PI2 * fRatio);
}

/*static*/ float HyTween::SineInOut(float fRatio)
{
	return 0.5f - cos(PI * fRatio) / 2.0f;
}

/*static*/ float HyTween::BounceIn(float fRatio)
{
	fRatio = 1.0f - fRatio;
	if(fRatio < B1) return 1.0f - 7.5625f * fRatio * fRatio;
	if(fRatio < B2) return 1.0f - (7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f);
	if(fRatio < B4) return 1.0f - (7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f);
	return 1.0f - (7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f);
}

/*static*/ float HyTween::BounceOut(float fRatio)
{
	if(fRatio < B1) return 7.5625f * fRatio * fRatio;
	if(fRatio < B2) return 7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f;
	if(fRatio < B4) return 7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f;
	return 7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f;
}

/*static*/ float HyTween::BounceInOut(float fRatio)
{
	if(fRatio < 0.5f) {
		fRatio = 1.0f - fRatio * 2.0f;
		if(fRatio < B1) return (1.0f - 7.5625f * fRatio * fRatio) / 2.0f;
		if(fRatio < B2) return (1.0f - (7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f)) / 2.0f;
		if(fRatio < B4) return (1.0f - (7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f)) / 2.0f;
		return (1.0f - (7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f)) / 2.0f;
	}
	fRatio = fRatio * 2.0f - 1.0f;
	if(fRatio < B1) return (7.5625f * fRatio * fRatio) / 2.0f + 0.5f;
	if(fRatio < B2) return (7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f) / 2.0f + 0.5f;
	if(fRatio < B4) return (7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f) / 2.0f + 0.5f;
	return (7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f) / 2.0f + 0.5f;
}

/*static*/ float HyTween::CircIn(float fRatio)
{
	return 1.0f - sqrt(1.0f - fRatio * fRatio);
}

/*static*/ float HyTween::CircOut(float fRatio)
{
	--fRatio;
	return sqrt(1.0f - fRatio * fRatio);
}

/*static*/ float HyTween::CircInOut(float fRatio)
{
	return fRatio <= 0.5f ? (sqrt(1.0f - fRatio * fRatio * 4.0f) - 1.0f) / -2.0f : (sqrt(1.0f - (fRatio * 2.0f - 2.0f) * (fRatio * 2.0f - 2.0f)) + 1.0f) / 2.0f;
}

/*static*/ float HyTween::ExpoIn(float fRatio)
{
	return pow(2.0f, 10.0f * (fRatio - 1.0f));
}

/*static*/ float HyTween::ExpoOut(float fRatio)
{
	return -pow(2.0f, -10.0f * fRatio) + 1.0f;
}

/*static*/ float HyTween::ExpoInOut(float fRatio)
{
	return fRatio < 0.5f ? pow(2.0f, 10.0f * (fRatio * 2.0f - 1.0f)) / 2.0f : (-pow(2.0f, -10.0f * (fRatio * 2.0f - 1.0f)) + 2.0f) / 2.0f;
}

/*static*/ float HyTween::BackIn(float fRatio)
{
	return fRatio * fRatio * (2.70158f * fRatio - 1.70158f);
}

/*static*/ float HyTween::BackOut(float fRatio)
{
	return 1.0f + 2.70158f * pow(fRatio - 1.0f, 3.0f) + 1.70158f * pow(fRatio - 1.0f, 2.0f);
	//return 1.0f - (--fRatio) * (fRatio)* (-2.70158f * fRatio - 1.70158f);
}

/*static*/ float HyTween::BackInOut(float fRatio)
{
	fRatio *= 2.0f;
	if(fRatio < 1.0f) return fRatio * fRatio * (2.70158f * fRatio - 1.70158f) / 2.0f;
	fRatio -= 2.0f;
	return (1.0f - fRatio * fRatio * (-2.70158f * fRatio - 1.70158f)) / 2.0f + 0.5f;
}
