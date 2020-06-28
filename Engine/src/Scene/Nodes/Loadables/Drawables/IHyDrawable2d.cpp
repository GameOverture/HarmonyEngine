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
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyDrawable2d::IHyDrawable2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent) :
	IHyLoadable2d(eNodeType, sPrefix, sName, pParent),
	m_fAlpha(1.0f),
	m_fCachedAlpha(1.0f),
	m_iDisplayOrder(0),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsDrawable;

	topColor.Set(1.0f);
	botColor.Set(1.0f);

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :
	IHyLoadable2d(copyRef),
	IHyDrawable(copyRef),
	m_iDisplayOrder(copyRef.m_iDisplayOrder),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsDrawable;

	topColor = copyRef.topColor;
	botColor = copyRef.botColor;
	alpha = copyRef.alpha;

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyDrawable2d::IHyDrawable2d(IHyDrawable2d &&donor) :
	IHyLoadable2d(std::move(donor)),
	IHyDrawable(std::move(donor)),
	m_iDisplayOrder(std::move(donor.m_iDisplayOrder)),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsDrawable;

	topColor = std::move(donor.topColor);
	botColor = std::move(donor.botColor);
	alpha = std::move(donor.alpha);

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyDrawable2d::~IHyDrawable2d()
{
}

IHyDrawable2d &IHyDrawable2d::operator=(const IHyDrawable2d &rhs)
{
	IHyLoadable2d::operator=(rhs);
	IHyDrawable::operator=(rhs);

	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor = rhs.topColor;
	botColor = rhs.botColor;
	alpha = rhs.alpha;

	CalculateColor();

	return *this;
}

IHyDrawable2d &IHyDrawable2d::operator=(IHyDrawable2d &&donor)
{
	IHyLoadable2d::operator=(std::move(donor));
	IHyDrawable::operator=(std::move(donor));

	m_iDisplayOrder = std::move(donor.m_iDisplayOrder);

	topColor = std::move(donor.topColor);
	botColor = std::move(donor.botColor);
	alpha = std::move(donor.alpha);

	CalculateColor();

	return *this;
}

void IHyDrawable2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyDrawable2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyDrawable2d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyDrawable2d::CalculateTopTint()
{
	CalculateColor();
	return m_CachedTopColor;
}

const glm::vec3 &IHyDrawable2d::CalculateBotTint()
{
	CalculateColor();
	return m_CachedBotColor;
}

int32 IHyDrawable2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

/*virtual*/ void IHyDrawable2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

/*virtual*/ void IHyDrawable2d::ResetDisplayOrder()
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

/*virtual*/ void IHyDrawable2d::Update() /*override*/
{
	IHyLoadable2d::Update();
}

/*virtual*/ int32 IHyDrawable2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding)
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

void IHyDrawable2d::CalculateColor()
{
	if(IsDirty(DIRTY_Color))
	{
		m_fCachedAlpha = alpha.Get();
		m_CachedTopColor = topColor.Get();
		m_CachedBotColor = botColor.Get();

		if(m_pParent)
		{
			m_fCachedAlpha *= m_pParent->CalculateAlpha();
			m_CachedTopColor *= m_pParent->CalculateTopTint();
			m_CachedBotColor *= m_pParent->CalculateTopTint();
		}

		ClearDirty(DIRTY_Color);
	}
}

/*virtual*/ IHyNode &IHyDrawable2d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

/*virtual*/ HyEntity2d *IHyDrawable2d::_VisableGetParent2dPtr() /*override final*/
{
	return m_pParent;
}

/*virtual*/ HyEntity3d *IHyDrawable2d::_VisableGetParent3dPtr() /*override final*/
{
	return nullptr;
}

/*friend*/ void _CtorSetupNewChild(HyEntity2d &parentRef, IHyDrawable2d &childRef)
{
	childRef._SetCoordinateSystem(parentRef.GetCoordinateSystem(), false);

	if(parentRef.IsScissorSet())
		childRef._SetScissor(parentRef.m_pScissor, false);

	if(parentRef.IsStencilSet())
		childRef._SetStencil(parentRef.m_hStencil, false);

	parentRef.SetChildrenDisplayOrder(false);
}
