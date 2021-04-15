/**************************************************************************
*	IHyDrawable3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h"
#include "HyEngine.h"

IHyDrawable3d::IHyDrawable3d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity3d *pParent) :
	IHyBody3d(eNodeType, sPrefix, sName, pParent)
{
	m_uiFlags |= NODETYPE_IsDrawable;
}

IHyDrawable3d::IHyDrawable3d(const IHyDrawable3d &copyRef) :
	IHyBody3d(copyRef),
	IHyDrawable(copyRef)
{
	m_uiFlags |= NODETYPE_IsDrawable;
}

IHyDrawable3d::IHyDrawable3d(IHyDrawable3d &&donor) noexcept :
	IHyBody3d(std::move(donor)),
	IHyDrawable(std::move(donor))
{
	m_uiFlags |= NODETYPE_IsDrawable;
}

IHyDrawable3d::~IHyDrawable3d()
{
}

IHyDrawable3d &IHyDrawable3d::operator=(const IHyDrawable3d &rhs)
{
	IHyLoadable3d::operator=(rhs);
	IHyDrawable::operator=(rhs);

	return *this;
}

IHyDrawable3d &IHyDrawable3d::operator=(IHyDrawable3d &&donor) noexcept
{
	IHyLoadable3d::operator=(std::move(donor));
	IHyDrawable::operator=(std::move(donor));

	return *this;
}

/*virtual*/ bool IHyDrawable3d::IsValidToRender() /*override final*/
{
	return (m_uiFlags & (SETTING_IsVisible | SETTING_IsRegistered)) == (SETTING_IsVisible | SETTING_IsRegistered) &&
			IsLoaded() &&
			OnIsValidToRender();
}

/*virtual*/ void IHyDrawable3d::Update() /*override final*/
{
	IHyBody3d::Update();
}

/*virtual*/ void IHyDrawable3d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = HyEngine::DefaultShaderHandle(GetType());

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyDrawable3d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}

/*virtual*/ IHyNode &IHyDrawable3d::_DrawableGetNodeRef() /*override final*/
{
	return *this;
}
