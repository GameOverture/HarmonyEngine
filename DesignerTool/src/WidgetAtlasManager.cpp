/**************************************************************************
 *	WidgetAtlasManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAtlasManager.h"
#include "ui_WidgetAtlasManager.h"
#include "HyGlobal.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "MainWindow.h"

WidgetAtlasManager::WidgetAtlasManager(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::WidgetAtlasManager)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAtlasManager::WidgetAtlasManager() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasManager::WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                             ui(new Ui::WidgetAtlasManager),
                                                                                             m_pProjOwner(pProjOwner),
                                                                                             m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases))
{
    ui->setupUi(this);
    
    Reload();
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    delete ui;
}

void WidgetAtlasManager::SaveData()
{
    QJsonArray atlasGroupArray;
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        QJsonObject atlasGroupObj;
        static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->GetAtlasInfo(atlasGroupObj);
        atlasGroupArray.append(atlasGroupObj);
    }

    QJsonDocument atlasInfoDoc;
    atlasInfoDoc.setArray(atlasGroupArray);

    QFile atlasInfoFile(m_DataDir.absolutePath() % "/" % HYGUIPATH_DataAtlasFileName);
    if(atlasInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
    }
    else
    {
        qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);
        }

        atlasInfoFile.close();
    }
}

void WidgetAtlasManager::PreviewAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_AtlasManager);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
}

/*friend*/ void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    AtlasGroup_DrawOpen(atlasMan.m_pProjOwner, hyApp, *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget()));
}

/*friend*/ void AtlasManager_DrawClose(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    AtlasGroup_DrawClose(atlasMan.m_pProjOwner, hyApp, *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget()));
}

/*friend*/ void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    AtlasGroup_DrawShow(atlasMan.m_pProjOwner, hyApp, *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget()));
}

/*friend*/ void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    for(int i = 0; i < atlasMan.ui->atlasGroups->count(); ++i)
        AtlasGroup_DrawHide(atlasMan.m_pProjOwner, hyApp, *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->widget(i)));
}

/*friend*/ void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    AtlasGroup_DrawUpdate(atlasMan.m_pProjOwner, hyApp, *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget()));
}

void WidgetAtlasManager::Reload()
{
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();//ui->atlasGroups->removeWidget(ui->atlasGroups->currentWidget());

    if(m_MetaDir.exists() == false)
    {
        HyGuiLog("Meta atlas directory is missing!", LOGTYPE_Error);
        return;
    }
    if(m_DataDir.exists() == false)
    {
        HyGuiLog("Data atlas directory is missing!", LOGTYPE_Error);
        return;
    }

    QFileInfoList metaAtlasDirs = m_MetaDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    if(metaAtlasDirs.empty())
    {
        HyGuiLog("Empty atlas directory, creating new empty group", LOGTYPE_Info);
        AddAtlasGroup();
    }
    else
    {
        foreach(QFileInfo dir, metaAtlasDirs)
        {
            if(dir.isDir())
            {
                bool bWorked = false;
                int iId = dir.baseName().toInt(&bWorked);

                if(bWorked && iId >= 0)
                    AddAtlasGroup(iId);
            }
        }
    }
}

void WidgetAtlasManager::AddAtlasGroup(int iId /*= -1*/)
{
    if(iId == -1)
    {
        // Find first available directory name
        iId = 0;
        QFileInfoList atlasDirs = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
        foreach(QFileInfo info, atlasDirs)
        {
            if(info.isDir() && info.baseName().toInt() == iId)
                iId++;
        }
        
        // Make new atlas group in both meta and data directories
        if(false == m_MetaDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new meta-atlas directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new meta-atlas group: " + iId, LOGTYPE_Info)

        if(false == m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new data-atlas directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new data-atlas group: " + iId, LOGTYPE_Info)
    }
    
    QDir newMetaAtlasDir(m_MetaDir);
    newMetaAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId));

    QDir newDataAtlasDir(m_DataDir);
    newDataAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId));

    WidgetAtlasGroup *pNewAtlas = new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this);
    ui->atlasGroups->setCurrentIndex(ui->atlasGroups->addWidget(pNewAtlas));
}

void WidgetAtlasManager::on_atlasGroups_currentChanged(int iIndex)
{
//    if(ui->atlasGroups->currentWidget())
//    {
//        ui->atlasGroups->currentWidget()->metaObject()
//        static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget())->ResizeAtlasListColumns();
//    }
}

void WidgetAtlasManager::on_btnAddGroup_clicked()
{

}
