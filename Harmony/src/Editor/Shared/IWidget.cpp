/**************************************************************************
*	IWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "IWidget.h"
#include "ProjectItem.h"

IWidget::IWidget(ProjectItem &itemRef, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	m_ItemRef(itemRef)
{
}

IWidget::~IWidget()
{
}

ProjectItem &IWidget::GetItem()
{
	return m_ItemRef;
}
