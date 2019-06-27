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
	m_hLayer(HY_UNUSED_HANDLE)
{
	setText("Add Text Layer");
}

/*virtual*/ TextUndoCmd_AddLayer::~TextUndoCmd_AddLayer()
{
}

void TextUndoCmd_AddLayer::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	if(m_hLayer == HY_UNUSED_HANDLE)
		m_hLayer = pModel->AddNewLayer(m_sFontName, m_eRenderMode, m_fThickness, m_fSize);
	else
		pModel->ReAddLayer(m_hLayer);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

void TextUndoCmd_AddLayer::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->RemoveLayer(m_hLayer);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_RemoveLayer::TextUndoCmd_RemoveLayer(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_hLayer(hLayer)
{
	setText("Remove Text Layer");
}

/*virtual*/ TextUndoCmd_RemoveLayer::~TextUndoCmd_RemoveLayer()
{
}

void TextUndoCmd_RemoveLayer::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->RemoveLayer(m_hLayer);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

void TextUndoCmd_RemoveLayer::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	pModel->ReAddLayer(m_hLayer);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TextUndoCmd_FontChange::TextUndoCmd_FontChange(ProjectItem &itemRef, int iStateIndex, QString sNewFont, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_sPrevFont(static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex)->GetFont()),
	m_sNewFont(sNewFont)
{
	setText("Change Font");
}

/*virtual*/ TextUndoCmd_FontChange::~TextUndoCmd_FontChange()
{
}

void TextUndoCmd_FontChange::redo() /*override*/
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->SetFont(m_sNewFont);

	m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
}

void TextUndoCmd_FontChange::undo() /*override*/
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->SetFont(m_sPrevFont);

	m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_FontSizeChange::TextUndoCmd_FontSizeChange(ProjectItem &itemRef, int iStateIndex, float fNewSize, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_fPrevSize(static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex)->GetFontSize()),
	m_fNewSize(fNewSize)
{
	setText("Change Font Size");
}
/*virtual*/ TextUndoCmd_FontSizeChange::~TextUndoCmd_FontSizeChange()
{
}

void TextUndoCmd_FontSizeChange::redo() /*override*/
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->SetFontSize(m_fNewSize);

	m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
}

void TextUndoCmd_FontSizeChange::undo() /*override*/
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->SetFontSize(m_fPrevSize);

	m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_LayerRenderMode::TextUndoCmd_LayerRenderMode(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_hLayer(hLayer),
	m_ePrevRenderMode(ePrevMode),
	m_eNewRenderMode(eNewMode)
{
	setText("Layer Render Mode");
}

/*virtual*/ TextUndoCmd_LayerRenderMode::~TextUndoCmd_LayerRenderMode()
{
}

void TextUndoCmd_LayerRenderMode::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->setData(pModel->GetIndex(m_hLayer, TextLayersModel::COLUMN_Mode), m_eNewRenderMode);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

void TextUndoCmd_LayerRenderMode::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->setData(pModel->GetIndex(m_hLayer, TextLayersModel::COLUMN_Mode), m_ePrevRenderMode);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_LayerOutlineThickness::TextUndoCmd_LayerOutlineThickness(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, float fPrevThickness, float fNewThickness, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_hLayer(hLayer),
	m_fPrevThickness(fPrevThickness),
	m_fNewThickness(fNewThickness)
{
	setText("Stage Outline Thickness");
}

/*virtual*/ TextUndoCmd_LayerOutlineThickness::~TextUndoCmd_LayerOutlineThickness()
{
}

void TextUndoCmd_LayerOutlineThickness::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->setData(pModel->GetIndex(m_hLayer, TextLayersModel::COLUMN_Thickness), m_fNewThickness);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

void TextUndoCmd_LayerOutlineThickness::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->setData(pModel->GetIndex(m_hLayer, TextLayersModel::COLUMN_Thickness), m_fPrevThickness);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_LayerColors::TextUndoCmd_LayerColors(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_hLayer(hLayer),
	m_PrevTopColor(prevTopColor),
	m_PrevBotColor(prevBotColor),
	m_NewTopColor(newTopColor),
	m_NewBotColor(newBotColor)
{
	setText("Set Layer Colors");
}

/*virtual*/ TextUndoCmd_LayerColors::~TextUndoCmd_LayerColors()
{
}

void TextUndoCmd_LayerColors::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->GetFontManager().SetColors(m_hLayer, m_NewTopColor, m_NewBotColor);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

void TextUndoCmd_LayerColors::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	pModel->GetFontManager().SetColors(m_hLayer, m_PrevTopColor, m_PrevBotColor);

	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextUndoCmd_LayerOrder::TextUndoCmd_LayerOrder(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_ItemRef(itemRef),
	m_iStateIndex(iStateIndex),
	m_hLayer(hLayer),
	m_iPrevRowIndex(iPrevRowIndex),
	m_iNewRowIndex(iNewRowIndex)
{
	if(m_iPrevRowIndex > m_iNewRowIndex)
		setText("Order Layer Upwards");
	else
		setText("Order Layer Downwards");
}

/*virtual*/ TextUndoCmd_LayerOrder::~TextUndoCmd_LayerOrder()
{
}

void TextUndoCmd_LayerOrder::redo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	int iOffset = m_iNewRowIndex - m_iPrevRowIndex;
	if(iOffset > 0)
		pModel->MoveRowDown(m_iPrevRowIndex);
	else
		pModel->MoveRowUp(m_iPrevRowIndex);
		
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}

void TextUndoCmd_LayerOrder::undo()
{
	TextLayersModel *pModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(m_iStateIndex);
	
	int iOffset = m_iPrevRowIndex - m_iNewRowIndex;
	if(iOffset > 0)
		pModel->MoveRowDown(m_iNewRowIndex);
	else
		pModel->MoveRowUp(m_iNewRowIndex);
	
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_hLayer);
}
