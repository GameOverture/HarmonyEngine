/**************************************************************************
 *	WidgetAudioManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETAUDIOMANAGER_H
#define WIDGETAUDIOMANAGER_H

#include "ItemProject.h"
#include "WidgetAudioBank.h"
#include "WidgetAudioModelView.h"

#include <QWidget>

namespace Ui {
class WidgetAudioManager;
}

class WidgetAudioManager : public QWidget
{
    Q_OBJECT
    
    ItemProject *                   m_pProjOwner;

    WidgetAudioBankModel *          m_pBankModel;
    WidgetAudioCategoryModel *      m_pCategoryModel;
    WidgetAudioCategoryDelegate *   m_pCategoryDelegate;
    
    QDir                            m_MetaDir;
    QDir                            m_DataDir;

public:
    explicit WidgetAudioManager(QWidget *parent = 0);
    explicit WidgetAudioManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAudioManager();
    
    HyGuiWave *CreateWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint32 uiErrors);

private slots:
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

#endif // WIDGETAUDIOMANAGER_H
