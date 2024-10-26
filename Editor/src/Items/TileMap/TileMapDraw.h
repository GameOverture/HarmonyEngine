/**************************************************************************
*	TileMapDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TILEMAPDRAW_H
#define TILEMAPDRAW_H

#include "IDrawEx.h"
#include "TileMapModels.h"

class TileMapDraw : public IDrawEx
{
public:
	TileMapDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~TileMapDraw();

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;

protected:
	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) override;
	virtual void OnResizeRenderer() override;
	
	virtual void OnUpdate() override;

	virtual void OnRequestSelection(QList<IDrawExItem *> selectionList) override;
	virtual void OnPerformTransform() override;
};

#endif // TILEMAPDRAW_H
