/**************************************************************************
*	IHyBody2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"
#include "HyEngine.h"

IHyBody2d::IHyBody2d(HyType eNodeType, const HyNodePath &nodePath, HyEntity2d *pParent) :
	IHyLoadable2d(eNodeType, nodePath, pParent),
	IHyBody(),
	m_fAlpha(1.0f),
	m_fCachedAlpha(1.0f),
	m_iDisplayOrder(0),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsBody;

	// Initialize as 'invalid'
	HyMath::InvalidateAABB(m_SceneAABB);

	topColor.Set(1.0f);
	botColor.Set(1.0f);

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();

	if(m_pParent)
	{
		m_iCoordinateSystem = m_pParent->GetCoordinateSystem();

		if(m_pParent->IsScissorSet())
		{
			if(m_pScissor == nullptr)
				m_pScissor = HY_NEW ScissorRect();

			m_pScissor->m_WorldScissorRect = m_pParent->m_pScissor->m_WorldScissorRect;
		}

		if(m_pParent->IsStencilSet())
			m_hStencil = m_pParent->m_hStencil;

		m_pParent->SetChildrenDisplayOrder(false);
	}
}

IHyBody2d::IHyBody2d(const IHyBody2d &copyRef) :
	IHyLoadable2d(copyRef),
	IHyBody(copyRef),
	m_iDisplayOrder(copyRef.m_iDisplayOrder),
	m_SceneAABB(copyRef.m_SceneAABB),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsBody;

	topColor = copyRef.topColor;
	botColor = copyRef.botColor;
	alpha = copyRef.alpha;

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyBody2d::IHyBody2d(IHyBody2d &&donor) noexcept :
	IHyLoadable2d(std::move(donor)),
	IHyBody(std::move(donor)),
	m_iDisplayOrder(std::move(donor.m_iDisplayOrder)),
	m_SceneAABB(std::move(donor.m_SceneAABB)),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsBody;

	topColor = std::move(donor.topColor);
	botColor = std::move(donor.botColor);
	alpha = std::move(donor.alpha);

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyBody2d::~IHyBody2d()
{
}

IHyBody2d &IHyBody2d::operator=(const IHyBody2d &rhs)
{
	IHyLoadable2d::operator=(rhs);
	IHyBody::operator=(rhs);

	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor = rhs.topColor;
	botColor = rhs.botColor;
	alpha = rhs.alpha;

	CalculateColor(0.0f);

	return *this;
}

IHyBody2d &IHyBody2d::operator=(IHyBody2d &&donor) noexcept
{
	IHyLoadable2d::operator=(std::move(donor));
	IHyBody::operator=(std::move(donor));

	m_iDisplayOrder = std::move(donor.m_iDisplayOrder);

	topColor = std::move(donor.topColor);
	botColor = std::move(donor.botColor);
	alpha = std::move(donor.alpha);

	CalculateColor(0.0f);

	return *this;
}

void IHyBody2d::SetTint(HyColor color)
{
	topColor.Set(color.GetRedF(), color.GetGreenF(), color.GetBlueF());
	botColor = topColor;
}

void IHyBody2d::SetTint(HyColor topClr, HyColor botClr)
{
	topColor.Set(topClr.GetRedF(), topClr.GetGreenF(), topClr.GetBlueF());
	botColor.Set(botClr.GetRedF(), botClr.GetGreenF(), botClr.GetBlueF());
}

float IHyBody2d::CalculateAlpha(float fExtrapolatePercent)
{
	CalculateColor(fExtrapolatePercent);
	return m_fCachedAlpha;
}

const glm::vec3 &IHyBody2d::CalculateTopTint(float fExtrapolatePercent)
{
	CalculateColor(fExtrapolatePercent);
	return m_CachedTopColor;
}

const glm::vec3 &IHyBody2d::CalculateBotTint(float fExtrapolatePercent)
{
	CalculateColor(fExtrapolatePercent);
	return m_CachedBotColor;
}

int32 IHyBody2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

/*virtual*/ void IHyBody2d::SetDisplayOrder(int32 iOrderValue)
{
	m_uiFlags |= EXPLICIT_DisplayOrder;

	if(m_iDisplayOrder != iOrderValue)
	{
		m_iDisplayOrder = iOrderValue;
		HyScene::SetInstOrderingDirty();
	}
}

/*virtual*/ void IHyBody2d::ResetDisplayOrder()
{
	m_uiFlags &= ~EXPLICIT_DisplayOrder;

	HyEntity2d *pRootParent = m_pParent;
	while(pRootParent)
	{
		if(pRootParent->m_pParent)
			pRootParent = pRootParent->m_pParent;
		else
			break;
	}

	if(pRootParent)
	{
		pRootParent->SetChildrenDisplayOrder(false);
		HyScene::SetInstOrderingDirty();
	}
}

//const HyShape2d &IHyDrawable2d::GetLocalBoundingVolume()
//{
//	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValidShape() == false)
//	{
//		OnCalcBoundingVolume();
//		ClearDirty(DIRTY_BoundingVolume);
//	}
//
//	return m_LocalBoundingVolume;
//}

float IHyBody2d::GetSceneHeight()
{
	const b2AABB &aabbRef = GetSceneAABB();
	if(aabbRef.IsValid())
		return aabbRef.GetExtents().y * 2.0f;

	return 0.0f;
}

float IHyBody2d::GetSceneWidth()
{
	const b2AABB &aabbRef = GetSceneAABB();
	if(aabbRef.IsValid())
		return aabbRef.GetExtents().x * 2.0f;

	return 0.0f;
}

/*virtual*/ void IHyBody2d::SetDirty(uint32 uiDirtyFlags) /*override*/
{
	IHyNode2d::SetDirty(uiDirtyFlags);
}

///*virtual*/ void IHyBody2d::Update() /*override*/
//{
//	IHyLoadable2d::Update();
//}

/*virtual*/ int32 IHyBody2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiFlags & EXPLICIT_DisplayOrder))
	{
		m_iDisplayOrder = iOrderValue;
		iOrderValue += 1;

		HyScene::SetInstOrderingDirty();
	}

	return iOrderValue;
}

void IHyBody2d::CalculateColor(float fExtrapolatePercent)
{
	if(IsDirty(DIRTY_Color))
	{
		m_fCachedAlpha = alpha.Extrapolate(fExtrapolatePercent);
		m_CachedTopColor = topColor.Extrapolate(fExtrapolatePercent);
		m_CachedBotColor = botColor.Extrapolate(fExtrapolatePercent);

		if(m_pParent)
		{
			m_fCachedAlpha *= m_pParent->CalculateAlpha(fExtrapolatePercent);
			m_CachedTopColor *= m_pParent->CalculateTopTint(fExtrapolatePercent);
			m_CachedBotColor *= m_pParent->CalculateTopTint(fExtrapolatePercent);
		}

		ClearDirty(DIRTY_Color);
	}
}

/*virtual*/ IHyNode &IHyBody2d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

/*virtual*/ HyEntity2d *IHyBody2d::_VisableGetParent2dPtr() /*override final*/
{
	return m_pParent;
}

/*virtual*/ HyEntity3d *IHyBody2d::_VisableGetParent3dPtr() /*override final*/
{
	return nullptr;
}
