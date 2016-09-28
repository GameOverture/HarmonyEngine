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

ItemFontCmd_AddFontSize::ItemFontCmd_AddFontSize(WidgetFont &widgetFont, QComboBox *pCmb, double dSize, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                            m_pCmbSizes(pCmb),
                                                                                                                                            m_dNewSize(dSize)
{
    setText("Add Type Size");
}

/*virtual*/ ItemFontCmd_AddFontSize::~ItemFontCmd_AddFontSize()
{
}

void ItemFontCmd_AddFontSize::redo()
{
    m_pCmbSizes->addItem(QString::number(m_dNewSize, 'g', 2));
    m_iIndexAdded = m_pCmbSizes->count() - 1;

    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_AddFontSize::undo()
{
    m_pCmbSizes->removeItem(m_iIndexAdded);
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemFontCmd_RemoveFontSize::ItemFontCmd_RemoveFontSize(WidgetFont &widgetFont, QComboBox *pCmb, int iIndexToRemove, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                        m_pCmbSizes(pCmb),
                                                                                                                                                        m_iIndexToRemove(iIndexToRemove)
{
    m_dOldSize = m_pCmbSizes->itemText(m_iIndexToRemove).toDouble();
    setText("Remove Type Size");
}

/*virtual*/ ItemFontCmd_RemoveFontSize::~ItemFontCmd_RemoveFontSize()
{
}

void ItemFontCmd_RemoveFontSize::redo()
{
    m_pCmbSizes->removeItem(m_iIndexToRemove);
    m_WidgetFontRef.GeneratePreview();
}

void ItemFontCmd_RemoveFontSize::undo()
{
    m_pCmbSizes->insertItem(m_iIndexToRemove, QString::number(m_dOldSize, 'g', 2));
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

ItemFontCmd_FontSelection::ItemFontCmd_FontSelection(WidgetFont &widgetFont, QComboBox *pCmbFontList, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                        m_pCmbFontList(pCmbFontList)
{
    setText("Font Selection");
}

/*virtual*/ ItemFontCmd_FontSelection::~ItemFontCmd_FontSelection()
{
}

void ItemFontCmd_FontSelection::redo()
{
}

void ItemFontCmd_FontSelection::undo()
{
}
