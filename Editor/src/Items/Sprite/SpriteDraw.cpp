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

SpriteDraw::SpriteDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
	m_pSprite = new HySprite2d("", "+GuiPreview", this);
	m_pSwapSprite = new HySprite2d("", "+GuiPreview", this);
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
	delete m_pSprite;
	delete m_pSwapSprite;
}

void SpriteDraw::PlayAnim(quint32 uiFrameIndex)
{
	if(m_pSprite->IsAnimReverse())
		m_pSprite->SetAnimCtrl(HYANIMCTRL_Reverse);
	else
		m_pSprite->SetAnimCtrl(HYANIMCTRL_DontReverse);

	m_pSprite->SetAnimCtrl(HYANIMCTRL_Reset);
	m_pSprite->SetAnimCtrl(HYANIMCTRL_Play);
	m_pSprite->SetFrame(uiFrameIndex);
}

void SpriteDraw::SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex)
{
	m_pSprite->SetState(uiStateIndex);
	m_pSprite->SetFrame(uiFrameIndex);
}

/*virtual*/ void SpriteDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_pSprite->IsAnimPaused())
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

	IDraw::OnKeyPressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->isAutoRepeat() == false &&
	   (pEvent->key() == Qt::Key_Left ||
		pEvent->key() == Qt::Key_Right ||
		pEvent->key() == Qt::Key_Up ||
		pEvent->key() == Qt::Key_Down))
	{
		if(pEvent->key() == Qt::Key_Left)
			HyGuiLog("left released", LOGTYPE_Normal);

		if(pEvent->key() == Qt::Key_Up)
			HyGuiLog("up released", LOGTYPE_Normal);

		// Submit the pending transform to the model and reset it
		if(m_pProjItem->GetWidget())
			static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTransform(m_vTranslateAmt);

		m_vTranslateAmt.setX(0);
		m_vTranslateAmt.setY(0);
	}


	IDraw::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMousePressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseReleaseEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDraw::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseMoveEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnApplyJsonData(HyJsonDoc &itemDataDocRef) /*override*/
{
	if(m_pSwapSprite->AcquireData() != nullptr)
	{
		// Clear whatever anim ctrl was set since it will only set the proper attributes from GuiOverrideData, leaving stale flags behind
		for(uint32 i = 0; i < m_pSwapSprite->GetNumStates(); ++i)
		{
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_DontLoop, i);
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_DontBounce, i);
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_DontReverse, i);
			m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_Play, i);
		}
	}

#undef GetObject
	HyJsonObj itemDataObj = itemDataDocRef.GetObject();

	m_pSwapSprite->GuiOverrideData<HySpriteData>(itemDataObj);
	m_pSwapSprite->SetAnimCtrl(HYANIMCTRL_Reset);
	
	SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
	m_pSwapSprite->SetAnimPause(pWidget->IsPlayingAnim() == false);
	m_pSwapSprite->Load();

	m_pSprite->Unload();
	HySprite2d *pTmpSwap = m_pSprite;
	m_pSprite = m_pSwapSprite;
	m_pSwapSprite = pTmpSwap;
}

/*virtual*/ void SpriteDraw::OnShow() /*override*/
{
	SetVisible(true);
}

/*virtual*/ void SpriteDraw::OnHide() /*override*/
{
	SetVisible(false);
}

/*virtual*/ void SpriteDraw::OnResizeRenderer() /*override*/
{
}

/*virtual*/ void SpriteDraw::OnUpdate() /*override*/
{
	SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
	if(pWidget == nullptr)
		return;
	
	m_pSprite->SetAnimPause(pWidget->IsPlayingAnim() == false);

	m_pSprite->pos.Set(m_vTranslateAmt.x(), m_vTranslateAmt.y());

	// NOTE: Data in sprite may be invalid/null because of GUI usage
	if(m_pSprite->AcquireData() == nullptr)
		return;
	
	if(m_pSprite->IsAnimPaused())
	{
		int iStateIndex, iFrameIndex;
		pWidget->GetSpriteInfo(iStateIndex, iFrameIndex);
		
		if(iStateIndex < 0)
			iStateIndex = 0;

		if(iFrameIndex < 0)
			iFrameIndex = 0;

		m_pSprite->SetState(static_cast<uint32>(iStateIndex));
		m_pSprite->SetFrame(static_cast<uint32>(iFrameIndex));
	}
	else
	{
		pWidget->SetSelectedFrame(m_pSprite->GetFrame());

		if(m_pSprite->IsAnimFinished())
			pWidget->StopPlayingAnim();
	}

	UpdateDrawStatus(QString("[") % QString::number(m_pSprite->GetFrameWidth()) % " " % QString::number(m_pSprite->GetFrameHeight()) % "]");
}
