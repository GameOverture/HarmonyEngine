/**************************************************************************
 *	IPolygon2dView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IGfxShapeView.h"
#include "GfxGrabPointView.h"

IPolygon2dView::IPolygon2dView() :
	m_pModel(nullptr)
{
}

/*virtual*/ IPolygon2dView::~IPolygon2dView()
{
}

Polygon2dModel *IPolygon2dView::GetModel() const
{
	return m_pModel;
}

void IPolygon2dView::SetModel(Polygon2dModel *pModel)
{
	if(pModel == nullptr)
	{
		m_pModel = nullptr;
		m_pModel->RemoveView(this);
		return;
	}

	m_pModel = pModel;
	m_pModel->AddView(this);
}
