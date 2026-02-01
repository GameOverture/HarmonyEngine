/**************************************************************************
 *	IGfxEditView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IGfxEditView_H
#define IGfxEditView_H

#include "Global.h"
#include "IGfxEditModel.h"

class GfxGrabPointView;

class IGfxEditView : public HyEntity2d
{
protected:
	IGfxEditModel *				m_pModel;

	QList<GfxGrabPointView *>	m_GrabPointViewList;	// Project to window coordinates

public:
	IGfxEditView(HyEntity2d *pParent = nullptr);
	virtual ~IGfxEditView();

	IGfxEditModel *GetModel() const;
	void SetModel(IGfxEditModel *pModel);

	virtual void RefreshColor() = 0;
	void RefreshView(ShapeMouseMoveResult eResult, bool bMouseDown);

	void ClearGrabPoints();

protected:
	virtual void DoRefreshView(ShapeMouseMoveResult eResult, bool bMouseDown) = 0;
};

#endif // IGfxEditView_H
