/**************************************************************************
 *	WidgetAtlasGroup.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAtlasGroup.h"
#include "ui_WidgetAtlasGroup.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStack>
#include <QPainter>
#include <QByteArray>
#include <QTreeWidgetItemIterator>

#include <ctime>

#include "MainWindow.h"
#include "WidgetAtlasManager.h"
#include "DlgInputName.h"

WidgetAtlasGroup::WidgetAtlasGroup(QWidget *parent) :   QWidget(parent),
                                                        ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);

    // Invalid constructor. This exists so Q_OBJECT can work.
    HyGuiLog("WidgetAtlasGroup::WidgetAtlasGroup() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasGroup::WidgetAtlasGroup(QDir metaDir, QDir dataDir, WidgetAtlasManager *pManager, QWidget *parent) : QWidget(parent),
                                                                                                                m_pManager(pManager),
                                                                                                                m_MetaDir(metaDir),
                                                                                                                m_DataDir(dataDir),
                                                                                                                ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);


}

WidgetAtlasGroup::~WidgetAtlasGroup()
{
    delete ui;
}

bool WidgetAtlasGroup::IsMatching(QDir metaDir, QDir dataDir)
{
    return (m_MetaDir == metaDir && m_DataDir == dataDir);
}


int WidgetAtlasGroup::GetId()
{
    return m_MetaDir.dirName().toInt();
}

void WidgetAtlasGroup::on_btnAddImages_clicked()
{

}

void WidgetAtlasGroup::on_btnAddDir_clicked()
{

}

///*virtual*/ void WidgetAtlasGroup::enterEvent(QEvent *pEvent)
//{
//    m_pManager->PreviewAtlasGroup();
//    QWidget::enterEvent(pEvent);
//}

///*virtual*/ void WidgetAtlasGroup::leaveEvent(QEvent *pEvent)
//{
//    m_pManager->HideAtlasGroup();
//    QWidget::leaveEvent(pEvent);
//}

///*virtual*/ void WidgetAtlasGroup::resizeEvent(QResizeEvent * event)
//{
//    ResizeAtlasListColumns();
//    QWidget::resizeEvent(event);
//}

///*virtual*/ void WidgetAtlasGroup::showEvent(QShowEvent * event)
//{
//    ResizeAtlasListColumns();
//    QWidget::showEvent(event);
//}


void WidgetAtlasGroup::on_btnSettings_clicked()
{

}

void WidgetAtlasGroup::on_atlasList_itemSelectionChanged()
{

}

void WidgetAtlasGroup::on_actionDeleteImages_triggered()
{

}

void WidgetAtlasGroup::on_actionReplaceImages_triggered()
{

}

void WidgetAtlasGroup::on_actionAddFilter_triggered()
{

}
