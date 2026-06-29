/**************************************************************************
 *	IEditModeView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IEditModeView.h"
#include "IEditModeModel.h"

IEditModeView::IEditModeView(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_pModel(nullptr)
{
}

/*virtual*/ IEditModeView::~IEditModeView()
{
	if(m_pModel)
		m_pModel->RemoveView(this);
}

IEditModeModel *IEditModeView::GetModel() const
{
	return m_pModel;
}

void IEditModeView::SetModel(IEditModeModel *pModel)
{
	if(m_pModel == pModel)
		return;
	if(m_pModel)
		m_pModel->RemoveView(this);

	m_pModel = pModel;
	if(m_pModel)
		m_pModel->AddView(this);
}
