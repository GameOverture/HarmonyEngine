/**************************************************************************
*	SpriteDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteModels.h"

class SpriteDraw : public IDraw
{
	HySprite2d *			m_pSprite;		// The currently displayed sprite
	HySprite2d *			m_pSwapSprite;	// SwapSprite exists because we need to call Unload() before GuiOverrideData<>() incase 'required atlas indices' will change, but we don't want HyAssets to unload & reload the sprite's textures that are unaffected

	QPoint					m_vTranslateAmt;

public:
	SpriteDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~SpriteDraw();

	void PlayAnim(quint32 uiFrameIndex);
	void SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex);

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;

protected:
	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) override;
	virtual void OnResizeRenderer() override;
	
	virtual void OnUpdate() override;
};

#endif // SPRITEDRAW_H
