/**************************************************************************
 *	ItemSpriteCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMFONTCMDS_H
#define ITEMFONTCMDS_H

#include <QUndoCommand>
#include <QComboBox>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_AtlasGroupChanged : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    int                 m_iNewIndex;
    int                 m_iOriginalIndex;
    
public:
    ItemFontCmd_AtlasGroupChanged(QComboBox *pCmb, int iIndex, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AtlasGroupChanged();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_RemoveState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    
public:
    ItemFontCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_RemoveState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // ITEMFONTCMDS_H
