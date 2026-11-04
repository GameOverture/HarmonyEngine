/**************************************************************************
*	TextDraw.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2019 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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

	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) override;
	virtual void OnResizeRenderer() override;

private:
	void SetPreviewText();
};

#endif // TEXTDRAW_H
