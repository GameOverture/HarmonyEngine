/**************************************************************************
 *	HyAnimVec3.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAnimVec3_h__
#define __HyAnimVec3_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/HyAnimFloat.h"

#include <vector>
using std::vector;

class HyAnimVec3
{
	vec3		m_vValue;
	enum {
		NumDimensions = 3
	};

	vector<HyAnimFloat> m_vAnimFloats;

public:
	HyAnimVec3()
	{
		for(int i = 0; i < NumDimensions; ++i)
			m_vAnimFloats.push_back(HyAnimFloat(m_vValue[i]));
	}
	~HyAnimVec3()
	{ }

	inline const vec3 &Get() const						{ return m_vValue; }

	inline float X() const								{ return m_vAnimFloats[0].Get(); }
	inline void X(float fValue)							{ m_vAnimFloats[0] = fValue; }

	inline float Y() const 								{ return m_vAnimFloats[1].Get(); }
	inline void Y(float fValue)							{ m_vAnimFloats[1] = fValue; }

	inline float Z() const 								{ return m_vAnimFloats[2].Get(); }
	inline void Z(float fValue)							{ m_vAnimFloats[2] = fValue; }

	inline void Set(float fAll)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(fAll); }
	inline void Set(float fX, float fY, float fZ)		{ m_vAnimFloats[0] = fX; m_vAnimFloats[1] = fY; m_vAnimFloats[2] = fZ; }
	inline void Set(const vec3 &srcVec)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(srcVec[i]); }
	inline void Set(const HyAnimVec3 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(srcVec[i]); }

	inline void Offset(float fX, float fY, float fZ)	{ m_vAnimFloats[0] += fX; m_vAnimFloats[1] += fY; m_vAnimFloats[2] += fZ; }
	inline void Offset(const vec3 &srcVec)				{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Offset(srcVec[i]); }
	inline void Offset(const HyAnimVec3 &srcVec)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Offset(srcVec[i]); }

	inline HyAnimFloat &AnimX()							{ return m_vAnimFloats[0]; }
	inline HyAnimFloat &AnimY()							{ return m_vAnimFloats[1]; }
	inline HyAnimFloat &AnimZ()							{ return m_vAnimFloats[2]; }

	void Tween(float fX, float fY, float fZ, float fSeconds, HyTweenUpdateFunc fpEase)
	{
		m_vAnimFloats[0].TweenOffset(fX, fSeconds, fpEase);
		m_vAnimFloats[1].TweenOffset(fY, fSeconds, fpEase);
		m_vAnimFloats[2].TweenOffset(fZ, fSeconds, fpEase);
	}
	void TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenUpdateFunc fpEase);

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam) { for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].SetOnDirtyCallback(fpOnDirty, pParam); }

	HyAnimVec3 &operator+=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs; return *this; }
	HyAnimVec3 &operator-=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs; return *this; }
	HyAnimVec3 &operator*=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs; return *this; }
	HyAnimVec3 &operator/=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs; return *this; }

	HyAnimVec3 &operator+=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs[i]; return *this; }
	HyAnimVec3 &operator-=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs[i]; return *this; }
	HyAnimVec3 &operator*=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs[i]; return *this; }
	HyAnimVec3 &operator/=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs[i]; return *this; }

	HyAnimVec3 &operator+=(const vec3 &rhs)				{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs[i]; return *this; }
	HyAnimVec3 &operator-=(const vec3 &rhs)				{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs[i]; return *this; }
	HyAnimVec3 &operator*=(const vec3 &rhs)				{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs[i]; return *this; }
	HyAnimVec3 &operator/=(const vec3 &rhs)				{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs[i]; return *this; }

	float operator [](int i) const						{ return m_vValue[i]; }
};

#endif /* __HyAnimVec3_h__ */