/**************************************************************************
 *	ItemAudioCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemAudioCmds.h"
#include "HyGlobal.h"

ItemAudioCmd_AddState::ItemAudioCmd_AddState(WidgetAudio *pOwner, QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                         m_pComboBox(pCmb),
                                                                                                                                                         m_pAudioState(new WidgetAudioState(pOwner, stateActionList))
{
    setText("Add Audio State");
}

/*virtual*/ ItemAudioCmd_AddState::~ItemAudioCmd_AddState()
{
}

void ItemAudioCmd_AddState::redo()
{
    int iIndex = m_pComboBox->count();

    QVariant v;
    v.setValue(m_pAudioState);

    m_pComboBox->addItem(QString::number(iIndex) % " - " % m_pAudioState->GetName(), v);
    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);

    m_pComboBox->setCurrentIndex(iIndex);
}

void ItemAudioCmd_AddState::undo()
{
    QVariant v;
    v.setValue(m_pAudioState);

    int iIndex = m_pComboBox->findData(v);
    m_pComboBox->removeItem(iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemAudioCmd_RemoveState::ItemAudioCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                        m_pComboBox(pCmb),
                                                                                                        m_pAudioState(m_pComboBox->currentData().value<WidgetAudioState *>()),
                                                                                                        m_iIndex(m_pComboBox->currentIndex())
{
    setText("Remove Audio State");
}

/*virtual*/ ItemAudioCmd_RemoveState::~ItemAudioCmd_RemoveState()
{
}

void ItemAudioCmd_RemoveState::redo()
{
    m_pComboBox->removeItem(m_iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

void ItemAudioCmd_RemoveState::undo()
{
    QVariant v;
    v.setValue(m_pAudioState);

    m_pComboBox->insertItem(m_iIndex, QString::number(m_iIndex) % " - " % m_pAudioState->GetName(), v);
    m_pComboBox->setCurrentIndex(m_iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemAudioCmd_RenameState::ItemAudioCmd_RenameState(QComboBox *pCmb, QString sNewName, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                            m_pComboBox(pCmb),
                                                                                                                            m_pAudioState(m_pComboBox->currentData().value<WidgetAudioState *>()),
                                                                                                                            m_sNewName(sNewName),
                                                                                                                            m_sOldName(m_pAudioState->GetName())
{
    setText("Rename Audio State");
}

/*virtual*/ ItemAudioCmd_RenameState::~ItemAudioCmd_RenameState()
{
}

void ItemAudioCmd_RenameState::redo()
{
    m_pAudioState->SetName(m_sNewName);
    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);

    for(int i = 0; i < m_pComboBox->count(); ++i)
    {
        if(m_pComboBox->itemData(i).value<WidgetAudioState *>() == m_pAudioState)
        {
            m_pComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void ItemAudioCmd_RenameState::undo()
{
    m_pAudioState->SetName(m_sOldName);
    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);

    for(int i = 0; i < m_pComboBox->count(); ++i)
    {
        if(m_pComboBox->itemData(i).value<WidgetAudioState *>() == m_pAudioState)
        {
            m_pComboBox->setCurrentIndex(i);
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemAudioCmd_MoveStateBack::ItemAudioCmd_MoveStateBack(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                            m_pComboBox(pCmb),
                                                                                                            m_pAudioState(m_pComboBox->currentData().value<WidgetAudioState *>())
{
    setText("Shift Audio State Index <-");
}

/*virtual*/ ItemAudioCmd_MoveStateBack::~ItemAudioCmd_MoveStateBack()
{
}

void ItemAudioCmd_MoveStateBack::redo()
{
    QVariant v;
    v.setValue(m_pAudioState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex -= 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pAudioState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

void ItemAudioCmd_MoveStateBack::undo()
{
    QVariant v;
    v.setValue(m_pAudioState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex += 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pAudioState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemAudioCmd_MoveStateForward::ItemAudioCmd_MoveStateForward(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                    m_pComboBox(pCmb),
                                                                                                                    m_pAudioState(m_pComboBox->currentData().value<WidgetAudioState *>())
{
    setText("Shift Audio State Index ->");
}

/*virtual*/ ItemAudioCmd_MoveStateForward::~ItemAudioCmd_MoveStateForward()
{
}

void ItemAudioCmd_MoveStateForward::redo()
{
    QVariant v;
    v.setValue(m_pAudioState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex += 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pAudioState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

void ItemAudioCmd_MoveStateForward::undo()
{
    QVariant v;
    v.setValue(m_pAudioState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex -= 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pAudioState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetAudioState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

