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
	void SyncModel(EditModeState eEditModeState, EditModeAction eResult);
	void SyncPreview(EditModeState eEditModeState, EditModeAction eResult, int iGrabPointIndex, glm::vec2 vDragDelta);

	void ClearGrabPoints();

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) = 0;
	virtual void OnSyncPreview(EditModeState eEditModeState, EditModeAction eResult, int iGrabPointIndex, glm::vec2 vDragDelta) = 0;
};

#endif // IGfxEditView_H
