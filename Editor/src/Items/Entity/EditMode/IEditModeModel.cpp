/**************************************************************************
*	IEditModeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "IEditModeModel.h"
#include "IEditModeView.h"

IEditModeModel::IEditModeModel(EditModeType eEditModeType) :
	m_eEditModeType(eEditModeType)
{
}

/*virtual*/ IEditModeModel::~IEditModeModel()
{
}

EditModeType IEditModeModel::GetEditModeType() const
{
	return m_eEditModeType;
}

void IEditModeModel::AddView(IEditModeView *pView)
{
	if(m_ViewList.contains(pView))
		return;
	m_ViewList.push_back(pView);
}

bool IEditModeModel::RemoveView(IEditModeView *pView)
{
	return m_ViewList.removeOne(pView);
}

void IEditModeModel::SyncViews(EditModeState eEditModeState) const
{
	for(IEditModeView *pView : m_ViewList)
		pView->SyncWithModel(eEditModeState);
}
