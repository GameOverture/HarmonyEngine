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
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable3d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyDrawable3d::IHyDrawable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent) :
	IHyLoadable3d(eNodeType, szPrefix, szName, pParent),
	m_fAlpha(1.0f),
	m_fCachedAlpha(1.0f),
	tint(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiExplicitAndTypeFlags |= NODETYPE_IsVisable;

	tint.Set(1.0f);
	m_CachedTint = tint.Get();

	if(m_pParent)
		SetupNewChild(*m_pParent, *this);
}

IHyDrawable3d::IHyDrawable3d(const IHyDrawable3d &copyRef) :
	IHyLoadable3d(copyRef),
	IHyDrawable(copyRef),
	m_fAlpha(copyRef.m_fAlpha),
	tint(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	tint.Set(copyRef.tint.Get());
	alpha.Set(copyRef.alpha.Get());

	CalculateColor();
}

IHyDrawable3d::~IHyDrawable3d()
{
}

const IHyDrawable3d &IHyDrawable3d::operator=(const IHyDrawable3d &rhs)
{
	IHyLoadable3d::operator=(rhs);
	IHyDrawable::operator=(rhs);

	m_fAlpha = rhs.m_fAlpha;

	tint.Set(rhs.tint.Get());
	alpha.Set(rhs.alpha.Get());

	CalculateColor();

	return *this;
}

void IHyDrawable3d::SetTint(float fR, float fG, float fB)
{
	tint.Set(fR, fG, fB);
}

void IHyDrawable3d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyDrawable3d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyDrawable3d::CalculateTint()
{
	CalculateColor();
	return m_CachedTint;
}

/*virtual*/ void IHyDrawable3d::Update() /*override*/
{
	IHyLoadable3d::Update();
}

void IHyDrawable3d::CalculateColor()
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

/*virtual*/ IHyNode &IHyDrawable3d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

/*virtual*/ HyEntity2d *IHyDrawable3d::_VisableGetParent2dPtr() /*override final*/
{
	return nullptr;
}

/*virtual*/ HyEntity3d *IHyDrawable3d::_VisableGetParent3dPtr() /*override final*/
{
	return m_pParent;
}

/*friend*/ void SetupNewChild(HyEntity3d &parentRef, IHyDrawable3d &childRef)
{
	childRef._SetCoordinateSystem(parentRef.GetCoordinateSystem(), false);

	if(parentRef.IsScissorSet())
		static_cast<IHyDrawable3d &>(childRef)._SetScissor(parentRef.m_pScissor, false);
}
