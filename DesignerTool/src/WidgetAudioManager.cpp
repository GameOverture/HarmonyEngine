/**************************************************************************
 *	WidgetAudioManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAudioManager.h"
#include "ui_WidgetAudioManager.h"

#include "WidgetAudioBank.h"
#include "DlgInputName.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

WidgetAudioBankModel::WidgetAudioBankModel(QStackedWidget &atlasGroupsRef, QObject *pParent) :  QStringListModel(pParent),
                                                                                                        m_AudioBanksRef(atlasGroupsRef)
{ }

/*virtual*/ QVariant WidgetAudioBankModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
    if(role == Qt::DisplayRole)
    {
        if(m_AudioBanksRef.count() == 0)
            return "";
        else
            return static_cast<WidgetAudioBank *>(m_AudioBanksRef.widget(index.row()))->GetName();
    }
    else
        return QStringListModel::data(index, role);
}

/*virtual*/ int	WidgetAudioBankModel::rowCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
    return m_AudioBanksRef.count();
}


WidgetAudioManager::WidgetAudioManager(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::WidgetAudioManager)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAudioManager::WidgetAudioManager() invalid constructor used", LOGTYPE_Error);
}

WidgetAudioManager::WidgetAudioManager(ItemProject *pProjOwner, QWidget *parent) :  QWidget(parent),
                                                                                    ui(new Ui::WidgetAudioManager),
                                                                                    m_pProjOwner(pProjOwner),
                                                                                    m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAudioBanks) + HyGlobal::ItemExt(ITEM_DirAudioBanks)),
                                                                                    m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAudioBanks) + HyGlobal::ItemExt(ITEM_DirAudioBanks))
{
    ui->setupUi(this);
    while(ui->audioBanks->currentWidget())
        delete ui->audioBanks->currentWidget();
    
    ui->btnAddCategory->setDefaultAction(ui->actionAddCategory);
    ui->btnRenameCategory->setDefaultAction(ui->actionRenameCategory);
    
    ui->actionRemoveCategory->setEnabled(false);
    ui->btnRemoveCategory->setDefaultAction(ui->actionRemoveCategory);
    
    if(m_MetaDir.exists() == false)
    {
        HyGuiLog("Meta audio bank directory is missing, recreating", LOGTYPE_Info);
        m_MetaDir.mkpath(m_MetaDir.absolutePath());
    }
    if(m_DataDir.exists() == false)
    {
        HyGuiLog("Data audio bank directory is missing, recreating", LOGTYPE_Info);
        m_DataDir.mkpath(m_DataDir.absolutePath());
    }
    
    QFileInfoList metaAudioBankDirs = m_MetaDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    if(metaAudioBankDirs.empty())
    {
        HyGuiLog("Empty atlas directory, creating new empty group", LOGTYPE_Info);
        AddAudioBankGroup();
    }
    else
    {
        foreach(QFileInfo dir, metaAudioBankDirs)
        {
            if(dir.isDir())
            {
                bool bWorked = false;
                int iId = dir.baseName().toInt(&bWorked);

                if(bWorked && iId >= 0)
                    AddAudioBankGroup(iId);
            }
        }
    }
    
    QFile audioInfoFile(m_DataDir.absoluteFilePath(HYGUIPATH_DataAudio));
    if(audioInfoFile.exists())
    {
        if(!audioInfoFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYGUIPATH_MetaAtlasSettings, LOGTYPE_Error);

        QJsonDocument audioDoc = QJsonDocument::fromJson(audioInfoFile.readAll());
        QJsonObject audioObj = audioDoc.object();
        
        // TODO: Read audio info file
        audioObj["name"].toString();
        
        audioInfoFile.close();
    }
    else
    {
        
    }
//    else if(audioInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//    {
//        QJsonDocument settingsDoc(settingsObj);

//#ifdef HYGUI_UseBinaryMetaFiles
//        qint64 iBytesWritten = settingsFile.write(settingsDoc.toBinaryData());
//#else
//        qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
//#endif
//        if(0 == iBytesWritten || -1 == iBytesWritten)
//        {
//            HyGuiLog("Could not write to atlas settings file: " % settingsFile.errorString(), LOGTYPE_Error);
//        }

//        settingsFile.close();
    
//    }
//    else
//        HyGuiLog("Couldn't open data audio info file for initial writing", LOGTYPE_Error);
    
    
    ui->cmbAudioBanks->setModel(new WidgetAudioBankModel(*ui->audioBanks, this));
}

WidgetAudioManager::~WidgetAudioManager()
{
    while(ui->audioBanks->currentWidget())
        delete ui->audioBanks->currentWidget();
    
    delete ui;
}

void WidgetAudioManager::AddAudioBankGroup(int iId /*= -1*/)
{
    if(iId == -1)
    {
        // Find first available directory name
        iId = 0;
        QFileInfoList audioBankDirs = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
        foreach(QFileInfo info, audioBankDirs)
        {
            if(info.isDir() && info.baseName().toInt() == iId)
                iId++;
        }
        
        // Make new audio bank in both meta and data directories
        if(false == m_MetaDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new meta-audio bank directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new meta-audio bank: " + iId, LOGTYPE_Info)
    }
    
    QDir newMetaAudioBankDir(m_MetaDir);
    if(newMetaAudioBankDir.cd(HyGlobal::MakeFileNameFromCounter(iId)) == false)
    {
        HyGuiLog("Failed to 'cd' into meta-audio bank directory: " + iId, LOGTYPE_Error);
        return;
    }

    bool bAudioBankAlreadyExists = false;
//    for(int i = 0; i < ui->waveBanks->count(); ++i)
//    {
//        if(static_cast<WidgetAudioBank *>(ui->waveBanks->widget(i))->IsMatching(newMetaAudioBankDir, newDataAudioBankDir))
//        {
//            bAudioBankAlreadyExists = true;
//            break;
//        }
//    }
    
    if(bAudioBankAlreadyExists == false)
    {
        WidgetAudioBank *pNewAudioBank = new WidgetAudioBank(newMetaAudioBankDir, m_DataDir, this, this);
        int iTest = ui->audioBanks->count();
        ui->audioBanks->setCurrentIndex(ui->audioBanks->addWidget(pNewAudioBank));
        
        iTest = ui->audioBanks->count();
        
        ui->cmbAudioBanks->setCurrentIndex(ui->audioBanks->currentIndex());
    }
}

void WidgetAudioManager::on_cmbAudioBanks_currentIndexChanged(int index)
{
    // Then set the new atlas group
    ui->audioBanks->setCurrentIndex(index);
}

void WidgetAudioManager::on_actionAddCategory_triggered()
{
    DlgInputName *pDlg = new DlgInputName("New Category Name", "Unnamed", this);
    if(pDlg->exec() == QDialog::Accepted)
    {
        
//        QUndoCommand *pCmd = new ItemFontCmd_RenameState(ui->cmbStates, pDlg->GetName());
//        m_pItemFont->GetUndoStack()->push(pCmd);
    }
    delete pDlg;
}

void WidgetAudioManager::on_actionRenameCategory_triggered()
{
    
}

void WidgetAudioManager::on_actionRemoveCategory_triggered()
{
    
}

void WidgetAudioManager::on_actionAddAudioBank_triggered()
{
    
}

void WidgetAudioManager::on_actionDeleteAudioBank_triggered()
{
    
}
