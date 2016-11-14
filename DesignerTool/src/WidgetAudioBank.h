/**************************************************************************
 *	WidgetAudioBank.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETAUDIOBANK_H
#define WIDGETAUDIOBANK_H

#include "HyGlobal.h"

#include "WidgetAudioModelView.h"

#include <QWidget>

namespace Ui {
class WidgetAudioBank;
}

class WidgetAudioManager;

class WidgetAudioBank : public QWidget
{
    Q_OBJECT
    
    // NOTE: Order of these member variables matter here for the member initializer list
    WidgetAudioManager *        m_pManager;
    
    QDir                        m_MetaDir;
    QDir                        m_DataDir;
    
    QString                     m_sName;
    
    WidgetAudioBankModel *      m_pModel;

public:
    explicit WidgetAudioBank(QWidget *parent = 0);
    explicit WidgetAudioBank(QDir metaDir, QDir dataDir, WidgetAudioManager *pManager, QWidget *pParent = 0);
    ~WidgetAudioBank();
    
    QString GetName();
    int GetId();

private slots:
    void on_btnAddWaves_pressed();

    void on_btnAddDir_pressed();

private:
    Ui::WidgetAudioBank *ui;

    void ImportWaves(QStringList sWaveFileList);

    void Refresh();
};

#endif // WIDGETAUDIOBANK_H
