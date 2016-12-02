/**************************************************************************
 *	ItemFontCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFontUndoCmds.h"

#include "WidgetFont.h"
#include "HyGlobal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontUndoCmd_AddLayer::WidgetFontUndoCmd_AddLayer(WidgetFont &widgetFont, QComboBox *pCmbStates, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                m_pCmbStates(pCmbStates),
                                                                                                                                                                                                m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                m_eRenderMode(eRenderMode),
                                                                                                                                                                                                m_fSize(fSize),
                                                                                                                                                                                                m_fThickness(fThickness),
                                                                                                                                                                                                m_iId(-1)
{
    setText("Add Font Layer");
}

/*virtual*/ WidgetFontUndoCmd_AddLayer::~WidgetFontUndoCmd_AddLayer()
{
}

void WidgetFontUndoCmd_AddLayer::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    if(m_iId == -1)
        m_iId = pModel->AddNewLayer(m_eRenderMode, m_fSize, m_fThickness);
    else
        pModel->ReAddLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void WidgetFontUndoCmd_AddLayer::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->RemoveLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontUndoCmd_RemoveLayer::WidgetFontUndoCmd_RemoveLayer(WidgetFont &widgetFont, QComboBox *pCmbStates, int iId, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                            m_pCmbStates(pCmbStates),
                                                                                                                                            m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                            m_iId(iId)
{
    setText("Remove Font Layer");
}

/*virtual*/ WidgetFontUndoCmd_RemoveLayer::~WidgetFontUndoCmd_RemoveLayer()
{
}

void WidgetFontUndoCmd_RemoveLayer::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->RemoveLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void WidgetFontUndoCmd_RemoveLayer::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->ReAddLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontUndoCmd_LayerRenderMode::WidgetFontUndoCmd_LayerRenderMode(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                    m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                    m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                                    m_iLayerId(iLayerId),
                                                                                                                                                                                                                    m_ePrevRenderMode(ePrevMode),
                                                                                                                                                                                                                    m_eNewRenderMode(eNewMode)
{
    setText("Stage Render Mode");
}

/*virtual*/ WidgetFontUndoCmd_LayerRenderMode::~WidgetFontUndoCmd_LayerRenderMode()
{
}

void WidgetFontUndoCmd_LayerRenderMode::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_eNewRenderMode);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void WidgetFontUndoCmd_LayerRenderMode::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_ePrevRenderMode);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontUndoCmd_LayerOutlineThickness::WidgetFontUndoCmd_LayerOutlineThickness(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                            m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                            m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                                            m_iLayerId(iLayerId),
                                                                                                                                                                                                                            m_fPrevThickness(fPrevThickness),
                                                                                                                                                                                                                            m_fNewThickness(fNewThickness)
{
    setText("Stage Outline Thickness");
}

/*virtual*/ WidgetFontUndoCmd_LayerOutlineThickness::~WidgetFontUndoCmd_LayerOutlineThickness()
{
}

void WidgetFontUndoCmd_LayerOutlineThickness::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fNewThickness);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void WidgetFontUndoCmd_LayerOutlineThickness::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fPrevThickness);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontUndoCmd_LayerColors::WidgetFontUndoCmd_LayerColors(WidgetFont &widgetFont, QComboBox *pCmbStates, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                                                m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                                                m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                                                                m_iLayerId(iLayerId),
                                                                                                                                                                                                                                                m_PrevTopColor(prevTopColor),
                                                                                                                                                                                                                                                m_PrevBotColor(prevBotColor),
                                                                                                                                                                                                                                                m_NewTopColor(newTopColor),
                                                                                                                                                                                                                                                m_NewBotColor(newBotColor)
{
    setText("Set Layer Colors");
}

/*virtual*/ WidgetFontUndoCmd_LayerColors::~WidgetFontUndoCmd_LayerColors()
{
}

void WidgetFontUndoCmd_LayerColors::redo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();

    pModel->SetLayerColors(m_iLayerId, m_NewTopColor, m_NewBotColor);

    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.UpdateActions();
}

void WidgetFontUndoCmd_LayerColors::undo()
{
    WidgetFontModel *pModel = m_pFontState->GetFontModel();

    pModel->SetLayerColors(m_iLayerId, m_PrevTopColor, m_PrevBotColor);

    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontUndoCmd_LayerOrder::WidgetFontUndoCmd_LayerOrder(WidgetFont &widgetFont, QComboBox *pCmbStates, WidgetFontTableView *pFontTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                        m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                        m_pFontState(m_pCmbStates->currentData().value<WidgetFontState *>()),
                                                                                                                                                                                                                        m_pFontTableView(pFontTableView),
                                                                                                                                                                                                                        m_iPrevRowIndex(iPrevRowIndex),
                                                                                                                                                                                                                        m_iNewRowIndex(iNewRowIndex)
{
    if(m_iPrevRowIndex > m_iNewRowIndex)
        setText("Order Layer Upwards");
    else
        setText("Order Layer Downwards");
}

/*virtual*/ WidgetFontUndoCmd_LayerOrder::~WidgetFontUndoCmd_LayerOrder()
{
}

void WidgetFontUndoCmd_LayerOrder::redo()
{
    WidgetFontModel *pModel = static_cast<WidgetFontModel *>(m_pFontTableView->model());
    
    int iOffset = m_iNewRowIndex - m_iPrevRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iPrevRowIndex);
    else
        pModel->MoveRowUp(m_iPrevRowIndex);
        
    m_pFontTableView->selectRow(m_iNewRowIndex);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void WidgetFontUndoCmd_LayerOrder::undo()
{
    WidgetFontModel *pModel = static_cast<WidgetFontModel *>(m_pFontTableView->model());
    
    int iOffset = m_iPrevRowIndex - m_iNewRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iNewRowIndex);
    else
        pModel->MoveRowUp(m_iNewRowIndex);
    
    m_pFontTableView->selectRow(m_iPrevRowIndex);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<WidgetFontState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

