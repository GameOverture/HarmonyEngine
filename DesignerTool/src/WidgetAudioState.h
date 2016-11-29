/**************************************************************************
 *	WidgetAudioState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETAUDIOSTATE_H
#define WIDGETAUDIOSTATE_H

#include <QWidget>

namespace Ui {
class WidgetAudioState;
}

class WidgetAudio;

class WidgetAudioState : public QWidget
{
    Q_OBJECT
    
    WidgetAudio *                       m_pOwner;
    QString                             m_sName;
    
    int                                 m_iPrevCategoryIndex;
    int                                 m_iPrevPlayTypeIndex;
    
public:
    explicit WidgetAudioState(WidgetAudio *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetAudioState();
    
    QString GetName();
    void SetName(QString sName);
    
    void UpdateActions();
    
private slots:
    
    void on_cmbCategory_currentIndexChanged(int index);
    
    void on_chkLimitInst_clicked();
    
    void on_chkLooping_clicked();
    
    void on_cmbPlayType_currentIndexChanged(int index);
    
private:
    Ui::WidgetAudioState *ui;
};

#endif // WIDGETAUDIOSTATE_H
