/**************************************************************************
 *	IGfxShapeView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IGfxShapeView_H
#define IGfxShapeView_H

#include "Global.h"
#include "GfxShapeModel.h"

class IGfxShapeView
{
protected:
	GfxShapeModel *			m_pModel;

public:
	IGfxShapeView();
	virtual ~IGfxShapeView();

	GfxShapeModel *GetModel() const;
	void SetModel(GfxShapeModel *pModel);

	virtual void RefreshColor() = 0;
	virtual void RefreshView(ShapeMouseMoveResult eResult, bool bMouseDown) = 0;
};

#endif // IGfxShapeView_H
