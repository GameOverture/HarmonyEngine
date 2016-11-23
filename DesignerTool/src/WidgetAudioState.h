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
    
public:
    explicit WidgetAudioState(WidgetAudio *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetAudioState();
    
    QString GetName();
    void SetName(QString sName);
    
private:
    Ui::WidgetAudioState *ui;
};

#endif // WIDGETAUDIOSTATE_H
