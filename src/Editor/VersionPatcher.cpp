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
	// Setup each file's directory
	QDir metaDir(pProj->GetMetaDataAbsPath());
	if(metaDir.exists() == false)
	{
		HyGuiLog("Meta directory is missing, recreating", LOGTYPE_Info);
		metaDir.mkpath(metaDir.absolutePath());
	}
	
	QDir dataDir(pProj->GetAssetsAbsPath());
	if(dataDir.exists() == false)
	{
		HyGuiLog("Data directory is missing, recreating", LOGTYPE_Info);
		dataDir.mkpath(dataDir.absolutePath());
	}
	
	QDir metaAtlasDir(pProj->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	if(metaAtlasDir.exists() == false)
	{
		HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
		metaAtlasDir.mkpath(metaAtlasDir.absolutePath());
	}
	
	QDir dataAtlasDir(pProj->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	if(dataAtlasDir.exists() == false)
	{
		HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
		dataAtlasDir.mkpath(dataAtlasDir.absolutePath());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reach each file for its file version
	uiMetaItemsVersion = GetFileVersion(metaDir.absoluteFilePath(HYMETA_DataFile), metaItemsDoc, true);
	uiDataItemsVersion = GetFileVersion(dataDir.absoluteFilePath(HYASSETS_DataFile), dataItemsDoc, false);
	uiMetaAtlasVersion = GetFileVersion(metaAtlasDir.absoluteFilePath(HYMETA_AtlasFile), metaAtlasDoc, true);
	uiDataAtlasVersion = GetFileVersion(dataAtlasDir.absoluteFilePath(HYASSETS_AtlasFile), dataAtlasDoc, false);
	
	// -1 means file is missing (or didn't open)
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
		return;
	}

	if(iFileVersion > HYGUI_FILE_VERSION)
	{
		HyGuiLog("File versions (" % QString(iFileVersion) % ") are from a future editor and may not be compatible.\nCurrent Editor file version: " % QString::number(HYGUI_FILE_VERSION), LOGTYPE_Error);
		return;
	}

	// Upgrade files if necessary
	if(iFileVersion < HYGUI_FILE_VERSION)
	{
		// Note: Each patch 'falls through' each case statement
		switch(iFileVersion)
		{
		case 0:
			Patch_0to1(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
		case 1:
			Patch_1to2(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
		case 2:
			// current version
			static_assert(HYGUI_FILE_VERSION == 2, "Improper file version set in VersionPatcher");
			break;

		default:
			HyGuiLog("File version unknown: " % QString::number(iFileVersion), LOGTYPE_Error);
		}

		RewriteFile(metaDir.absoluteFilePath(HYMETA_DataFile), metaItemsDoc, true);
		RewriteFile(dataDir.absoluteFilePath(HYASSETS_DataFile), dataItemsDoc, false);
		RewriteFile(metaAtlasDir.absoluteFilePath(HYMETA_AtlasFile), metaAtlasDoc, true);
		RewriteFile(dataAtlasDir.absoluteFilePath(HYASSETS_AtlasFile), dataAtlasDoc, false);
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
		if(fileObj.keys().contains("$fileVersion") == false)
			return 0;
		else
			return fileObj["$fileVersion"].toInt();
	}
	else
		return -1;
}

/*static*/ void VersionPatcher::Patch_0to1(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	QJsonObject metaItemsObj = metaItemsDocRef.object();
	QJsonObject dataItemsObj = dataItemsDocRef.object();

	// Sprites
	QJsonObject metaSpritesObj = metaItemsObj["Sprites"].toObject();
	QJsonObject dataSpritesObj = dataItemsObj["Sprites"].toObject();

	QStringList sSpritesKeysList = dataSpritesObj.keys();
	for(int i = 0; i < sSpritesKeysList.size(); ++i)
	{
		QJsonObject metaSpriteObj = metaSpritesObj[sSpritesKeysList[i]].toObject();
		QJsonArray newMetaSpriteStatesArray;

		QJsonArray dataSpriteStatesArray = dataSpritesObj[sSpritesKeysList[i]].toArray();
		for(int j = 0; j < dataSpriteStatesArray.size(); ++j)
		{
			QJsonObject dataSpriteStateObj = dataSpriteStatesArray[j].toObject();

			QJsonArray newMetaSpriteStateFramesArray;
			QJsonArray dataSpriteStateFramesArray = dataSpriteStateObj["frames"].toArray();
			for(int k = 0; k < dataSpriteStateFramesArray.size(); ++k)
			{
				QJsonObject dataSpriteFrameObj = dataSpriteStateFramesArray[k].toObject();
				newMetaSpriteStateFramesArray.append(dataSpriteFrameObj["id"].toInt());

				dataSpriteFrameObj.remove("id");
				dataSpriteStateFramesArray[k] = dataSpriteFrameObj;
			}

			QJsonObject newMetaSpriteStateObj;
			newMetaSpriteStateObj.insert("name", dataSpriteStateObj["name"].toString());
			newMetaSpriteStateObj.insert("frameIds", newMetaSpriteStateFramesArray);
			newMetaSpriteStatesArray.append(newMetaSpriteStateObj);

			dataSpriteStateObj["frames"] = dataSpriteStateFramesArray;
			dataSpriteStateObj.remove("name");
			dataSpriteStatesArray[j] = dataSpriteStateObj;
		}

		QJsonObject newDataSpriteObj;
		newDataSpriteObj.insert("stateArray", dataSpriteStatesArray);
		dataSpritesObj[sSpritesKeysList[i]] = newDataSpriteObj;

		metaSpriteObj.insert("stateArray", newMetaSpriteStatesArray);
		metaSpritesObj[sSpritesKeysList[i]] = metaSpriteObj;
	}

	metaItemsObj["Sprites"] = metaSpritesObj;
	dataItemsObj["Sprites"] = dataSpritesObj;

	// Texts
	QJsonObject metaTextsObj = metaItemsObj["Texts"].toObject();
	QJsonObject dataTextsObj = dataItemsObj["Texts"].toObject();

	QStringList sTextsKeysList = dataTextsObj.keys();
	for(int i = 0; i < sTextsKeysList.size(); ++i)
	{
		QJsonObject metaTextObj = metaTextsObj[sTextsKeysList[i]].toObject();
		QJsonObject dataTextObj = dataTextsObj[sTextsKeysList[i]].toObject();

		// Create a meta 'stateArray'
		QJsonArray metaTextStateArray;
		QJsonArray dataTextStateArray = dataTextObj["stateArray"].toArray();
		for(int j = 0; j < dataTextStateArray.size(); ++j)
		{
			QJsonObject metaTextStateObj;
			metaTextStateObj.insert("name", dataTextStateArray[j].toObject()["name"].toString());
			metaTextStateArray.append(metaTextStateObj);
		}

		metaTextObj.insert("stateArray", metaTextStateArray);
		metaTextObj.insert("availableGlyphs", dataTextObj["availableGlyphs"].toObject());
		metaTextObj.insert("id", dataTextObj["id"].toInt());
		dataTextObj.remove("availableGlyphs");
		dataTextObj.remove("id");

		metaTextsObj[sTextsKeysList[i]] = metaTextObj;
		dataTextsObj[sTextsKeysList[i]] = dataTextObj;
	}

	metaItemsObj["Texts"] = metaTextsObj;
	dataItemsObj["Texts"] = dataTextsObj;

	// Replace manipulated object of metaItemsDocRef and dataItemsDocRef and add $fileVersion
	metaItemsObj.insert("$fileVersion", 1);
	metaItemsDocRef.setObject(metaItemsObj);

	dataItemsObj.insert("$fileVersion", 1);
	dataItemsDocRef.setObject(dataItemsObj);

	// Add $fileVersion to 'metaAtlasDocRef'
	QJsonObject metaAtlasObj = metaAtlasDocRef.object();
	metaAtlasObj.insert("$fileVersion", 1);
	metaAtlasDocRef.setObject(metaAtlasObj);

	// Convert 'dataAtlasDocRef' from an atlas to object and add $fileVersion
	QJsonArray atlasArray = dataAtlasDocRef.array();
	QJsonObject dataAtlasObj;
	dataAtlasObj.insert("$fileVersion", 1);
	dataAtlasObj.insert("atlasGroups", atlasArray);
	dataAtlasDocRef.setObject(dataAtlasObj);
}

/*static*/ void VersionPatcher::Patch_1to2(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// lambda function that will fix 'data.hygui' for every atlas frame
	std::function<void(QJsonObject &, quint32, QUuid)> fpDataItemsReplace =
		[=](QJsonObject &metaItemsObjRef, quint32 uiId, QUuid uuid)
	{
		// Sprites
		QJsonObject metaItemsSpritesObj = metaItemsObjRef["Sprites"].toObject();
		QStringList sSpritesKeysList = metaItemsSpritesObj.keys();
		for(int iKeyIndex = 0; iKeyIndex < sSpritesKeysList.size(); ++iKeyIndex)
		{
			QJsonObject metaSpriteObj = metaItemsSpritesObj[sSpritesKeysList[iKeyIndex]].toObject();
			QJsonArray metaSpriteStatesArray = metaSpriteObj["stateArray"].toArray();
			for(int iStateIndex = 0; iStateIndex < metaSpriteStatesArray.size(); ++iStateIndex)
			{
				QJsonObject metaSpriteStateObj = metaSpriteStatesArray[iStateIndex].toObject();

				QJsonArray spriteStateFramesArray = metaSpriteStateObj["frameIds"].toArray();
				for(int iFrameIndex = 0; iFrameIndex < spriteStateFramesArray.size(); ++iFrameIndex)
				{
					if(spriteStateFramesArray[iFrameIndex].isDouble() && spriteStateFramesArray[iFrameIndex].toInt() == uiId)
						spriteStateFramesArray[iFrameIndex] = uuid.toString(QUuid::WithoutBraces);
				}
				metaSpriteStateObj["frameIds"] = spriteStateFramesArray;

				metaSpriteStatesArray[iStateIndex] = metaSpriteStateObj;
			}

			metaSpriteObj["stateArray"] = metaSpriteStatesArray;
			metaItemsSpritesObj[sSpritesKeysList[iKeyIndex]] = metaSpriteObj;
		}
		metaItemsObjRef["Sprites"] = metaItemsSpritesObj;

		// Texts
		QJsonObject metaItemsTextsObj = metaItemsObjRef["Texts"].toObject();
		QStringList sTextsKeysList = metaItemsTextsObj.keys();
		for(int iKeyIndex = 0; iKeyIndex < sTextsKeysList.size(); ++iKeyIndex)
		{
			QJsonObject metaTextObj = metaItemsTextsObj[sTextsKeysList[iKeyIndex]].toObject();
			if(metaTextObj["id"].isDouble() && metaTextObj["id"].toInt() == uiId)
			{
				metaTextObj.remove("id");
				metaTextObj.insert("frameUUID", uuid.toString(QUuid::WithoutBraces));
			}
			metaItemsTextsObj[sTextsKeysList[iKeyIndex]] = metaTextObj;
		}
		metaItemsObjRef["Texts"] = metaItemsTextsObj;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Iterate through every frame within 'atlas.hygui' and fix, and also fix the other files
	QJsonObject metaItemsObj = metaItemsDocRef.object();
	QJsonObject metaAtlasObj = metaAtlasDocRef.object();
	QJsonArray framesArray = metaAtlasObj["frames"].toArray();
	for(int i = 0; i < framesArray.size(); ++i)
	{
		QJsonObject frameObj = framesArray[i].toObject();

		quint32 uiId = JSONOBJ_TOINT(frameObj, "id");
		QUuid uuid = QUuid::createUuid();

		// Invoke above lambda function that fixes the meta items file with this information
		fpDataItemsReplace(metaItemsObj, uiId, uuid);

		frameObj.remove("id");
		frameObj.insert("frameUUID", uuid.toString(QUuid::WithoutBraces));

		framesArray[i] = frameObj;
	}
	metaAtlasObj.remove("frames");
	metaAtlasObj.insert("frames", framesArray);

	// Finalize
	metaItemsObj["$fileVersion"] = 2;
	metaItemsDocRef.setObject(metaItemsObj);

	QJsonObject dataItemsObj = dataItemsDocRef.object();
	dataItemsObj["$fileVersion"] = 2;
	dataItemsDocRef.setObject(dataItemsObj);

	metaAtlasObj["$fileVersion"] = 2;
	metaAtlasDocRef.setObject(metaAtlasObj);

	QJsonObject dataAtlasObj = dataAtlasDocRef.object();
	dataAtlasObj["$fileVersion"] = 2;
	dataAtlasDocRef.setObject(dataAtlasObj);
}

/*static*/ void VersionPatcher::RewriteFile(QString sFilePath, QJsonDocument &fileDocRef, bool bIsMeta)
{
	QFile dataFile(sFilePath);
	if(dataFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog(QString("Couldn't open ") % HYASSETS_DataFile % " for writing: " % dataFile.errorString(), LOGTYPE_Error);
	else
	{
		qint64 iBytesWritten = 0;

		if(bIsMeta)
		{
#ifdef HYGUI_UseBinaryMetaFiles
			iBytesWritten = dataFile.write(fileDocRef.toBinaryData());
#else
			iBytesWritten = dataFile.write(fileDocRef.toJson());
#endif
		}
		else
			iBytesWritten = dataFile.write(fileDocRef.toJson());

		if(0 == iBytesWritten || -1 == iBytesWritten)
			HyGuiLog(QString("Could not write to ") % sFilePath % ": " % dataFile.errorString(), LOGTYPE_Error);

		dataFile.close();
	}
}
