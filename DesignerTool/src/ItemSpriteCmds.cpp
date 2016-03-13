/**************************************************************************
 *	ItemSpriteCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemSpriteCmds.h"

ItemSpriteCmd_NewState::ItemSpriteCmd_NewState(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
    setText("Add a new state");
}

ItemSpriteCmd_NewState::~ItemSpriteCmd_NewState()
{
}

void ItemSpriteCmd_NewState::undo()
{
}

void ItemSpriteCmd_NewState::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_DeleteState::ItemSpriteCmd_DeleteState(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_DeleteState::~ItemSpriteCmd_DeleteState()
{
}
void ItemSpriteCmd_DeleteState::undo()
{
}
void ItemSpriteCmd_DeleteState::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_RenameState::ItemSpriteCmd_RenameState(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_RenameState::~ItemSpriteCmd_RenameState()
{
}
void ItemSpriteCmd_RenameState::undo()
{
}
void ItemSpriteCmd_RenameState::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_MoveStateUp::ItemSpriteCmd_MoveStateUp(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_MoveStateUp::~ItemSpriteCmd_MoveStateUp()
{
}
void ItemSpriteCmd_MoveStateUp::undo()
{
}
void ItemSpriteCmd_MoveStateUp::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_MoveStateDown::ItemSpriteCmd_MoveStateDown(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_MoveStateDown::~ItemSpriteCmd_MoveStateDown()
{
}
void ItemSpriteCmd_MoveStateDown::undo()
{
}
void ItemSpriteCmd_MoveStateDown::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_Import::ItemSpriteCmd_Import(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_Import::~ItemSpriteCmd_Import()
{
}
void ItemSpriteCmd_Import::undo()
{
}
void ItemSpriteCmd_Import::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_DeleteFrame::ItemSpriteCmd_DeleteFrame(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_DeleteFrame::~ItemSpriteCmd_DeleteFrame()
{
}
void ItemSpriteCmd_DeleteFrame::undo()
{
}
void ItemSpriteCmd_DeleteFrame::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_MoveFrameUp::ItemSpriteCmd_MoveFrameUp(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_MoveFrameUp::~ItemSpriteCmd_MoveFrameUp()
{
}
void ItemSpriteCmd_MoveFrameUp::undo()
{
}
void ItemSpriteCmd_MoveFrameUp::redo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_MoveFrameDown::ItemSpriteCmd_MoveFrameDown(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_MoveFrameDown::~ItemSpriteCmd_MoveFrameDown()
{
}
void ItemSpriteCmd_MoveFrameDown::undo()
{
}
void ItemSpriteCmd_MoveFrameDown::redo()
{
}
