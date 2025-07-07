/**************************************************************************
 *	WgtAppendContainer.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtAppendContainer.h"
#include "ui_WgtAppendContainer.h"
#include "WgtSrcDependency.h"

#include <QPushButton>

WgtAppendContainer::WgtAppendContainer(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtAppendContainer)
{
	ui->setupUi(this);

	ui->btnAddElement->setDefaultAction(ui->actionAddNewElement);
}

WgtAppendContainer::~WgtAppendContainer()
{
	for(int i = 0; i < m_WidgetList.count(); ++i)
		delete m_WidgetList[i];

	delete ui;
}

void WgtAppendContainer::RemoveWidget(IWgtAppendWidget *pRemoved)
{
	for(int i = 0; i < m_WidgetList.count(); ++i)
	{
		if(m_WidgetList[i] == pRemoved)
		{
			delete m_WidgetList.takeAt(i);
			break;
		}
	}
	Refresh();
}

void WgtAppendContainer::Refresh()
{
	if(ui->lytContainer->count() != m_WidgetList.count())
	{
		if(ui->lytContainer->count() < m_WidgetList.count())
		{
			for(int i = ui->lytContainer->count(); i < m_WidgetList.count(); ++i)
				ui->lytContainer->addWidget(m_WidgetList[i]);
		}
		else
		{
			for(int i = m_WidgetList.count(); i < ui->lytContainer->count() - 1; ++i) // Keep at least '1'
				delete ui->lytContainer->takeAt(i);
		}
	}
}

void WgtAppendContainer::on_actionAddNewElement_triggered()
{
	m_WidgetList.append(AllocWidget());
	ui->lytContainer->addWidget(m_WidgetList[m_WidgetList.count() - 1]);
	
	//connect(m_WidgetList[m_WidgetList.count() - 1], &WgtSrcDependency::OnDirty, this, &WgtAppendContainer::ErrorCheck);

	Refresh();
}
