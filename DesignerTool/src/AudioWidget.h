/**************************************************************************
 *	WidgetAudio.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include "AudioData.h"
#include "AudioWidgetState.h"

#include <QWidget>

namespace Ui {
class AudioWidget;
}

class AudioWidget : public QWidget
{
    Q_OBJECT
    
    AudioData *             m_pItemAudio;
    AudioWidgetState *      m_pCurAudioState;

    QList<QAction *>        m_StateActionsList;
    
public:
    explicit AudioWidget(AudioData *pOwner, QWidget *parent = 0);
    ~AudioWidget();
    
    AudioData *GetData();
    
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
    Ui::AudioWidget *ui;
};

#endif // AUDIOWIDGET_H
