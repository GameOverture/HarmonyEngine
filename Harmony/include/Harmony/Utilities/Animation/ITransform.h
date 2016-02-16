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

#include "Scene/HyScene.h"
#include "Utilities/Animation/HyAnimVec3.h"

template<typename tVec>
class ITransform
{
protected:
	HyCoordinateType	m_eCoordType;

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
	tVec				scale;

	HyCoordinateType GetCoordinateType()						{ return m_eCoordType; }
	virtual void SetCoordinateType(HyCoordinateType eCoordType, bool bDoConversion);

	inline bool	IsEnabled()										{ return m_bEnabled; }
	inline void	SetEnabled(bool bEnabled)						{ m_bEnabled = bEnabled; }
	
	// Returns the converted pixel position to the specified HyCoordinateType
	void GetLocalTransform(mat4 &outMtx) const;
	void GetLocalTransform_SRT(mat4 &outMtx) const;

	void SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam = NULL);
};

template<typename tVec>
ITransform<tVec>::ITransform() :	m_eCoordType(HYCOORD_Default), 
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
/*virtual*/ void ITransform<tVec>::SetCoordinateType(HyCoordinateType eCoordType, bool bDoConversion)
{
	if(m_eCoordType == eCoordType)
		return;

	if(m_fpOnDirty)
		m_fpOnDirty(m_pOnDirtyParam);

	if(m_eCoordType == HYCOORD_Default)
		m_eCoordType = HyScene::DefaultCoordinateType();
	else if(m_eCoordType == eCoordType)
		return;
	else
		m_eCoordType = eCoordType;

	if(bDoConversion == false)
		return;

	if(m_eCoordType == HYCOORD_Meter)
		pos /= HyScene::PixelsPerMeter();
	else
		pos *= HyScene::PixelsPerMeter();
}

template<typename tVec>
void ITransform<tVec>::GetLocalTransform(mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	// TODO: Make this support 3d by exposing 'NumDimensions' in AnimVec and loop, assign tVec's using bracket overload

	vec3 ptPos(0.0f);

	ptPos.x = pos.X();
	ptPos.y = pos.Y();

	vec3 vScale(1.0f);
	vScale.x = scale.X();
	vScale.y = scale.Y();

	if(m_eCoordType == HYCOORD_Meter)
		outMtx = glm::translate(outMtx, ptPos * HyScene::PixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);

	outMtx = glm::rotate(outMtx, rot.Get().x, vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Get().y, vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get().z, vec3(0, 0, 1));
	outMtx = glm::scale(outMtx, vScale);
}

template<typename tVec>
void ITransform<tVec>::GetLocalTransform_SRT(mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	// TODO: Make this support 3d by exposing 'NumDimensions' in AnimVec and loop, assign tVec's using bracket overload
	
	vec3 ptPos(0.0f);
	ptPos.x = pos.X();
	ptPos.y = pos.Y();

	vec3 vScale(1.0f);
	vScale.x = scale.X();
	vScale.y = scale.Y();

	outMtx = glm::scale(outMtx, vScale);
	outMtx = glm::rotate(outMtx, rot.Get().x, vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Get().y, vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get().z, vec3(0, 0, 1));
	
	if(m_eCoordType == HYCOORD_Meter)
		outMtx = glm::translate(outMtx, ptPos * HyScene::PixelsPerMeter());
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

#endif /* __ITransform_h__ */