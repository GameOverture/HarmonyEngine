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
#include "WidgetSprite.h"

void EnsureProperNamingInComboBox(QComboBox *pCmb)
{
    // Ensure that all the entry names in the combobox match their index
    for(int i = 0; i < pCmb->count(); ++i)
    {
        QString sName(QString::number(i) % " - " % pCmb->itemData(i).value<WidgetSpriteState *>()->GetName());
        pCmb->setItemText(i, sName);
    }
}

ItemSpriteCmd_AddState::ItemSpriteCmd_AddState(QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                    m_pComboBox(pCmb),
                                                                                                                                    m_pSpriteState(new WidgetSpriteState(stateActionList))
{
    setText("Add Sprite State");
}

ItemSpriteCmd_AddState::~ItemSpriteCmd_AddState()
{
}

void ItemSpriteCmd_AddState::redo()
{
    int iIndex = m_pComboBox->count();
    
    QVariant v;
    v.setValue(m_pSpriteState);
    
    m_pComboBox->addItem(QString::number(iIndex) % " - " % m_pSpriteState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);
}

void ItemSpriteCmd_AddState::undo()
{
    QVariant v;
    v.setValue(m_pSpriteState);
    
    int iIndex = m_pComboBox->findData(v);
    m_pComboBox->removeItem(iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_RemoveState::ItemSpriteCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                        m_pComboBox(pCmb),
                                                                                                        m_pSpriteState(m_pComboBox->currentData().value<WidgetSpriteState *>()),
                                                                                                        m_iIndex(m_pComboBox->currentIndex())
{
    setText("Remove Sprite State");
}

ItemSpriteCmd_RemoveState::~ItemSpriteCmd_RemoveState()
{
}

void ItemSpriteCmd_RemoveState::redo()
{
    m_pComboBox->removeItem(m_iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

void ItemSpriteCmd_RemoveState::undo()
{
    QVariant v;
    v.setValue(m_pSpriteState);
    
    m_pComboBox->insertItem(m_iIndex, QString::number(m_iIndex) % " - " % m_pSpriteState->GetName(), v);
    m_pComboBox->setCurrentIndex(m_iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_RenameState::ItemSpriteCmd_RenameState(QComboBox *pCmb, QString sNewName, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                            m_pComboBox(pCmb),
                                                                                                                            m_pSpriteState(m_pComboBox->currentData().value<WidgetSpriteState *>()),
                                                                                                                            m_sNewName(sNewName),
                                                                                                                            m_sOldName(m_pSpriteState->GetName())
{
    setText("Rename Sprite State");
}

ItemSpriteCmd_RenameState::~ItemSpriteCmd_RenameState()
{
}

void ItemSpriteCmd_RenameState::redo()
{
    m_pSpriteState->SetName(m_sNewName);
    EnsureProperNamingInComboBox(m_pComboBox);
}

void ItemSpriteCmd_RenameState::undo()
{
    m_pSpriteState->SetName(m_sOldName);
    EnsureProperNamingInComboBox(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_MoveStateBack::ItemSpriteCmd_MoveStateBack(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                            m_pComboBox(pCmb),
                                                                                                            m_pSpriteState(m_pComboBox->currentData().value<WidgetSpriteState *>())
{
    setText("Shift State Index <-");
}

ItemSpriteCmd_MoveStateBack::~ItemSpriteCmd_MoveStateBack()
{
}

void ItemSpriteCmd_MoveStateBack::redo()
{
    QVariant v;
    v.setValue(m_pSpriteState);
    
    int iIndex = m_pComboBox->findData(v);
    
    m_pComboBox->removeItem(iIndex);
    iIndex -= 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pSpriteState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

void ItemSpriteCmd_MoveStateBack::undo()
{
    QVariant v;
    v.setValue(m_pSpriteState);
    
    int iIndex = m_pComboBox->findData(v);
    
    m_pComboBox->removeItem(iIndex);
    iIndex += 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pSpriteState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_MoveStateForward::ItemSpriteCmd_MoveStateForward(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                    m_pComboBox(pCmb),
                                                                                                                    m_pSpriteState(m_pComboBox->currentData().value<WidgetSpriteState *>())
{
    setText("Shift State Index ->");
}

ItemSpriteCmd_MoveStateForward::~ItemSpriteCmd_MoveStateForward()
{
}

void ItemSpriteCmd_MoveStateForward::redo()
{
    QVariant v;
    v.setValue(m_pSpriteState);
    
    int iIndex = m_pComboBox->findData(v);
    
    m_pComboBox->removeItem(iIndex);
    iIndex += 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pSpriteState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

void ItemSpriteCmd_MoveStateForward::undo()
{
    QVariant v;
    v.setValue(m_pSpriteState);
    
    int iIndex = m_pComboBox->findData(v);
    
    m_pComboBox->removeItem(iIndex);
    iIndex -= 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pSpriteState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);
    
    EnsureProperNamingInComboBox(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_AddFrames::ItemSpriteCmd_AddFrames(QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent)
{
}
ItemSpriteCmd_AddFrames::~ItemSpriteCmd_AddFrames()
{
}
void ItemSpriteCmd_AddFrames::undo()
{
}
void ItemSpriteCmd_AddFrames::redo()
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
