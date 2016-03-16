/**************************************************************************
 *	HyAnimVec4.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAnimVec4_h__
#define __HyAnimVec4_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/HyAnimFloat.h"

#include <vector>
using std::vector;

class HyAnimVec4
{
	vec4		m_vValue;
	enum {
		NumDimensions = 4
	};

	vector<HyAnimFloat> m_vAnimFloats;

public:
	HyAnimVec4()
	{
		for(int i = 0; i < NumDimensions; ++i)
			m_vAnimFloats.push_back(HyAnimFloat(m_vValue[i]));
	}
	~HyAnimVec4()
	{ }

	inline const vec4 &Get() const											{ return m_vValue; }

	inline float X() const													{ return m_vAnimFloats[0].Get(); }
	inline void X(float fValue)												{ m_vAnimFloats[0] = fValue; }
	inline float R() const													{ return m_vAnimFloats[0].Get(); }
	inline void R(float fValue)												{ m_vAnimFloats[0] = fValue; }

	inline float Y() const													{ return m_vAnimFloats[1].Get(); }
	inline void Y(float fValue)												{ m_vAnimFloats[1] = fValue; }
	inline float G() const													{ return m_vAnimFloats[1].Get(); }
	inline void G(float fValue)												{ m_vAnimFloats[1] = fValue; }

	inline float Z() const													{ return m_vAnimFloats[2].Get(); }
	inline void Z(float fValue)												{ m_vAnimFloats[2] = fValue; }
	inline float B() const													{ return m_vAnimFloats[2].Get(); }
	inline void B(float fValue)												{ m_vAnimFloats[2] = fValue; }

	inline float W() const													{ return m_vAnimFloats[3].Get(); }
	inline void W(float fValue)												{ m_vAnimFloats[3] = fValue; }
	inline float A() const													{ return m_vAnimFloats[3].Get(); }
	inline void A(float fValue)												{ m_vAnimFloats[3] = fValue; }

	inline void Set(float fAll)												{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(fAll); }
	inline void Set(float fXorR, float fYorG, float fZorB, float fWorA)		{ m_vAnimFloats[0] = fXorR; m_vAnimFloats[1] = fYorG; m_vAnimFloats[2] = fZorB; m_vAnimFloats[3] = fWorA; }
	inline void Set(const vec4 &srcVec)										{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(srcVec[i]); }
	inline void Set(const HyAnimVec4 &srcVec)								{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(srcVec[i]); }

	inline void Offset(float fXorR, float fYorG, float fZorB, float fWorA)	{ m_vAnimFloats[0] += fXorR; m_vAnimFloats[1] += fYorG; m_vAnimFloats[2] += fZorB; m_vAnimFloats[3] += fWorA; }
	inline void Offset(const vec4 &srcVec)									{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Offset(srcVec[i]); }
	inline void Offset(const HyAnimVec4 &srcVec)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Offset(srcVec[i]); }

	inline HyAnimFloat &AnimX()												{ return m_vAnimFloats[0]; }
	inline HyAnimFloat &AnimY()												{ return m_vAnimFloats[1]; }
	inline HyAnimFloat &AnimZ()												{ return m_vAnimFloats[2]; }
	inline HyAnimFloat &AnimW()												{ return m_vAnimFloats[3]; }

	void Tween(float fX, float fY, float fZ, float fSeconds, HyTweenUpdateFunc fpEase);
	void TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenUpdateFunc fpEase);

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].SetOnDirtyCallback(fpOnDirty, pParam); }

	HyAnimVec4 &operator+=(float rhs)										{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs; return *this; }
	HyAnimVec4 &operator-=(float rhs)										{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs; return *this; }
	HyAnimVec4 &operator*=(float rhs)										{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs; return *this; }
	HyAnimVec4 &operator/=(float rhs)										{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs; return *this; }

	HyAnimVec4 &operator+=(const HyAnimVec4 &rhs)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs[i]; return *this; }
	HyAnimVec4 &operator-=(const HyAnimVec4 &rhs)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs[i]; return *this; }
	HyAnimVec4 &operator*=(const HyAnimVec4 &rhs)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs[i]; return *this; }
	HyAnimVec4 &operator/=(const HyAnimVec4 &rhs)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs[i]; return *this; }

	HyAnimVec4 &operator+=(const vec4 &rhs)									{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs[i]; return *this; }
	HyAnimVec4 &operator-=(const vec4 &rhs)									{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs[i]; return *this; }
	HyAnimVec4 &operator*=(const vec4 &rhs)									{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs[i]; return *this; }
	HyAnimVec4 &operator/=(const vec4 &rhs)									{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs[i]; return *this; }

	float operator [](int i) const											{ return m_vValue[i]; }
};

#endif /* __HyAnimVec4_h__ */