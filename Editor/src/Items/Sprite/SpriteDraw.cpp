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
	m_Sprite.Init("", "+GuiPreview", this);
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

void SpriteDraw::PlayAnim(quint32 uiFrameIndex)
{
	if(m_Sprite.IsAnimReverse())
		m_Sprite.SetAnimCtrl(HYANIMCTRL_Reverse);
	else
		m_Sprite.SetAnimCtrl(HYANIMCTRL_DontReverse);

	m_Sprite.SetAnimCtrl(HYANIMCTRL_Reset);
	m_Sprite.SetAnimCtrl(HYANIMCTRL_Play);
	m_Sprite.SetFrame(uiFrameIndex);
}

void SpriteDraw::SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex)
{
	m_Sprite.SetState(uiStateIndex);
	m_Sprite.SetFrame(uiFrameIndex);
}

/*virtual*/ void SpriteDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_Sprite.IsAnimPaused())
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
	if(m_Sprite.AcquireData() != nullptr)
	{
		// Clear whatever anim ctrl was set since it will only set the proper attributes from GuiOverrideData, leaving stale flags behind
		for(uint32 i = 0; i < m_Sprite.GetNumStates(); ++i)
		{
			m_Sprite.SetAnimCtrl(HYANIMCTRL_DontLoop, i);
			m_Sprite.SetAnimCtrl(HYANIMCTRL_DontBounce, i);
			m_Sprite.SetAnimCtrl(HYANIMCTRL_DontReverse, i);
			m_Sprite.SetAnimCtrl(HYANIMCTRL_Play, i);
		}
	}

#undef GetObject
	HyJsonObj itemDataObj = itemDataDocRef.GetObject();

	m_Sprite.GuiOverrideData<HySpriteData>(itemDataObj);
	m_Sprite.SetAnimCtrl(HYANIMCTRL_Reset);
	
	SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
	m_Sprite.SetAnimPause(pWidget->IsPlayingAnim() == false);

	m_Sprite.Load();
}

/*virtual*/ void SpriteDraw::OnShow() /*override*/
{
	SetVisible(true);
	m_Sprite.SetVisible(true);
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
	
	m_Sprite.SetAnimPause(pWidget->IsPlayingAnim() == false);

	m_Sprite.pos.Set(m_vTranslateAmt.x(), m_vTranslateAmt.y());

	// NOTE: Data in sprite may be invalid/null because of GUI usage
	if(m_Sprite.AcquireData() == nullptr)
		return;
	
	if(m_Sprite.IsAnimPaused())
	{
		int iStateIndex, iFrameIndex;
		pWidget->GetSpriteInfo(iStateIndex, iFrameIndex);
		
		if(iStateIndex < 0)
			iStateIndex = 0;

		if(iFrameIndex < 0)
			iFrameIndex = 0;

		m_Sprite.SetState(static_cast<uint32>(iStateIndex));
		m_Sprite.SetFrame(static_cast<uint32>(iFrameIndex));
	}
	else
	{
		pWidget->SetSelectedFrame(m_Sprite.GetFrame());

		if(m_Sprite.IsAnimFinished())
			pWidget->StopPlayingAnim();
	}

	UpdateDrawStatus(QString("[") % QString::number(m_Sprite.GetFrameWidth()) % " " % QString::number(m_Sprite.GetFrameHeight()) % "]");
}
