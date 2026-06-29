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
#include "IEditModeView.h"
#include "GfxGrabPointView.h"

class EditModeView : public IEditModeView
{
protected:
	HyPrimitive2d				m_CameraPrim;
	HyPrimitive2d				m_ScenePrim;

	QList<GfxGrabPointView *>	m_GrabPointViewList;	// Project to window coordinates
	GfxGrabPointView			m_CenterGrabPoint;

public:
	EditModeView(HyEntity2d *pParent = nullptr);
	virtual ~EditModeView();

	void SyncColor();
	virtual void SyncWithModel(EditModeState eEditModeState) override;

	void ClearGrabPoints();
};

#endif // EditModeView_H
