/**************************************************************************
 *	TileMapModels.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapModels.h"
#include "Project.h"

#include <QJsonArray>

TileMapStateData::TileMapStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData)
{
}

/*virtual*/ TileMapStateData::~TileMapStateData()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileMapModel::TileMapModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
	InitStates<TileMapStateData>(itemFileDataRef);
}

/*virtual*/ TileMapModel::~TileMapModel()
{
}

/*virtual*/ void TileMapModel::OnPopState(int iPoppedStateIndex) /*override*/
{
	//QList<IAssetItemData *> frameList = static_cast<TileMapStateData *>(m_StateList[iPoppedStateIndex])->GetAtlasFrames();

	//QList<TreeModelItemData *> convertedTypeList;
	//for(IAssetItemData *pFrame : frameList)
	//	convertedTypeList.push_back(pFrame);

	//m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, convertedTypeList);
}

/*virtual*/ void TileMapModel::InsertItemSpecificData(FileDataPair &itemFileDataOut) /*override*/
{
}

/*virtual*/ void TileMapModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	TileMapStateData *pState = static_cast<TileMapStateData *>(m_StateList[uiIndex]);
}

/*virtual*/ void TileMapModel::OnItemDeleted() /*override*/
{
	// No item-specific resources to delete
}
