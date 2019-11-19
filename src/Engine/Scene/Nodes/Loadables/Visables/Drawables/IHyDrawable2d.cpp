/**************************************************************************
*	IHyDrawable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"
#include "HyEngine.h"

IHyDrawable2d::IHyDrawable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :
	IHyVisable2d(eNodeType, szPrefix, szName, pParent),
	m_LocalBoundingVolume(this)
{
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :
	IHyVisable2d(copyRef),
	IHyDrawable(copyRef),
	m_LocalBoundingVolume(this, copyRef.m_LocalBoundingVolume)
{
}

IHyDrawable2d::~IHyDrawable2d()
{
	ParentDetach();
	Unload();
}

const IHyDrawable2d &IHyDrawable2d::operator=(const IHyDrawable2d &rhs)
{
	IHyVisable2d::operator=(rhs);
	IHyDrawable::operator=(rhs);
	
	m_LocalBoundingVolume = rhs.m_LocalBoundingVolume;
	m_AABB = rhs.m_AABB;

	return *this;
}

const HyShape2d &IHyDrawable2d::GetLocalBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValid() == false)
	{
		OnCalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_LocalBoundingVolume;
}

/*virtual*/ const b2AABB &IHyDrawable2d::GetWorldAABB() /*override*/
{
	if(IsDirty(DIRTY_WorldAABB))
	{
		glm::mat4 mtxWorld = GetWorldTransform();
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

		GetLocalBoundingVolume(); // This will update BV if it's dirty
		if(m_LocalBoundingVolume.IsValid() && m_LocalBoundingVolume.GetB2Shape())
			m_LocalBoundingVolume.GetB2Shape()->ComputeAABB(&m_AABB, b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), 0);
		else
		{
			m_AABB.lowerBound.SetZero();
			m_AABB.upperBound.SetZero();
		}

		ClearDirty(DIRTY_WorldAABB);
	}

	return m_AABB;
}

/*virtual*/ void IHyDrawable2d::Update() /*override final*/
{
	IHyVisable2d::Update();
}

/*virtual*/ bool IHyDrawable2d::IsValid() /*override final*/
{
	return m_bVisible && OnIsValid();
}

/*virtual*/ void IHyDrawable2d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyDrawable2d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}

/*virtual*/ IHyNode &IHyDrawable2d::_DrawableGetNodeRef() /*override final*/
{
	return *this;
}
