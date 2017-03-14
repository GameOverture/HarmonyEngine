/**************************************************************************
 *	ItemFontCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontUndoCmds.h"

#include "FontWidget.h"
#include "HyGuiGlobal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_AddLayer::FontUndoCmd_AddLayer(FontWidget &widgetFont, QComboBox *pCmbStates, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                m_pCmbStates(pCmbStates),
                                                                                                                                                                                                m_pFontState(m_pCmbStates->currentData().value<FontWidgetState *>()),
                                                                                                                                                                                                m_eRenderMode(eRenderMode),
                                                                                                                                                                                                m_fSize(fSize),
                                                                                                                                                                                                m_fThickness(fThickness),
                                                                                                                                                                                                m_iId(-1)
{
    setText("Add Font Layer");
}

/*virtual*/ FontUndoCmd_AddLayer::~FontUndoCmd_AddLayer()
{
}

void FontUndoCmd_AddLayer::redo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    if(m_iId == -1)
        m_iId = pModel->AddNewLayer(m_eRenderMode, m_fSize, m_fThickness);
    else
        pModel->ReAddLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void FontUndoCmd_AddLayer::undo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->RemoveLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_RemoveLayer::FontUndoCmd_RemoveLayer(FontWidget &widgetFont, QComboBox *pCmbStates, int iId, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                            m_pCmbStates(pCmbStates),
                                                                                                                                            m_pFontState(m_pCmbStates->currentData().value<FontWidgetState *>()),
                                                                                                                                            m_iId(iId)
{
    setText("Remove Font Layer");
}

/*virtual*/ FontUndoCmd_RemoveLayer::~FontUndoCmd_RemoveLayer()
{
}

void FontUndoCmd_RemoveLayer::redo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->RemoveLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void FontUndoCmd_RemoveLayer::undo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->ReAddLayer(m_iId);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerRenderMode::FontUndoCmd_LayerRenderMode(FontWidget &widgetFont, QComboBox *pCmbStates, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                                    m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                    m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                    m_pFontState(m_pCmbStates->currentData().value<FontWidgetState *>()),
                                                                                                                                                                                                                    m_iLayerId(iLayerId),
                                                                                                                                                                                                                    m_ePrevRenderMode(ePrevMode),
                                                                                                                                                                                                                    m_eNewRenderMode(eNewMode)
{
    setText("Stage Render Mode");
}

/*virtual*/ FontUndoCmd_LayerRenderMode::~FontUndoCmd_LayerRenderMode()
{
}

void FontUndoCmd_LayerRenderMode::redo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_eNewRenderMode);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void FontUndoCmd_LayerRenderMode::undo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_ePrevRenderMode);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerOutlineThickness::FontUndoCmd_LayerOutlineThickness(FontWidget &widgetFont, QComboBox *pCmbStates, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                                            m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                            m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                            m_pFontState(m_pCmbStates->currentData().value<FontWidgetState *>()),
                                                                                                                                                                                                                            m_iLayerId(iLayerId),
                                                                                                                                                                                                                            m_fPrevThickness(fPrevThickness),
                                                                                                                                                                                                                            m_fNewThickness(fNewThickness)
{
    setText("Stage Outline Thickness");
}

/*virtual*/ FontUndoCmd_LayerOutlineThickness::~FontUndoCmd_LayerOutlineThickness()
{
}

void FontUndoCmd_LayerOutlineThickness::redo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fNewThickness);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

void FontUndoCmd_LayerOutlineThickness::undo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fPrevThickness);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerColors::FontUndoCmd_LayerColors(FontWidget &widgetFont, QComboBox *pCmbStates, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                                                m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                                                m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                                                m_pFontState(m_pCmbStates->currentData().value<FontWidgetState *>()),
                                                                                                                                                                                                                                                m_iLayerId(iLayerId),
                                                                                                                                                                                                                                                m_PrevTopColor(prevTopColor),
                                                                                                                                                                                                                                                m_PrevBotColor(prevBotColor),
                                                                                                                                                                                                                                                m_NewTopColor(newTopColor),
                                                                                                                                                                                                                                                m_NewBotColor(newBotColor)
{
    setText("Set Layer Colors");
}

/*virtual*/ FontUndoCmd_LayerColors::~FontUndoCmd_LayerColors()
{
}

void FontUndoCmd_LayerColors::redo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();

    pModel->SetLayerColors(m_iLayerId, m_NewTopColor, m_NewBotColor);

    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.UpdateActions();
}

void FontUndoCmd_LayerColors::undo()
{
    FontTableModel *pModel = m_pFontState->GetFontModel();

    pModel->SetLayerColors(m_iLayerId, m_PrevTopColor, m_PrevBotColor);

    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }

    m_WidgetFontRef.UpdateActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerOrder::FontUndoCmd_LayerOrder(FontWidget &widgetFont, QComboBox *pCmbStates, FontTableView *pFontTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                        m_WidgetFontRef(widgetFont),
                                                                                                                                                                                                                        m_pCmbStates(pCmbStates),
                                                                                                                                                                                                                        m_pFontState(m_pCmbStates->currentData().value<FontWidgetState *>()),
                                                                                                                                                                                                                        m_pFontTableView(pFontTableView),
                                                                                                                                                                                                                        m_iPrevRowIndex(iPrevRowIndex),
                                                                                                                                                                                                                        m_iNewRowIndex(iNewRowIndex)
{
    if(m_iPrevRowIndex > m_iNewRowIndex)
        setText("Order Layer Upwards");
    else
        setText("Order Layer Downwards");
}

/*virtual*/ FontUndoCmd_LayerOrder::~FontUndoCmd_LayerOrder()
{
}

void FontUndoCmd_LayerOrder::redo()
{
    FontTableModel *pModel = static_cast<FontTableModel *>(m_pFontTableView->model());
    
    int iOffset = m_iNewRowIndex - m_iPrevRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iPrevRowIndex);
    else
        pModel->MoveRowUp(m_iPrevRowIndex);
        
    m_pFontTableView->selectRow(m_iNewRowIndex);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

void FontUndoCmd_LayerOrder::undo()
{
    FontTableModel *pModel = static_cast<FontTableModel *>(m_pFontTableView->model());
    
    int iOffset = m_iPrevRowIndex - m_iNewRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iNewRowIndex);
    else
        pModel->MoveRowUp(m_iNewRowIndex);
    
    m_pFontTableView->selectRow(m_iPrevRowIndex);
    
    for(int i = 0; i < m_pCmbStates->count(); ++i)
    {
        if(m_pCmbStates->itemData(i).value<FontWidgetState *>() == m_pFontState)
        {
            m_pCmbStates->setCurrentIndex(i);
            break;
        }
    }
    
    m_WidgetFontRef.GeneratePreview();
    m_WidgetFontRef.UpdateActions();
}

