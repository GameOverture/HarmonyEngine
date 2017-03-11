/**************************************************************************
 *	WidgetAudio.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETAUDIO_H
#define WIDGETAUDIO_H

#include "ItemAudio.h"
#include "WidgetAudioState.h"

#include <QWidget>

namespace Ui {
class WidgetAudio;
}

class WidgetAudio : public QWidget
{
    Q_OBJECT
    
    ItemAudio *             m_pItemAudio;
    WidgetAudioState *      m_pCurAudioState;

    QList<QAction *>        m_StateActionsList;
    
public:
    explicit WidgetAudio(ItemAudio *pOwner, QWidget *parent = 0);
    ~WidgetAudio();
    
    ItemAudio *GetData();
    
    QComboBox *GetCmbStates();
    
    void UpdateActions();
    
private Q_SLOTS:
    void on_actionAddState_triggered();

    void on_actionRemoveState_triggered();

    void on_actionRenameState_triggered();

    void on_actionOrderStateBackwards_triggered();

    void on_actionOrderStateForwards_triggered();

    void on_cmbStates_currentIndexChanged(int index);

private:
    Ui::WidgetAudio *ui;
};

#endif // WIDGETAUDIO_H
