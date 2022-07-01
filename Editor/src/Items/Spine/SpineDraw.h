/**************************************************************************
*	SpineDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINEDRAW_H
#define SPINEDRAW_H

#include "IDraw.h"

class SpineDraw : public IDraw
{
	HySpine2d				m_Spine;
	QList<HyTextureHandle>	m_hTextureList;

public:
	SpineDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~SpineDraw();

	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
};

#endif // SPINEDRAW_H
