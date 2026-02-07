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

class GfxChainView : public IGfxEditView
{
	friend class GfxPrimitiveView;

	HyPrimitive2d				m_PrimOutline;			// This primitive is set as line chain. It forms an outline unaffected by camera zoom, using window coordinates
	QList<HyPrimitive2d *>		m_PrimPreviewList;		// A list of primitives used for previewing transformations before they are committed. Uses world/camera coordinates

public:
	GfxChainView(HyEntity2d *pParent = nullptr);
	virtual ~GfxChainView();

	virtual void SyncColor() override;
	virtual void ClearPreview() override;

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;
	virtual void OnSyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta) override;
};

#endif // GfxChainView_H
