/**************************************************************************
 *	IGfxShapeView.cpp
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

IGfxShapeView::IGfxShapeView() :
	m_pModel(nullptr)
{
}

/*virtual*/ IGfxShapeView::~IGfxShapeView()
{
}

GfxShapeModel *IGfxShapeView::GetModel() const
{
	return m_pModel;
}

void IGfxShapeView::SetModel(GfxShapeModel *pModel)
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
