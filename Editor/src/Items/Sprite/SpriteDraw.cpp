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
#include "MainWindow.h"

#include <QKeyEvent>
#include <QGuiApplication>

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

	m_pSprite->SetAllBoundsIncludeAlphaCrop(true);
	m_pSwapSprite->SetAllBoundsIncludeAlphaCrop(true);
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

	MainWindow::SetStatus(QString("Cropped Size [") % QString::number(pSprite->GetFrameWidth()) % " x " % QString::number(pSprite->GetFrameHeight()) % "]", 0);
}

/*virtual*/ void SpriteDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	IDrawEx::OnKeyPressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
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
}

/*virtual*/ void SpriteDraw::OnRequestSelection(QList<IDrawExItem *> selectionList) /*override*/
{
	m_pSpriteDrawItem->SetSelected(selectionList.contains(m_pSpriteDrawItem));
	m_SelectedItemList = selectionList;
}

/*virtual*/ void SpriteDraw::OnPerformTransform() /*override*/
{
	if(m_pProjItem->GetWidget())
	{
		QPoint vOffsetAmt(static_cast<int>(HyMath::Round(m_ActiveTransform.pos.GetX())), static_cast<int>(HyMath::Round(m_ActiveTransform.pos.GetY())));

		Qt::KeyboardModifiers kbModifiers = QGuiApplication::queryKeyboardModifiers();
		if(kbModifiers.testFlag(Qt::ControlModifier)) // Applies the mouse drag as an OFFSET - Which matters if all frames are selected
			static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTranslate(vOffsetAmt, true);
		else // Apply the mouse drag as a POSITION - Which is applied to all frames if all frames are selected
		{
			QPoint vCurFrameOffset = static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->GetSelectedFrameOffset();
			static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTranslate(vCurFrameOffset + vOffsetAmt, false);
		}
	}
}
