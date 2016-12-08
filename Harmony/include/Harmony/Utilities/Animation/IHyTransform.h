/**************************************************************************
 *	IHyTransform.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyTransform_h__
#define __IHyTransform_h__

#include "Afx/HyStdAfx.h"

#include "IHyApplication.h"
#include "Utilities/Animation/HyAnimVec3.h"

#include <functional>
class HyActionQueue
{
	float								m_fDuration;
	std::queue<std::function<void()> >	m_Queue;

public:
	HyActionQueue() : m_fDuration(0.0f)
	{ }

	void Update()
	{
		m_fDuration = HyClamp(m_fDuration - IHyTime::GetUpdateStepSeconds(), 0.0f, m_fDuration);

		if(m_fDuration == 0.0f)
		{
			if(m_Queue.empty() == false)
			{
				m_Queue.front()();
				m_Queue.pop();
			}
		}
	}

	void AppendAction(float fDuration, float fDeferAmt, std::function<void()> task)
	{
		m_Queue.push(task);
	}
};

template<typename tVec>
class IHyTransform
{
	friend class IHyTransform2d;
	friend class IHyTransform3d;

	// This ctor is hidden since any class that wants to derive from IHyTransform should use either 'IHyTransform2d' or 'IHyTransform3d'
	IHyTransform(HyType eInstType);

protected:
	const HyType			m_eTYPE;
	HyCoordinateUnit		m_eCoordUnit;

	tVec					m_ptRotationAnchor;
	tVec					m_ptScaleAnchor;

	void (*m_fpOnDirty)(void *);
	void *					m_pOnDirtyParam;

	bool					m_bEnabled;

	HyActionQueue			m_ActionQueue;

public:
	virtual ~IHyTransform(void);

	HyType GetType()											{ return m_eTYPE; }

	// Exposing these HyAnimVec's for user API convenience
	tVec				pos;
	tVec				rot_pivot;
	tVec				scale;

	HyCoordinateUnit GetCoordinateUnit()						{ return m_eCoordUnit; }
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	bool IsEnabled()											{ return m_bEnabled; }
	void SetEnabled(bool bEnabled)								{ m_bEnabled = bEnabled; }
	
	// Returns the converted pixel position to the specified HyCoordinateType
	virtual void GetLocalTransform(glm::mat4 &outMtx) const = 0;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const = 0;
	virtual void SetOnDirtyCallback(void(*fpOnDirty)(void *), void *pParam = NULL) = 0;

	void Update();
	virtual void OnUpdate() = 0;
};

template<typename tVec>
IHyTransform<tVec>::IHyTransform(HyType eInstType) :	m_eTYPE(eInstType),
														m_eCoordUnit(HYCOORDUNIT_Default),
														m_fpOnDirty(NULL),
														m_pOnDirtyParam(NULL),
														m_bEnabled(true)
{
	scale.Set(1.0f);
}

template<typename tVec>
/*virtual*/ IHyTransform<tVec>::~IHyTransform(void)
{ }

template<typename tVec>
void IHyTransform<tVec>::SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion)
{
	if(eCoordUnit == HYCOORDUNIT_Default)
		eCoordUnit = IHyApplication::DefaultCoordinateUnit();

	if(m_eCoordUnit == eCoordUnit)
		return;

	if(bDoConversion)
	{
		switch(eCoordUnit)
		{
		case HYCOORDUNIT_Meters:	pos *= IHyApplication::PixelsPerMeter();	break;
		case HYCOORDUNIT_Pixels:	pos /= IHyApplication::PixelsPerMeter();	break;
		}
	}
	m_eCoordUnit = eCoordUnit;

	if(m_fpOnDirty)
		m_fpOnDirty(m_pOnDirtyParam);
}

template<typename tVec>
void IHyTransform<tVec>::Update()
{
	OnUpdate();

	// TODO: process the procedural action queue
}

#endif /* __ITransform_h__ */
