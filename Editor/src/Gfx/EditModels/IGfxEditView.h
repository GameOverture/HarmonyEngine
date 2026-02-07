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
#include "GfxGrabPointView.h"

class IGfxEditView : public HyEntity2d
{
protected:
	IGfxEditModel *				m_pModel;

	QList<GfxGrabPointView *>	m_GrabPointViewList;	// Project to window coordinates
	GfxGrabPointView			m_CenterGrabPoint;

public:
	IGfxEditView(HyEntity2d *pParent = nullptr);
	virtual ~IGfxEditView();

	IGfxEditModel *GetModel() const;
	void SetModel(IGfxEditModel *pModel);

	virtual void SyncColor() = 0;
	void SyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction);
	void SyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta);
	virtual void ClearPreview() = 0;

	void ClearGrabPoints();

protected:
	virtual void OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) = 0;
	virtual void OnSyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta) = 0;
};

#endif // IGfxEditView_H
