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

#include <QWidget>
#include <QStringListModel>
#include <QStackedWidget>

namespace Ui {
class WidgetAudioManager;
}

class WidgetAudioBankModelView : public QStringListModel
{
    QStackedWidget &        m_AudioBanksRef;

public:
    WidgetAudioBankModelView(QStackedWidget &audioBanksRef, QObject *pParent);

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetAudioManager : public QWidget
{
    Q_OBJECT
    
    ItemProject *                   m_pProjOwner;
    
    QDir                            m_MetaDir;
    QDir                            m_DataDir;

public:
    explicit WidgetAudioManager(QWidget *parent = 0);
    explicit WidgetAudioManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAudioManager();

private slots:
    void on_cmbAudioBanks_currentIndexChanged(int index);
    
    void on_actionAddCategory_triggered();
    
    void on_actionRenameCategory_triggered();
    
    void on_actionRemoveCategory_triggered();
    
    void on_actionAddAudioBank_triggered();
    
    void on_actionDeleteAudioBank_triggered();
    
private:
    Ui::WidgetAudioManager *ui;
    
    void AddAudioBankGroup(int iId = -1);
};

#endif // WIDGETAUDIOMANAGER_H
