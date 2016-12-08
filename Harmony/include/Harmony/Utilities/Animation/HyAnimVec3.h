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

class HyAnimVec3
{
	glm::vec3	m_vValue;
	enum {
		NumDimensions = 3
	};

	std::vector<HyAnimFloat> m_AnimFloatList;

public:
	HyAnimVec3()
	{
		for(int i = 0; i < NumDimensions; ++i)
			m_AnimFloatList.push_back(HyAnimFloat(m_vValue[i]));
	}
	~HyAnimVec3()
	{ }

	inline const glm::vec3 &Get() const					{ return m_vValue; }

	inline float X() const								{ return m_AnimFloatList[0].Get(); }
	inline void X(float fValue)							{ m_AnimFloatList[0] = fValue; }

	inline float Y() const 								{ return m_AnimFloatList[1].Get(); }
	inline void Y(float fValue)							{ m_AnimFloatList[1] = fValue; }

	inline float Z() const 								{ return m_AnimFloatList[2].Get(); }
	inline void Z(float fValue)							{ m_AnimFloatList[2] = fValue; }

	inline void Set(float fAll)							{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Set(fAll); }
	inline void Set(float fX, float fY, float fZ)		{ m_AnimFloatList[0] = fX; m_AnimFloatList[1] = fY; m_AnimFloatList[2] = fZ; }
	inline void Set(const glm::vec3 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Set(srcVec[i]); }
	inline void Set(const HyAnimVec3 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Set(srcVec[i]); }

	inline void Offset(float fX, float fY, float fZ)	{ m_AnimFloatList[0] += fX; m_AnimFloatList[1] += fY; m_AnimFloatList[2] += fZ; }
	inline void Offset(const glm::vec3 &srcVec)			{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Offset(srcVec[i]); }
	inline void Offset(const HyAnimVec3 &srcVec)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].Offset(srcVec[i]); }

	inline HyAnimFloat &AnimX()							{ return m_AnimFloatList[0]; }
	inline HyAnimFloat &AnimY()							{ return m_AnimFloatList[1]; }
	inline HyAnimFloat &AnimZ()							{ return m_AnimFloatList[2]; }

	void Tween(float fX, float fY, float fZ, float fSeconds, HyTweenUpdateFunc fpEase)
	{
		m_AnimFloatList[0].TweenOffset(fX, fSeconds, fpEase);
		m_AnimFloatList[1].TweenOffset(fY, fSeconds, fpEase);
		m_AnimFloatList[2].TweenOffset(fZ, fSeconds, fpEase);
	}
	void TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenUpdateFunc fpEase);

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam) { for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i].SetOnDirtyCallback(fpOnDirty, pParam); }

	HyAnimVec3 &operator+=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] += rhs; return *this; }
	HyAnimVec3 &operator-=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] -= rhs; return *this; }
	HyAnimVec3 &operator*=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] *= rhs; return *this; }
	HyAnimVec3 &operator/=(float rhs)					{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] /= rhs; return *this; }

	HyAnimVec3 &operator+=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] += rhs[i]; return *this; }
	HyAnimVec3 &operator-=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] -= rhs[i]; return *this; }
	HyAnimVec3 &operator*=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] *= rhs[i]; return *this; }
	HyAnimVec3 &operator/=(const HyAnimVec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] /= rhs[i]; return *this; }

	HyAnimVec3 &operator+=(const glm::vec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] += rhs[i]; return *this; }
	HyAnimVec3 &operator-=(const glm::vec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] -= rhs[i]; return *this; }
	HyAnimVec3 &operator*=(const glm::vec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] *= rhs[i]; return *this; }
	HyAnimVec3 &operator/=(const glm::vec3 &rhs)		{ for(int i = 0; i < NumDimensions; ++i) m_AnimFloatList[i] /= rhs[i]; return *this; }

	float operator [](int i) const						{ return m_vValue[i]; }
};

#endif /* __HyAnimVec3_h__ */