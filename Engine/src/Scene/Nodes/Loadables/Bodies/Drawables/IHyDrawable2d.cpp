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
#include "Assets/Nodes/Objects/HySpineData.h"
#include "Assets/Nodes/Objects/HySpriteData.h"
#include "Assets/Nodes/Objects/HyTextData.h"

IHyDrawable2d::IHyDrawable2d(HyType eInstType, const HyNodePath &nodePath, HyEntity2d *pParent) :
	IHyBody2d(eInstType, nodePath, pParent)
{
	m_uiFlags |= NODETYPE_IsDrawable;
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :
	IHyBody2d(copyRef),
	IHyDrawable(copyRef)
{
	m_uiFlags |= NODETYPE_IsDrawable;
}

IHyDrawable2d::IHyDrawable2d(IHyDrawable2d &&donor) noexcept :
	IHyBody2d(std::move(donor)),
	IHyDrawable(std::move(donor))
{
	m_uiFlags |= NODETYPE_IsDrawable;
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

	return *this;
}

IHyDrawable2d &IHyDrawable2d::operator=(IHyDrawable2d &&donor) noexcept
{
	IHyBody2d::operator=(std::move(donor));
	IHyDrawable::operator=(std::move(donor));

	return *this;
}

/*virtual*/ const b2AABB &IHyDrawable2d::GetSceneAABB() /*override*/
{
	if(IsDirty(DIRTY_SceneAABB))
	{
		HyMath::InvalidateAABB(m_SceneAABB);

		// Sets 'm_SceneAABB' to a scene transformed, conforming AABB around the visible portion of *this
		HyShape2d shape;
		CalcLocalBoundingShape(shape);
		if(shape.IsValidShape())
			shape.ComputeAABB(m_SceneAABB, GetSceneTransform(0.0f));

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

#ifdef HY_PLATFORM_GUI
template<typename HYDATATYPE>
void IHyDrawable2d::GuiOverrideData(HyJsonObj itemDataObj, bool bUseGuiOverrideName /*= true*/)
{
	// TODO: THREAD SAFETY FIX! Ensure HarmonyWidget::paintGL() doesn't invoke m_pHyEngine->Update() while we delete/reallocate 'm_pData'
	delete m_pData;
	m_pData = HY_NEW HYDATATYPE(HyNodePath(bUseGuiOverrideName ? HY_GUI_DATAOVERRIDE : ""), itemDataObj, *IHyLoadable::sm_pHyAssets);
	OnDataAcquired();

	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = HyEngine::DefaultShaderHandle(GetType());
}

// Explicit instantiations
template void IHyDrawable2d::GuiOverrideData<HySpriteData>(HyJsonObj, bool);
template void IHyDrawable2d::GuiOverrideData<HySpineData>(HyJsonObj, bool);
template void IHyDrawable2d::GuiOverrideData<HyTextData>(HyJsonObj, bool);
#endif

/*virtual*/ IHyNode &IHyDrawable2d::_DrawableGetNodeRef() /*override final*/
{
	return *this;
}
