/**************************************************************************
*	SpineDraw.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2021 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#ifndef SPINEDRAW_H
#define SPINEDRAW_H

#include "IDraw.h"
#include "SpineModel.h"

class SpineDraw : public IDraw
{
	HySpine2d				m_Spine;
	QList<HyTextureHandle>	m_hTextureList;

public:
	SpineDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~SpineDraw();

	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) override;
	virtual void OnResizeRenderer() override;

	void SetCrossFadePreview(SpineCrossFade *pCrossFade);
};

#endif // SPINEDRAW_H
