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
class ItemFontCmd_AddFontSize : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QComboBox *         m_pCmbSizes;
    double              m_dNewSize;
    int                 m_iIndexAdded;
    
public:
    ItemFontCmd_AddFontSize(WidgetFont &widgetFont, QComboBox *pCmb, double dSize, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AddFontSize();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_RemoveFontSize : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QComboBox *         m_pCmbSizes;
    int                 m_iIndexToRemove;
    double              m_dOldSize;

public:
    ItemFontCmd_RemoveFontSize(WidgetFont &widgetFont, QComboBox *pCmb, int iIndexToRemove, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_RemoveFontSize();

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
