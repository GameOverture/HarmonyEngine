/**************************************************************************
*	HyContainer.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Containers/HyContainer.h"
#include "UI/Layouts/HyBoxLayout.h"

HyContainer::HyContainer(HyLayoutType eRootLayout, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Container, pParent),
	m_pRootLayout(nullptr)
{
	switch(eRootLayout)
	{
	case HYLAYOUT_Horizontal:
		m_pRootLayout = HY_NEW HyBoxLayout(HYORIEN_Horizontal, this);
		break;
	case HYLAYOUT_Vertical:
		m_pRootLayout = HY_NEW HyBoxLayout(HYORIEN_Vertical, this);
		break;

	default:
		HyError("HyContainer::HyContainer layout type not supported");
		break;
	}
}

/*virtual*/ HyContainer::~HyContainer()
{
	delete m_pRootLayout;
}

glm::ivec2 HyContainer::GetSize() const
{
	return m_pRootLayout->GetSize();
}

void HyContainer::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	m_pRootLayout->SetSize(iNewWidth, iNewHeight);
}

IHyLayout *HyContainer::GetRootLayout()
{
	return m_pRootLayout;
}
