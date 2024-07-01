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

#include "IDrawEx.h"
#include "SpriteModels.h"

class SpriteDraw : public IDrawEx
{
	class SpriteDrawItem : public IDrawExItem
	{
		bool m_bIsSelected;
		HySprite2d *m_pSprite;		// The currently displayed sprite
		HySprite2d *m_pSwapSprite;	// SwapSprite exists because we need to call Unload() before GuiOverrideData<>() incase 'required atlas indices' will change, but we don't want HyAssets to unload & reload the sprite's textures that are unaffected

	public:
		SpriteDrawItem(SpriteDraw *pParent) : 
			IDrawExItem(pParent),
			m_bIsSelected(false),
			m_pSprite(nullptr),
			m_pSwapSprite(nullptr)
		{
			m_pSprite = new HySprite2d("", "+GuiPreview", pParent);
			m_pSwapSprite = new HySprite2d("", "+GuiPreview", pParent);
		}
		virtual ~SpriteDrawItem()
		{
			delete m_pSprite;
			delete m_pSwapSprite;
		}

		virtual IHyBody2d *GetHyNode() { return m_pSprite; }
		virtual bool IsSelected() { return m_bIsSelected; }
		void SetSelected(bool bSelected)
		{
			m_bIsSelected = bSelected;
			if(m_bIsSelected)
				ShowTransformCtrl(false);
			else
				HideTransformCtrl();
		}

		void OnApplyJsonData(HyJsonDoc &itemDataDocRef, bool bIsAnimPlaying);
	};
	SpriteDrawItem			m_Sprite;

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

	virtual void OnRequestSelection(QList<IDrawExItem *> selectionList) override;
	virtual void OnPerformTransform() override;
};

#endif // SPRITEDRAW_H
