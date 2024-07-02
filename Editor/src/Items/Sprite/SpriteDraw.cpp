/**************************************************************************
*	SpriteDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "SpriteDraw.h"
#include "SpriteWidget.h"
#include "SpriteUndoCmds.h"
#include "Project.h"

#include <QKeyEvent>

void SpriteDraw::SpriteDrawItem::OnApplyJsonData(HyJsonDoc &itemDataDocRef, bool bIsAnimPlaying)
{
	if(m_pSwapSprite->AcquireData() != nullptr)
	{
		// Clear whatever anim ctrl was set since it will only set the proper attributes from GuiOverrideData, leaving stale flags behind
		for(uint32 i = 0; i < m_pSwapSprite->GetNumStates(); ++i)
		{
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_DontLoop, i);
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_DontBounce, i);
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_DontReverse, i);
		}
	}

#undef GetObject
	HyJsonObj itemDataObj = itemDataDocRef.GetObject();

	m_pSwapSprite->GuiOverrideData<HySpriteData>(itemDataObj);
	m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_Reset);

	m_pSwapSprite->SetAnimPause(bIsAnimPlaying == false);
	m_pSwapSprite->Load();

	m_pSprite->Unload();
	HySprite2d *pTmpSwap = m_pSprite;
	m_pSprite = m_pSwapSprite;
	m_pSwapSprite = pTmpSwap;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteDraw::SpriteDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDrawEx(pProjItem, initFileDataRef),
	m_pSpriteDrawItem(nullptr)
{
	m_pSpriteDrawItem = new SpriteDrawItem(this);
	m_ItemList.push_back(m_pSpriteDrawItem);
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

void SpriteDraw::PlayAnim(quint32 uiFrameIndex)
{
	HySprite2d *pSprite = static_cast<HySprite2d *>(m_pSpriteDrawItem->GetHyNode());

	if(pSprite->IsAnimReverse())
		pSprite->SetAnimCtrl(HYANIMCTRL_Reverse);
	else
		pSprite->SetAnimCtrl(HYANIMCTRL_DontReverse);

	pSprite->SetAnimCtrl(HYANIMCTRL_ResetAndPlay);
	pSprite->SetFrame(uiFrameIndex);
}

void SpriteDraw::SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex)
{
	HySprite2d *pSprite = static_cast<HySprite2d *>(m_pSpriteDrawItem->GetHyNode());

	pSprite->SetState(uiStateIndex);
	pSprite->SetFrame(uiFrameIndex);
}

/*virtual*/ void SpriteDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(static_cast<HySprite2d *>(m_pSpriteDrawItem->GetHyNode())->IsAnimPaused())
	{
		if(pEvent->key() == Qt::Key_Left)
			m_vTranslateAmt.setX(m_vTranslateAmt.x() - 1);
		if(pEvent->key() == Qt::Key_Right)
			m_vTranslateAmt.setX(m_vTranslateAmt.x() + 1);
		if(pEvent->key() == Qt::Key_Up)
			m_vTranslateAmt.setY(m_vTranslateAmt.y() + 1);
		if(pEvent->key() == Qt::Key_Down)
			m_vTranslateAmt.setY(m_vTranslateAmt.y() - 1);
	}

	IDrawEx::OnKeyPressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->isAutoRepeat() == false &&
	   (pEvent->key() == Qt::Key_Left ||
		pEvent->key() == Qt::Key_Right ||
		pEvent->key() == Qt::Key_Up ||
		pEvent->key() == Qt::Key_Down))
	{
		// Submit the pending transform to the model and reset it
		if(m_pProjItem->GetWidget())
			static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTransform(m_vTranslateAmt);

		m_vTranslateAmt.setX(0);
		m_vTranslateAmt.setY(0);
	}

	IDrawEx::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMousePressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseReleaseEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseMoveEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnApplyJsonData(HyJsonDoc &itemDataDocRef) /*override*/
{
	SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
	if(pWidget == nullptr)
	{
		HyGuiLog("SpriteDraw::OnApplyJsonData() - pWidget is nullptr", LOGTYPE_Error);
		return;
	}

	m_pSpriteDrawItem->OnApplyJsonData(itemDataDocRef, pWidget->IsPlayingAnim());
}

/*virtual*/ void SpriteDraw::OnResizeRenderer() /*override*/
{
}

/*virtual*/ void SpriteDraw::OnUpdate() /*override*/
{
	IDrawEx::OnUpdate();

	SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
	if(pWidget == nullptr)
		return;

	HySprite2d *pSprite = static_cast<HySprite2d *>(m_pSpriteDrawItem->GetHyNode());
	
	pSprite->SetAnimPause(pWidget->IsPlayingAnim() == false);

	pSprite->pos.Set(m_vTranslateAmt.x(), m_vTranslateAmt.y());

	// NOTE: Data in sprite may be invalid/null because of GUI usage
	if(pSprite->AcquireData() == nullptr)
		return;
	
	if(pSprite->IsAnimPaused())
	{
		int iStateIndex, iFrameIndex;
		pWidget->GetSpriteInfo(iStateIndex, iFrameIndex);
		
		if(iStateIndex < 0)
			iStateIndex = 0;

		if(iFrameIndex < 0)
			iFrameIndex = 0;

		pSprite->SetState(static_cast<uint32>(iStateIndex));
		pSprite->SetFrame(static_cast<uint32>(iFrameIndex));
	}
	else
	{
		pWidget->SetSelectedFrame(pSprite->GetFrame());

		if(pSprite->IsAnimFinished())
			pWidget->StopPlayingAnim();
	}

	UpdateDrawStatus(QString("[") % QString::number(pSprite->GetFrameWidth()) % " " % QString::number(pSprite->GetFrameHeight()) % "]");
}

/*virtual*/ void SpriteDraw::OnRequestSelection(QList<IDrawExItem *> selectionList) /*override*/
{
	m_pSpriteDrawItem->SetSelected(selectionList.contains(m_pSpriteDrawItem));
	m_SelectedItemList = selectionList;
}

/*virtual*/ void SpriteDraw::OnPerformTransform() /*override*/
{
	if(m_pProjItem->GetWidget())
		static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTransform(QPoint(m_ActiveTransform.pos.GetX(), m_ActiveTransform.pos.GetY()));

	m_vTranslateAmt.setX(0);
	m_vTranslateAmt.setY(0);
}
