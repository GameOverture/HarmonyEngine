/**************************************************************************
 *	ItemFontCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemFontCmds.h"

#include "WidgetFont.h"
#include "HyGlobal.h"

ItemFontCmd_AtlasGroupChanged::ItemFontCmd_AtlasGroupChanged(WidgetFont &widgetFont, QComboBox *pCmb, int iPrevIndex, int iNewIndex, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                        m_iPrevIndex(iPrevIndex),
                                                                                                                                                                        m_iNewIndex(iNewIndex),
                                                                                                                                                                        m_pCmbAtlasGroups(pCmb)
{
    setText("Atlas Group Changed");
}

/*virtual*/ ItemFontCmd_AtlasGroupChanged::~ItemFontCmd_AtlasGroupChanged()
{
}

void ItemFontCmd_AtlasGroupChanged::redo()
{
    m_pCmbAtlasGroups->blockSignals(true);
    m_pCmbAtlasGroups->setCurrentIndex(m_iNewIndex);
    m_pCmbAtlasGroups->blockSignals(false);

    QSize prevSize = m_WidgetFontRef.GetAtlasDimensions(m_iPrevIndex);
    QSize curSize = m_WidgetFontRef.GetAtlasDimensions(m_iNewIndex);

    if(prevSize.width() < curSize.width() || prevSize.height() < curSize.height())
        m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_AtlasGroupChanged::undo()
{
    m_pCmbAtlasGroups->blockSignals(true);
    m_pCmbAtlasGroups->setCurrentIndex(m_iPrevIndex);
    m_pCmbAtlasGroups->blockSignals(false);

    QSize prevSize = m_WidgetFontRef.GetAtlasDimensions(m_iNewIndex);
    QSize curSize = m_WidgetFontRef.GetAtlasDimensions(m_iPrevIndex);

    if(prevSize.width() < curSize.width() || prevSize.height() < curSize.height())
        m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_CheckBox::ItemFontCmd_CheckBox(WidgetFont &widgetFont, QCheckBox *pCheckBox, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                            m_pCheckBox(pCheckBox)
{
    m_bInitialValue = m_pCheckBox->isChecked();
    setText((m_bInitialValue ? "Checked " : "Unchecked ") % m_pCheckBox->text());
}

/*virtual*/ ItemFontCmd_CheckBox::~ItemFontCmd_CheckBox()
{
}

void ItemFontCmd_CheckBox::redo()
{
    m_pCheckBox->setChecked(m_bInitialValue);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_CheckBox::undo()
{
    m_pCheckBox->setChecked(!m_bInitialValue);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_LineEditSymbols::ItemFontCmd_LineEditSymbols(WidgetFont &widgetFont, QLineEdit *pTxtAdditionalSymbols, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                    m_pTxtAdditionalSymbols(pTxtAdditionalSymbols),
                                                                                                                                                    m_bFirstTimeSkipRedo(true)
{
    setText("Additional Symbols");
}

/*virtual*/ ItemFontCmd_LineEditSymbols::~ItemFontCmd_LineEditSymbols()
{
}

void ItemFontCmd_LineEditSymbols::redo()
{
    if(m_bFirstTimeSkipRedo)
        m_bFirstTimeSkipRedo = false;
    else
        m_pTxtAdditionalSymbols->redo();

    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_LineEditSymbols::undo()
{
    m_pTxtAdditionalSymbols->undo();
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_AddState::ItemFontCmd_AddState(WidgetFont &widgetFont, QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                        m_pFontState(new WidgetFontState(&m_WidgetFontRef, stateActionList)),
                                                                                                                                                        m_pComboBox(pCmb)
{
    setText("Add Font State");
}

/*virtual*/ ItemFontCmd_AddState::~ItemFontCmd_AddState()
{
}

void ItemFontCmd_AddState::redo()
{
    int iIndex = m_pComboBox->count();

    QVariant v;
    v.setValue(m_pFontState);

    m_pComboBox->addItem(QString::number(iIndex) % " - " % m_pFontState->GetName(), v);
    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);

    m_pComboBox->setCurrentIndex(iIndex);
}

void ItemFontCmd_AddState::undo()
{
    QVariant v;
    v.setValue(m_pFontState);

    int iIndex = m_pComboBox->findData(v);
    m_pComboBox->removeItem(iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemFontCmd_RemoveState::ItemFontCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                    m_pComboBox(pCmb),
                                                                                                    m_pFontState(m_pComboBox->currentData().value<WidgetFontState *>()),
                                                                                                    m_iIndex(m_pComboBox->currentIndex())
{
    setText("Remove Font State");
}

/*virtual*/ ItemFontCmd_RemoveState::~ItemFontCmd_RemoveState()
{
}

void ItemFontCmd_RemoveState::redo()
{
    m_pComboBox->removeItem(m_iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

void ItemFontCmd_RemoveState::undo()
{
    QVariant v;
    v.setValue(m_pFontState);

    m_pComboBox->insertItem(m_iIndex, QString::number(m_iIndex) % " - " % m_pFontState->GetName(), v);
    m_pComboBox->setCurrentIndex(m_iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemFontCmd_RenameState::ItemFontCmd_RenameState(QComboBox *pCmb, QString sNewName, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                        m_pComboBox(pCmb),
                                                                                                                        m_pFontState(m_pComboBox->currentData().value<WidgetFontState *>()),
                                                                                                                        m_sNewName(sNewName),
                                                                                                                        m_sOldName(m_pFontState->GetName())
{
    setText("Rename Font State");
}

/*virtual*/ ItemFontCmd_RenameState::~ItemFontCmd_RenameState()
{
}

void ItemFontCmd_RenameState::redo()
{
    m_pFontState->SetName(m_sNewName);
    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
    
    for(int i = 0; i < m_pComboBox->count(); ++i)
    {
        if(m_pComboBox->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void ItemFontCmd_RenameState::undo()
{
    m_pFontState->SetName(m_sOldName);
    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
    
    for(int i = 0; i < m_pComboBox->count(); ++i)
    {
        if(m_pComboBox->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pComboBox->setCurrentIndex(i);
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemFontCmd_MoveStateBack::ItemFontCmd_MoveStateBack(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                        m_pComboBox(pCmb),
                                                                                                        m_pFontState(m_pComboBox->currentData().value<WidgetFontState *>())
{
    setText("Shift Font State Index <-");
}

/*virtual*/ ItemFontCmd_MoveStateBack::~ItemFontCmd_MoveStateBack()
{
}

void ItemFontCmd_MoveStateBack::redo()
{
    QVariant v;
    v.setValue(m_pFontState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex -= 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pFontState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

void ItemFontCmd_MoveStateBack::undo()
{
    QVariant v;
    v.setValue(m_pFontState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex += 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pFontState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemFontCmd_MoveStateForward::ItemFontCmd_MoveStateForward(QComboBox *pCmb, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                m_pComboBox(pCmb),
                                                                                                                m_pFontState(m_pComboBox->currentData().value<WidgetFontState *>())
{
    setText("Shift Font State Index ->");
}

/*virtual*/ ItemFontCmd_MoveStateForward::~ItemFontCmd_MoveStateForward()
{
}

void ItemFontCmd_MoveStateForward::redo()
{
    QVariant v;
    v.setValue(m_pFontState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex += 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pFontState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

void ItemFontCmd_MoveStateForward::undo()
{
    QVariant v;
    v.setValue(m_pFontState);

    int iIndex = m_pComboBox->findData(v);

    m_pComboBox->removeItem(iIndex);
    iIndex -= 1;
    m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pFontState->GetName(), v);
    m_pComboBox->setCurrentIndex(iIndex);

    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_AddLayer::ItemFontCmd_AddLayer(WidgetFont &widgetFont, QComboBox *pCmbStates, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                                                    m_pCmbStates(pCmbStates),
                                                                                                                                                                                    m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                    m_eRenderMode(eRenderMode),
                                                                                                                                                                                    m_fSize(fSize),
                                                                                                                                                                                    m_fThickness(fThickness),
                                                                                                                                                                                    m_iId(-1)
{
    setText("Add Font Layer");
}

/*virtual*/ ItemFontCmd_AddLayer::~ItemFontCmd_AddLayer()
{
}

void ItemFontCmd_AddLayer::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    if(m_iId == -1)
        m_iId = pModel->AddNewLayer(m_eRenderMode, m_fSize, m_fThickness);
    else
        pModel->ReAddLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void ItemFontCmd_AddLayer::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->RemoveLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_RemoveLayer::ItemFontCmd_RemoveLayer(WidgetFont &widgetFont, QComboBox *pCmbStates, int iId, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                            m_pCmbStates(pCmbStates),
                                                                                                                                            m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                            m_iId(iId)
{
    setText("Remove Font Layer");
}

/*virtual*/ ItemFontCmd_RemoveLayer::~ItemFontCmd_RemoveLayer()
{
}

void ItemFontCmd_RemoveLayer::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->RemoveLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void ItemFontCmd_RemoveLayer::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->ReAddLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_FontSelection::ItemFontCmd_FontSelection(WidgetFont &widgetFont, QComboBox *pCmbStates, QComboBox *pCmbFontList, int iPrevIndex, int iNewIndex, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                m_pCmbStates(pCmbFontList),
                                                                                                                                                                                                m_pCmbFontList(pCmbFontList),
                                                                                                                                                                                                m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                m_iPrevIndex(iPrevIndex),
                                                                                                                                                                                                m_iNewIndex(iNewIndex)
{
    setText("Font Selection");
}

/*virtual*/ ItemFontCmd_FontSelection::~ItemFontCmd_FontSelection()
{
}

void ItemFontCmd_FontSelection::redo()
{
    m_pCmbFontList->blockSignals(true);
    m_pCmbFontList->setCurrentIndex(m_iNewIndex);
    
//    for(int i = 0; i < m_pCmbStates->count(); ++i)
//    {
//        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
//        {
//            m_pCmbStates->setCurrentIndex(i);
//            break;
//        }
//    }
    m_pCmbFontList->blockSignals(false);
    
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_FontSelection::undo()
{
    m_pCmbFontList->blockSignals(true);
    m_pCmbFontList->setCurrentIndex(m_iPrevIndex);
    
//    for(int i = 0; i < m_pCmbStates->count(); ++i)
//    {
//        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
//        {
//            m_pCmbStates->setCurrentIndex(i);
//            break;
//        }
//    }
    m_pCmbFontList->blockSignals(false);
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_FontSize::ItemFontCmd_FontSize(WidgetFont &widgetFont, QComboBox *pCmbStates, QDoubleSpinBox *pSbSize, float fPrevSize, float fNewSize, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                                        m_pCmbStates(pCmbStates),
                                                                                                                                                                                        m_pSbSize(pSbSize),
                                                                                                                                                                                        m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                        m_fPrevSize(fPrevSize),
                                                                                                                                                                                        m_fNewSize(fNewSize)
{
    setText("Font Size");
}

/*virtual*/ ItemFontCmd_FontSize::~ItemFontCmd_FontSize()
{
}

void ItemFontCmd_FontSize::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetFontSize(m_fNewSize);
    
    m_pSbSize->blockSignals(true);
    m_pSbSize->setValue(m_fNewSize);
    m_pSbSize->blockSignals(false);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_FontSize::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetFontSize(m_fPrevSize);
    
    m_pSbSize->blockSignals(true);
    m_pSbSize->setValue(m_fPrevSize);
    m_pSbSize->blockSignals(false);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_LayerRenderMode::ItemFontCmd_LayerRenderMode(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                        m_pCmbStates(pCmbStates),
                                                                                                                                                                                                        m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                        m_iLayerId(iLayerId),
                                                                                                                                                                                                        m_ePrevRenderMode(ePrevMode),
                                                                                                                                                                                                        m_eNewRenderMode(eNewMode)
{
    setText("Stage Render Mode");
}

/*virtual*/ ItemFontCmd_LayerRenderMode::~ItemFontCmd_LayerRenderMode()
{
}

void ItemFontCmd_LayerRenderMode::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_eNewRenderMode);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_LayerRenderMode::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_ePrevRenderMode);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_LayerOutlineThickness::ItemFontCmd_LayerOutlineThickness(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                                m_iLayerId(iLayerId),
                                                                                                                                                                                                                m_fPrevThickness(fPrevThickness),
                                                                                                                                                                                                                m_fNewThickness(fNewThickness)
{
    setText("Stage Outline Thickness");
}

/*virtual*/ ItemFontCmd_LayerOutlineThickness::~ItemFontCmd_LayerOutlineThickness()
{
}

void ItemFontCmd_LayerOutlineThickness::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fNewThickness);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_LayerOutlineThickness::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fPrevThickness);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_LayerOrder::ItemFontCmd_LayerOrder(WidgetFont &widgetFont, QComboBox *pCmbStates, WidgetFontTableView *pFontTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                            m_pCmbStates(pCmbStates),
                                                                                                                                                                                                            m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                            m_pFontTableView(pFontTableView),
                                                                                                                                                                                                            m_iPrevRowIndex(iPrevRowIndex),
                                                                                                                                                                                                            m_iNewRowIndex(iNewRowIndex)
{
    if(m_iPrevRowIndex > m_iNewRowIndex)
        setText("Order Layer Upwards");
    else
        setText("Order Layer Downwards");
}

/*virtual*/ ItemFontCmd_LayerOrder::~ItemFontCmd_LayerOrder()
{
}

void ItemFontCmd_LayerOrder::redo()
{
    WidgetFontModel *pModel = static_cast<WidgetFontModel *>(m_pFontTableView->model());
    
    int iOffset = m_iNewRowIndex - m_iPrevRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iPrevRowIndex);
    else
        pModel->MoveRowUp(m_iPrevRowIndex);
        
    m_pFontTableView->selectRow(m_iNewRowIndex);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void ItemFontCmd_LayerOrder::undo()
{
    WidgetFontModel *pModel = static_cast<WidgetFontModel *>(m_pFontTableView->model());
    
    int iOffset = m_iPrevRowIndex - m_iNewRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iNewRowIndex);
    else
        pModel->MoveRowUp(m_iNewRowIndex);
    
    m_pFontTableView->selectRow(m_iPrevRowIndex);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

