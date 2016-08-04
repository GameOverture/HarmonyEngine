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

#include "HyGuiFrame.h"

#include <QWidget>
#include <QTableWidgetItem>

class ItemSprite;
struct SpriteFrame;

namespace Ui {
class WidgetSpriteState;
}

class WidgetSpriteState : public QWidget
{
    Q_OBJECT

    QString                             m_sName;
    QMap<quint32, SpriteFrame *>        m_RemovedFrameMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes

public:
    explicit WidgetSpriteState(QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetSpriteState();
    
    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(HyGuiFrame *pFrame);
    void RemoveFrame(HyGuiFrame *pFrame);

    SpriteFrame *SelectedFrame();
    int SelectedIndex();
    
    void AppendFramesToListRef(QList<HyGuiFrame *> &drawInstListRef);
    
    void GetStateFrameInfo(QJsonArray &stateArrayOut);
    
private:
    Ui::WidgetSpriteState *ui;
};
Q_DECLARE_METATYPE(WidgetSpriteState *)

#endif // WIDGETSPRITESTATE_H
