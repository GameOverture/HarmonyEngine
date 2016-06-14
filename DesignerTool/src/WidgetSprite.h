/**************************************************************************
 *	WidgetSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITE_H
#define WIDGETSPRITE_H

#include <QWidget>
#include <QMenu>
#include <QUndoGroup>
#include <QUndoStack>
#include <QComboBox>
#include <QTableWidgetItem>

namespace Ui {
class WidgetSprite;
}

class ItemSprite;

class SpriteState
{
    int                         m_iIndex;
    QString                     m_sName;
    QUndoStack *                m_pUndoStack;

    QList<QTableWidgetItem *>   m_FrameList;
};
Q_DECLARE_METATYPE(SpriteState *)

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
    ItemSprite *            m_pItemSprite;
    QUndoGroup *            m_pUndoGroup;void on_actionAddState_triggered();


    QList<SpriteState *>    m_States;

public:
    explicit WidgetSprite(ItemSprite *pItemSprite, QWidget *parent = 0);
    ~WidgetSprite();
    
private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
