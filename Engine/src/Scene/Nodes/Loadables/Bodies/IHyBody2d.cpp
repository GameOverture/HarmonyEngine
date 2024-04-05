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
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"
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
	alpha(m_fAlpha, *this, DIRTY_Color),
	m_hScissorStencil(HY_UNUSED_HANDLE)
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
			m_hScissorStencil = m_pParent->m_hScissorStencil;

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
	alpha(m_fAlpha, *this, DIRTY_Color),
	m_hScissorStencil(copyRef.m_hScissorStencil)
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
	alpha(m_fAlpha, *this, DIRTY_Color),
	m_hScissorStencil(std::move(donor.m_hScissorStencil))
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
	ClearScissor(true);
}

IHyBody2d &IHyBody2d::operator=(const IHyBody2d &rhs)
{
	IHyLoadable2d::operator=(rhs);
	IHyBody::operator=(rhs);

	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor = rhs.topColor;
	botColor = rhs.botColor;
	alpha = rhs.alpha;
	m_hScissorStencil = rhs.m_hScissorStencil;

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
	m_hScissorStencil = std::move(donor.m_hScissorStencil);

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

bool IHyBody2d::IsScissorSet() const
{
	return m_hScissorStencil != HY_UNUSED_HANDLE;
}

void IHyBody2d::GetScissor(HyRect &scissorOut) const
{
	if(IsScissorSet() == false)
	{
		scissorOut = HyRect();
		return;
	}

	HyStencil *pScissorStencil = IHyRenderer::FindStencil(m_hScissorStencil);
	HyAssert(pScissorStencil &&
		pScissorStencil->GetBehavior() == HYSTENCILBEHAVIOR_Scissor &&
		pScissorStencil->GetInstanceList().size() == 1 &&
		pScissorStencil->GetInstanceList()[0]->GetType() == HYTYPE_Primitive, "IHyBody::GetScissor() m_hScissorStencil was a stencil that is not a scissor");

	uint32 uiVertCount = static_cast<HyPrimitive2d *>(pScissorStencil->GetInstanceList()[0])->GetNumVerts();
	const glm::vec2 *pVertArray = static_cast<HyPrimitive2d *>(pScissorStencil->GetInstanceList()[0])->GetVerts();

	// Find dimensions of the box
	float minX = pVertArray[0].x;
	float maxX = pVertArray[0].x;
	float minY = pVertArray[0].y;
	float maxY = pVertArray[0].y;
	for(int iVertIndex = 0; iVertIndex < uiVertCount; ++iVertIndex)
	{
		if(pVertArray[iVertIndex].x < minX)
			minX = pVertArray[iVertIndex].x;
		if(pVertArray[iVertIndex].x > maxX)
			maxX = pVertArray[iVertIndex].x;
		if(pVertArray[iVertIndex].y < minY)
			minY = pVertArray[iVertIndex].y;
		if(pVertArray[iVertIndex].y > maxY)
			maxY = pVertArray[iVertIndex].y;
	}
	float fHalfWidth = (maxX - minX) / 2.0f;
	float fHalfHeight = (maxY - minY) / 2.0f;

	// Find center of the box
	glm::vec2 ptCenter = glm::vec2(0.0f);
	for(int iVertIndex = 0; iVertIndex < uiVertCount; ++iVertIndex)
		ptCenter += pVertArray[iVertIndex];
	ptCenter /= static_cast<float>(uiVertCount); // Averaging the sum coordinates of the vertices.

	// Calculate rotation angle
	glm::vec2 vEdge1 = pVertArray[1] - pVertArray[0];
	glm::vec2 vEdge2 = pVertArray[2] - pVertArray[1];
	float fRotDegrees = glm::degrees(glm::acos(glm::dot(glm::normalize(vEdge1), glm::normalize(vEdge2))));

	scissorOut.Set(fHalfWidth, fHalfHeight, ptCenter, fRotDegrees);
}

//const HyStencil *IHyBody2d::GetScissorStencil() const
//{
//	return IHyRenderer::FindStencil(m_hScissorStencil);
//}

HyStencilHandle IHyBody2d::GetScissorHandle() const
{
	return m_hScissorStencil;
}

/*virtual*/ void IHyBody2d::SetScissor(const HyRect &scissorRect)
{
	if((m_uiFlags & EXPLICIT_ScissorStencil) != 0)
	{
		HyStencil *pScissorStencil = IHyRenderer::FindStencil(m_hScissorStencil);
		HyAssert(pScissorStencil &&
			pScissorStencil->GetBehavior() == HYSTENCILBEHAVIOR_Scissor &&
			pScissorStencil->GetInstanceList().size() == 1 &&
			pScissorStencil->GetInstanceList()[0]->GetType() == HYTYPE_Primitive, "IHyBody2d::SetScissor() m_hScissorStencil was a stencil that is not a scissor");

		HyPrimitive2d *pScissorPrim = static_cast<HyPrimitive2d *>(pScissorStencil->GetInstanceList()[0]);
		pScissorPrim->SetAsBox(scissorRect);
	}
	else
	{
		HyStencil *pNewScissorStencil = HY_NEW HyStencil();
		pNewScissorStencil->SetAsScissor(scissorRect, this);
		m_uiFlags |= SETTING_AllocScissorStencil;

		m_hScissorStencil = pNewScissorStencil->GetHandle();
		m_uiFlags |= EXPLICIT_ScissorStencil;
	}

	SetDirty(DIRTY_ScissorStencil);
}

/*virtual*/ void IHyBody2d::SetScissor(HyStencilHandle hScissorHandle)
{
	ClearScissor(false);

	m_hScissorStencil = hScissorHandle;
	m_uiFlags |= EXPLICIT_ScissorStencil;
}

/*virtual*/ void IHyBody2d::ClearScissor(bool bUseParentScissor)
{
	m_hScissorStencil = HY_UNUSED_HANDLE;

	if((m_uiFlags & SETTING_AllocScissorStencil) != 0)
	{
		delete IHyRenderer::FindStencil(m_hScissorStencil);
		m_uiFlags &= ~SETTING_AllocScissorStencil;
	}

	if(bUseParentScissor == false)
		m_uiFlags |= EXPLICIT_ScissorStencil;
	else
	{
		m_uiFlags &= ~EXPLICIT_ScissorStencil;
		if(m_pParent && m_pParent->IsScissorSet())
			m_hScissorStencil = m_pParent->GetScissorHandle();
	}
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

/*virtual*/ void IHyBody2d::_SetScissorStencil(HyStencilHandle hHandle, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiFlags &= ~EXPLICIT_ScissorStencil;

	if(0 == (m_uiFlags & EXPLICIT_ScissorStencil))
		m_hScissorStencil = hHandle;
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
