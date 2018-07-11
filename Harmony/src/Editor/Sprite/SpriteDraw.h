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
	HySprite2d                              m_Sprite;

	QPoint                                  m_vTranslateAmt;


public:
	SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
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
	virtual void OnApplyJsonData(jsonxx::Value &valueRef) override;
	virtual void OnShow(IHyApplication &hyApp) override;
	virtual void OnHide(IHyApplication &hyApp) override;
	virtual void OnResizeRenderer() override;
	
	virtual void OnUpdate() override;
};

#endif // SPRITEDRAW_H