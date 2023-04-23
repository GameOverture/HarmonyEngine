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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpineModel::SpineModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_bIsBinaryRuntime(false),
	m_fScale(1.0f),
	m_fDefaultMix(0.0f),
#ifdef HY_USE_SPINE
	m_pAtlasData(nullptr),
	m_pSkeletonData(nullptr),
	m_pAnimStateData(nullptr),
#endif
	m_bUsingTempFiles(true)
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

		// Store all imported files into temp directory for now. Will be moved appropriately once this item is saved
		QDir spineTempDir = HyGlobal::PrepTempDir(GetItem().GetProject(), sUuidName);
		
		// Import Spine files - SKELETON -----------------------------------------------------------------------------------------
		QFileInfo importFileInfo(itemFileDataRef.m_Meta["newImport"].toString());
		m_SkeletonFileInfo.setFile(spineTempDir.absoluteFilePath(sUuidName % "." % importFileInfo.completeSuffix()));

		if(importFileInfo.exists() == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " does not exist", LOGTYPE_Error);
		if(QFile::copy(importFileInfo.absoluteFilePath(), m_SkeletonFileInfo.absoluteFilePath()) == false)
			HyGuiLog("SpineModel import: " % importFileInfo.absoluteFilePath() % " did not copy to runtime data", LOGTYPE_Error);

		// Import Spine files - ATLAS -----------------------------------------------------------------------------------------
		QFileInfo atlasFileInfo(importFileInfo.absolutePath() + "/" + importFileInfo.baseName() + ".atlas");
		m_AtlasFileInfo.setFile(spineTempDir.absoluteFilePath(sUuidName % ".atlas"));

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
		// Copy atlas image files exported from spine tool, to spine's temp dir (once saved, it'll be copied once more to its meta dir)
		
		for(QString sTextureFile : textureFileNameList)
		{
			QFileInfo textureFileInfo(importFileInfo.absolutePath() % '/' % sTextureFile);
			QString sTextureDestinationPath = spineTempDir.absoluteFilePath(textureFileInfo.fileName());

			if(QFile::copy(textureFileInfo.absoluteFilePath(), sTextureDestinationPath) == false)
				HyGuiLog("SpineModel import: " % textureFileInfo.absoluteFilePath() % " did not copy to: " % sTextureDestinationPath, LOGTYPE_Error);

			SpineSubAtlas subAtlas;
			subAtlas.m_ImageFileInfo.setFile(sTextureDestinationPath);
			subAtlas.m_pAtlasFrame = nullptr;
			m_SubAtlasList.push_back(subAtlas);
		}

		m_bIsBinaryRuntime = importFileInfo.suffix().compare("skel", Qt::CaseInsensitive) == 0;
		m_fScale = 1.0f;

		m_bUsingTempFiles = true;
	}
	else // Existing Meta and Data JSON exist (and therefore any Spine imported files are in their respective directories)
	{
		if(itemFileDataRef.m_Data.contains("isBinary") == false)
			return; // If 'isBinary' doesn't exist, it could be due to a unsaved, discarded Spine item
			
		m_bIsBinaryRuntime = itemFileDataRef.m_Data["isBinary"].toBool();

		if(itemFileDataRef.m_Data.contains("scale") == false)
			HyGuiLog("SpineModel did not contain 'scale'", LOGTYPE_Error);
		m_fScale = static_cast<float>(itemFileDataRef.m_Data["scale"].toDouble());

		m_SkeletonFileInfo.setFile(itemRef.GetProject().GetAssetsAbsPath() % HYASSETS_SpineDir % sUuidName % (m_bIsBinaryRuntime ? ".skel" : ".json"));
		m_AtlasFileInfo.setFile(itemRef.GetProject().GetAssetsAbsPath() % HYASSETS_SpineDir % sUuidName % ".atlas");

		QDir metaDir(itemRef.GetProject().GetMetaAbsPath());
		metaDir.mkdir("Spine");
		if(metaDir.cd("Spine") == false)
			HyGuiLog("SpineModel could not navigate to Spine meta directory", LOGTYPE_Error);
		if(metaDir.cd(sUuidName) == false)
			HyGuiLog("SpineModel could not navigate to Spine meta UUID directory", LOGTYPE_Error);
		QJsonArray atlasesMetaArray = itemFileDataRef.m_Meta["atlases"].toArray();
		for(int i = 0; i < atlasesMetaArray.size(); ++i)
		{
			QJsonObject atlasMetaObj = atlasesMetaArray[i].toObject();

			QList<QUuid> uuidRequestList;
			uuidRequestList.append(QUuid(atlasMetaObj["assetUUID"].toString()));
			QList<TreeModelItemData *> dependList = m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, uuidRequestList);
			
			if(dependList.size() == 1)
			{
				SpineSubAtlas subAtlas;
				subAtlas.m_pAtlasFrame = static_cast<AtlasFrame *>(dependList[0]);
				subAtlas.m_ImageFileInfo.setFile(metaDir.absoluteFilePath(atlasMetaObj["textureFileName"].toString()));
				m_SubAtlasList.push_back(subAtlas);
			}
			else
				HyGuiLog("More than one frame returned for a spine sub-atlas", LOGTYPE_Error);
		}

		m_bUsingTempFiles = false;
	}

	AcquireSpineData();

	FileDataPair newFileDataPair = itemFileDataRef;
	if(newFileDataPair.m_Data.contains("stateArray") == false || newFileDataPair.m_Meta.contains("stateArray"))
	{
		uint32 uiNumStates = 0;
#ifdef HY_USE_SPINE
		uiNumStates = static_cast<uint32>(m_pSkeletonData->getAnimations().size());
#endif

		QJsonArray metaStateArray;
		QJsonArray dataStateArray;
		for(uint32 i = 0; i < uiNumStates; ++i)
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
	if(m_bUsingTempFiles)
	{
		QDir metaDir(m_ItemRef.GetProject().GetMetaAbsPath());
		metaDir.mkdir("Spine");
		if(metaDir.cd("Spine") == false)
		{
			HyGuiLog("SpineModel could not navigate to Spine meta directory", LOGTYPE_Error);
			return false;
		}
		QDir dataDir(m_ItemRef.GetProject().GetAssetsAbsPath());
		dataDir.mkdir("Spine");
		if(dataDir.cd("Spine") == false)
		{
			HyGuiLog("SpineModel could not navigate to Spine data directory", LOGTYPE_Error);
			return false;
		}

		// Move temp files into their appropriate meta/data dirs
		QString sUuidName = GetUuid().toString(QUuid::WithoutBraces);
		metaDir.mkdir(sUuidName);
		if(metaDir.cd(sUuidName) == false)
		{
			HyGuiLog("SpineModel could not navigate to this Spine's UUID meta directory: " % sUuidName, LOGTYPE_Error);
			return false;
		}

		// Skeleton File
		QFile skelFile(m_SkeletonFileInfo.absoluteFilePath());
		QString sSkelFileDestination = dataDir.absoluteFilePath(m_SkeletonFileInfo.fileName());

		QFile::remove(sSkelFileDestination);
		if(false == skelFile.copy(sSkelFileDestination))
		{
			HyGuiLog("SpineModel could not copy temp Spine Skeleton (" % m_SkeletonFileInfo.absoluteFilePath() % ") to this Spine's UUID data directory: " % sUuidName, LOGTYPE_Error);
			return false;
		}
		m_SkeletonFileInfo.setFile(sSkelFileDestination);

		// Atlas File
		QFile atlasFile(m_AtlasFileInfo.absoluteFilePath());
		QString sAtlasFileDestination = dataDir.absoluteFilePath(m_AtlasFileInfo.fileName());

		QFile::remove(sAtlasFileDestination);
		if(false == atlasFile.copy(sAtlasFileDestination))
		{
			HyGuiLog("SpineModel could not copy Spine Atlas file to this Spine's UUID data directory: " % sUuidName, LOGTYPE_Error);
			return false;
		}
		m_AtlasFileInfo.setFile(sAtlasFileDestination);

		// Image/Textures Files
		for(SpineSubAtlas &subAtlas : m_SubAtlasList)
		{
			QFile subAtlasFile(subAtlas.m_ImageFileInfo.absoluteFilePath());
			QString sSubAtlasFileDestination = metaDir.absoluteFilePath(subAtlas.m_ImageFileInfo.fileName());

			QFile::remove(sSubAtlasFileDestination);
			if(false == subAtlasFile.copy(sSubAtlasFileDestination))
			{
				HyGuiLog("SpineModel could not copy Spine Sub-Atlas image file to this Spine's UUID data directory: " % sUuidName, LOGTYPE_Error);
				return false;
			}
			subAtlas.m_ImageFileInfo.setFile(sSubAtlasFileDestination);


			// Pack sub atlases into 
			QImage subAtlasImage(subAtlas.m_ImageFileInfo.absoluteFilePath());

			// Best determine atlas bank to save generated texture in
			uint uiSubAtlasIndex = 0;
			if(subAtlas.m_pAtlasFrame == nullptr)
			{
				if(m_ItemRef.GetProject().GetAtlasWidget())
					uiSubAtlasIndex = m_ItemRef.GetProject().GetAtlasModel().GetBankIndexFromBankId(m_ItemRef.GetProject().GetAtlasWidget()->GetSelectedBankId());
			}
			else
				uiSubAtlasIndex = m_ItemRef.GetProject().GetAtlasModel().GetBankIndexFromBankId(subAtlas.m_pAtlasFrame->GetBankId());

			// Ensure newly generated font sub-atlas will fit in atlas bank dimensions
			quint32 uiSubAtlasBankId = m_ItemRef.GetProject().GetAtlasModel().GetBankIdFromBankIndex(uiSubAtlasIndex);
			if(m_ItemRef.GetProject().GetAtlasModel().IsImageValid(subAtlasImage, uiSubAtlasBankId) == false)
			{
				QSize atlasDimensions = m_ItemRef.GetProject().GetAtlasModel().GetAtlasDimensions(uiSubAtlasIndex);
				HyGuiLog("Cannot pack Spine sub-atlas for " % m_ItemRef.GetName(true) % " because it will not fit in atlas group '" % QString::number(uiSubAtlasBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
				return false;
			}

			// Keep track of sub-atlas replacements
			if(subAtlas.m_pAtlasFrame)
			{
				if(m_ItemRef.GetProject().GetAtlasModel().ReplaceFrame(subAtlas.m_pAtlasFrame, subAtlas.m_pAtlasFrame->GetName(), subAtlasImage) == false)
				{
					HyGuiLog("Cannot ReplaceFrame Spine sub-atlas for " % m_ItemRef.GetName(true), LOGTYPE_Error);
					return false;
				}
			}
			else
				subAtlas.m_pAtlasFrame = m_ItemRef.GetProject().GetAtlasModel().GenerateFrame(&m_ItemRef, subAtlas.m_ImageFileInfo.fileName(), subAtlasImage, uiSubAtlasBankId, ITEM_Spine);
		}

		m_ItemRef.GetProject().GetAtlasModel().FlushRepack();
		m_bUsingTempFiles = false;
	}

	return true;
}

/*virtual*/ void SpineModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	if(m_bUsingTempFiles)
		itemSpecificFileDataOut.m_Data.insert("usingTempFiles", QString(m_SkeletonFileInfo.absolutePath() % "/"));

	itemSpecificFileDataOut.m_Data.insert("UUID", GetUuid().toString(QUuid::WithoutBraces));
	itemSpecificFileDataOut.m_Data.insert("isBinary", m_bIsBinaryRuntime);
	itemSpecificFileDataOut.m_Data.insert("scale", m_fScale);
	itemSpecificFileDataOut.m_Data.insert("defaultMix", m_fDefaultMix);

#ifdef HY_USE_SPINE
	if(m_SubAtlasList.size() != m_pAtlasData->getPages().size())
		HyGuiLog("Spine SubAtlasList size did not equal Atlas file pages size", LOGTYPE_Error);

	QJsonArray atlasesMetaArray;
	QJsonArray atlasesDataArray;
	for(int i = 0; i < m_pAtlasData->getPages().size(); ++i)
	{
		QJsonObject atlasMetaObj;
		QJsonObject atlasDataObj;

		atlasMetaObj.insert("assetUUID", m_SubAtlasList[i].m_pAtlasFrame == nullptr ? 0 : m_SubAtlasList[i].m_pAtlasFrame->GetUuid().toString(QUuid::WithoutBraces));
		atlasMetaObj.insert("textureFileName", m_SubAtlasList[i].m_ImageFileInfo.fileName());

		atlasDataObj.insert("checksum", m_SubAtlasList[i].m_pAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_SubAtlasList[i].m_pAtlasFrame->GetChecksum())));
		atlasDataObj.insert("name", m_pAtlasData->getPages()[i]->name.buffer());

		atlasDataObj.insert("subAtlasWidth", m_SubAtlasList[i].m_pAtlasFrame == nullptr ? 0 : QJsonValue(m_SubAtlasList[i].m_pAtlasFrame->GetSize().width()));
		atlasDataObj.insert("subAtlasHeight", m_SubAtlasList[i].m_pAtlasFrame == nullptr ? 0 : QJsonValue(m_SubAtlasList[i].m_pAtlasFrame->GetSize().height()));

		atlasesMetaArray.append(atlasMetaObj);
		atlasesDataArray.append(atlasDataObj);
	}
	itemSpecificFileDataOut.m_Meta.insert("atlases", atlasesMetaArray);
	itemSpecificFileDataOut.m_Data.insert("atlases", atlasesDataArray);
