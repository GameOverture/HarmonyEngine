/**************************************************************************
 *	TextUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2019 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TextUndoCmds.h"
#include "ProjectItem.h"
#include "TextModel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_AddLayer::TextUndoCmd_AddLayer(ProjectItem &itemRef, int iStateIndex, QString sFontName, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_sFontName(sFontName),
	m_eRenderMode(eRenderMode),
	m_fSize(fSize),
	m_fThickness(fThickness),
	m_hFont(TEXTFONTHANDLE_NotUsed)
{
	setText("Add Text Layer");
}

/*virtual*/ TextUndoCmd_AddLayer::~TextUndoCmd_AddLayer()
{
}

void TextUndoCmd_AddLayer::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	if(m_hFont == TEXTFONTHANDLE_NotUsed)
		m_hFont = pModel->AddNewLayer(m_sFontName, m_eRenderMode, m_fSize, m_fThickness);
	else
		pModel->ReAddLayer(m_hFont);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hFont);
}

void TextUndoCmd_AddLayer::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->RemoveLayer(m_hFont);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hFont);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_RemoveLayer::TextUndoCmd_RemoveLayer(ProjectItem &itemRef, int iStateIndex, TextFontHandle hFont, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_hFont(hFont)
{
	setText("Remove Text Layer");
}

/*virtual*/ TextUndoCmd_RemoveLayer::~TextUndoCmd_RemoveLayer()
{
}

void TextUndoCmd_RemoveLayer::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->RemoveLayer(m_hFont);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hFont);
}

void TextUndoCmd_RemoveLayer::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->ReAddLayer(m_hFont);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hFont);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerRenderMode::FontUndoCmd_LayerRenderMode(ProjectItem &itemRef, int iStateIndex, int iLayerId, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
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
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->SetLayerRenderMode(m_iLayerId, m_eNewRenderMode);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_iLayerId);
}

void FontUndoCmd_LayerRenderMode::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->SetLayerRenderMode(m_iLayerId, m_ePrevRenderMode);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_iLayerId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerOutlineThickness::FontUndoCmd_LayerOutlineThickness(ProjectItem &itemRef, int iStateIndex, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
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
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->SetLayerOutlineThickness(m_iLayerId, m_fNewThickness);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_iLayerId);
}

void FontUndoCmd_LayerOutlineThickness::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->SetLayerOutlineThickness(m_iLayerId, m_fPrevThickness);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_iLayerId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerColors::FontUndoCmd_LayerColors(ProjectItem &itemRef, int iStateIndex, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
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
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);

	pModel->SetLayerColors(m_iLayerId, m_NewTopColor, m_NewBotColor);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_iLayerId);
}

void FontUndoCmd_LayerColors::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);

	pModel->SetLayerColors(m_iLayerId, m_PrevTopColor, m_PrevBotColor);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_iLayerId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontUndoCmd_LayerOrder::FontUndoCmd_LayerOrder(ProjectItem &itemRef, int iStateIndex, FontTableView *pTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
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
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	int iOffset = m_iNewRowIndex - m_iPrevRowIndex;
	if(iOffset > 0)
		pModel->MoveRowDown(m_iPrevRowIndex);
	else
		pModel->MoveRowUp(m_iPrevRowIndex);
		
	m_ItemRef.FocusWidgetState(m_iStateIndex, pModel->GetLayerId(m_iNewRowIndex));
}

void FontUndoCmd_LayerOrder::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	int iOffset = m_iPrevRowIndex - m_iNewRowIndex;
	if(iOffset > 0)
		pModel->MoveRowDown(m_iNewRowIndex);
	else
		pModel->MoveRowUp(m_iNewRowIndex);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, pModel->GetLayerId(m_iPrevRowIndex));
}

