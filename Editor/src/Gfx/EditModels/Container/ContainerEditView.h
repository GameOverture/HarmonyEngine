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

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;
};

#endif // ContainerEditView_H
