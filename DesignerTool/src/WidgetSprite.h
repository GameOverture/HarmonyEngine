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
    QString                     m_sName;
    QList<QTableWidgetItem *>   m_FrameList;
    
public:
    SpriteState();
    ~SpriteState();
    
    QString GetName();
    void SetName(QString sNewName);
};
Q_DECLARE_METATYPE(SpriteState *)

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
    ItemSprite *            m_pItemSprite;
    QUndoStack *            m_pUndoStack;

    QList<QUndoCommand *>   m_CmdsNotInUndoStack;

public:
    explicit WidgetSprite(ItemSprite *pItemSprite, QWidget *parent = 0);
    ~WidgetSprite();
    
private slots:
    void on_actionAddState_triggered();
    
    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();
    
    void on_actionAddFrames_triggered();
    
    void on_actionRemoveFrame_triggered();
    
    void on_actionOrderFrameUpwards_triggered();
    
    void on_actionOrderFrameDownwards_triggered();
    
private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
