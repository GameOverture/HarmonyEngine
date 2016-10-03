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

void EnsureProperNamingInCmbFontState(QComboBox *pCmb)
{
    // Ensure that all the entry names in the combobox match their index
    for(int i = 0; i < pCmb->count(); ++i)
    {
        QString sName(QString::number(i) % " - " % pCmb->itemData(i).value<WidgetFontState *>()->GetName());
        pCmb->setItemText(i, sName);
    }
}

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

ItemFontCmd_AddStage::ItemFontCmd_AddStage(WidgetFont &widgetFont, WidgetFontTableView *pTable, float fSize, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                m_pTable(pTable),
                                                                                                                                                m_fSize(fSize),
                                                                                                                                                m_iId(-1)
{
    setText("Add Font Stage");
}

/*virtual*/ ItemFontCmd_AddStage::~ItemFontCmd_AddStage()
{
}

void ItemFontCmd_AddStage::redo()
{
    if(m_iId == -1)
        m_iId = static_cast<WidgetFontModel *>(m_pTable->model())->AddNewStage(RENDER_NORMAL, m_fSize, 0.0f, QColor(0, 0, 0), QColor(0, 0, 0));
    else
        static_cast<WidgetFontModel *>(m_pTable->model())->AddExistingStage(m_iId);

    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_AddStage::undo()
{
    static_cast<WidgetFontModel *>(m_pTable->model())->RemoveStage(m_iId);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_RemoveStage::ItemFontCmd_RemoveStage(WidgetFont &widgetFont, WidgetFontTableView *pTable, int iRowIndex, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                        m_pTable(pTable)
{
    m_iId = static_cast<WidgetFontModel *>(m_pTable->model())->GetStageId(iRowIndex);
    setText("Remove Font Stage");
}

/*virtual*/ ItemFontCmd_RemoveStage::~ItemFontCmd_RemoveStage()
{
}

void ItemFontCmd_RemoveStage::redo()
{
    static_cast<WidgetFontModel *>(m_pTable->model())->RemoveStage(m_iId);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_RemoveStage::undo()
{
    static_cast<WidgetFontModel *>(m_pTable->model())->AddExistingStage(m_iId);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_FontSelection::ItemFontCmd_FontSelection(WidgetFont &widgetFont, QComboBox *pCmbFontList, int iPrevIndex, int iNewIndex, QDir fontMetaDir, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                                        m_pCmbFontList(pCmbFontList),
                                                                                                                                                                                        m_iPrevIndex(iPrevIndex),
                                                                                                                                                                                        m_iNewIndex(iNewIndex),
                                                                                                                                                                                        m_FontMetaDir(fontMetaDir)
{
    setText("Font Selection");
}

/*virtual*/ ItemFontCmd_FontSelection::~ItemFontCmd_FontSelection()
{
}

void ItemFontCmd_FontSelection::redo()
{
    MoveFontIntoTempDir(m_iNewIndex);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_FontSelection::undo()
{
    MoveFontIntoTempDir(m_iPrevIndex);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_FontSelection::MoveFontIntoTempDir(int iIndex)
{
    QFileInfo originalFontFile(m_pCmbFontList->itemData(iIndex).toString());
    if(originalFontFile.exists() == false)
    {
        HyGuiLog("Font file (" % originalFontFile.absoluteFilePath() % ") doesn't exist", LOGTYPE_Error);
        return;
    }

    QDir fontMetaTempDir(m_FontMetaDir.absolutePath() % "/" % HYGUIPATH_TempDir % m_WidgetFontRef.GetFullItemName());
    if(fontMetaTempDir.removeRecursively() == false)
    {
        HyGuiLog("Could not clear temp font directory: " % fontMetaTempDir.absolutePath(), LOGTYPE_Error);
        return;
    }

    if(fontMetaTempDir.mkpath(fontMetaTempDir.absolutePath()) == false)
    {
        HyGuiLog("Failed making font meta directory path: " % m_FontMetaDir.absolutePath(), LOGTYPE_Error);
        return;
    }

    QFileInfo newFontFilePathDestination(fontMetaTempDir.absoluteFilePath(originalFontFile.fileName()));
    if(newFontFilePathDestination.exists() == false)
    {
        if(QFile::copy(originalFontFile.absoluteFilePath(), newFontFilePathDestination.absoluteFilePath()) == false)
        {
            HyGuiLog("Failed copying font to meta directory: " % newFontFilePathDestination.absoluteFilePath(), LOGTYPE_Error);
            return;
        }
    }
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

ItemFontCmd_StageSize::ItemFontCmd_StageSize(WidgetFont &widgetFont, WidgetFontModel *pFontModel, int iRowIndex, float fPrevSize, float fNewSize, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                                                    m_pFontModel(pFontModel),
                                                                                                                                                                                    m_iRowIndex(iRowIndex),
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
    m_pFontModel->SetStageSize(m_iRowIndex, m_fNewSize);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_StageSize::undo()
{
    m_pFontModel->SetStageSize(m_iRowIndex, m_fPrevSize);
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
