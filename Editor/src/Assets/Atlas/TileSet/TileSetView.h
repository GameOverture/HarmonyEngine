/**************************************************************************
*	TileSetView.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TILESETVIEW_H
#define TILESETVIEW_H

#include "CommonGfxView.h"

class AuxTileSet;
class TileSetScene;

class TileSetView : public CommonGfxView
{
	Q_OBJECT

	AuxTileSet *				m_pAuxTileSet;

	enum DragState
	{
		DRAGSTATE_None,
		DRAGSTATE_InitialPress,
		DRAGSTATE_MarqueeSelect,
		DRAGSTATE_ArrangingSelection,
		DRAGSTATE_Painting
	};
	DragState					m_eDragState;
	QPoint						m_ptDragStart; // In View Coordinates

public:
	TileSetView(QWidget *pParent = nullptr);
	virtual ~TileSetView();

	TileSetScene *GetScene() const;
	void SetScene(AuxTileSet *pAuxTileSet, TileSetScene *pTileSetScene);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *pEvent) override;

	//virtual void drawBackground(QPainter *pPainter, const QRectF &rect) override;
	//virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override;

	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;

private:
	void DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor textColor);
};

#endif // TILESETVIEW_H
