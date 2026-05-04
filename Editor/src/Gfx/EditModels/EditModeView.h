/**************************************************************************
 *	EditModeView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EditModeView_H
#define EditModeView_H

#include "Global.h"
#include "EditModeModel.h"
#include "GfxGrabPointView.h"

class EditModeView : public HyEntity2d
{
protected:
	EditModeModel *				m_pModel;

	HyPrimitive2d				m_CameraPrim;
	HyPrimitive2d				m_ScenePrim;

	QList<GfxGrabPointView *>	m_GrabPointViewList;	// Project to window coordinates
	GfxGrabPointView			m_CenterGrabPoint;

public:
	EditModeView(HyEntity2d *pParent = nullptr);
	virtual ~EditModeView();

	EditModeModel *GetModel() const;
	void SetModel(EditModeModel *pModel);

	void SyncColor();
	virtual void SyncWithModel(EditModeState eEditModeState, EditModeAction eEditModeAction);

	void ClearGrabPoints();
};

#endif // EditModeView_H
