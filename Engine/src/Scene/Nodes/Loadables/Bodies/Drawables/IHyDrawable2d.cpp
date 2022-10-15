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

IHyDrawable2d::IHyDrawable2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent) :
	IHyBody2d(eNodeType, sPrefix, sName, pParent)
{
	m_uiFlags |= NODETYPE_IsDrawable;
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :
	IHyBody2d(copyRef),
	IHyDrawable(copyRef)
{
	m_uiFlags |= NODETYPE_IsDrawable;
	m_LocalBoundingVolume = copyRef.m_LocalBoundingVolume;
}

IHyDrawable2d::IHyDrawable2d(IHyDrawable2d &&donor) noexcept :
	IHyBody2d(std::move(donor)),
	IHyDrawable(std::move(donor))
{
	m_uiFlags |= NODETYPE_IsDrawable;
	m_LocalBoundingVolume = donor.m_LocalBoundingVolume;
}

IHyDrawable2d::~IHyDrawable2d()
{
	// Detach from parent here, so parent entity isn't considered unloaded because of this node
	ParentDetach();

	// Required Unload() here because we want to invoke this class's virtual IHyDrawable2d::OnUnloaded
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

/*virtual*/ const b2AABB &IHyDrawable2d::GetSceneAABB() /*override*/
{
	if(IsDirty(DIRTY_SceneAABB))
	{
		HyMath::InvalidateAABB(m_SceneAABB);

		GetLocalBoundingVolume(); // This will update 'm_LocalBoundingVolume' if it's dirty
		m_LocalBoundingVolume.ComputeAABB(m_SceneAABB, GetSceneTransform(0.0f));

		ClearDirty(DIRTY_SceneAABB);
	}

	return m_SceneAABB;
}

/*virtual*/ void IHyDrawable2d::Update() /*override final*/
{
	IHyBody2d::Update();
}

/*virtual*/ bool IHyDrawable2d::IsValidToRender() /*override final*/
{
	return (m_uiFlags & (SETTING_IsVisible | SETTING_IsRegistered | EXPLICIT_ParentsVisible)) == (SETTING_IsVisible | SETTING_IsRegistered | EXPLICIT_ParentsVisible) &&
			IsLoaded() &&
			OnIsValidToRender();
}

/*virtual*/ void IHyDrawable2d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = HyEngine::DefaultShaderHandle(GetType());

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
