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
#include <QSize>

class WidgetFont;
class WidgetFontState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_AtlasGroupChanged : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;

    QSize               m_CurrentAtlasDimensions;
    QComboBox *         m_pCmbAtlasGroups;
    int                 m_iPrevIndex;
    int                 m_iNewIndex;
    
public:
    ItemFontCmd_AtlasGroupChanged(WidgetFont &widgetFont, QComboBox *pCmb, int iPrevIndex, int iNewIndex, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AtlasGroupChanged();

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
class ItemFontCmd_AddState : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    WidgetFontState *       m_pFontState;
    QComboBox *             m_pComboBox;
    
public:
    ItemFontCmd_AddState(WidgetFont &widgetFont, QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AddState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_RemoveState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetFontState *   m_pFontState;
    int                 m_iIndex;

public:
    ItemFontCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_RemoveState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_RenameState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetFontState *   m_pFontState;
    QString             m_sNewName;
    QString             m_sOldName;

public:
    ItemFontCmd_RenameState(QComboBox *pCmb, QString sNewName, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_RenameState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_MoveStateBack : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetFontState *   m_pFontState;

public:
    ItemFontCmd_MoveStateBack(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_MoveStateBack();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_MoveStateForward : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetFontState *   m_pFontState;

public:
    ItemFontCmd_MoveStateForward(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_MoveStateForward();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_AddLayer : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    WidgetFontModel *       m_pModel;
    rendermode_t            m_eRenderMode;
    float                   m_fSize;
    float                   m_fThickness;
    int                     m_iId;

public:
    ItemFontCmd_AddLayer(WidgetFont &widgetFont, WidgetFontModel *pModel, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_AddLayer();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_RemoveLayer : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    WidgetFontModel *       m_pModel;
    int                     m_iId;

public:
    ItemFontCmd_RemoveLayer(WidgetFont &widgetFont, WidgetFontModel *pModel, int iId, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_RemoveLayer();

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

public:
    ItemFontCmd_FontSelection(WidgetFont &widgetFont, QComboBox *pCmbFontList, int iPrevIndex, int iNewIndex, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_FontSelection();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_StageRenderMode : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    WidgetFontModel *   m_pFontModel;
    int                 m_iStageId;
    rendermode_t        m_ePrevRenderMode;
    rendermode_t        m_eNewRenderMode;

public:
    ItemFontCmd_StageRenderMode(WidgetFont &widgetFont, WidgetFontModel *pFontModel, int iStageId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_StageRenderMode();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_StageSize : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    WidgetFontModel *   m_pFontModel;
    int                 m_iRowIndex;
    float               m_fPrevSize;
    float               m_fNewSize;

public:
    ItemFontCmd_StageSize(WidgetFont &widgetFont, WidgetFontModel *pFontModel, float fPrevSize, float fNewSize, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_StageSize();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemFontCmd_LayerOutlineThickness : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    WidgetFontModel *   m_pFontModel;
    int                 m_iRowIndex;
    float               m_fPrevThickness;
    float               m_fNewThickness;

public:
    ItemFontCmd_LayerOutlineThickness(WidgetFont &widgetFont, WidgetFontModel *pFontModel, int iRowIndex, float fPrevThickness, float fNewThickness, QUndoCommand *pParent = 0);
    virtual ~ItemFontCmd_LayerOutlineThickness();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // ITEMFONTCMDS_H
