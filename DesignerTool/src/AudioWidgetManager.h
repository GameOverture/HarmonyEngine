/**************************************************************************
 *	WidgetAudioManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGETMANAGER_H
#define AUDIOWIDGETMANAGER_H

#include "Project.h"
#include "AudioWidgetBank.h"
#include "AudioModelView.h"

#include <QWidget>
#include <QMap>

namespace Ui {
class WidgetAudioManager;
}

class AudioWidgetManager : public QWidget
{
    Q_OBJECT
    
    Project *                   m_pProjOwner;
    
    QDir                            m_MetaDir;
    QDir                            m_DataDir;

    QMap<quint32, AudioWave *>      m_DependencyMap;

    AudioManagerStringListModel *       m_pBankModel;
    AudioCategoryStringListModel *      m_pCategoryModel;
    AudioCategoryDelegate *   m_pCategoryDelegate;

public:
    explicit AudioWidgetManager(QWidget *parent = 0);
    explicit AudioWidgetManager(Project *pProjOwner, QWidget *parent = 0);
    ~AudioWidgetManager();

    Project *GetItemProject();
    
    AudioWave *CreateWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint32 uiErrors);
    
    AudioCategoryStringListModel *GetCategoryModel();

private Q_SLOTS:
    void on_cmbAudioBanks_currentIndexChanged(int index);
    
    void on_actionAddCategory_triggered();
    
    void on_actionRemoveCategory_triggered();
    
    void on_actionAddAudioBank_triggered();
    
    void on_actionDeleteAudioBank_triggered();

    void on_categoryList_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    
private:
    Ui::WidgetAudioManager *ui;
    
    void AddAudioBankGroup(int iId = -1);
};

#endif // AUDIOWIDGETMANAGER_H
