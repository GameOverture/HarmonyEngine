/**************************************************************************
 *	IEditModeView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IEditModeView_H
#define IEditModeView_H

#include "Global.h"
#include "EditModeModel.h"

class IEditModeView : public HyEntity2d
{
protected:
	IEditModeModel *			m_pModel;

public:
	IEditModeView(HyEntity2d *pParent = nullptr);
	virtual ~IEditModeView();

	IEditModeModel *GetModel() const;
	void SetModel(IEditModeModel *pModel);

	virtual void SyncWithModel(EditModeState eEditModeState) = 0;
};

#endif // IEditModeView_H
