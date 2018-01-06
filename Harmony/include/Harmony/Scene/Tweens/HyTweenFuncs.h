/**************************************************************************
 *	HyTweenFuncs.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTweenFuncs_h__
#define HyTweenFuncs_h__

#include "Afx/HyStdAfx.h"

#include <math.h>

class IHyNode;

typedef float (*HyTweenUpdateFunc)(float);

class HyTween
{
	static float PI;// = atan(1.0f)*4.0f;
	static float PI2;// = (atan(1.0f)*4.0f) / 2.0f;
	static float B1;// = 1.0f / 2.75f;
	static float B2;// = 2.0f / 2.75f;
	static float B3;// = 1.5f / 2.75f;
	static float B4;// = 2.5f / 2.75f;
	static float B5;// = 2.25f / 2.75f;
	static float B6;// = 2.625f / 2.75f;

public:
	// Linear, no easing
	static float Linear(float fRatio);

	// Quadratic in
	static float QuadIn(float fRatio);

	// Quadratic out
	static float QuadOut(float fRatio);

	// Quadratic in and out
	static float QuadInOut(float fRatio);

	// Cubic in
	static float CubeIn(float fRatio);

	// Cubic out
	static float CubeOut(float fRatio);

	// Cubic in and out
	static float CubeInOut(float fRatio);

	// Quartic in
	static float QuartIn(float fRatio);

	// Quartic out
	static float QuartOut(float fRatio);

	// Quartic in and out
	static float QuartInOut(float fRatio);

	// Quintic in
	static float QuintIn(float fRatio);

	// Quintic out
	static float QuintOut(float fRatio);

	// Quintic in and out
	static float QuintInOut(float fRatio);

	// Sine in
	static float SineIn(float fRatio);

	// Sine out
	static float SineOut(float fRatio);

	// Sine in and out
	static float SineInOut(float fRatio);

	// Bounce in
	static float BounceIn(float fRatio);

	// Bounce out
	static float BounceOut(float fRatio);

	// Bounce in and out
	static float BounceInOut(float fRatio);

	// Circle in
	static float CircIn(float fRatio);

	// Circle out
	static float CircOut(float fRatio);

	// Circle in and out
	static float CircInOut(float fRatio);

	// Exponential in
	static float ExpoIn(float fRatio);

	// Exponential out
	static float ExpoOut(float fRatio);

	// Exponential in and out
	static float ExpoInOut(float fRatio);

	// Back in
	static float BackIn(float fRatio);

	// Back out
	static float BackOut(float fRatio);

	// Back in and out
	static float BackInOut(float fRatio);

	// Default (do-nothing) callback when tween finishes
	static void NullTweenCallback(IHyNode *)
	{ }
};
#endif /* HyTweenFuncs_h__ */
