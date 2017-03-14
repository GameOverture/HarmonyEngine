/**************************************************************************
 *	ItemSpriteCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTUNDOCMDS_H
#define FONTUNDOCMDS_H

#include "FontModelView.h"

#include <QUndoCommand>
#include <QComboBox>

class FontWidget;
class FontWidgetState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_AddLayer : public QUndoCommand
{
    FontWidget &            m_WidgetFontRef;
    QComboBox *             m_pCmbStates;
    FontWidgetState *       m_pFontState;
    rendermode_t            m_eRenderMode;
    float                   m_fSize;
    float                   m_fThickness;
    int                     m_iId;

public:
    FontUndoCmd_AddLayer(FontWidget &widgetFont, QComboBox *pCmbStates, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_AddLayer();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_RemoveLayer : public QUndoCommand
{
    FontWidget &            m_WidgetFontRef;
    QComboBox *             m_pCmbStates;
    FontWidgetState *       m_pFontState;
    int                     m_iId;

public:
    FontUndoCmd_RemoveLayer(FontWidget &widgetFont, QComboBox *pCmbStates, int iId, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_RemoveLayer();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerRenderMode : public QUndoCommand
{
    FontWidget &        m_WidgetFontRef;
    QComboBox *         m_pCmbStates;
    FontWidgetState *   m_pFontState;
    int                 m_iLayerId;
    rendermode_t        m_ePrevRenderMode;
    rendermode_t        m_eNewRenderMode;

public:
    FontUndoCmd_LayerRenderMode(FontWidget &widgetFont, QComboBox *pCmbStates, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerRenderMode();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerOutlineThickness : public QUndoCommand
{
    FontWidget &        m_WidgetFontRef;
    QComboBox *         m_pCmbStates;
    FontWidgetState *   m_pFontState;
    int                 m_iLayerId;
    float               m_fPrevThickness;
    float               m_fNewThickness;

public:
    FontUndoCmd_LayerOutlineThickness(FontWidget &widgetFont, QComboBox *pCmbStates, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerOutlineThickness();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerColors : public QUndoCommand
{
    FontWidget &        m_WidgetFontRef;
    QComboBox *         m_pCmbStates;
    FontWidgetState *   m_pFontState;
    int                 m_iLayerId;
    QColor              m_PrevTopColor;
    QColor              m_PrevBotColor;
    QColor              m_NewTopColor;
    QColor              m_NewBotColor;

public:
    FontUndoCmd_LayerColors(FontWidget &widgetFont, QComboBox *pCmbStates, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerColors();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerOrder : public QUndoCommand
{
    FontWidget &                m_WidgetFontRef;
    QComboBox *                 m_pCmbStates;
    FontWidgetState *           m_pFontState;
    FontTableView *       m_pFontTableView;
    int                         m_iPrevRowIndex;
    int                         m_iNewRowIndex;

public:
    FontUndoCmd_LayerOrder(FontWidget &widgetFont, QComboBox *pCmbStates, FontTableView *pFontTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerOrder();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // FONTUNDOCMDS_H
