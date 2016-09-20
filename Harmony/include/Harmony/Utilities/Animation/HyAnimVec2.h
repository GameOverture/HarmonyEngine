/**************************************************************************
 *	HyAnimVec2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAnimVec2_h__
#define __HyAnimVec2_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/HyAnimFloat.h"

#include <vector>
using std::vector;

class HyAnimVec2
{
	glm::vec2			m_vValue;
	enum {
		NumDimensions = 2
	};

	vector<HyAnimFloat> m_vAnimFloats;

public:
	HyAnimVec2()
	{
		for(int i = 0; i < NumDimensions; ++i)
			m_vAnimFloats.push_back(HyAnimFloat(m_vValue[i]));
	}
	~HyAnimVec2()
	{ }

	inline const glm::vec2 &Get() const					{ return m_vValue; }

	inline float X() const								{ return m_vAnimFloats[0].Get(); }
	inline void X(float fValue)							{ m_vAnimFloats[0] = fValue; }

	inline float Y() const								{ return m_vAnimFloats[1].Get(); }
	inline void Y(float fValue)							{ m_vAnimFloats[1] = fValue; }

	// TODO: Unwind the loops in these functions
	inline void Set(float fAll)							{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(fAll); }
	inline void Set(float fX, float fY)					{ m_vAnimFloats[0] = fX; m_vAnimFloats[1] = fY; }
	inline void Set(const glm::vec2 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(srcVec[i]); }
	inline void Set(const HyAnimVec2 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Set(srcVec[i]); }

	inline void Offset(float fX, float fY)				{ m_vAnimFloats[0] += fX; m_vAnimFloats[1] += fY; }
	inline void Offset(const glm::vec2 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Offset(srcVec[i]); }
	inline void Offset(const HyAnimVec2 &srcVec)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].Offset(srcVec[i]); }

	inline HyAnimFloat &AnimX()							{ return m_vAnimFloats[0]; }
	inline HyAnimFloat &AnimY()							{ return m_vAnimFloats[1]; }

	void Tween(float fX, float fY, float fSeconds, HyTweenUpdateFunc fpEase)
	{
		m_vAnimFloats[0].Tween(fX, fSeconds, fpEase);
		m_vAnimFloats[1].Tween(fY, fSeconds, fpEase);
	}

	void TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenUpdateFunc fpEase)
	{
		m_vAnimFloats[0].TweenOffset(fOffsetX, fSeconds, fpEase);
		m_vAnimFloats[1].TweenOffset(fOffsetY, fSeconds, fpEase);
	}

	bool IsTransforming()
	{
		return (m_vAnimFloats[0].IsTransforming() || m_vAnimFloats[1].IsTransforming());
	}

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam) { for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i].SetOnDirtyCallback(fpOnDirty, pParam); }

	HyAnimVec2 &operator+=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs; return *this; }
	HyAnimVec2 &operator-=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs; return *this; }
	HyAnimVec2 &operator*=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs; return *this; }
	HyAnimVec2 &operator/=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs; return *this; }

	HyAnimVec2 &operator+=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs[i]; return *this; }
	HyAnimVec2 &operator-=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs[i]; return *this; }
	HyAnimVec2 &operator*=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs[i]; return *this; }
	HyAnimVec2 &operator/=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs[i]; return *this; }

	HyAnimVec2 &operator+=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] += rhs[i]; return *this; }
	HyAnimVec2 &operator-=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] -= rhs[i]; return *this; }
	HyAnimVec2 &operator*=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] *= rhs[i]; return *this; }
	HyAnimVec2 &operator/=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_vAnimFloats[i] /= rhs[i]; return *this; }

	float operator [](int i) const						{ return m_vValue[i]; }
};

#endif /* __HyAnimVec2_h__ */