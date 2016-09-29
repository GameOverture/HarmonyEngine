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

#include "WidgetFontModelView.h"

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QDir>

class WidgetFont;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_AtlasGroupChanged : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;

    QComboBox *         m_pCmbAtlasGroups;
    int                 m_iNewIndex;
    int                 m_iOriginalIndex;
    
public:
    ItemFontCmd_AtlasGroupChanged(WidgetFont &widgetFont, QComboBox *pCmb, int iIndex, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AtlasGroupChanged();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_AddStage : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    WidgetFontTableView *   m_pTable;
    float                   m_fSize;
    FontStage *             m_pFontStage;
    int                     m_iRowIndex;
    
public:
    ItemFontCmd_AddStage(WidgetFont &widgetFont, WidgetFontTableView *pTable, float fSize, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AddStage();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_RemoveStage : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    WidgetFontTableView *   m_pTable;
    FontStage *             m_pFontStage;
    int                     m_iRowIndex;

public:
    ItemFontCmd_RemoveStage(WidgetFont &widgetFont, WidgetFontTableView *pTable, FontStage *pFontStage, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_RemoveStage();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_CheckBox : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QCheckBox *         m_pCheckBox;
    bool                m_bInitialValue;

public:
    ItemFontCmd_CheckBox(WidgetFont &widgetFont, QCheckBox *pCheckBox, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_CheckBox();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_LineEditSymbols : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QLineEdit *         m_pTxtAdditionalSymbols;
    bool                m_bFirstTimeSkipRedo;

public:
    ItemFontCmd_LineEditSymbols(WidgetFont &widgetFont, QLineEdit *pTxtAdditionalSymbols, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_LineEditSymbols();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_FontSelection : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QComboBox *         m_pCmbFontList;
    int                 m_iPrevIndex;
    int                 m_iNewIndex;
    QDir                m_FontMetaDir;

public:
    ItemFontCmd_FontSelection(WidgetFont &widgetFont, QComboBox *pCmbFontList, int iPrevIndex, int iNewIndex, QDir fontMetaDir, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_FontSelection();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

    void MoveFontIntoTempDir(int iIndex);
};

#endif // ITEMFONTCMDS_H
