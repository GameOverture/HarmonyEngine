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
#include "ItemSprite.h"
#include "WidgetSprite.h"
#include "WidgetAtlasManager.h"

void EnsureProperNamingInComboBox(QComboBox *pCmb)
{
    // Ensure that all the entry names in the combobox match their index
    for(int i = 0; i < pCmb->count(); ++i)
    {
        QString sName(QString::number(i) % " - " % pCmb->itemData(i).value<WidgetSpriteState *>()->GetName());
        pCmb->setItemText(i, sName);
    }
}

ItemSpriteCmd_AddState::ItemSpriteCmd_AddState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                            m_pComboBox(pCmb),
                                                                                                                                                            m_pSpriteState(new WidgetSpriteState(pOwner, stateActionList))
{
    setText("Add Sprite State");
}

/*virtual*/ ItemSpriteCmd_AddState::~ItemSpriteCmd_AddState()
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

/*virtual*/ ItemSpriteCmd_RemoveState::~ItemSpriteCmd_RemoveState()
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

/*virtual*/ ItemSpriteCmd_RenameState::~ItemSpriteCmd_RenameState()
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

/*virtual*/ ItemSpriteCmd_MoveStateBack::~ItemSpriteCmd_MoveStateBack()
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

/*virtual*/ ItemSpriteCmd_MoveStateForward::~ItemSpriteCmd_MoveStateForward()
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
ItemSpriteCmd_AddFrames::ItemSpriteCmd_AddFrames(ItemWidget *pItem, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                        m_pItem(pItem)
{
    setText("Import Frames");
    m_Frames.clear();
}

/*virtual*/ ItemSpriteCmd_AddFrames::~ItemSpriteCmd_AddFrames()
{
}

void ItemSpriteCmd_AddFrames::redo()
{
    m_Frames = m_pItem->GetAtlasManager().RequestFrames(m_pItem, m_Frames);
}

void ItemSpriteCmd_AddFrames::undo()
{
    m_pItem->GetAtlasManager().RelinquishFrames(m_pItem, m_Frames);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_DeleteFrame::ItemSpriteCmd_DeleteFrame(ItemWidget *pItem, HyGuiFrame *pFrame, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                m_pItem(pItem)
{
    setText("Remove Frame");
    m_Frames.append(pFrame);
}

/*virtual*/ ItemSpriteCmd_DeleteFrame::~ItemSpriteCmd_DeleteFrame()
{
}

void ItemSpriteCmd_DeleteFrame::redo()
{
    m_pItem->GetAtlasManager().RelinquishFrames(m_pItem, m_Frames);
}

void ItemSpriteCmd_DeleteFrame::undo()
{
    m_Frames = m_pItem->GetAtlasManager().RequestFrames(m_pItem, m_Frames);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_OrderFrame::ItemSpriteCmd_OrderFrame(WidgetSpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                                                m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                                m_iFrameIndex(iFrameIndex),
                                                                                                                                                                                m_iFrameIndexDest(iFrameIndexDestination)
{
    if(m_iFrameIndex > m_iFrameIndexDest)
        setText("Order Frame Upwards");
    else
        setText("Order Frame Downwards");
}

/*virtual*/ ItemSpriteCmd_OrderFrame::~ItemSpriteCmd_OrderFrame()
{
}

void ItemSpriteCmd_OrderFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());
    
    int iOffset = m_iFrameIndexDest - m_iFrameIndex;
    while(iOffset > 0)
    {
        pSpriteFramesModel->MoveRowDown(m_iFrameIndex);
        iOffset--;
    }
    
    while(iOffset < 0)
    {
        pSpriteFramesModel->MoveRowUp(m_iFrameIndex);
        iOffset++;
    }
    
    m_pSpriteTableView->selectRow(m_iFrameIndexDest);
}

void ItemSpriteCmd_OrderFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());
    
    int iOffset = m_iFrameIndex - m_iFrameIndexDest;
    while(iOffset > 0)
    {
        pSpriteFramesModel->MoveRowDown(m_iFrameIndexDest);
        iOffset--;
    }
    
    while(iOffset < 0)
    {
        pSpriteFramesModel->MoveRowUp(m_iFrameIndexDest);
        iOffset++;
    }
    
    m_pSpriteTableView->selectRow(m_iFrameIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_OffsetFrame::ItemSpriteCmd_OffsetFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QPoint vOffset, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                            m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                            m_iFrameIndex(iIndex),
                                                                                                                                                            m_vNewOffset(vOffset)
{
    setText("Translate Frame Offset");
    
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_vOffset);
    }
    else
        m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset);
}

/*virtual*/ ItemSpriteCmd_OffsetFrame::~ItemSpriteCmd_OffsetFrame()
{
}

void ItemSpriteCmd_OffsetFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->OffsetFrame(i, m_vNewOffset);
    }
    else
    {
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, m_vNewOffset);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

void ItemSpriteCmd_OffsetFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->OffsetFrame(i, m_OriginalOffsetList[i]);
    }
    else
    {
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, m_OriginalOffsetList[0]);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_DurationFrame::ItemSpriteCmd_DurationFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, float fDuration, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                m_iFrameIndex(iIndex),
                                                                                                                                                                m_fNewDuration(fDuration)
{
    setText("Modify Frame Duration");
    
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            m_OriginalDurationList.append(pSpriteFramesModel->GetFrameAt(i)->m_fDuration);
    }
    else
        m_OriginalDurationList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_fDuration);
}

/*virtual*/ ItemSpriteCmd_DurationFrame::~ItemSpriteCmd_DurationFrame()
{
}

void ItemSpriteCmd_DurationFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->DurationFrame(i, m_fNewDuration);
    }
    else
    {
        pSpriteFramesModel->DurationFrame(m_iFrameIndex, m_fNewDuration);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

void ItemSpriteCmd_DurationFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->DurationFrame(i, m_OriginalDurationList[i]);
    }
    else
    {
        pSpriteFramesModel->DurationFrame(m_iFrameIndex, m_OriginalDurationList[0]);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemSpriteCmd_Checkbox::ItemSpriteCmd_Checkbox(QCheckBox *pCheckBox, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                        m_pCheckBox(pCheckBox)
{
    m_bInitialValue = m_pCheckBox->isChecked();
    setText((m_bInitialValue ? "Checked " : "Unchecked ") % m_pCheckBox->text());
}

/*virtual*/ ItemSpriteCmd_Checkbox::~ItemSpriteCmd_Checkbox()
{
}

void ItemSpriteCmd_Checkbox::redo()
{
    m_pCheckBox->setChecked(m_bInitialValue);
}

void ItemSpriteCmd_Checkbox::undo()
{
   m_pCheckBox->setChecked(!m_bInitialValue);
}
