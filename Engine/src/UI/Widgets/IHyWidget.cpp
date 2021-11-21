/**************************************************************************
*	IHyWidget.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"

IHyWidget::IHyWidget(HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Widget, pParent)
{
	for(int32 i = 0; i < HYNUM_ORIENTATIONS; ++i)
		m_SizePolicies[i] = HYSIZEPOLICY_Fixed;
}

/*virtual*/ IHyWidget::~IHyWidget()
{
}

HySizePolicy IHyWidget::GetSizePolicy(HyOrientation eOrien) const
{
	return m_SizePolicies[eOrien];
}

HySizePolicy IHyWidget::GetHorizontalPolicy() const
{
	return m_SizePolicies[HYORIEN_Horizontal];
}

HySizePolicy IHyWidget::GetVerticalPolicy() const
{
	return m_SizePolicies[HYORIEN_Vertical];
}

void IHyWidget::SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy)
{
	m_SizePolicies[HYORIEN_Horizontal] = eHorizPolicy;
	m_SizePolicies[HYORIEN_Vertical] = eVertPolicy;
}

void IHyWidget::SetHorizontalPolicy(HySizePolicy ePolicy)
{
	m_SizePolicies[HYORIEN_Horizontal] = ePolicy;
}

void IHyWidget::SetVerticalPolicy(HySizePolicy ePolicy)
{
	m_SizePolicies[HYORIEN_Vertical] = ePolicy;
}

