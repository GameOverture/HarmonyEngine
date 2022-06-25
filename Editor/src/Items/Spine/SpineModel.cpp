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
	if(stateFileData.m_Data.contains("crossFades"))
	{
		QJsonObject crossFadesObj = stateFileData.m_Data["crossFades"].toObject();
		for(auto sKey : crossFadesObj.keys())
			m_CrossFadeMap.insert(sKey, crossFadesObj.value(sKey).toDouble());
	}
	else
		stateFileData.m_Data.insert("crossFades", QJsonObject());
}

/*virtual*/ SpineStateData::~SpineStateData()
{
}

const QMap<QString, double> &SpineStateData::GetCrossFadeMap() const
{
	return m_CrossFadeMap;
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
	IModel(itemRef, itemFileDataRef),
	m_bIsBinaryRuntime(false),
	m_fScale(1.0f),
	m_fDefaultMix(0.0f),
	m_pAtlasData(nullptr),
	m_pSkeletonData(nullptr),
	m_pAnimStateData(nullptr)
{
	QString sUuidName = GetUuid().toString(QUuid::WithoutBraces);

	// "newImport" indicates that this item is brand new and should setup its meta/data accordingly
	if(itemFileDataRef.m_Meta.contains("newImport"))
	{
		// Initialize Spine directories -----------------------------------------------------------------------------------------
		QDir metaDir(itemRef.GetProject().GetMetaAbsPath());
		metaDir.mkdir("Spine");
		if(metaDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine meta directory", LOGTYPE_Error);
		QDir dataDir(itemRef.GetProject().GetAssetsAbsPath());
		dataDir.mkdir("Spine");
		if(dataDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine data directory", LOGTYPE_Error);
		
		// Import Spine files - SKELETON -----------------------------------------------------------------------------------------
		QFileInfo importFileInfo(itemFileDataRef.m_Meta["newImport"].toString());
		m_SkeletonFileInfo.setFile(dataDir.absoluteFilePath(sUuidName % "." % importFileInfo.completeSuffix()));

		if(importFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(importFileInfo.absoluteFilePath(), m_SkeletonFileInfo.absoluteFilePath()) == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		// Import Spine files - ATLAS -----------------------------------------------------------------------------------------
		QFileInfo atlasFileInfo(importFileInfo.absolutePath() + "/" + importFileInfo.baseName() + ".atlas");
		m_AtlasFileInfo.setFile(dataDir.absoluteFilePath(sUuidName % ".atlas"));

		if(atlasFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % atlasFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(atlasFileInfo.absoluteFilePath(), m_AtlasFileInfo.absoluteFilePath()) == false)
			HyGuiLog("SpineModel import: " % atlasFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		// Import Spine Files - PNGs ------------------------------------------------------------------------------------------
		QFile atlasFile(m_AtlasFileInfo.absoluteFilePath());
		atlasFile.open(QIODevice::ReadOnly);
		if(!atlasFile.isOpen())
			HyGuiLog("SpineModel could not open atlas file", LOGTYPE_Error);

		QTextStream stream(&atlasFile);
		QString sLine = stream.readLine();
		QStringList textureFileNameList;
		while(!sLine.isNull())
		{
			if(sLine.contains(".png", Qt::CaseInsensitive))
				textureFileNameList.append(sLine);
			
			sLine = stream.readLine();
		};
		// Copy atlas files (exported from spine tool) to meta data
		metaDir.mkdir(sUuidName);
		for(QString sTextureFile : textureFileNameList)
		{
			QFileInfo textureFileInfo(importFileInfo.absolutePath() % '/' % sTextureFile);
			QString sTextureDestinationPath = metaDir.absoluteFilePath(sUuidName % '/' % textureFileInfo.fileName());

			if(QFile::copy(textureFileInfo.absoluteFilePath(), sTextureDestinationPath) == false)
				HyGuiLog("SpineModel import: " % textureFileInfo.absoluteFilePath() % " did not copy to: " % sTextureDestinationPath, LOGTYPE_Error);
		}

		m_bIsBinaryRuntime = importFileInfo.suffix().compare("skel", Qt::CaseInsensitive) == 0;
		m_fScale = 1.0f;
	}
	else
	{
		if(itemFileDataRef.m_Data.contains("isBinary") == false)
			HyGuiLog("SpineModel did not contain 'isBinary'", LOGTYPE_Error);
		m_bIsBinaryRuntime = itemFileDataRef.m_Data["isBinary"].toBool();

		if(itemFileDataRef.m_Data.contains("scale") == false)
			HyGuiLog("SpineModel did not contain 'scale'", LOGTYPE_Error);
		m_fScale = static_cast<float>(itemFileDataRef.m_Data["scale"].toDouble());

		m_SkeletonFileInfo.setFile(itemRef.GetProject().GetAssetsAbsPath() % HYASSETS_SpineDir % sUuidName % (m_bIsBinaryRuntime ? ".skel" : ".json"));
		m_AtlasFileInfo.setFile(itemRef.GetProject().GetAssetsAbsPath() % HYASSETS_SpineDir % sUuidName % ".atlas");
	}

	AcquireSpineData();

	FileDataPair newFileDataPair = itemFileDataRef;
	if(newFileDataPair.m_Data.contains("stateArray") == false || newFileDataPair.m_Meta.contains("stateArray"))
	{
		uint32 uiNumStates = 0;
#ifdef HY_USE_SPINE
		uiNumState = static_cast<uint32>(m_pSkeletonData->getAnimations().size());
#endif

		QJsonArray metaStateArray;
		QJsonArray dataStateArray;
		for(int i = 0; i < uiNumStates; ++i)
		{
			FileDataPair stateFileData;
			InsertStateSpecificData(i, stateFileData);

			metaStateArray.append(stateFileData.m_Meta);
			dataStateArray.append(stateFileData.m_Data);
		}
		
		newFileDataPair.m_Meta["stateArray"] = metaStateArray;
		newFileDataPair.m_Data["stateArray"] = dataStateArray;
	}

	// Only checks for "stateArray" within itemFileDataRef.m_Meta/m_Data and initializes states
	InitStates<SpineStateData>(newFileDataPair);
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
	itemSpecificFileDataOut.m_Data.insert("UUID", m_UUID.toString(QUuid::WithoutBraces));
	itemSpecificFileDataOut.m_Data.insert("isBinary", m_bIsBinaryRuntime);
	itemSpecificFileDataOut.m_Data.insert("scale", m_fScale);
	itemSpecificFileDataOut.m_Data.insert("defaultMix", m_fDefaultMix);
}

/*virtual*/ void SpineModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
#ifdef HY_USE_SPINE
	spine::Animation *pSpineAnim = m_pSkeletonData->getAnimations()[uiIndex];
	stateFileDataOut.m_Meta.insert("name", pSpineAnim->getName().buffer());
	
	QJsonObject crossFadesObj;
	if(uiIndex < m_StateList.size())
	{
		auto &crossFadeMap = static_cast<SpineStateData *>(m_StateList[uiIndex])->GetCrossFadeMap();
		for(QString sKey : crossFadeMap.keys())
			crossFadesObj.insert(sKey, crossFadeMap[sKey]);
	}

	stateFileDataOut.m_Data.insert("crossFades", crossFadesObj);
#endif
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

void SpineModel::AcquireSpineData()
{
#ifdef HY_USE_SPINE
	delete m_pAtlasData;
	delete m_pSkeletonData;
	delete m_pAnimStateData;
	
	m_pAtlasData = HY_NEW spine::Atlas(m_AtlasFileInfo.absoluteFilePath().toStdString().c_str(), nullptr, false);

	if(m_bIsBinaryRuntime)
	{
		spine::SkeletonBinary binParser(m_pAtlasData);
		m_pSkeletonData = binParser.readSkeletonDataFile(m_SkeletonFileInfo.absoluteFilePath().toStdString().c_str());
		if(m_pSkeletonData == nullptr)
			HyGuiLog("HySpineData binary load failed: " % QString(binParser.getError().buffer()), LOGTYPE_Error);
	}
	else
	{
		spine::SkeletonJson jsonParser(m_pAtlasData);
		m_pSkeletonData = jsonParser.readSkeletonDataFile(m_SkeletonFileInfo.absoluteFilePath().toStdString().c_str());
		if(m_pSkeletonData == nullptr)
			HyGuiLog("HySpineData json load failed: " % QString(jsonParser.getError().buffer()), LOGTYPE_Error);
	}

	m_pAnimStateData = HY_NEW spine::AnimationStateData(m_pSkeletonData);
#endif
}
