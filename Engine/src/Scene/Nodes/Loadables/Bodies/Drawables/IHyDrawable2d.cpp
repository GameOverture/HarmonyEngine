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
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "HyEngine.h"

extern HyShaderHandle Hy_DefaultShaderHandle(HyType eType);

IHyDrawable2d::IHyDrawable2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent) :
	IHyBody2d(eNodeType, sPrefix, sName, pParent)
{
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :
	IHyBody2d(copyRef),
	IHyDrawable(copyRef),
	m_LocalBoundingVolume(copyRef.m_LocalBoundingVolume)
{
}

IHyDrawable2d::IHyDrawable2d(IHyDrawable2d &&donor) noexcept :
	IHyBody2d(std::move(donor)),
	IHyDrawable(std::move(donor)),
	m_LocalBoundingVolume(std::move(donor.m_LocalBoundingVolume))
{
}

IHyDrawable2d::~IHyDrawable2d()
{
	ParentDetach();
	Unload();
}

IHyDrawable2d &IHyDrawable2d::operator=(const IHyDrawable2d &rhs)
{
	IHyBody2d::operator=(rhs);
	IHyDrawable::operator=(rhs);
	
	m_LocalBoundingVolume = rhs.m_LocalBoundingVolume;

	return *this;
}

IHyDrawable2d &IHyDrawable2d::operator=(IHyDrawable2d &&donor) noexcept
{
	IHyBody2d::operator=(std::move(donor));
	IHyDrawable::operator=(std::move(donor));

	m_LocalBoundingVolume = std::move(donor.m_LocalBoundingVolume);

	return *this;
}

const HyShape2d &IHyDrawable2d::GetLocalBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValidShape() == false)
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

/*virtual*/ void IHyDrawable2d::Update() /*override final*/
{
	IHyBody2d::Update();
}

/*virtual*/ bool IHyDrawable2d::IsValidToRender() /*override final*/
{
	return (m_uiFlags & (SETTING_IsVisible | SETTING_IsRegistered)) == (SETTING_IsVisible | SETTING_IsRegistered) && OnIsValidToRender();
}

/*virtual*/ void IHyDrawable2d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(GetType());

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
