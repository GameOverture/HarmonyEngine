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

#include "FontModelView.h"

#include <QUndoCommand>
#include <QComboBox>

class WidgetFont;
class WidgetFontState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontUndoCmd_AddLayer : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    QComboBox *             m_pCmbStates;
    WidgetFontState *       m_pFontState;
    rendermode_t            m_eRenderMode;
    float                   m_fSize;
    float                   m_fThickness;
    int                     m_iId;

public:
    WidgetFontUndoCmd_AddLayer(WidgetFont &widgetFont, QComboBox *pCmbStates, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent = 0);
    virtual ~WidgetFontUndoCmd_AddLayer();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontUndoCmd_RemoveLayer : public QUndoCommand
{
    WidgetFont &            m_WidgetFontRef;
    QComboBox *             m_pCmbStates;
    WidgetFontState *       m_pFontState;
    int                     m_iId;

public:
    WidgetFontUndoCmd_RemoveLayer(WidgetFont &widgetFont, QComboBox *pCmbStates, int iId, QUndoCommand *pParent = 0);
    virtual ~WidgetFontUndoCmd_RemoveLayer();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontUndoCmd_LayerRenderMode : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QComboBox *         m_pCmbStates;
    WidgetFontState *   m_pFontState;
    int                 m_iLayerId;
    rendermode_t        m_ePrevRenderMode;
    rendermode_t        m_eNewRenderMode;

public:
    WidgetFontUndoCmd_LayerRenderMode(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent = 0);
    virtual ~WidgetFontUndoCmd_LayerRenderMode();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontUndoCmd_LayerOutlineThickness : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QComboBox *         m_pCmbStates;
    WidgetFontState *   m_pFontState;
    int                 m_iLayerId;
    float               m_fPrevThickness;
    float               m_fNewThickness;

public:
    WidgetFontUndoCmd_LayerOutlineThickness(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent = 0);
    virtual ~WidgetFontUndoCmd_LayerOutlineThickness();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontUndoCmd_LayerColors : public QUndoCommand
{
    WidgetFont &        m_WidgetFontRef;
    QComboBox *         m_pCmbStates;
    WidgetFontState *   m_pFontState;
    int                 m_iLayerId;
    QColor              m_PrevTopColor;
    QColor              m_PrevBotColor;
    QColor              m_NewTopColor;
    QColor              m_NewBotColor;

public:
    WidgetFontUndoCmd_LayerColors(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent = 0);
    virtual ~WidgetFontUndoCmd_LayerColors();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontUndoCmd_LayerOrder : public QUndoCommand
{
    WidgetFont &                m_WidgetFontRef;
    QComboBox *                 m_pCmbStates;
    WidgetFontState *           m_pFontState;
    WidgetFontTableView *       m_pFontTableView;
    int                         m_iPrevRowIndex;
    int                         m_iNewRowIndex;

public:
    WidgetFontUndoCmd_LayerOrder(WidgetFont &widgetFont, QComboBox *pCmbStates, WidgetFontTableView *pFontTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent = 0);
    virtual ~WidgetFontUndoCmd_LayerOrder();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // ITEMFONTCMDS_H
