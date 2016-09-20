/**************************************************************************
 *	ITransform.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __ITransform_h__
#define __ITransform_h__

#include "Afx/HyStdAfx.h"

#include "IHyApplication.h"
#include "Utilities/Animation/HyAnimVec3.h"

template<typename tVec>
class ITransform
{
protected:
	HyCoordinateUnit	m_eCoordUnit;

	tVec				m_ptRotationAnchor;
	tVec				m_ptScaleAnchor;

	void (*m_fpOnDirty)(void *);
	void *				m_pOnDirtyParam;

	bool				m_bEnabled;

public:
	ITransform();
	virtual ~ITransform(void);

	// Exposing these HyAnimVec's for user API convenience
	tVec				pos;
	HyAnimVec3			rot;
	tVec				rot_pivot;
	tVec				scale;

	HyCoordinateUnit GetCoordinateUnit()						{ return m_eCoordUnit; }
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	bool IsEnabled()											{ return m_bEnabled; }
	void SetEnabled(bool bEnabled)								{ m_bEnabled = bEnabled; }
	
	// Returns the converted pixel position to the specified HyCoordinateType
	void GetLocalTransform(glm::mat4 &outMtx) const;
	void GetLocalTransform_SRT(glm::mat4 &outMtx) const;

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam = NULL);

	void Update();
	virtual void OnUpdate() = 0;
};

template<typename tVec>
ITransform<tVec>::ITransform() :	m_eCoordUnit(HYCOORDUNIT_Default), 
									m_fpOnDirty(NULL),
									m_pOnDirtyParam(NULL),
									m_bEnabled(true)
{
	scale.Set(1.0f);
}

template<typename tVec>
/*virtual*/ ITransform<tVec>::~ITransform(void)
{ }

template<typename tVec>
void ITransform<tVec>::SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion)
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
void ITransform<tVec>::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	// TODO: Make this support 3d by exposing 'NumDimensions' in AnimVec and loop, assign tVec's using bracket overload

	glm::vec3 ptPos(0.0f);
	ptPos.x = pos.X();
	ptPos.y = pos.Y();

	glm::vec3 vScale(1.0f);
	vScale.x = scale.X();
	vScale.y = scale.Y();

	glm::vec3 ptRotPivot(0.0f);
	ptRotPivot.x = rot_pivot.X();
	ptRotPivot.y = rot_pivot.Y();

	if(m_eCoordUnit == HYCOORDUNIT_Meters)
		outMtx = glm::translate(outMtx, ptPos * IHyApplication::PixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);

	outMtx = glm::translate(outMtx, ptRotPivot);
	outMtx = glm::rotate(outMtx, rot.Get().x, glm::vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Get().y, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get().z, glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, ptRotPivot * -1.0f);

	outMtx = glm::scale(outMtx, vScale);
}

template<typename tVec>
void ITransform<tVec>::GetLocalTransform_SRT(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	// TODO: Make this support 3d by exposing 'NumDimensions' in AnimVec and loop, assign tVec's using bracket overload
	
	glm::vec3 ptPos(0.0f);
	ptPos.x = pos.X();
	ptPos.y = pos.Y();

	glm::vec3 vScale(1.0f);
	vScale.x = scale.X();
	vScale.y = scale.Y();

	outMtx = glm::scale(outMtx, vScale);
	outMtx = glm::rotate(outMtx, rot.Get().x, glm::vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Get().y, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get().z, glm::vec3(0, 0, 1));
	
	if(m_eCoordUnit == HYCOORDUNIT_Meters)
		outMtx = glm::translate(outMtx, ptPos * IHyApplication::PixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);
}

template<typename tVec>
void ITransform<tVec>::SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam = NULL)
{
	m_fpOnDirty = fpOnDirty;
	m_pOnDirtyParam = pParam;

	pos.SetOnDirtyCallback(m_fpOnDirty, m_pOnDirtyParam);
	rot.SetOnDirtyCallback(m_fpOnDirty, m_pOnDirtyParam);
	scale.SetOnDirtyCallback(m_fpOnDirty, m_pOnDirtyParam);
}

template<typename tVec>
void ITransform<tVec>::Update()
{
	OnUpdate();

	// TODO: process the procedural action queue
}

#endif /* __ITransform_h__ */
