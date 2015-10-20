#include "WidgetAtlasManager.h"
#include "ui_WidgetAtlasManager.h"
#include "HyGlobal.h"

#include <QJsonDocument>

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

    QFileInfoList metaAtlasDirs = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
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

    ui->atlasGroups->addWidget(new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this));
}

void WidgetAtlasManager::LoadData()
{


//    QFileInfoList atlasGroupsList = metaAtlasDir.entryInfoList(NoFilter, QDir::Name);
//    if(atlasGroupsList.empty())
//    {
//        metaAtlasDir.mkdir("00001");
//        m_AtlasGroups.push_back(new HyGuiAtlasGroup());

//        return;
//    }

//    for(unsigned int i = 0; i < atlasGroupsList.size(); ++i)
//    {
//        if(atlasGroupsList[i].isDir() == false)
//            continue;

//        QFileInfo atlasGrpSettings(atlasGroupsList[i].absolutePath() % HYGUIPATH_DataAtlasFileName);
//        QFile dataFile(atlasGrpSettings.absoluteFilePath());
//        if(dataFile.open(QIODevice::ReadOnly))
//        {
//            QJsonDocument dataJsonDoc = QJsonDocument::fromJson(dataFile.readAll());
//            QJsonObject contents = dataJsonDoc.object();

//            QJsonArray textureArray = contents["textures"].toArray();
//            foreach (const QJsonValue &textureInfo, textureArray)
//            {
//                HyGuiTexture *pNewTexture = new HyGuiTexture(this);

//                QJsonArray srcFramesArray = textureInfo.toObject()["srcFrames"].toArray();
//                foreach(const QJsonValue &frameInfo, srcFramesArray)
//                {
//                    quint32 uiHash = JSONOBJ_TOINT(frameInfo.toObject(), "hash");
//                    foreach(const QFileInfo imgInfo, srcFrameImgList)
//                    {
//                        QString sImgName = imgInfo.baseName();
//                        quint32 uiTestHash = static_cast<quint32>(sImgName.left(sImgName.indexOf(QChar('-'))).toLongLong());
//                        if(uiHash == uiTestHash)
//                        {
//                            int iSplitIndex = sImgName.indexOf(QChar('-'));
//                            sImgName = sImgName.right(sImgName.length() - iSplitIndex - 1); // -1 so we don't include the '-'
//                            pNewTexture->LoadFrame(QImage(imgInfo.absoluteFilePath()), uiHash, sImgName, imgInfo.absoluteFilePath());
//                        }
//                    }

//                }

//                QList<QStringList> unPackedList = pNewTexture->PackFrames();
//                if(unPackedList.empty() == false)
//                    HYLOG("Loading an atlas failed to pack properly", LOGTYPE_Error);

//                m_Textures.append(pNewTexture);
//            }
//        }
//        else
//            HYLOG("Atlas group settings file not found. Generating new one.", LOGTYPE_Info);

//        QFileInfoList atlasGrp = QDir(atlasGroupsList[i].absolutePath())..entryInfoList(NoFilter, QDir::Name);
//    }
}

void WidgetAtlasManager::SaveData()
{
//    QJsonDocument atlasInfoDoc;

//    QJsonArray atlasGroupArray;
//    // Fill out array

//    atlasInfoDoc.setArray(atlasGroupArray);
}
