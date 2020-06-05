/**************************************************************************
 *	IManagerModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IManagerModel.h"

IManagerModel::IManagerModel(Project &projRef) :
	ITreeModel(2, QStringList(), nullptr),
	m_BanksModel(*this),
	m_ProjectRef(projRef)
{
}

/*virtual*/ IManagerModel::~IManagerModel()
{
}
