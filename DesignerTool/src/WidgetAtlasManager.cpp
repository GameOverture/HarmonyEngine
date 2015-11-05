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
    HYLOG("WidgetAtlasManager::WidgetAtlasManager() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasManager::WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                             ui(new Ui::WidgetAtlasManager),
                                                                                             m_pProjOwner(pProjOwner),
                                                                                             m_MetaDir(m_pProjOwner->GetPath(HYGUIPATH_RelMetaAtlasDir)),
                                                                                             m_DataDir(m_pProjOwner->GetPath(HYGUIPATH_RelDataAtlasDir))
{
    ui->setupUi(this);
    while(ui->atlasGroups->currentWidget())
        ui->atlasGroups->removeWidget(ui->atlasGroups->currentWidget());

    if(m_MetaDir.exists() == false)
    {
        HYLOG("Meta atlas directory is missing!", LOGTYPE_Error);
        return;
    }
    if(m_DataDir.exists() == false)
    {
        HYLOG("Data atlas directory is missing!", LOGTYPE_Error);
        return;
    }

    QFileInfoList metaAtlasDirs = m_MetaDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    if(metaAtlasDirs.empty())
    {
        HYLOG("Empty atlas directory, creating new empty group", LOGTYPE_Info);
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

//    // Find atlasInfo.json file
//    for(unsigned int i = 0; i < atlasDirs.size(); ++i)
//    {
//        if(atlasFiles[i].isFile())
//        {
//            if(atlasFiles[i].fileName().toLower() == QString(HYGUIPATH_DataAtlasFileName).toLower())
//            {
//                QFile dataFile(atlasGrpSettings.absoluteFilePath());
//                QJsonDocument atlasInfoDoc = QJsonDocument::fromJson(dataFile.readAll());

//                QJsonArray atlasGrpArray = atlasInfoDoc.array();
//                foreach(const QJsonValue atlasGrp, atlasGrpArray)
//                {
//                    atlasGrp.toObject();
//                }
//            }
//        }
//    }
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
       HYLOG("Couldn't open atlas data info file for writing", LOGTYPE_Error);
    }
    else
    {
        qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HYLOG("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);
        }

        atlasInfoFile.close();
    }
}

void WidgetAtlasManager::PreviewAtlasGroup(bool bForceLoad)
{
    m_pProjOwner->SetAtlasGroupDrawState(ui->atlasGroups->currentIndex(), bForceLoad);
    MainWindow::OpenItem(m_pProjOwner);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    MainWindow::CloseItem(m_pProjOwner);
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
            HYLOG("Failed to create new meta-atlas directory", LOGTYPE_Error)
        else
            HYLOG("Created new meta-atlas group: " + iId, LOGTYPE_Info)

        if(false == m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HYLOG("Failed to create new data-atlas directory", LOGTYPE_Error)
        else
            HYLOG("Created new data-atlas group: " + iId, LOGTYPE_Info)
    }
    
    QDir newMetaAtlasDir(m_MetaDir);
    newMetaAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId));

    QDir newDataAtlasDir(m_DataDir);
    newDataAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId));

    ui->atlasGroups->setCurrentIndex(ui->atlasGroups->addWidget(new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this)));
}
