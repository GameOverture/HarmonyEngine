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
#include "AtlasModel.h"
#include "ManagerWidget.h"

#include <QImage>

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
		QDir metaDir(itemRef.GetProject().GetMetaAbsPath());
		metaDir.mkdir("Spine");
		if(metaDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine meta directory", LOGTYPE_Error);

		QDir dataDir(itemRef.GetProject().GetAssetsAbsPath());
		dataDir.mkdir("Spine");
		if(dataDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine data directory", LOGTYPE_Error);
		
		// Import Spine files
		QString sUuidName = itemFileDataRef.m_Meta["UUID"].toString();

		QFileInfo importFileInfo(itemFileDataRef.m_Meta["newImport"].toString());
		if(importFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(importFileInfo.absoluteFilePath(), dataDir.absoluteFilePath(sUuidName % "." % importFileInfo.completeSuffix())) == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		QFileInfo atlasFileInfo(importFileInfo.absolutePath() + "/" + importFileInfo.baseName() + ".atlas");
		QString sNewAtlasFilePath = dataDir.absoluteFilePath(sUuidName % ".atlas");
		if(atlasFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % atlasFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(atlasFileInfo.absoluteFilePath(), sNewAtlasFilePath) == false)
			HyGuiLog("SpineModel import: " % atlasFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		// Get PNGs imported into atlas manage
		QFile atlasFile(sNewAtlasFilePath);
		atlasFile.open(QIODevice::ReadOnly);
		if(!atlasFile.isOpen())
			HyGuiLog("SpineModel could not open atlas file", LOGTYPE_Error);

		QTextStream stream(&atlasFile);
		QString sLine = stream.readLine();
		QStringList atlasFileNameList;
		while(!sLine.isNull())
		{
			sLine = stream.readLine();
			if(sLine.contains(".png", Qt::CaseInsensitive))
				atlasFileNameList.append(sLine);
		};

		quint32 uiAtlasBankIndex = 0;
		if(m_ItemRef.GetProject().GetAtlasWidget())
			uiAtlasBankIndex = m_ItemRef.GetProject().GetAtlasModel().GetBankIndexFromBankId(m_ItemRef.GetProject().GetAtlasWidget()->GetSelectedBankId());
		
		for(QString sAtlasFile : atlasFileNameList)
		{
			QImage atlasPageImage(importFileInfo.absolutePath() + "/" + sAtlasFile);
			AtlasFrame *pNewPage = m_ItemRef.GetProject().GetAtlasModel().GenerateFrame(&m_ItemRef,
																						m_ItemRef.GetName(false),
																						atlasPageImage,
																						uiAtlasBankIndex,
																						ITEM_Text);
			m_AtlasFrameList.append(pNewPage);

			//if(m_ItemRef.GetProject().GetAtlasModel().ReplaceFrame(m_pAtlasFrame, m_ItemRef.GetName(false), fontAtlasImage, true) == false)
			//{
			//	HyGuiLog("Cannot ReplaceFrame text sub-atlas for " % m_ItemRef.GetName(true), LOGTYPE_Error);
			//	return false;
			//}
		}

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
