#include "Global.h"
#include "VersionPatcher.h"
#include "Project.h"

/*static*/ void VersionPatcher::Run(Project *pProj)
{
	// **********************************************************
	// NOTE: 'pProj' is only partially constructed at this point
	// **********************************************************

	int uiMetaItemsVersion = -1;
	int uiDataItemsVersion = -1;
	int uiMetaAtlasVersion = -1;
	int uiDataAtlasVersion = -1;

	// Acquire each QJsonDocument to be sent into the patching functions
	QJsonDocument metaItemsDoc;
	QJsonDocument dataItemsDoc;
	QJsonDocument metaAtlasDoc;
	QJsonDocument dataAtlasDoc;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir metaDir(pProj->GetMetaDataAbsPath());
	if(metaDir.exists() == false)
	{
		HyGuiLog("Meta directory is missing, recreating", LOGTYPE_Info);
		metaDir.mkpath(metaDir.absolutePath());
	}
	uiMetaItemsVersion = GetFileVersion(metaDir.absoluteFilePath(HYMETA_DataFile), metaItemsDoc, true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir dataDir(pProj->GetAssetsAbsPath());
	if(dataDir.exists() == false)
	{
		HyGuiLog("Data directory is missing, recreating", LOGTYPE_Info);
		dataDir.mkpath(dataDir.absolutePath());
	}
	uiDataItemsVersion = GetFileVersion(dataDir.absoluteFilePath(HYASSETS_DataFile), dataItemsDoc, true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir metaAtlasDir(pProj->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	if(metaAtlasDir.exists() == false)
	{
		HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
		metaAtlasDir.mkpath(metaAtlasDir.absolutePath());
	}
	uiMetaAtlasVersion = GetFileVersion(metaAtlasDir.absoluteFilePath(HYMETA_AtlasFile), metaAtlasDoc, true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir dataAtlasDir(pProj->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	if(dataAtlasDir.exists() == false)
	{
		HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
		dataAtlasDir.mkpath(dataAtlasDir.absolutePath());
	}
	uiDataAtlasVersion = GetFileVersion(dataAtlasDir.absoluteFilePath(HYASSETS_AtlasFile), dataAtlasDoc, false);

	if(uiMetaItemsVersion == -1 || uiDataItemsVersion == -1 || uiMetaAtlasVersion == -1 || uiDataAtlasVersion == -1)
	{
		HyGuiLog("Missing files between assets and meta. Skipping Version Patcher", LOGTYPE_Warning);
		return;
	}

	int iFileVersion = uiMetaItemsVersion;
	if(iFileVersion != uiDataItemsVersion ||
	   iFileVersion != uiMetaAtlasVersion ||
	   iFileVersion != uiDataAtlasVersion)
	{
		HyGuiLog("Mismatching versions found between files (assets and meta)", LOGTYPE_Error);
	}

	// Note: Each patch 'falls through' each case statement
	switch(iFileVersion)
	{
	case 0:
		Patch_0to1(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
	case 1:
		Patch_1to2(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
	case 2:
		break; // current version

	default:
		HyGuiLog("File version unknown: " % QString::number(iFileVersion), LOGTYPE_Error);
	}
}

/*static*/ int VersionPatcher::GetFileVersion(QString sFilePath, QJsonDocument &fileDocOut, bool bIsMeta)
{
	QFile file(sFilePath);
	if(file.exists())
	{
		if(!file.open(QIODevice::ReadOnly))
			HyGuiLog(QString("VersionPatcher::GetFileVersion() could not open ") % sFilePath, LOGTYPE_Error);

		if(bIsMeta)
		{
#ifdef HYGUI_UseBinaryMetaFiles
			fileDocOut = QJsonDocument::fromBinaryData(file.readAll());
#else
			fileDocOut = QJsonDocument::fromJson(file.readAll());
#endif
		}
		else
			fileDocOut = QJsonDocument::fromJson(file.readAll());

		file.close();

		if(fileDocOut.isArray()) // All files use object as root as of version 1
			return 0;

		QJsonObject fileObj = fileDocOut.object();
		if(fileObj.keys().contains("_fileVersion") == false)
			return 0;
		else
			return fileObj["_fileVersion"].toInt();
	}
	else
		return -1;
}

/*static*/ void VersionPatcher::Patch_0to1(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	QJsonObject metaItemsObj = metaItemsDocRef.object();
	QJsonObject metaSpritesObj = metaItemsObj["Sprites"].toObject();

	QJsonObject dataItemsObj = dataItemsDocRef.object();
	QJsonObject dataSpritesObj = dataItemsObj["Sprites"].toObject();

	QStringList sSpritesKeysList = dataSpritesObj.keys();
	for(int i = 0; i < sSpritesKeysList.size(); ++i)
	{
		QJsonObject metaSpriteObj = metaSpritesObj[sSpritesKeysList[i]].toObject();
		QJsonArray newMetaSpriteStatesArray;

		QJsonArray dataSpriteStatesArray = dataSpritesObj[sSpritesKeysList[i]].toArray();
		for(int j = 0; j < dataSpriteStatesArray.size(); ++j)
		{
			QJsonArray newMetaSpriteStateFramesArray;

			QJsonObject dataSpriteStateObj = dataSpriteStatesArray[j].toObject();
			QString sStateName = dataSpriteStateObj["name"].toString();

			QJsonArray dataSpriteStateFramesArray = dataSpriteStateObj["frames"].toArray();
			for(int k = 0; k < dataSpriteStateFramesArray.size(); ++k)
			{
				newMetaSpriteStateFramesArray.append(dataSpriteStateFramesArray[i].toObject()["id"].toInt());
			}

			QJsonObject newMetaSpriteStateObj;
			newMetaSpriteStateObj.insert("name", sStateName);
			newMetaSpriteStateObj.insert("frames", newMetaSpriteStateFramesArray);
			newMetaSpriteStatesArray.append(newMetaSpriteStateObj);
		}

		metaSpriteObj.insert("states", newMetaSpriteStatesArray);
		metaSpritesObj[sSpritesKeysList[i]] = metaSpriteObj;
	}

	metaItemsObj["Sprites"] = metaSpritesObj;
	dataItemsObj["Sprites"] = dataSpritesObj;

	metaItemsDocRef.setObject(metaItemsObj);
	dataItemsDocRef.setObject(dataItemsObj);


	// Convert 'dataAtlasDocRef' from an atlas to object
	QJsonArray atlasArray = dataAtlasDocRef.array();
	QJsonObject dataAtlasObj;
	dataAtlasObj.insert("_fileVersion", 1);
	dataAtlasObj.insert("atlasGroups", atlasArray);
	dataAtlasDocRef.setObject(dataAtlasObj);
}

/*static*/ void VersionPatcher::Patch_1to2(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	QJsonObject dataItemsObj = dataItemsDocRef.object();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QJsonObject metaItemsObj = metaItemsDoc.object();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir metaAtlasDir(pProj->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	QFile metaAtlasFile(metaAtlasDir.absoluteFilePath(HYMETA_AtlasFile));
	if(metaAtlasFile.open(QIODevice::ReadWrite) == false)
	{
		HyGuiLog(QString("Couldn't open ") % HYMETA_AtlasFile % " for reading/writing: " % metaAtlasFile.errorString(), LOGTYPE_Error);

		dataItemsFile.close();
		metaItemsFile.close();
		metaAtlasFile.close();
		return false;
	}
	QJsonDocument metaAtlasDoc;
#ifdef HYGUI_UseBinaryMetaFiles
	metaAtlasDoc = QJsonDocument::fromBinaryData(metaAtlasFile.readAll());
#else
	metaAtlasDoc = QJsonDocument::fromJson(metaAtlasFile.readAll());
#endif
	QJsonObject metaAtlasObj = metaAtlasDoc.object();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// lambda function that will fix 'data.json' and 'data.hygui' for every atlas frame
	std::function<void(QJsonObject &, QJsonObject &, quint32, QUuid)> fpDataItemsReplace =
		[](QJsonObject &dataItemsObjRef, QJsonObject &metaItemsObjRef, quint32 uiId, QUuid uuid)
	{
		// Data Items
		QJsonObject dataItemsSpritesObj = dataItemsObjRef["Sprites"].toObject();
		QStringList sSpritesKeysList = dataItemsSpritesObj.keys();
		for(int i = 0; i < sSpritesKeysList.size(); ++i)
		{
			QJsonArray spriteStateArray = dataItemsSpritesObj[sSpritesKeysList[i]].toArray();
			for(int j = 0; j < spriteStateArray.size(); ++j)
			{
				QJsonObject spriteStateObj = spriteStateArray[j].toObject();

				QJsonArray spriteStateFramesArray = spriteStateObj["frames"].toArray();
				for(int k = 0; k < spriteStateFramesArray.size(); ++k)
				{
					QJsonObject frameObj = spriteStateFramesArray[k].toObject();
					if(JSONOBJ_TOINT(frameObj, "id") == uiId)
					{
						// Meta Items
						QJsonObject metaItemsSpritesObj = metaItemsObjRef["Sprites"].toObject();
						QJsonObject metaSpriteObj = metaItemsSpritesObj[sSpritesKeysList[i]].toObject();


						metaItemsObjRef.remove("Sprites");
						metaItemsObjRef.insert("Sprites", metaItemsSpritesObj);
					}
				}

			}
		}
		dataItemsObjRef.remove("Sprites");
		dataItemsObjRef.insert("Sprites", dataItemsSpritesObj);


	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Iterate through every frame within 'atlas.hygui' and fix, and also fix the other files
	QJsonArray framesArray = metaAtlasObj["frames"].toArray();
	for(int i = 0; i < framesArray.size(); ++i)
	{
		QJsonObject frameObj = framesArray[i].toObject();

		quint32 uiId = JSONOBJ_TOINT(frameObj, "id");
		QUuid uuid = QUuid::createUuid();

		// Invoke above lambda function that fixes the other files with this information
		fpDataItemsReplace(dataItemsObj, metaItemsObj, uiId, uuid);

		frameObj.remove("id");
		frameObj.insert("frameUUID", uuid.toString(QUuid::WithoutBraces));

		framesArray[i] = frameObj;
	}
	metaAtlasObj.remove("frames");
	metaAtlasObj.insert("frames", framesArray);

	QJsonDocument userDoc;
	userDoc.setObject(m_SaveDataObj);
	qint64 iBytesWritten = dataFile.write(userDoc.toJson());
	if(0 == iBytesWritten || -1 == iBytesWritten)
	{
		HyGuiLog(QString("Could not write to ") % HYASSETS_DataFile % " file: " % dataFile.errorString(), LOGTYPE_Error);
	}
}
