/**************************************************************************
*	TileMapDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TileMapDraw.h"
#include "SpriteWidget.h"
#include "SpriteUndoCmds.h"
#include "Project.h"
#include "MainWindow.h"

#include <QKeyEvent>
#include <QGuiApplication>

TileMapDraw::TileMapDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDrawEx(pProjItem, initFileDataRef)
{
}

/*virtual*/ TileMapDraw::~TileMapDraw()
{
}

/*virtual*/ void TileMapDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	//if(pEvent->key() == Qt::Key_Left ||
	//	pEvent->key() == Qt::Key_Right ||
	//	pEvent->key() == Qt::Key_Up ||
	//	pEvent->key() == Qt::Key_Down)
	//{
	//	OnRequestSelection(QList<IDrawExItem *>() << m_pSpriteDrawItem);
	//}

	IDrawEx::OnKeyPressEvent(pEvent);
}

/*virtual*/ void TileMapDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDrawEx::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void TileMapDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMousePressEvent(pEvent);
}

/*virtual*/ void TileMapDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseReleaseEvent(pEvent);
}

/*virtual*/ void TileMapDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void TileMapDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseMoveEvent(pEvent);
}

/*virtual*/ void TileMapDraw::OnApplyJsonData(HyJsonDoc &itemDataDocRef) /*override*/
{
}

/*virtual*/ void TileMapDraw::OnResizeRenderer() /*override*/
{

}

/*virtual*/ void TileMapDraw::OnUpdate() /*override*/
{
	IDrawEx::OnUpdate();
}

/*virtual*/ void TileMapDraw::OnRequestSelection(QList<IDrawExItem *> selectionList) /*override*/
{
	//m_pSpriteDrawItem->SetSelected(selectionList.contains(m_pSpriteDrawItem));
	m_SelectedItemList = selectionList;
}

/*virtual*/ void TileMapDraw::OnPerformTransform() /*override*/
{
	//if(m_pProjItem->GetWidget())
	//{
	//	QPoint vOffsetAmt(static_cast<int>(HyMath::Round(m_ActiveTransform.pos.GetX())), static_cast<int>(HyMath::Round(m_ActiveTransform.pos.GetY())));

	//	Qt::KeyboardModifiers kbModifiers = QGuiApplication::queryKeyboardModifiers();
	//	if(kbModifiers.testFlag(Qt::ControlModifier)) // Applies the mouse drag as an OFFSET - Which matters if all frames are selected
	//		static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTranslate(vOffsetAmt, true);
	//	else // Apply the mouse drag as a POSITION - Which is applied to all frames if all frames are selected
	//	{
	//		QPoint vCurFrameOffset = static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->GetSelectedFrameOffset();
	//		static_cast<SpriteWidget *>(m_pProjItem->GetWidget())->ApplyTranslate(vCurFrameOffset + vOffsetAmt, false);
	//	}
	//}
}
