/**************************************************************************
 *	GfxChainView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GfxChainView_H
#define GfxChainView_H

#include "Global.h"
#include "IGfxEditView.h"

class GfxChainModel;

class GfxChainView : public IGfxEditView
{
	friend class GfxPrimitiveView;

	// 'm_DataPrim' is set as line chain. It forms an outline unaffected by camera zoom, using window coordinates

public:
	GfxChainView(HyEntity2d *pParent = nullptr);
	virtual ~GfxChainView();

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;

	GfxChainModel *GetChainModel();
};

#endif // GfxChainView_H
