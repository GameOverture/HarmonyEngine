/**************************************************************************
 *	TextUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2019 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TEXTUNDOCMDS_H
#define TEXTUNDOCMDS_H

#include "SubWidgets/TextFontManager.h"

#include "freetype-gl/freetype-gl.h"

#include <QUndoCommand>
#include <QComboBox>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_AddLayer : public QUndoCommand
{
	ProjectItem &			m_ItemRef;
	int						m_iStateIndex;

	QString					m_sFontName;
	rendermode_t			m_eRenderMode;
	float					m_fSize;
	float					m_fThickness;
	TextLayerHandle			m_hLayer;

public:
	TextUndoCmd_AddLayer(ProjectItem &itemRef, int iStateIndex, QString sFontName, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_AddLayer();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_RemoveLayer : public QUndoCommand
{
	ProjectItem &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;

public:
	TextUndoCmd_RemoveLayer(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_RemoveLayer();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_LayerRenderMode : public QUndoCommand
{
	ProjectItem &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	rendermode_t			m_ePrevRenderMode;
	rendermode_t			m_eNewRenderMode;

public:
	TextUndoCmd_LayerRenderMode(ProjectItem &itemRef, int iStateIndex, TextLayerHandle hLayer, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_LayerRenderMode();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class TextUndoCmd_LayerOutlineThickness : public QUndoCommand
//{
//	ProjectItem &       m_ItemRef;
//	int                 m_iStateIndex;
//
//	int                 m_iLayerId;
//	float               m_fPrevThickness;
//	float               m_fNewThickness;
//
//public:
//	TextUndoCmd_LayerOutlineThickness(ProjectItem &itemRef, int iStateIndex, int iLayerId, float fPrevThickness, float fNewThickness, QUndoCommand *pParent = 0);
//	virtual ~TextUndoCmd_LayerOutlineThickness();
//
//	void redo() override;
//	void undo() override;
//};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class TextUndoCmd_LayerColors : public QUndoCommand
//{
//	ProjectItem &       m_ItemRef;
//	int                 m_iStateIndex;
//
//	int                 m_iLayerId;
//	QColor              m_PrevTopColor;
//	QColor              m_PrevBotColor;
//	QColor              m_NewTopColor;
//	QColor              m_NewBotColor;
//
//public:
//	TextUndoCmd_LayerColors(ProjectItem &itemRef, int iStateIndex, int iLayerId, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent = 0);
//	virtual ~TextUndoCmd_LayerColors();
//
//	void redo() override;
//	void undo() override;
//};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class TextUndoCmd_LayerOrder : public QUndoCommand
//{
//	ProjectItem &       m_ItemRef;
//	int                 m_iStateIndex;
//
//	FontTableView *     m_pFontTableView;
//	int                 m_iPrevRowIndex;
//	int                 m_iNewRowIndex;
//
//public:
//	TextUndoCmd_LayerOrder(ProjectItem &itemRef, int iStateIndex, FontTableView *pTableView, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent = 0);
//	virtual ~TextUndoCmd_LayerOrder();
//
//	void redo() override;
//	void undo() override;
//};

#endif // TEXTUNDOCMDS_H
