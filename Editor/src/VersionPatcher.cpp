#include "Global.h"
#include "VersionPatcher.h"
#include "Project.h"

/*static*/ void VersionPatcher::Run(Project *pProj)
{
	// **********************************************************
	// NOTE: 'pProj' is only partially constructed at this point
	// **********************************************************

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
	QJsonDocument projDoc;
	int iFileVersion = GetFileVersion(pProj->GetAbsPath(), projDoc, false);
	if(iFileVersion == 0)
	{
		iFileVersion = 2; // version 2 is the first version that specifies file version on .hyproj file

		// Version 2 uses outdated "Atlas.hygui", so we check a file to ensure they're at least v2
		QJsonDocument tmpDoc;
		if(iFileVersion != GetFileVersion(metaAtlasDir.absoluteFilePath("Atlas.hygui"), tmpDoc, true))
		{
			HyGuiLog("Files are older than v2. Add your files' current version to .hyproj file ($fileVersion) in order to upgrade to latest.", LOGTYPE_Warning);
			return;
		}
	}

	// Assemble file names
	QString sMetaItemsPath = metaDir.absoluteFilePath(QString(HYGUIPATH_ItemsFileName) % HYGUIPATH_MetaExt);
	QString sDataItemsPath = dataDir.absoluteFilePath(QString(HYGUIPATH_ItemsFileName) % HYGUIPATH_DataExt);
	QString sMetaAtlasesPath = metaAtlasDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasImage, true) % HYGUIPATH_MetaExt);
	QString sDataAtlasesPath = dataAtlasDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasImage, true) % HYGUIPATH_DataExt);

	// Get files' versions and acquire each QJsonDocument to be sent into the patching functions
	int uiMetaItemsVersion = -1;
	int uiDataItemsVersion = -1;
	int uiMetaAtlasVersion = -1;
	int uiDataAtlasVersion = -1;
	QJsonDocument metaItemsDoc;
	QJsonDocument dataItemsDoc;
	QJsonDocument metaAtlasDoc;
	QJsonDocument dataAtlasDoc;
	if(iFileVersion <= 2) // Versions <= 2 used old filenames
	{
		uiMetaItemsVersion = GetFileVersion(metaDir.absoluteFilePath("data.hygui"), metaItemsDoc, true);
		uiDataItemsVersion = GetFileVersion(dataDir.absoluteFilePath("data.json"), dataItemsDoc, false);
		uiMetaAtlasVersion = GetFileVersion(metaAtlasDir.absoluteFilePath("atlas.hygui"), metaAtlasDoc, true);
		uiDataAtlasVersion = GetFileVersion(dataAtlasDir.absoluteFilePath("atlas.json"), dataAtlasDoc, false);
	}
	else
	{
		uiMetaItemsVersion = GetFileVersion(sMetaItemsPath, metaItemsDoc, true);
		uiDataItemsVersion = GetFileVersion(sDataItemsPath, dataItemsDoc, false);
		uiMetaAtlasVersion = GetFileVersion(sMetaAtlasesPath, metaAtlasDoc, true);
		uiDataAtlasVersion = GetFileVersion(sDataAtlasesPath, dataAtlasDoc, false);
	}
	
	// -1 means file is missing (or didn't open)
	if(uiMetaItemsVersion == -1 || uiDataItemsVersion == -1 || uiMetaAtlasVersion == -1 || uiDataAtlasVersion == -1)
	{
		HyGuiLog("Missing files between assets and meta. Skipping Version Patcher", LOGTYPE_Info);
		return;
	}

	if(iFileVersion != uiMetaItemsVersion ||
	   iFileVersion != uiDataItemsVersion ||
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
			HyGuiLog("Patching project " % pProj->GetGameName() % " files: version 0 -> 1", LOGTYPE_Info);
			Patch_0to1(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
		case 1:
			HyGuiLog("Patching project " % pProj->GetGameName() % " files: version 1 -> 2", LOGTYPE_Info);
			Patch_1to2(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
		case 2:
			HyGuiLog("Patching project " % pProj->GetGameName() % " files: version 2 -> 3", LOGTYPE_Info);
			Patch_2to3(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
		case 3:
			// current version
			static_assert(HYGUI_FILE_VERSION == 3, "Improper file version set in VersionPatcher");
			break;

		default:
			HyGuiLog("File version unknown: " % QString::number(iFileVersion), LOGTYPE_Error);
		}

		RewriteFile(sMetaItemsPath, metaItemsDoc, true);
		RewriteFile(sDataItemsPath, dataItemsDoc, false);
		RewriteFile(sMetaAtlasesPath, metaAtlasDoc, true);
		RewriteFile(sDataAtlasesPath, dataAtlasDoc, false);

		// Finalize hyproj
		QJsonObject projObj = projDoc.object();
		projObj["$fileVersion"] = HYGUI_FILE_VERSION;
		projDoc.setObject(projObj);
		RewriteFile(pProj->GetAbsPath(), projDoc, false);
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

/*static*/ void VersionPatcher::Patch_2to3(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// metaItemsDocRef
	QJsonObject metaItemsObj = metaItemsDocRef.object();

	// Sprites
	QJsonObject metaItemsSpritesObj = metaItemsObj["Sprites"].toObject();
	QStringList sSpritesKeysList = metaItemsSpritesObj.keys();
	for(int iKeyIndex = 0; iKeyIndex < sSpritesKeysList.size(); ++iKeyIndex)
	{
		QJsonObject metaSpriteObj = metaItemsSpritesObj[sSpritesKeysList[iKeyIndex]].toObject();
		QJsonArray metaSpriteStatesArray = metaSpriteObj["stateArray"].toArray();
		for(int iStateIndex = 0; iStateIndex < metaSpriteStatesArray.size(); ++iStateIndex)
		{
			QJsonObject metaSpriteStateObj = metaSpriteStatesArray[iStateIndex].toObject();

			QJsonArray spriteStateFramesArray = metaSpriteStateObj["frameIds"].toArray();
			metaSpriteStateObj["assetUUIDs"] = spriteStateFramesArray;

			metaSpriteStatesArray[iStateIndex] = metaSpriteStateObj;
		}

		metaSpriteObj["stateArray"] = metaSpriteStatesArray;
		metaItemsSpritesObj[sSpritesKeysList[iKeyIndex]] = metaSpriteObj;
	}
	metaItemsObj["Sprites"] = metaItemsSpritesObj;

	// Texts
	QJsonObject metaItemsTextsObj = metaItemsObj["Texts"].toObject();
	QStringList sTextsKeysList = metaItemsTextsObj.keys();
	for(int iKeyIndex = 0; iKeyIndex < sTextsKeysList.size(); ++iKeyIndex)
	{
		QJsonObject metaTextObj = metaItemsTextsObj[sTextsKeysList[iKeyIndex]].toObject();
		QString sAssetUuid = metaTextObj["frameUUID"].toString();
		metaTextObj.remove("frameUUID");
		metaTextObj.insert("assetUUID", sAssetUuid);

		metaItemsTextsObj[sTextsKeysList[iKeyIndex]] = metaTextObj;
	}
	metaItemsObj["Texts"] = metaItemsTextsObj;

	// Finalize
	metaItemsObj["$fileVersion"] = 3;
	metaItemsDocRef.setObject(metaItemsObj);

	///////////////////////////////////////////////////////////////////////////////////////////
	// dataItemsDocRef
	QJsonObject dataItemsObj = dataItemsDocRef.object();

	// Finalize
	dataItemsObj["$fileVersion"] = 3;
	dataItemsDocRef.setObject(dataItemsObj);

	///////////////////////////////////////////////////////////////////////////////////////////
	// metaAtlasDocRef
	QJsonObject metaAtlasObj = metaAtlasDocRef.object();

	QJsonArray framesArray = metaAtlasObj["frames"].toArray();
	QJsonArray metaBanksArray = metaAtlasObj["groups"].toArray();

	// Fix all the frames by adding 'textureFormat' and proper names
	for(int i = 0; i < framesArray.size(); ++i)
	{
		QJsonObject frameObj = framesArray.at(i).toObject();
		int iBankId = frameObj["atlasGrpId"].toInt();

		// Find 'textureType' in metaBanksArray that has the correct bank ID
		int iTextureFormat = HYTEXTURE_Unknown;
		for(int j = 0; j < metaBanksArray.size(); ++j)
		{
			QJsonObject bankObj = metaBanksArray[j].toObject();
			if(iBankId == bankObj["atlasGrpId"].toInt())
			{
				iTextureFormat = bankObj["textureType"].toInt();
				break;
			}
		}
		frameObj.insert("textureFormat", QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(iTextureFormat)).c_str()));

		frameObj.remove("atlasGrpId");
		frameObj.insert("bankId", iBankId);

		QString sUuid = frameObj["frameUUID"].toString();
		frameObj.remove("frameUUID");
		frameObj.insert("assetUUID", sUuid);

		QString sFilter = frameObj["filter"].toString();
		while(sFilter.startsWith('/'))
			sFilter.remove(0, 1);
		frameObj["filter"] = sFilter;

		// Convert atlas item type to item type
		int iAtlasItemType = frameObj["type"].toInt();
		// NOTE: Do not rearrange the order of this enum
		enum AtlasItemType
		{
			ATLASITEM_Unknown = -1,

			ATLASITEM_Filter = 0,
			ATLASITEM_Image,
			ATLASITEM_Font,
			ATLASITEM_Spine,
			ATLASITEM_Prefab,

			NUMATLASITEM
		};
		switch(iAtlasItemType)
		{
		case ATLASITEM_Filter:
			iAtlasItemType = ITEM_Filter;
			break;
		case ATLASITEM_Image:
			iAtlasItemType =ITEM_AtlasImage;
			break;
		case ATLASITEM_Font:
			iAtlasItemType =ITEM_Text;
			break;
		case ATLASITEM_Spine:
			iAtlasItemType =ITEM_Spine;
			break;
		case ATLASITEM_Prefab:
			iAtlasItemType = ITEM_Prefab;
			break;
		default:
			HyGuiLog("HyGlobal::GetItemFromAtlasItem() could not find the proper item", LOGTYPE_Error);
		}
		frameObj.remove("type");
		frameObj.insert("itemType", HyGlobal::ItemName(static_cast<HyGuiItemType>(iAtlasItemType), false));

		framesArray.replace(i, frameObj);
	}
	metaAtlasObj.remove("frames");
	metaAtlasObj.insert("assets", framesArray);

	// Now correct 'groups'
	for(int i = 0; i < metaBanksArray.size(); ++i)
	{
		QJsonObject bankObj = metaBanksArray.at(i).toObject();
		
		QString sName = bankObj["txtName"].toString();
		int iBankId = bankObj["atlasGrpId"].toInt();
		int iWidth = bankObj["sbTextureWidth"].toInt();
		int iHeight = bankObj["sbTextureHeight"].toInt();
		int iTextureFormat = bankObj["textureType"].toInt();
		bankObj.remove("txtName");
		bankObj.insert("bankName", sName);
		bankObj.remove("atlasGrpId");
		bankObj.insert("bankId", iBankId);
		bankObj.remove("sbTextureWidth");
		bankObj.insert("maxWidth", iWidth);
		bankObj.remove("sbTextureHeight");
		bankObj.insert("maxHeight", iHeight);

		// Determine how many textures there are by using 'dataAtlasDocRef'
		int iNumTextures = 0;
		QJsonObject tmpDataAtlasObj = dataAtlasDocRef.object();
		QJsonArray tmpBanksArray = tmpDataAtlasObj["atlasGroups"].toArray();

		// Move [width, height, textureType->format, and create assets array] into "textures" array of objects
		for(int i = 0; i < tmpBanksArray.size(); ++i)
		{
			QJsonObject tmpBankObj = tmpBanksArray[i].toObject();

			QJsonArray tmpTexturesArray = tmpBankObj["textures"].toArray();
			iNumTextures = tmpTexturesArray.size();
		}

		QJsonArray textureArray;
		for(int iTex = 0; iTex < iNumTextures; ++iTex)
		{
			QJsonObject textureObj;
			textureObj.insert("width", iWidth);
			textureObj.insert("height", iHeight);
			textureObj.insert("textureFormat", QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(iTextureFormat)).c_str()));
			
			textureArray.append(textureObj);
		}
		bankObj.insert("textures", textureArray);

		metaBanksArray.replace(i, bankObj);
	}

	metaAtlasObj.remove("groups");
	metaAtlasObj.insert("banks", metaBanksArray);

	int iNextBankId = metaAtlasObj["startAtlasId"].toInt();
	metaAtlasObj.remove("startAtlasId");
	metaAtlasObj.insert("nextBankId", iNextBankId);

	// Finalize
	metaAtlasObj["$fileVersion"] = 3;
	metaAtlasDocRef.setObject(metaAtlasObj);

	///////////////////////////////////////////////////////////////////////////////////////////
	// dataAtlasDocRef
	QJsonObject dataAtlasObj = dataAtlasDocRef.object();

	QJsonArray banksArray = dataAtlasObj["atlasGroups"].toArray();
	
	// Move [width, height, textureType->format, and create assets array] into "textures" array of objects
	for(int i = 0; i < banksArray.size(); ++i)
	{
		QJsonObject bankObj = banksArray.at(i).toObject();

		int iBankId = bankObj["atlasGrpId"].toInt();
		int iWidth = bankObj["width"].toInt();
		int iHeight = bankObj["height"].toInt();
		int iTextureType = bankObj["textureType"].toInt();

		// To be moved into "textures" array
		bankObj.remove("width");
		bankObj.remove("height");
		bankObj.remove("textureType");

		// Rename "atlasGroups" to "banks"
		bankObj.remove("atlasGrpId");
		bankObj.insert("bankId", iBankId);

		QJsonArray texturesArray = bankObj["textures"].toArray();
		for(int j = 0; j < texturesArray.size(); ++j)
		{
			QJsonObject newTextureObj;
			newTextureObj.insert("width", iWidth);
			newTextureObj.insert("height", iWidth);
			newTextureObj.insert("format", QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(iTextureType)).c_str()));
			newTextureObj.insert("assets", texturesArray.at(j).toArray());

			texturesArray.replace(j, newTextureObj);
		}
		bankObj["textures"] = texturesArray;

		banksArray.replace(i, bankObj);
	}

	// Rename "atlasGroups" to "banks"
	dataAtlasObj.remove("atlasGroups");
	dataAtlasObj.insert("banks", banksArray);

	// Finalize
	dataAtlasObj["$fileVersion"] = 3;
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
