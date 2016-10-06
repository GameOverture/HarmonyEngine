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
}

void ItemFontCmd_RenameState::undo()
{
    m_pFontState->SetName(m_sOldName);
    SetStateNamingConventionInComboBox<WidgetFontState>(m_pComboBox);
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

ItemFontCmd_AddLayer::ItemFontCmd_AddLayer(WidgetFont &widgetFont, WidgetFontModel *pModel, QString sFullFontPath, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                m_pModel(pModel),
                                                                                                                                                                                                                m_sFullFontPath(sFullFontPath),
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
    if(m_iId == -1)
        m_iId = m_WidgetFontRef.AddNewStage(m_sFullFontPath, m_eRenderMode, m_fSize, m_fThickness);
    else
        m_WidgetFontRef.ReAddStage(m_iId);

    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_AddLayer::undo()
{
    m_WidgetFontRef.RemoveStage(m_iId);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_RemoveLayer::ItemFontCmd_RemoveLayer(WidgetFont &widgetFont, int iId, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                    m_iId(iId)
{
    setText("Remove Font Stage");
}

/*virtual*/ ItemFontCmd_RemoveLayer::~ItemFontCmd_RemoveLayer()
{
}

void ItemFontCmd_RemoveLayer::redo()
{
    m_WidgetFontRef.RemoveStage(m_iId);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_RemoveLayer::undo()
{
    m_WidgetFontRef.ReAddStage(m_iId);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_FontSelection::ItemFontCmd_FontSelection(WidgetFont &widgetFont, QComboBox *pCmbFontList, int iPrevIndex, int iNewIndex, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                        m_pCmbFontList(pCmbFontList),
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
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_FontSelection::undo()
{
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_StageRenderMode::ItemFontCmd_StageRenderMode(WidgetFont &widgetFont, WidgetFontModel *pFontModel, int iRowIndex, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                m_pFontModel(pFontModel),
                                                                                                                                                                                                                m_iRowIndex(iRowIndex),
                                                                                                                                                                                                                m_ePrevRenderMode(ePrevMode),
                                                                                                                                                                                                                m_eNewRenderMode(eNewMode)
{
    setText("Stage Render Mode");
}

/*virtual*/ ItemFontCmd_StageRenderMode::~ItemFontCmd_StageRenderMode()
{
}

void ItemFontCmd_StageRenderMode::redo()
{
    m_pFontModel->SetStageRenderMode(m_iRowIndex, m_eNewRenderMode);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_StageRenderMode::undo()
{
    m_pFontModel->SetStageRenderMode(m_iRowIndex, m_ePrevRenderMode);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_StageSize::ItemFontCmd_StageSize(WidgetFont &widgetFont, WidgetFontModel *pFontModel, float fPrevSize, float fNewSize, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                                    m_pFontModel(pFontModel),
                                                                                                                                                                    m_fPrevSize(fPrevSize),
                                                                                                                                                                    m_fNewSize(fNewSize)
{
}

/*virtual*/ ItemFontCmd_StageSize::~ItemFontCmd_StageSize()
{
    setText("Stage Size");
}

void ItemFontCmd_StageSize::redo()
{
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_StageSize::undo()
{
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_StageOutlineThickness::ItemFontCmd_StageOutlineThickness(WidgetFont &widgetFont, WidgetFontModel *pFontModel, int iRowIndex, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                        m_pFontModel(pFontModel),
                                                                                                                                                                                                                        m_iRowIndex(iRowIndex),
                                                                                                                                                                                                                        m_fPrevThickness(fPrevThickness),
                                                                                                                                                                                                                        m_fNewThickness(fNewThickness)
{
    setText("Stage Outline Thickness");
}

/*virtual*/ ItemFontCmd_StageOutlineThickness::~ItemFontCmd_StageOutlineThickness()
{
}

void ItemFontCmd_StageOutlineThickness::redo()
{
    m_pFontModel->SetStageOutlineThickness(m_iRowIndex, m_fNewThickness);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_StageOutlineThickness::undo()
{
    m_pFontModel->SetStageOutlineThickness(m_iRowIndex, m_fPrevThickness);
    m_WidgetFontRef.GeneratePreview();
}
