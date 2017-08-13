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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_AddLayer : public QUndoCommand
{
    ProjectItem &           m_ItemRef;
    int                     m_iStateIndex;

    rendermode_t            m_eRenderMode;
    int                     m_iSize;
    float                   m_fThickness;
    int                     m_iId;

public:
    FontUndoCmd_AddLayer(ProjectItem &itemRef, int iStateIndex, rendermode_t eRenderMode, int iSize, float fThickness, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_AddLayer();

    void redo() override;
    void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_RemoveLayer : public QUndoCommand
{
    ProjectItem &           m_ItemRef;
    int                     m_iStateIndex;

    int                     m_iId;

public:
    FontUndoCmd_RemoveLayer(ProjectItem &itemRef, int iStateIndex, int iId, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_RemoveLayer();

    void redo() override;
    void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerRenderMode : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iStateIndex;

    int                 m_iLayerId;
    rendermode_t        m_ePrevRenderMode;
    rendermode_t        m_eNewRenderMode;

public:
    FontUndoCmd_LayerRenderMode(ProjectItem &itemRef, int iStateIndex, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerRenderMode();

    void redo() override;
    void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerOutlineThickness : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iStateIndex;

    int                 m_iLayerId;
    float               m_fPrevThickness;
    float               m_fNewThickness;

public:
    FontUndoCmd_LayerOutlineThickness(ProjectItem &itemRef, int iStateIndex, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerOutlineThickness();

    void redo() override;
    void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerColors : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iStateIndex;

    int                 m_iLayerId;
    QColor              m_PrevTopColor;
    QColor              m_PrevBotColor;
    QColor              m_NewTopColor;
    QColor              m_NewBotColor;

public:
    FontUndoCmd_LayerColors(ProjectItem &itemRef, int iStateIndex, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerColors();

    void redo() override;
    void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontUndoCmd_LayerOrder : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iStateIndex;

    FontTableView *     m_pFontTableView;
    int                 m_iPrevRowIndex;
    int                 m_iNewRowIndex;

public:
    FontUndoCmd_LayerOrder(ProjectItem &itemRef, int iStateIndex, FontTableView *pTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent = 0);
    virtual ~FontUndoCmd_LayerOrder();

    void redo() override;
    void undo() override;
};

#endif // FONTUNDOCMDS_H