#endif
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

bool SpineModel::IsUsingTempFiles() const
{
	return m_bUsingTempFiles;
}

const QList<SpineSubAtlas> &SpineModel::GetSubAtlasList() const
{
	return m_SubAtlasList;
}

// Bake the sub-atlas offset
void SpineModel::RewriteAtlasFile(AtlasFrame *pUpdatedFrame, QSize fullAtlasSize)
{
	for(int i = 0; i < m_SubAtlasList.size(); ++i)
	{
		if(m_SubAtlasList[i].m_pAtlasFrame == nullptr) // Only change runtime files, not temp
			continue;

		if(pUpdatedFrame == m_SubAtlasList[i].m_pAtlasFrame)
		{
			QFile atlasFile(m_AtlasFileInfo.absoluteFilePath());
			if(!atlasFile.open(QIODevice::ReadOnly | QIODevice::Text))
				return;

			QStringList sFileContents;

			// Read contents
			QTextStream in(&atlasFile);
			while(!in.atEnd())
				sFileContents.push_back(in.readLine());
			atlasFile.close();

			// Write (modified) contents
			if(!atlasFile.open(QIODevice::WriteOnly | QIODevice::Text))
				return;
			QTextStream out(&atlasFile);

			// Determine the sub-atlas associated with 'pUpdatedFrame'
			SpineSubAtlas *pAssociatedSubAtlas = nullptr;
			for(auto &subAtlasRef : m_SubAtlasList)
			{
				if(subAtlasRef.m_pAtlasFrame == pUpdatedFrame)
				{
					pAssociatedSubAtlas = &subAtlasRef;
					break;
				}
			}

			bool bSubAtlasActive = false;
			for(auto sLine : sFileContents)
			{
				for(auto &subAtlasRef : m_SubAtlasList)
				{
					if(sLine.compare(subAtlasRef.m_ImageFileInfo.fileName()) == 0)
					{
						bSubAtlasActive = (pAssociatedSubAtlas == &subAtlasRef);
						break;
					}
				}

				if(bSubAtlasActive)
				{
					if(sLine.startsWith("size:"))
						sLine = "size:" % QString::number(fullAtlasSize.width()) % "," % QString::number(fullAtlasSize.height());
					else if(sLine.startsWith("bounds:"))
					{
						QStringList sBoundsSplitList = sLine.split(':');
						sBoundsSplitList = sBoundsSplitList[1].split(',');

						sLine = "bounds:";
						int iX = sBoundsSplitList[0].toInt() + pAssociatedSubAtlas->m_pAtlasFrame->GetX();
						int iY = sBoundsSplitList[1].toInt() + pAssociatedSubAtlas->m_pAtlasFrame->GetY();

						sLine += QString::number(iX);
						sLine += ",";
						sLine += QString::number(iY);
						sLine += ",";
						sLine += sBoundsSplitList[2];
						sLine += ",";
						sLine += sBoundsSplitList[3];
					}
				}

				out << sLine << "\n";
			}
			atlasFile.close();
		}
	}
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
