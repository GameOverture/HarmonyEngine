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

#include "ProjectItem.h"
#include "FontWidget.h"
#include "FontModels.h"
#include "FontModelView.h"
#include "Global.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_AddLayer::FontUndoCmd_AddLayer(ProjectItem &itemRef, int iStateIndex, rendermode_t eRenderMode, int iSize, float fThickness, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                            m_ItemRef(itemRef),
                                                                                                                                                                            m_iStateIndex(iStateIndex),
                                                                                                                                                                            m_eRenderMode(eRenderMode),
                                                                                                                                                                            m_iSize(iSize),
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
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    if(m_iId == -1)
        m_iId = pModel->AddNewLayer(m_eRenderMode, m_iSize, m_fThickness);
    else
        pModel->ReAddLayer(m_iId);

    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

void FontUndoCmd_AddLayer::undo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    pModel->RemoveLayer(m_iId);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_RemoveLayer::FontUndoCmd_RemoveLayer(ProjectItem &itemRef, int iStateIndex, int iId, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                    m_ItemRef(itemRef),
                                                                                                                                    m_iStateIndex(iStateIndex),
                                                                                                                                    m_iId(iId)
{
    setText("Remove Font Layer");
}

/*virtual*/ FontUndoCmd_RemoveLayer::~FontUndoCmd_RemoveLayer()
{
}

void FontUndoCmd_RemoveLayer::redo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    pModel->RemoveLayer(m_iId);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

void FontUndoCmd_RemoveLayer::undo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    pModel->ReAddLayer(m_iId);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerRenderMode::FontUndoCmd_LayerRenderMode(ProjectItem &itemRef, int iStateIndex, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                m_ItemRef(itemRef),
                                                                                                                                                                                                m_iStateIndex(iStateIndex),
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
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_eNewRenderMode);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

void FontUndoCmd_LayerRenderMode::undo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    pModel->SetLayerRenderMode(m_iLayerId, m_ePrevRenderMode);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerOutlineThickness::FontUndoCmd_LayerOutlineThickness(ProjectItem &itemRef, int iStateIndex, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                                        m_ItemRef(itemRef),
                                                                                                                                                                                                        m_iStateIndex(iStateIndex),
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
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fNewThickness);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

void FontUndoCmd_LayerOutlineThickness::undo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    pModel->SetLayerOutlineThickness(m_iLayerId, m_fPrevThickness);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerColors::FontUndoCmd_LayerColors(ProjectItem &itemRef, int iStateIndex, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent /*= 0*/) :    QUndoCommand(pParent),
                                                                                                                                                                                                                            m_ItemRef(itemRef),
                                                                                                                                                                                                                            m_iStateIndex(iStateIndex),
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
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();

    pModel->SetLayerColors(m_iLayerId, m_NewTopColor, m_NewBotColor);

    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

void FontUndoCmd_LayerColors::undo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();

    pModel->SetLayerColors(m_iLayerId, m_PrevTopColor, m_PrevBotColor);

    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerOrder::FontUndoCmd_LayerOrder(ProjectItem &itemRef, int iStateIndex, FontTableView *pTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                                                        m_ItemRef(itemRef),
                                                                                                                                                                                        m_iStateIndex(iStateIndex),
                                                                                                                                                                                        m_pFontTableView(pTableView),
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
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    int iOffset = m_iNewRowIndex - m_iPrevRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iPrevRowIndex);
    else
        pModel->MoveRowUp(m_iPrevRowIndex);
        
    //m_pFontTableView->selectRow(m_iNewRowIndex);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

void FontUndoCmd_LayerOrder::undo()
{
    FontStateLayersModel *pModel = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(m_iStateIndex))->GetFontLayersModel();
    
    int iOffset = m_iPrevRowIndex - m_iNewRowIndex;
    if(iOffset > 0)
        pModel->MoveRowDown(m_iNewRowIndex);
    else
        pModel->MoveRowUp(m_iNewRowIndex);
    
    //m_pFontTableView->selectRow(m_iPrevRowIndex);
    
    m_ItemRef.FocusWidgetState(m_iStateIndex);
}

