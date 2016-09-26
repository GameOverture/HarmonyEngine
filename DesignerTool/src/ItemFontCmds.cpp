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

ItemFontCmd_AtlasGroupChanged::ItemFontCmd_AtlasGroupChanged(QComboBox *pCmb, int iIndex, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                            m_iNewIndex(iIndex),
                                                                                                                            m_pComboBox(pCmb)
{
    m_iOriginalIndex = m_pComboBox->currentIndex();
    setText("Atlas Group Changed");
}

/*virtual*/ ItemFontCmd_AtlasGroupChanged::~ItemFontCmd_AtlasGroupChanged()
{
}

void ItemFontCmd_AtlasGroupChanged::redo()
{
    if(m_pComboBox->currentIndex() != m_iNewIndex)
        m_pComboBox->setCurrentIndex(m_iNewIndex);
}

void ItemFontCmd_AtlasGroupChanged::undo()
{
    if(m_pComboBox->currentIndex() != m_iOriginalIndex)
        m_pComboBox->setCurrentIndex(m_iOriginalIndex);
}

