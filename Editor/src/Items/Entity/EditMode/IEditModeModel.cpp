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

bool IEditModeModel::IsVectorModel() const
{
	return	m_eEditModeType == EDITMODETYPE_PrimitiveShape ||
			m_eEditModeType == EDITMODETYPE_PrimitiveLineChain ||
			m_eEditModeType == EDITMODETYPE_FixtureShape ||
			m_eEditModeType == EDITMODETYPE_FixtureChain ||
			m_eEditModeType == EDITMODETYPE_FixturePoint;
}

QString IEditModeModel::GetCategoryName() const
{
	switch(m_eEditModeType)
	{
	case EDITMODETYPE_PrimitiveShape:		return "Primitive Layer";
	case EDITMODETYPE_PrimitiveLineChain:	return "Primitive Layer";
	case EDITMODETYPE_FixtureShape:			return "Shape";
	case EDITMODETYPE_FixtureChain:			return "Chain";
	case EDITMODETYPE_FixturePoint:			return "Point";
	case EDITMODETYPE_TileMap:				return "Tile Map";

	default:
		HyGuiLog("IEditModeModel::GetCategoryName() - unhandled edit mode type", LOGTYPE_Error);
		break;
	}
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

	OnSyncViews();
}
