/**************************************************************************
*	SpineModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "SpineModel.h"
#include "ProjectItemData.h"
#include "Project.h"

SpineStateData::SpineStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData)
{
}

/*virtual*/ SpineStateData::~SpineStateData()
{
}

/*virtual*/ QVariant SpineStateData::OnLinkAsset(AssetItemData *pAsset) /*override*/
{
	return 0;
}

/*virtual*/ void SpineStateData::OnUnlinkAsset(AssetItemData *pAsset) /*override*/
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpineModel::SpineModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
	// "newImport" indicates that this item is brand new and should setup its meta/data accordingly
	if(itemFileDataRef.m_Meta.contains("newImport"))
	{
		// Initialize Spine directories
		QDir metaDir(itemRef.GetProject().GetMetaDataAbsPath());
		metaDir.mkdir("Spine");
		if(metaDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine meta directory", LOGTYPE_Error);

		QDir dataDir(itemRef.GetProject().GetAssetsAbsPath());
		dataDir.mkdir("Spine");
		if(dataDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine data directory", LOGTYPE_Error);
		
		// Import Spine files
		QString sUuidName = itemRef.GetUuid().toString(QUuid::WithoutBraces);

		QFileInfo importFileInfo(itemFileDataRef.m_Meta["newImport"].toString());
		if(importFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(importFileInfo.absoluteFilePath(), dataDir.absoluteFilePath(sUuidName % "." % importFileInfo.completeSuffix())) == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		QFileInfo atlasFileInfo(importFileInfo.absolutePath() + "/" + importFileInfo.baseName() + ".atlas");
		if(atlasFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % atlasFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(atlasFileInfo.absoluteFilePath(), dataDir.absoluteFilePath(sUuidName % ".atlas")) == false)
			HyGuiLog("SpineModel import: " % atlasFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		// TODO: Get PNGs imported into atlas manager
		// TODO: fill out data/meta FileDataPair
	}


	InitStates<SpineStateData>(itemFileDataRef);
}

/*virtual*/ SpineModel::~SpineModel()
{
}

/*virtual*/ bool SpineModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void SpineModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
}

/*virtual*/ void SpineModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
}

/*virtual*/ QList<AssetItemData *> SpineModel::GetAssets(HyGuiItemType eType) const /*override*/
{
	QList<AssetItemData *> retAtlasFrameList;
	return retAtlasFrameList;
}

/*virtual*/ QStringList SpineModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
