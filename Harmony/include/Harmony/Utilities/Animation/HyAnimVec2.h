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

class HyAnimVec2
{
	glm::vec2			m_vValue;
	enum {
		NumDimensions = 2
	};

	std::vector<HyAnimFloat> m_AnimFloatList;

public:
	HyAnimVec2(IHyTransformNode &ownerRef)
	{
		for(int i = 0; i < NumDimensions; ++i)
			m_AnimFloatList.push_back(HyAnimFloat(m_vValue[i], ownerRef));
	}
	~HyAnimVec2()
	{ }

	inline const glm::vec2 &Get() const					{ return m_vValue; }

	inline float X() const								{ return m_AnimFloatList[0].Get(); }
	inline void X(float fValue)							{ m_AnimFloatList[0] = fValue; }

	inline float Y() const								{ return m_AnimFloatList[1].Get(); }
	inline void Y(float fValue)							{ m_AnimFloatList[1] = fValue; }

	// TODO: Unwind the loops in these functions
	inline void Set(float fAll)							{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Set(fAll); }
	inline void Set(float fX, float fY)					{ m_AnimFloatList[0] = fX; m_AnimFloatList[1] = fY; }
	inline void Set(const glm::vec2 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Set(srcVec[i]); }
	inline void Set(const HyAnimVec2 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Set(srcVec[i]); }

	inline void Offset(float fX, float fY)				{ m_AnimFloatList[0] += fX; m_AnimFloatList[1] += fY; }
	inline void Offset(const glm::vec2 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Offset(srcVec[i]); }
	inline void Offset(const HyAnimVec2 &srcVec)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Offset(srcVec[i]); }

	void Tween(float fX, float fY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback)
	{
		m_AnimFloatList[0].Tween(fX, fSeconds, fpEase, tweenFinishedCallback);
		m_AnimFloatList[1].Tween(fY, fSeconds, fpEase);
	}

	void TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback)
	{
		m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpEase, tweenFinishedCallback);
		m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpEase);
	}

	bool IsTweening()
	{
		return (m_AnimFloatList[0].IsTweening() || m_AnimFloatList[1].IsTweening());
	}

	HyAnimVec2 &operator+=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] += rhs; return *this; }
	HyAnimVec2 &operator-=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] -= rhs; return *this; }
	HyAnimVec2 &operator*=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] *= rhs; return *this; }
	HyAnimVec2 &operator/=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] /= rhs; return *this; }

	HyAnimVec2 &operator+=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] += rhs[i]; return *this; }
	HyAnimVec2 &operator-=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] -= rhs[i]; return *this; }
	HyAnimVec2 &operator*=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] *= rhs[i]; return *this; }
	HyAnimVec2 &operator/=(const HyAnimVec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] /= rhs[i]; return *this; }

	HyAnimVec2 &operator+=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] += rhs[i]; return *this; }
	HyAnimVec2 &operator-=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] -= rhs[i]; return *this; }
	HyAnimVec2 &operator*=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] *= rhs[i]; return *this; }
	HyAnimVec2 &operator/=(const glm::vec2 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] /= rhs[i]; return *this; }

	float operator [](int i) const						{ return m_vValue[i]; }
};

#endif /* __HyAnimVec2_h__ */