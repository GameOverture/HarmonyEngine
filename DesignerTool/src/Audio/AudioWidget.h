/**************************************************************************
 *	AudioWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include "AudioWidgetState.h"
#include "ProjectItem.h"

#include <QWidget>

namespace Ui {
class AudioWidget;
}

class AudioWidget : public QWidget
{
    Q_OBJECT
    
    ProjectItem &           m_ItemRef;
    AudioWidgetState *      m_pCurAudioState;

    QList<QAction *>        m_StateActionsList;
    
public:
    explicit AudioWidget(ProjectItem &itemRef, QWidget *parent = 0);
    ~AudioWidget();
    
    ProjectItem &GetItem();
    
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
