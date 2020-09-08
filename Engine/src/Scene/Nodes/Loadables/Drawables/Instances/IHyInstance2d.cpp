/**************************************************************************
*	IHyInstance2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h"
#include "HyEngine.h"

extern HyShaderHandle Hy_DefaultShaderHandle(HyType eType);

IHyInstance2d::IHyInstance2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent) :
	IHyDrawable2d(eNodeType, sPrefix, sName, pParent)
{
}

IHyInstance2d::IHyInstance2d(const IHyInstance2d &copyRef) :
	IHyDrawable2d(copyRef),
	IHyInstance(copyRef),
	m_LocalBoundingVolume(copyRef.m_LocalBoundingVolume)
{
}

IHyInstance2d::IHyInstance2d(IHyInstance2d &&donor) :
	IHyDrawable2d(std::move(donor)),
	IHyInstance(std::move(donor)),
	m_LocalBoundingVolume(std::move(donor.m_LocalBoundingVolume))
{
}

IHyInstance2d::~IHyInstance2d()
{
	ParentDetach();
	Unload();
}

IHyInstance2d &IHyInstance2d::operator=(const IHyInstance2d &rhs)
{
	IHyDrawable2d::operator=(rhs);
	IHyInstance::operator=(rhs);
	
	m_LocalBoundingVolume = rhs.m_LocalBoundingVolume;

	return *this;
}

IHyInstance2d &IHyInstance2d::operator=(IHyInstance2d &&donor)
{
	IHyDrawable2d::operator=(std::move(donor));
	IHyInstance::operator=(std::move(donor));

	m_LocalBoundingVolume = std::move(donor.m_LocalBoundingVolume);

	return *this;
}

const HyShape2d &IHyInstance2d::GetLocalBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValidShape() == false)
	{
		OnCalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_LocalBoundingVolume;
}

/*virtual*/ const b2AABB &IHyInstance2d::GetWorldAABB() /*override*/
{
	if(IsDirty(DIRTY_WorldAABB))
	{
		glm::mat4 mtxWorld = GetWorldTransform();
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

		GetLocalBoundingVolume(); // This will update BV if it's dirty
		if(m_LocalBoundingVolume.IsValidShape() && m_LocalBoundingVolume.GetB2Shape())
			m_LocalBoundingVolume.GetB2Shape()->ComputeAABB(&m_WorldAABB, b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), 0);
		else
		{
			m_WorldAABB.lowerBound.SetZero();
			m_WorldAABB.upperBound.SetZero();
		}

		ClearDirty(DIRTY_WorldAABB);
	}

	return m_WorldAABB;
}

/*virtual*/ void IHyInstance2d::Update() /*override final*/
{
	IHyDrawable2d::Update();
}

/*virtual*/ bool IHyInstance2d::IsValid() /*override final*/
{
	return (m_uiFlags & (SETTING_IsVisible | SETTING_IsRegistered)) == (SETTING_IsVisible | SETTING_IsRegistered) && OnIsValid();
}

/*virtual*/ void IHyInstance2d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(GetType());

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyInstance2d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}

/*virtual*/ IHyNode &IHyInstance2d::_DrawableGetNodeRef() /*override final*/
{
	return *this;
}
