/**************************************************************************
 *	ItemSpriteCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMSPRITECMDS_H
#define ITEMSPRITECMDS_H

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QAction>

class WidgetSpriteState;
class ItemWidget;
class WidgetAtlasManager;
class HyGuiFrame;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_AddState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetSpriteState * m_pSpriteState;
    
public:
    ItemSpriteCmd_AddState(QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_AddState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_RemoveState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetSpriteState * m_pSpriteState;
    int                 m_iIndex;
    
public:
    ItemSpriteCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_RemoveState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_RenameState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetSpriteState * m_pSpriteState;
    QString             m_sNewName;
    QString             m_sOldName;
    
public:
    ItemSpriteCmd_RenameState(QComboBox *pCmb, QString sNewName, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_RenameState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_MoveStateBack : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetSpriteState * m_pSpriteState;
    
public:
    ItemSpriteCmd_MoveStateBack(QComboBox *pCmb, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveStateBack();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_MoveStateForward : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetSpriteState * m_pSpriteState;
    
public:
    ItemSpriteCmd_MoveStateForward(QComboBox *pCmb, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveStateForward();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_ToggleStateReverse : public QUndoCommand
{
    QCheckBox *         m_pCheckBox;// <-- probably want to have each SpriteState hold its own controls.
    WidgetSpriteState * m_pSpriteState;
    
public:
    ItemSpriteCmd_ToggleStateReverse(QCheckBox *pCmb, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_ToggleStateReverse();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_AddFrames : public QUndoCommand
{
    ItemWidget *                            m_pItem;
    WidgetAtlasManager *                    m_pAtlasMan;
    QList<QPair<HyGuiFrame *, QVariant> >   m_Frames;
    
public:
    ItemSpriteCmd_AddFrames(ItemWidget *pItem, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_AddFrames();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_DeleteFrame : public QUndoCommand
{
    ItemWidget *                            m_pItem;
    QList<QPair<HyGuiFrame *, QVariant> >   m_Frames;

public:
    ItemSpriteCmd_DeleteFrame(ItemWidget *pItem, HyGuiFrame *pFrame, int iRowParam, QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_DeleteFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_MoveFrameUp : public QUndoCommand
{
public:
    ItemSpriteCmd_MoveFrameUp(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveFrameUp();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemSpriteCmd_MoveFrameDown : public QUndoCommand
{
public:
    ItemSpriteCmd_MoveFrameDown(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveFrameDown();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

#endif // ITEMSPRITECMDS_H
