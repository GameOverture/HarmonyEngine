/**************************************************************************
 *	AtlasWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasWidget.h"
#include "ui_AtlasWidget.h"
#include "Project.h"
#include "SpriteWidget.h"
#include "ExplorerItemData.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QPainter>
#include <QMessageBox>

#include "MainWindow.h"
#include "DlgInputName.h"
#include "DlgAtlasGroupSettings.h"

const float fTRANS_DUR = 0.5f;
const int iPADDING = 2;

//AtlasTreeWidget::AtlasTreeWidget(QWidget *parent /*= Q_NULLPTR*/) :
//	QTreeWidget(parent)
//{
//}
//void AtlasTreeWidget::SetAtlasOwner(AtlasWidget *pOwner)
//{
//	m_pOwner = pOwner;
//}
///*virtual*/ void AtlasTreeWidget::dropEvent(QDropEvent *e)
//{
//	QTreeWidget::dropEvent(e);
//
//	sortItems(0, Qt::AscendingOrder);
//	m_pOwner->GetData().WriteMetaSettings();
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool AtlasTreeItem::operator<(const QTreeWidgetItem &rhs) const
//{
//	bool bLeftIsFilter = this->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
//	bool bRightIsFilter = rhs.data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
//
//	if(bLeftIsFilter && bRightIsFilter == false)
//		return true;
//	if(bLeftIsFilter == false && bRightIsFilter)
//		return false;
//
//	return this->text(0) < rhs.text(0);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasWidget::AtlasWidget(QWidget *pParent) :
	QWidget(pParent),
	m_pModel(nullptr),
	m_Draw(nullptr),
	ui(new Ui::AtlasWidget)
{
	ui->setupUi(this);
	
	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("AtlasWidget::AtlasWidget() invalid constructor used", LOGTYPE_Error);
}

AtlasWidget::AtlasWidget(AtlasModel *pModel, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AtlasWidget),
	m_pModel(pModel),
	m_Draw(pModel),
	m_pMouseHoverItem(nullptr)
{
	ui->setupUi(this);


}

AtlasWidget::~AtlasWidget()
{
	delete ui;
}

void AtlasWidget::DrawUpdate()
{
	m_Draw.SetHover(ui->atlasList->itemAt(ui->atlasList->mapFromGlobal(QCursor::pos())));
	m_Draw.DrawUpdate();
}

void AtlasWidget::StashTreeWidgets()
{
	ui->atlasList->selectionModel()->clearSelection();
	
	QList<AtlasTreeItem *> stashedTreeItemList;
	while(ui->atlasList->topLevelItemCount())
		stashedTreeItemList.append(static_cast<AtlasTreeItem *>(ui->atlasList->takeTopLevelItem(0)));

	m_pModel->StashTreeWidgets(stashedTreeItemList);
}









void AtlasWidget::on_actionDeleteImages_triggered()
{

}

void AtlasWidget::on_actionReplaceImages_triggered()
{

}

void AtlasWidget::on_atlasList_itemSelectionChanged()
{

}

void AtlasWidget::on_actionRename_triggered()
{

}

void AtlasWidget::on_cmbAtlasGroups_currentIndexChanged(int index)
{
	
}

void AtlasWidget::on_actionAddGroup_triggered()
{

}

void AtlasWidget::on_actionGroupSettings_triggered()
{
	
}

void AtlasWidget::on_actionRemoveGroup_triggered()
{

}

void AtlasWidget::on_actionAtlasGrpTransfer_triggered(QAction *pAction)
{

}

void AtlasWidget::on_actionImportImages_triggered()
{

}

void AtlasWidget::on_actionImportDirectory_triggered()
{

}

void AtlasWidget::on_actionAddFilter_triggered()
{

}

