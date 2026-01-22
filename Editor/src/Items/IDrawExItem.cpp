/**************************************************************************
*	IDrawExItem.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "IDrawExItem.h"

IDrawExItem::IDrawExItem(HyEntity2d *pParent) :
	m_Transform(pParent)
{
	HideTransformCtrl();
}

/*virtual*/ IDrawExItem::~IDrawExItem()
{
}

GfxTransformCtrl &IDrawExItem::GetTransformCtrl()
{
	return m_Transform;
}

bool IDrawExItem::IsMouseInBounds()
{
	HyShape2d boundingShape;
	glm::mat4 transformMtx;
	ExtractTransform(boundingShape, transformMtx);

	glm::vec2 ptWorldMousePos;
	return HyEngine::Input().GetWorldMousePos(ptWorldMousePos) && boundingShape.TestPoint(ptWorldMousePos, transformMtx);
}

void IDrawExItem::RefreshTransform()
{
	HyShape2d boundingShape;
	glm::mat4 mtxShapeTransform;
	ExtractTransform(boundingShape, mtxShapeTransform);

	m_Transform.WrapTo(boundingShape, mtxShapeTransform);
}

void IDrawExItem::ShowTransformCtrl(bool bShowGrabPoints)
{
	m_Transform.Show(bShowGrabPoints);
}

void IDrawExItem::HideTransformCtrl()
{
	m_Transform.Hide();
}

void IDrawExItem::ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut)
{
	IHyBody2d *pHyBody = GetHyNode();
	pHyBody->CalcLocalBoundingShape(boundingShapeOut);
	transformMtxOut = GetHyNode()->GetSceneTransform(0.0f);
}
