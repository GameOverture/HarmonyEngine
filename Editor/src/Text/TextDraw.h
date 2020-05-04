/**************************************************************************
*	TextDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTDRAW_H
#define TEXTDRAW_H

#include "IDraw.h"

class TextDraw : public IDraw
{
	HyText2d			m_Text;
	HyTextureHandle		m_hTexture;

public:
	TextDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~TextDraw();

	void SetTextState(uint uiStateIndex);

	virtual void OnApplyJsonData(jsonxx::Object &itemDataObjRef) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
};

#endif // TEXTDRAW_H
