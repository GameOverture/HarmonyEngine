/**************************************************************************
 *	GfxPrimitiveView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GfxPrimitiveView_H
#define GfxPrimitiveView_H

#include "Global.h"
#include "IGfxEditView.h"
#include "GfxShapeHyView.h"
#include "GfxChainView.h"

class GfxPrimitiveView : public IGfxEditView
{
	GfxShapeHyView				m_ShapeView;
	GfxChainView				m_ChainView;

public:
	GfxPrimitiveView(HyEntity2d *pParent = nullptr);
	virtual ~GfxPrimitiveView();

	virtual void SyncColor() override;
	virtual void ClearPreview() override;

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) override;
	virtual void OnSyncPreview(EditModeState eEditModeState, EditModeAction eResult, int iGrabPointIndex, glm::vec2 vDragDelta) override;
};

#endif // GfxPrimitiveView_H
