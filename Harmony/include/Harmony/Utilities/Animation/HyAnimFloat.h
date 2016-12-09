/**************************************************************************
 *	HyAnimFloat.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAnimFloat_h__
#define __HyAnimFloat_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/HyTweenFuncs.h"

class IHyTransformNode;

class HyAnimFloat
{
	friend class IHyTransformNode;

	float &					m_fValueRef;
	IHyTransformNode &		m_OwnerRef;

	float					m_fStart;
	float					m_fTarget;
	float					m_fDuration;
	float					m_fElapsedTime;
	bool					m_bAddedToOwnerUpdate;

	HyTweenUpdateFunc		m_fpEaseFunc;

	bool (HyAnimFloat::*m_fpBehaviorUpdate)();

public:
	HyAnimFloat(float &valueReference, IHyTransformNode &ownerRef);
	~HyAnimFloat(void);

	inline float Get() const			{ return m_fValueRef; }
	void Set(float fValue);
	void Offset(float fValue);

	// Procedural transformation functions
	bool IsTransforming()				{ return m_bAddedToOwnerUpdate; }
	void Tween(float fFrom, float fTo, float fSeconds, HyTweenUpdateFunc fpEase);
	void Tween(float fTo, float fSeconds, HyTweenUpdateFunc fpEase);
	void TweenOffset(float fOffsetAmt, float fSeconds, HyTweenUpdateFunc fpEase);
	//void Follow(float &fToFollow, float fOffsetAmt);
	//void Sequence(...);

	HyAnimFloat &operator=(const float &rhs);
	HyAnimFloat &operator+=(const float &rhs);
	HyAnimFloat &operator-=(const float &rhs);
	HyAnimFloat &operator*=(const float &rhs);
	HyAnimFloat &operator/=(const float &rhs);

	HyAnimFloat &operator+=(const HyAnimFloat &rhs);
	HyAnimFloat &operator-=(const HyAnimFloat &rhs);
	HyAnimFloat &operator*=(const HyAnimFloat &rhs);
	HyAnimFloat &operator/=(const HyAnimFloat &rhs);

private:
	// Returns true if updating is still continuing. False otherwise, to signal HyScene to remove this instance from the ActiveAnimFloat vector
	bool UpdateFloat();

	//////////////////////////////////////////////////////////////////////////
	// Update Behaviors
	//////////////////////////////////////////////////////////////////////////
	bool Tween();
	bool Follow();

	static void _NullOnChange(void *pParam)
	{ }
};

#endif /* __HyAnimFloat_h__ */
