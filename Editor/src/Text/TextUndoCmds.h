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
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	QString					m_sFontName;
	rendermode_t			m_eRenderMode;
	float					m_fSize;
	float					m_fThickness;
	TextLayerHandle			m_hLayer;

public:
	TextUndoCmd_AddLayer(ProjectItemData &itemRef, int iStateIndex, QString sFontName, rendermode_t eRenderMode, float fSize, float fThickness, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_AddLayer();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_RemoveLayer : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;

public:
	TextUndoCmd_RemoveLayer(ProjectItemData &itemRef, int iStateIndex, TextLayerHandle hLayer, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_RemoveLayer();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_FontChange : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	QString					m_sPrevFont;
	QString					m_sNewFont;

public:
	TextUndoCmd_FontChange(ProjectItemData &itemRef, int iStateIndex, QString sNewFont, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_FontChange();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_FontSizeChange : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	float					m_fPrevSize;
	float					m_fNewSize;

public:
	TextUndoCmd_FontSizeChange(ProjectItemData &itemRef, int iStateIndex, float fNewSize, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_FontSizeChange();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_LayerRenderMode : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	rendermode_t			m_ePrevRenderMode;
	rendermode_t			m_eNewRenderMode;

public:
	TextUndoCmd_LayerRenderMode(ProjectItemData &itemRef, int iStateIndex, TextLayerHandle hLayer, rendermode_t ePrevMode, rendermode_t eNewMode, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_LayerRenderMode();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_LayerOutlineThickness : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	float					m_fPrevThickness;
	float					m_fNewThickness;

public:
	TextUndoCmd_LayerOutlineThickness(ProjectItemData &itemRef, int iStateIndex, TextLayerHandle hLayer, float fPrevThickness, float fNewThickness, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_LayerOutlineThickness();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_LayerColors : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	QColor					m_PrevTopColor;
	QColor					m_PrevBotColor;
	QColor					m_NewTopColor;
	QColor					m_NewBotColor;

public:
	TextUndoCmd_LayerColors(ProjectItemData &itemRef, int iStateIndex, TextLayerHandle hLayer, QColor prevTopColor, QColor prevBotColor, QColor newTopColor, QColor newBotColor, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_LayerColors();

	void redo() override;
	void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextUndoCmd_LayerOrder : public QUndoCommand
{
	ProjectItemData &			m_ItemRef;
	int						m_iStateIndex;

	TextLayerHandle			m_hLayer;
	int						m_iPrevRowIndex;
	int						m_iNewRowIndex;

public:
	TextUndoCmd_LayerOrder(ProjectItemData &itemRef, int iStateIndex, TextLayerHandle hLayer, int iPrevRowIndex, int iNewRowIndex, QUndoCommand *pParent = nullptr);
	virtual ~TextUndoCmd_LayerOrder();

	void redo() override;
	void undo() override;
};

#endif // TEXTUNDOCMDS_H
