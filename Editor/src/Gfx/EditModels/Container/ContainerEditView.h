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

public:
	ContainerEditView(bool bIsFixture, HyEntity2d *pParent = nullptr);
	virtual ~ContainerEditView();

	virtual void SyncColor() override;
	virtual void ClearPreview() override;

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;
	virtual void OnSyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta) override;
};

#endif // ContainerEditView_H
