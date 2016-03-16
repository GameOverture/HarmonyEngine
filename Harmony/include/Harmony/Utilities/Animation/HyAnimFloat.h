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

// Forward declaration
class HyScene;

// NOTE: The constructor that takes a float reference will not invoke its 'dirty callback' if
//		 manipulated outside of this class.
class HyAnimFloat
{
	friend class HyScene;
	static HyScene *		sm_pScene;

	float &					m_fValueRef;
	float					m_fStart;
	float					m_fTarget;
	float					m_fDuration;
	float					m_fElapsedTime;
	bool					m_bAddedToSceneUpdate;

	const bool				m_bSELF_ALLOCATED;

	HyTweenUpdateFunc			m_fpEaseFunc;
	//float (HyEase::*m_fpEaseFunc)(float);

	bool (HyAnimFloat::*m_fpBehaviorUpdate)();
	void (*m_fpOnDirty)(void *pParam);
	void *					m_pOnChangeParam;

public:
	HyAnimFloat();
	HyAnimFloat(float &valueReference);
	~HyAnimFloat(void);

	inline float Get() const			{ return m_fValueRef; }
	void Set(float fValue);
	void Offset(float fValue);

	// Procedural transformation functions
	bool IsTransforming()				{ return m_bAddedToSceneUpdate; }
	void Tween(float fFrom, float fTo, float fSeconds, HyTweenUpdateFunc fpEase);
	void Tween(float fTo, float fSeconds, HyTweenUpdateFunc fpEase);
	void TweenOffset(float fOffsetAmt, float fSeconds, HyTweenUpdateFunc fpEase);
	//void Follow(float &fToFollow, float fOffsetAmt);
	//void Sequence(...);

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam = NULL);

	// Returns true if updating is still continuing. False otherwise, to signal HyScene to remove this instance from the ActiveAnimFloat vector
	bool UpdateFloat();

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
	//////////////////////////////////////////////////////////////////////////
	// Update Behaviors
	//////////////////////////////////////////////////////////////////////////
	bool Tween();
	bool Follow();

	static void _NullOnChange(void *pParam)
	{ }
};

#endif /* __HyAnimFloat_h__ */
