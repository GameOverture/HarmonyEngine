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

#include <QWidget>

namespace Ui {
class WidgetAudio;
}

class WidgetAudio : public QWidget
{
    Q_OBJECT
    
    ItemAudio *     m_pItemAudio;
    
public:
    explicit WidgetAudio(ItemAudio *pOwner, QWidget *parent = 0);
    ~WidgetAudio();
    
private:
    Ui::WidgetAudio *ui;
};

#endif // WIDGETAUDIO_H
