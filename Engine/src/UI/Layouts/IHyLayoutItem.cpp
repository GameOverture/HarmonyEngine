/**************************************************************************
*	IHyLayoutItem.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayoutItem.h"

IHyLayoutItem::IHyLayoutItem(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_HorzPolicy(Preferred),
	m_VertPolicy(Preferred)
{
}

/*virtual*/ IHyLayoutItem::~IHyLayoutItem()
{
}

IHyLayoutItem::SizePolicy IHyLayoutItem::GetHorizontalPolicy() const
{
	return m_HorzPolicy;
}

IHyLayoutItem::SizePolicy IHyLayoutItem::GetVerticalPolicy() const
{
	return m_VertPolicy;
}

void IHyLayoutItem::SetSizePolicy(SizePolicy eHorizPolicy, SizePolicy eVertPolicy)
{
	m_HorzPolicy = eHorizPolicy;
	m_VertPolicy = eVertPolicy;
}

void IHyLayoutItem::SetHorizontalPolicy(SizePolicy ePolicy)
{
	m_HorzPolicy = ePolicy;
}

void IHyLayoutItem::SetVerticalPolicy(SizePolicy ePolicy)
{
	m_VertPolicy = ePolicy;
}

glm::ivec2 IHyLayoutItem::GetSizeHint() const
{
	return m_vSizeHint;
}

void IHyLayoutItem::SetSizeHint(int32 iWidth, int32 iHeight)
{
	HySetVec(m_vSizeHint, iWidth, iHeight);
}
