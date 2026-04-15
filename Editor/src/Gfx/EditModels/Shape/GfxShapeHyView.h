/**************************************************************************
 *	GfxShapeHyView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GfxShapeHyView_H
#define GfxShapeHyView_H

#include "Global.h"
#include "IGfxEditView.h"

class GfxShapeModel;

class GfxShapeHyView : public IGfxEditView
{
	friend class GfxPrimitiveView;

	bool						m_bIsFixture;

	enum Layer
	{
		LAYER_Fill = 0,
		LAYER_Outline,
		LAYER_Preview
	};

public:
	GfxShapeHyView(bool bIsFixture, HyEntity2d *pParent = nullptr);
	virtual ~GfxShapeHyView();

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;

	void DoGrabPointPreview(EditModeState eEditModeState, EditModeAction eEditModeAction);

	GfxShapeModel *GetShapeModel();
};

#endif // GfxShapeHyView_H
