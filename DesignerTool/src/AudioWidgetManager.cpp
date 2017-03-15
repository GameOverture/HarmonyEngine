/**************************************************************************
 *	WidgetAudioManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioWidgetManager.h"
#include "ui_AudioWidgetManager.h"

#include "AudioWidgetBank.h"
#include "DlgInputName.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>

AudioWidgetManager::AudioWidgetManager(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::AudioWidgetManager)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAudioManager::WidgetAudioManager() invalid constructor used", LOGTYPE_Error);
}

AudioWidgetManager::AudioWidgetManager(Project *pProjOwner, QWidget *parent) :  QWidget(parent),
                                                                                    ui(new Ui::AudioWidgetManager),
                                                                                    m_pProjOwner(pProjOwner),
                                                                                    m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAudioBanks) + HyGlobal::ItemExt(ITEM_DirAudioBanks)),
                                                                                    m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAudioBanks) + HyGlobal::ItemExt(ITEM_DirAudioBanks))
{
    ui->setupUi(this);
    while(ui->audioBanks->currentWidget())
        delete ui->audioBanks->currentWidget();
    
    ui->btnAddCategory->setDefaultAction(ui->actionAddCategory);

    ui->actionRemoveCategory->setEnabled(false);
    ui->btnRemoveCategory->setDefaultAction(ui->actionRemoveCategory);

    ui->btnAddAudioBank->setDefaultAction(ui->actionAddAudioBank);
    ui->btnDeleteAudioBank->setDefaultAction(ui->actionDeleteAudioBank);
    
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
        for(int i = 0; i < metaAudioBankDirs.size(); ++i)
        {
            if(metaAudioBankDirs[i].isDir())
            {
                bool bWorked = false;
                int iId = metaAudioBankDirs[i].baseName().toInt(&bWorked);

                if(bWorked && iId >= 0)
                    AddAudioBankGroup(iId);
            }
        }
    }

    m_pCategoryDelegate = new AudioCategoryDelegate(this);
    ui->categoryList->setItemDelegate(m_pCategoryDelegate);

    m_pCategoryModel = new AudioCategoryStringListModel(m_DataDir, this);
    ui->categoryList->setModel(m_pCategoryModel);

    QItemSelectionModel *pSelModel = ui->categoryList->selectionModel();
    connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_categoryList_selectionChanged(const QItemSelection &, const QItemSelection &)));

    ui->cmbAudioBanks->setModel(new AudioManagerStringListModel(*ui->audioBanks, this));
}

AudioWidgetManager::~AudioWidgetManager()
{
    while(ui->audioBanks->currentWidget())
        delete ui->audioBanks->currentWidget();
    
    delete ui;
}

Project *AudioWidgetManager::GetItemProject()
{
    return m_pProjOwner;
}

AudioWave *AudioWidgetManager::CreateWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint32 uiErrors)
{
    AudioWave *pNewWave = NULL;

    if(m_DependencyMap.contains(uiChecksum))
    {
        HyGuiLog("WidgetAtlasManager::CreateWave() already contains wave with this checksum: " % QString::number(uiChecksum), LOGTYPE_Error);

        pNewWave = new AudioWave(uiWaveBankId, uiChecksum, sName, uiFormatType, uiNumChannels, uiBitsPerSample, uiSamplesPerSec, uiErrors);
        pNewWave->SetError(GUIFRAMEERROR_Duplicate);
    }
    else
    {
        pNewWave = new AudioWave(uiWaveBankId, uiChecksum, sName, uiFormatType, uiNumChannels, uiBitsPerSample, uiSamplesPerSec, uiErrors);
        m_DependencyMap[uiChecksum] = pNewWave;
    }

    return pNewWave;
}

AudioCategoryStringListModel *AudioWidgetManager::GetCategoryModel()
{
    return m_pCategoryModel;
}

void AudioWidgetManager::AddAudioBankGroup(int iId /*= -1*/)
{
    if(iId == -1)
    {
        // Find first available directory name
        iId = 0;
        QFileInfoList audioBankDirs = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
        for(int i = 0; i < audioBankDirs.size(); ++i)
        {
            if(audioBankDirs[i].isDir() && audioBankDirs[i].baseName().toInt() == iId)
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

    AudioWidgetBank *pNewAudioBank = new AudioWidgetBank(newMetaAudioBankDir, m_DataDir, this, this);
    ui->audioBanks->setCurrentIndex(ui->audioBanks->addWidget(pNewAudioBank));
    ui->cmbAudioBanks->setCurrentIndex(ui->audioBanks->currentIndex());
}

void AudioWidgetManager::on_cmbAudioBanks_currentIndexChanged(int index)
{
    // Then set the new atlas group
    ui->audioBanks->setCurrentIndex(index);
}

void AudioWidgetManager::on_actionAddCategory_triggered()
{
    DlgInputName *pDlg = new DlgInputName("New Category Name", "Unnamed", this);
    if(pDlg->exec() == QDialog::Accepted)
    {
        QStringList sCategoryList = m_pCategoryModel->stringList();
        sCategoryList.append(pDlg->GetName());

        m_pCategoryModel->setStringList(sCategoryList);
    }

    delete pDlg;
}

void AudioWidgetManager::on_actionRemoveCategory_triggered()
{
    m_pCategoryModel->removeRow(ui->categoryList->currentIndex().row());
}

void AudioWidgetManager::on_actionAddAudioBank_triggered()
{
    
}

void AudioWidgetManager::on_actionDeleteAudioBank_triggered()
{
    
}

void AudioWidgetManager::on_categoryList_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    ui->actionRemoveCategory->setEnabled(ui->categoryList->currentIndex().row() != 0);
}
