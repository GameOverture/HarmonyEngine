/**************************************************************************
*	IHyBody3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody3d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyBody3d::IHyBody3d(HyType eNodeType, const HyNodePath &nodePath, HyEntity3d *pParent) :
	IHyLoadable3d(eNodeType, nodePath, pParent),
	m_fAlpha(1.0f),
	m_fCachedAlpha(1.0f),
	tint(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsBody;

	tint.Set(1.0f);

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
	}
}

IHyBody3d::IHyBody3d(const IHyBody3d &copyRef) :
	IHyLoadable3d(copyRef),
	IHyBody(copyRef),
	tint(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsBody;

	tint = copyRef.tint;
	alpha = copyRef.alpha;
}

IHyBody3d::IHyBody3d(IHyBody3d &&donor) noexcept :
	IHyLoadable3d(std::move(donor)),
	IHyBody(std::move(donor)),
	tint(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsBody;

	tint = std::move(donor.tint);
	alpha = std::move(donor.alpha);

	CalculateColor();
}

IHyBody3d::~IHyBody3d()
{
}

IHyBody3d &IHyBody3d::operator=(const IHyBody3d &rhs)
{
	IHyLoadable3d::operator=(rhs);
	IHyBody::operator=(rhs);

	tint.Set(rhs.tint.Get());
	alpha.Set(rhs.alpha.Get());

	CalculateColor();

	return *this;
}

IHyBody3d &IHyBody3d::operator=(IHyBody3d &&donor)
{
	IHyLoadable3d::operator=(std::move(donor));
	IHyBody::operator=(std::move(donor));

	tint = std::move(donor.tint);
	alpha = std::move(donor.alpha);

	CalculateColor();

	return *this;
}

void IHyBody3d::SetTint(float fR, float fG, float fB)
{
	tint.Set(fR, fG, fB);
}

void IHyBody3d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyBody3d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyBody3d::CalculateTint()
{
	CalculateColor();
	return m_CachedTint;
}

/*virtual*/ void IHyBody3d::Update() /*override*/
{
	IHyLoadable3d::Update();
}

void IHyBody3d::CalculateColor()
{
	if(IsDirty(DIRTY_Color))
	{
		m_fCachedAlpha = alpha.Get();
		m_CachedTint = tint.Get();

		if(m_pParent)
		{
			m_fCachedAlpha *= m_pParent->CalculateAlpha();
			m_CachedTint *= m_pParent->CalculateTint();
		}

		ClearDirty(DIRTY_Color);
	}
}

/*virtual*/ IHyNode &IHyBody3d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

/*virtual*/ HyEntity2d *IHyBody3d::_VisableGetParent2dPtr() /*override final*/
{
	return nullptr;
}

/*virtual*/ HyEntity3d *IHyBody3d::_VisableGetParent3dPtr() /*override final*/
{
	return m_pParent;
}
