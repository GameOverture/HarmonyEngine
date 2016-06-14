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

class ItemSpriteCmd_AddState : public QUndoCommand
{
public:
    ItemSpriteCmd_AddState(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_AddState();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_RemoveState : public QUndoCommand
{
public:
    ItemSpriteCmd_RemoveState(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_RemoveState();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_RenameState : public QUndoCommand
{
public:
    ItemSpriteCmd_RenameState(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_RenameState();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_MoveStateUp : public QUndoCommand
{
public:
    ItemSpriteCmd_MoveStateUp(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveStateUp();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_MoveStateDown : public QUndoCommand
{
public:
    ItemSpriteCmd_MoveStateDown(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveStateDown();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_AddFrames : public QUndoCommand
{
public:
    ItemSpriteCmd_AddFrames(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_AddFrames();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_DeleteFrame : public QUndoCommand
{
public:
    ItemSpriteCmd_DeleteFrame(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_DeleteFrame();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_MoveFrameUp : public QUndoCommand
{
public:
    ItemSpriteCmd_MoveFrameUp(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveFrameUp();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

class ItemSpriteCmd_MoveFrameDown : public QUndoCommand
{
public:
    ItemSpriteCmd_MoveFrameDown(QUndoCommand *pParent = 0);
    ~ItemSpriteCmd_MoveFrameDown();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
};

#endif // ITEMSPRITECMDS_H
