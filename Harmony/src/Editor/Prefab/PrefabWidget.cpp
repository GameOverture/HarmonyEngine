/**************************************************************************
 *	PrefabWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "PrefabWidget.h"
#include "ui_PrefabWidget.h"

PrefabWidget::PrefabWidget(ProjectItem &itemRef, QWidget *parent) :	QWidget(parent),
																	ui(new Ui::PrefabWidget),
																	m_ItemRef(itemRef)
{
    ui->setupUi(this);
}

PrefabWidget::~PrefabWidget()
{
    delete ui;
}

void PrefabWidget::OnGiveMenuActions(QMenu *pMenu)
{
}

void PrefabWidget::FocusState(int iStateIndex, QVariant subState)
{
}
