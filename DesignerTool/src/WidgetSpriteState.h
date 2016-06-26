/**************************************************************************
 *	WidgetSpriteState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITESTATE_H
#define WIDGETSPRITESTATE_H

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class WidgetSpriteState;
}

class WidgetSpriteState : public QWidget
{
    Q_OBJECT

    QString                     m_sName;
    QList<QTableWidgetItem *>   m_FrameList;

public:
    explicit WidgetSpriteState(QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetSpriteState();

    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(HyGuiFrame *pFrame);
    void RemoveFrame(HyGuiFrame *pFrame);
    
private:
    Ui::WidgetSpriteState *ui;
};
Q_DECLARE_METATYPE(WidgetSpriteState *)

#endif // WIDGETSPRITESTATE_H
