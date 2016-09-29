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

ItemFontCmd_AtlasGroupChanged::ItemFontCmd_AtlasGroupChanged(WidgetFont &widgetFont, QComboBox *pCmb, int iIndex, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                    m_iNewIndex(iIndex),
                                                                                                                                                    m_pCmbAtlasGroups(pCmb)
{
    m_iOriginalIndex = m_pCmbAtlasGroups->currentIndex();
    setText("Atlas Group Changed");
}

/*virtual*/ ItemFontCmd_AtlasGroupChanged::~ItemFontCmd_AtlasGroupChanged()
{
}

void ItemFontCmd_AtlasGroupChanged::redo()
{
    if(m_pCmbAtlasGroups->currentIndex() != m_iNewIndex)
    {
        m_pCmbAtlasGroups->setCurrentIndex(m_iNewIndex);
        m_WidgetFontRef.GeneratePreview();
    }
}

void ItemFontCmd_AtlasGroupChanged::undo()
{
    if(m_pCmbAtlasGroups->currentIndex() != m_iOriginalIndex)
    {
        m_pCmbAtlasGroups->setCurrentIndex(m_iOriginalIndex);
        m_WidgetFontRef.GeneratePreview();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_AddStage::ItemFontCmd_AddStage(WidgetFont &widgetFont, WidgetFontTableView *pTable, float fSize, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                m_pTable(pTable),
                                                                                                                                                m_fSize(fSize),
                                                                                                                                                m_pFontStage(NULL),
                                                                                                                                                m_iRowIndex(-1)
{
    setText("Add Font Stage");
}

/*virtual*/ ItemFontCmd_AddStage::~ItemFontCmd_AddStage()
{
}

void ItemFontCmd_AddStage::redo()
{
    if(m_pFontStage)
        static_cast<WidgetFontModel *>(m_pTable->model())->AddStage(m_pFontStage, m_iRowIndex);
    else
        m_pFontStage = static_cast<WidgetFontModel *>(m_pTable->model())->AddStage(FontStage::eType::TYPE_Normal, m_fSize, 0.0f, QColor(0, 0, 0), QColor(0, 0, 0));

    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_AddStage::undo()
{
    m_iRowIndex = static_cast<WidgetFontModel *>(m_pTable->model())->RemoveStage(m_pFontStage);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_RemoveStage::ItemFontCmd_RemoveStage(WidgetFont &widgetFont, WidgetFontTableView *pTable, FontStage *pFontStage, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                m_pTable(pTable),
                                                                                                                                                                m_pFontStage(pFontStage)
{
    setText("Remove Font Stage");
}

/*virtual*/ ItemFontCmd_RemoveStage::~ItemFontCmd_RemoveStage()
{
}

void ItemFontCmd_RemoveStage::redo()
{
    m_iRowIndex = static_cast<WidgetFontModel *>(m_pTable->model())->RemoveStage(m_pFontStage);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_RemoveStage::undo()
{
    static_cast<WidgetFontModel *>(m_pTable->model())->AddStage(m_pFontStage, m_iRowIndex);
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
