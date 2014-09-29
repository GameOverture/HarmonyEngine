/**************************************************************************
 *	HyEaseFuncs.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEaseFuncs_h__
#define __HyEaseFuncs_h__

#include "Afx/HyStdAfx.h"

#include <math.h>

typedef float (*EaseUpdateFunc)(float);

class HyEase
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
	/** Linear, no easing. */
	static float linear (float fRatio)
	{
		return fRatio;
	}

	/** Quadratic in. */
	static float quadIn (float fRatio)
	{
		return fRatio * fRatio;
	}

	/** Quadratic out. */
	static float quadOut (float fRatio)
	{
		return fRatio * (2 - fRatio);
	}

	/** Quadratic in and out. */
	static float quadInOut (float fRatio)
	{
		return fRatio <= 0.5f ? fRatio * fRatio * 2.0f : 1.0f - (--fRatio) * fRatio * 2.0f;
	}

	/** Cubic in. */
	static float cubeIn (float fRatio)
	{
		return fRatio * fRatio * fRatio;
	}

	/** Cubic out. */
	static float cubeOut (float fRatio)
	{
		return 1.0f + (--fRatio) * fRatio * fRatio;
	}

	/** Cubic in and out. */
	static float cubeInOut (float fRatio)
	{
		return fRatio <= 0.5f ? fRatio * fRatio * fRatio * 4.0f : 1.0f + (--fRatio) * fRatio * fRatio * 4.0f;
	}

	/** Quartic in. */
	static float quartIn (float fRatio)
	{
		return fRatio * fRatio * fRatio * fRatio;
	}

	/** Quartic out. */
	static float quartOut (float fRatio)
	{
		return 1.0f - (--fRatio) * fRatio * fRatio * fRatio;
	}

	/** Quartic in and out. */
	static float quartInOut (float fRatio)
	{
		return fRatio <= 0.5f ? fRatio * fRatio * fRatio * fRatio * 8.0f : (1.0f - (fRatio = fRatio * 2.0f - 2.0f) * fRatio * fRatio * fRatio) / 2.0f + 0.5f;
	}

	/** Quintic in. */
	static float quintIn (float fRatio)
	{
		return fRatio * fRatio * fRatio * fRatio * fRatio;
	}

	/** Quintic out. */
	static float quintOut (float fRatio)
	{
		return (fRatio = fRatio - 1.0f) * fRatio * fRatio * fRatio * fRatio + 1.0f;
	}

	/** Quintic in and out. */
	static float quintInOut (float fRatio)
	{
		return ((fRatio *= 2.0f) < 1.0f) ? (fRatio * fRatio * fRatio * fRatio * fRatio) / 2.0f : ((fRatio -= 2.0f) * fRatio * fRatio * fRatio * fRatio + 2.0f) / 2.0f;
	}

	/** Sine in. */
	static float sineIn (float fRatio)
	{
		return 1.0f - cos(PI2 * fRatio);
	}

	/** Sine out. */
	static float sineOut (float fRatio)
	{
		return sin(PI2 * fRatio);
	}

	/** Sine in and out. */
	static float sineInOut (float fRatio)
	{
		return 0.5f - cos(PI * fRatio) / 2.0f;
	}

	/** Bounce in. */
	static float bounceIn (float fRatio)
	{
		fRatio = 1.0f - fRatio;
		if (fRatio < B1) return 1.0f - 7.5625f * fRatio * fRatio;
		if (fRatio < B2) return 1.0f - (7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f);
		if (fRatio < B4) return 1.0f - (7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f);
		return 1.0f - (7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f);
	}

	/** Bounce out. */
	static float bounceOut (float fRatio)
	{
		if (fRatio < B1) return 7.5625f * fRatio * fRatio;
		if (fRatio < B2) return 7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f;
		if (fRatio < B4) return 7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f;
		return 7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f;
	}

	/** Bounce in and out. */
	static float bounceInOut (float fRatio)
	{
		if (fRatio < 0.5f) {
			fRatio = 1.0f - fRatio * 2.0f;
			if (fRatio < B1) return (1.0f - 7.5625f * fRatio * fRatio) / 2.0f;
			if (fRatio < B2) return (1.0f - (7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f)) / 2.0f;
			if (fRatio < B4) return (1.0f - (7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f)) / 2.0f;
			return (1.0f - (7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f)) / 2.0f;
		}
		fRatio = fRatio * 2.0f - 1.0f;
		if (fRatio < B1) return (7.5625f * fRatio * fRatio) / 2.0f + 0.5f;
		if (fRatio < B2) return (7.5625f * (fRatio - B3) * (fRatio - B3) + 0.75f) / 2.0f + 0.5f;
		if (fRatio < B4) return (7.5625f * (fRatio - B5) * (fRatio - B5) + 0.9375f) / 2.0f + 0.5f;
		return (7.5625f * (fRatio - B6) * (fRatio - B6) + 0.984375f) / 2.0f + 0.5f;
	}

	/** Circle in. */
	static float circIn (float fRatio)
	{
		return 1.0f - sqrt(1.0f - fRatio * fRatio);
	}

	/** Circle out. */
	static float circOut (float fRatio)
	{
		--fRatio;
		return sqrt(1.0f - fRatio * fRatio);
	}

	/** Circle in and out. */
	static float circInOut (float fRatio)
	{
		return fRatio <= 0.5f ? (sqrt(1.0f - fRatio * fRatio * 4.0f) - 1.0f) / -2.0f : (sqrt(1.0f - (fRatio * 2.0f - 2.0f) * (fRatio * 2.0f - 2.0f)) + 1.0f) / 2.0f;
	}

	/** Exponential in. */
	static float expoIn (float fRatio)
	{
		return pow(2.0f, 10.0f * (fRatio - 1.0f));
	}

	/** Exponential out. */
	static float expoOut (float fRatio)
	{
		return -pow(2.0f, -10.0f * fRatio) + 1.0f;
	}

	/** Exponential in and out. */
	static float expoInOut (float fRatio)
	{
		return fRatio < 0.5f ? pow(2.0f, 10.0f * (fRatio * 2.0f - 1.0f)) / 2.0f : (-pow(2.0f, -10.0f * (fRatio * 2.0f - 1.0f)) + 2.0f) / 2.0f;
	}

	/** Back in. */
	static float backIn (float fRatio)
	{
		return fRatio * fRatio * (2.70158f * fRatio - 1.70158f);
	}

	/** Back out. */
	static float backOut (float fRatio)
	{
		return 1.0f - (--fRatio) * (fRatio) * (-2.70158f * fRatio - 1.70158f);
	}

	/** Back in and out. */
	static float backInOut (float fRatio)
	{
		fRatio *= 2.0f;
		if (fRatio < 1.0f) return fRatio * fRatio * (2.70158f * fRatio - 1.70158f) / 2.0f;
		fRatio -= 2.0f;
		return (1.0f - fRatio * fRatio * (-2.70158f * fRatio - 1.70158f)) / 2.0f + 0.5f;
	}
};
#endif /* __HyEaseFuncs_h__ */
