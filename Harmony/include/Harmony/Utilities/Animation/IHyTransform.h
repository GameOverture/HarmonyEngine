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

#include "Scene/Transforms/IHyTransformNode.h"

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
class IHyTransform : public IHyTransformNode
{
protected:
	HyActionQueue			m_ActionQueue;

	HyCoordinateUnit		m_eCoordUnit;

public:
	IHyTransform(HyType eInstType);
	virtual ~IHyTransform(void);

	HyCoordinateUnit GetCoordinateUnit()						{ return m_eCoordUnit; }
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	// Exposing these HyAnimVec's for user API convenience
	tVec				pos;
	tVec				rot_pivot;
	tVec				scale;
	
	// Returns the converted pixel position to the specified HyCoordinateType
	virtual void GetLocalTransform(glm::mat4 &outMtx) const = 0;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const = 0;

	virtual void OnTransformUpdate();

private:
	virtual void OnUpdate() = 0;
};

template<typename tVec>
IHyTransform<tVec>::IHyTransform(HyType eInstType) :	IHyTransformNode(eInstType),
														m_eCoordUnit(HYCOORDUNIT_Default),
														pos(*this),
														rot_pivot(*this),
														scale(*this)
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

	SetDirty();
}

template<typename tVec>
void IHyTransform<tVec>::OnTransformUpdate()
{
	
	// TODO: process the procedural action queue

	OnUpdate();
}

#endif /* __ITransform_h__ */
