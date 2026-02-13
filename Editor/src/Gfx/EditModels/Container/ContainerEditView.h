/**************************************************************************
 *	ContainerEditView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ContainerEditView_H
#define ContainerEditView_H

#include "Global.h"
#include "IGfxEditView.h"

class ContainerEditView : public IGfxEditView
{
	friend class GfxPrimitiveView;

	bool						m_bIsFixture;

	HyPrimitive2d				m_PrimOutline;			// This primitive is set as line chain. It forms an outline unaffected by camera zoom, using window coordinates
	QList<HyPrimitive2d *>		m_PrimList;				// A list of primitives that represents how the shape is currently stored in data. Uses world/camera coordinates. Usually just one primitive, but could be multiple for complex polygons

	QList<HyPrimitive2d *>		m_PrimPreviewList;		// A list of primitives used for previewing transformations before they are committed. Uses world/camera coordinates

public:
	ContainerEditView(bool bIsFixture, HyEntity2d *pParent = nullptr);
	virtual ~ContainerEditView();

	virtual void SyncColor() override;
	virtual void ClearPreview() override;

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;
	virtual void OnSyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta) override;

	void ClearPrimitives();

	void DoGrabPointPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta);
};

#endif // ContainerEditView_H
