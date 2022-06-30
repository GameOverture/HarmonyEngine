#include "Global.h"
#include "VersionPatcher.h"
#include "Project.h"

#include <QMessageBox>

/*static*/ bool VersionPatcher::Run(Project *pProj)
{
	// ********************************************************************************************
	// WARNING: 'pProj' is only partially constructed at this point
	//          Should only rely on pProj->GetAbsPath() and pProj->GetDirPath() on being accurate
	//          pProj->GetSettingsObj() will return a JSON object that may be from an older file version
	// ********************************************************************************************

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Determine the file version
	QJsonDocument projDoc;
	int iFileVersion = GetFileVersion(pProj->GetAbsPath(), projDoc, false);
	if(iFileVersion == -1)
	{
		HyGuiLog("VersionPatcher - HyProj file cannot be found.", LOGTYPE_Error);
		return false;
	}
	if(iFileVersion == 0)
	{
		iFileVersion = 2; // version 2 is the first version that specifies file version on .hyproj file
		HyGuiLog("File version is older than v2. Attempting recover as v2", LOGTYPE_Warning);
	}

	// Meta path key has changed from versions so do not rely on GetMetaAbsPath()
	QString sMetaAbsPath;
	if(iFileVersion <= 7)
		sMetaAbsPath = QDir::cleanPath(pProj->GetDirPath() + '/' + pProj->GetSettingsObj()["MetaDataPath"].toString()) + '/';
	else
		sMetaAbsPath = QDir::cleanPath(pProj->GetDirPath() + '/' + pProj->GetSettingsObj()["MetaPath"].toString()) + '/';

	// Data path key hasn't changed, but grabbing it manually here instead of using pProj->GetAssetsAbsPath() for posterity's sake
	QString sDataAbsPath = QDir::cleanPath(pProj->GetDirPath() + '/' + pProj->GetSettingsObj()["DataPath"].toString()) + '/';

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup each file's directory

	// ITEMS
	QDir metaDir(sMetaAbsPath);
	if(metaDir.exists() == false)
	{
		HyGuiLog("Meta directory is missing, recreating: " % sMetaAbsPath, LOGTYPE_Info);
		metaDir.mkpath(metaDir.absolutePath());
	}
	QDir dataDir(sDataAbsPath);
	if(dataDir.exists() == false)
	{
		HyGuiLog("Data directory is missing, recreating: " % sDataAbsPath, LOGTYPE_Info);
		dataDir.mkpath(dataDir.absolutePath());
	}
	
	// ATLAS
	QDir metaAtlasDir(sMetaAbsPath + HyGlobal::AssetName(ASSET_Atlas));
	if(metaAtlasDir.exists() == false)
	{
		HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
		metaAtlasDir.mkpath(metaAtlasDir.absolutePath());
	}
	QDir dataAtlasDir(sDataAbsPath + HyGlobal::AssetName(ASSET_Atlas));
	if(dataAtlasDir.exists() == false)
	{
		HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
		dataAtlasDir.mkpath(dataAtlasDir.absolutePath());
	}

	// AUDIO
	QDir metaAudioDir(sMetaAbsPath + HyGlobal::AssetName(ASSET_Audio));
	if(metaAudioDir.exists() == false)
	{
		HyGuiLog("Meta audio directory is missing, recreating", LOGTYPE_Info);
		metaAudioDir.mkpath(metaAudioDir.absolutePath());
	}
	QDir dataAudioDir(sDataAbsPath + HyGlobal::AssetName(ASSET_Audio));
	if(dataAudioDir.exists() == false)
	{
		HyGuiLog("Data audio directory is missing, recreating", LOGTYPE_Info);
		dataAudioDir.mkpath(dataAudioDir.absolutePath());
	}

	// Assemble correct/proper file names
	QString sMetaItemsPath = metaDir.absoluteFilePath(QString(HYGUIPATH_ItemsFileName) % HYGUIPATH_MetaExt);
	QString sDataItemsPath = dataDir.absoluteFilePath(QString(HYGUIPATH_ItemsFileName) % HYGUIPATH_DataExt);
	QString sMetaAtlasesPath = metaAtlasDir.absoluteFilePath(HyGlobal::AssetName(ASSET_Atlas) % HYGUIPATH_MetaExt);
	QString sDataAtlasesPath = dataAtlasDir.absoluteFilePath(HyGlobal::AssetName(ASSET_Atlas) % HYGUIPATH_DataExt);
	QString sMetaAudioPath = metaAudioDir.absoluteFilePath(HyGlobal::AssetName(ASSET_Audio) % HYGUIPATH_MetaExt);
	QString sDataAudioPath = dataAudioDir.absoluteFilePath(HyGlobal::AssetName(ASSET_Audio) % HYGUIPATH_DataExt);

	// Get files' versions and acquire each QJsonDocument to be sent into the patching functions
	QJsonDocument metaItemsDoc;
	QJsonDocument dataItemsDoc;
	QJsonDocument metaAtlasDoc;
	QJsonDocument dataAtlasDoc;
	QJsonDocument metaAudioDoc;
	QJsonDocument dataAudioDoc;
	int uiMetaItemsVersion = -1;
	int uiDataItemsVersion = -1;
	int uiMetaAtlasVersion = -1;
	int uiDataAtlasVersion = -1;
	int uiMetaAudioVersion = GetFileVersion(sMetaAudioPath, metaAudioDoc, true);
	int uiDataAudioVersion = GetFileVersion(sDataAudioPath, dataAudioDoc, false);
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
	if((iFileVersion != uiMetaItemsVersion && uiMetaItemsVersion != -1) ||
	   (iFileVersion != uiDataItemsVersion && uiDataItemsVersion != -1) ||
	   (iFileVersion != uiMetaAtlasVersion && uiMetaAtlasVersion != -1) ||
	   (iFileVersion != uiDataAtlasVersion && uiDataAtlasVersion != -1) ||
	   (iFileVersion != uiMetaAudioVersion && uiMetaAudioVersion != -1) ||
	   (iFileVersion != uiDataAudioVersion && uiDataAudioVersion != -1))
	{
		HyGuiLog("Mismatching versions found between files (assets and meta)", LOGTYPE_Error);
		return false;
	}
	if(iFileVersion > HYGUI_FILE_VERSION)
	{
		HyGuiLog("File versions (" % QString(iFileVersion) % ") are from a future editor and may not be compatible.\nCurrent Editor file version: " % QString::number(HYGUI_FILE_VERSION), LOGTYPE_Error);
		return false;
	}

	// Upgrade files if necessary
	if(iFileVersion < HYGUI_FILE_VERSION)
	{
		// Note: Each patch 'falls through' each case statement
		switch(iFileVersion)
		{
		case 0:
			HyGuiLog("Patching project files: version 0 -> 1", LOGTYPE_Info);
			Patch_0to1(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
			[[fallthrough]];
		case 1:
			HyGuiLog("Patching project files: version 1 -> 2", LOGTYPE_Info);
			Patch_1to2(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
			[[fallthrough]];
		case 2:
			HyGuiLog("Patching project files: version 2 -> 3", LOGTYPE_Info);
			Patch_2to3(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
			[[fallthrough]];
		case 3:
			HyGuiLog("Patching project files: version 3 -> 4", LOGTYPE_Info);
			Patch_3to4(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
			[[fallthrough]];
		case 4:
			HyGuiLog("Patching project files: version 4 -> 5", LOGTYPE_Info);
			Patch_4to5(metaItemsDoc, dataItemsDoc, metaAtlasDoc, dataAtlasDoc);
			[[fallthrough]];
		case 5:
			HyGuiLog("Patching project files: version 5 -> 6", LOGTYPE_Info);
			Patch_5to6(projDoc);
			[[fallthrough]];
		case 6:
			HyGuiLog("Patching project files: version 6 -> 7", LOGTYPE_Info);
			Patch_6to7(pProj, projDoc);
			[[fallthrough]];
		case 7:
			HyGuiLog("Patching project files: version 7 -> 8", LOGTYPE_Info);
			Patch_7to8(projDoc);
			[[fallthrough]];
		case 8:
			HyGuiLog("Patching project files: version 8 -> 9", LOGTYPE_Info);
			Patch_8to9(metaAtlasDoc, dataAtlasDoc);
			[[fallthrough]];
		case 9:
			// current version
			static_assert(HYGUI_FILE_VERSION == 9, "Improper file version set in VersionPatcher");
			break;

		default:
			HyGuiLog("File version unknown: " % QString::number(iFileVersion), LOGTYPE_Error);
		}

		RewriteFile(sMetaItemsPath, metaItemsDoc, true);
		RewriteFile(sDataItemsPath, dataItemsDoc, false);
		RewriteFile(sMetaAtlasesPath, metaAtlasDoc, true);
		RewriteFile(sDataAtlasesPath, dataAtlasDoc, false);
		RewriteFile(sMetaAudioPath, metaAudioDoc, true);
		RewriteFile(sDataAudioPath, dataAudioDoc, false);

		// Finalize hyproj
		RewriteFile(pProj->GetAbsPath(), projDoc, false);

		return true;
	}
	else
		return false;
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

/*static*/ void VersionPatcher::Patch_3to4(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	// Meta Items
	QJsonObject metaItemsObj = metaItemsDocRef.object();
	metaItemsObj.insert("$fileVersion", 4);
	metaItemsDocRef.setObject(metaItemsObj);

	// Data Items
	QJsonObject dataItemsObj = dataItemsDocRef.object();
	dataItemsObj.insert("$fileVersion", 4);
	dataItemsDocRef.setObject(dataItemsObj);

	// Meta atlas
	QJsonObject metaAtlasObj = metaAtlasDocRef.object();
	QJsonArray banksArray = metaAtlasObj["banks"].toArray();
	for(int i = 0; i < banksArray.size(); ++i)
	{
		QJsonObject bankObj = banksArray.at(i).toObject();
		bankObj.insert("textureFiltering", "Bilinear");
		banksArray.replace(i, bankObj);
	}
	metaAtlasObj.insert("banks", banksArray);
	metaAtlasObj["$fileVersion"] = 4;
	metaAtlasDocRef.setObject(metaAtlasObj);

	// Data atlas
	QJsonObject dataAtlasObj = dataAtlasDocRef.object();
	banksArray = dataAtlasObj["banks"].toArray();
	for(int i = 0; i < banksArray.size(); ++i)
	{
		QJsonObject bankObj = banksArray.at(i).toObject();
		QJsonArray texturesArray = bankObj["textures"].toArray();
		for(int j = 0; j < texturesArray.size(); ++j)
		{
			QJsonObject textureObj = texturesArray.at(j).toObject();
			textureObj.insert("filtering", "Bilinear");
			texturesArray.replace(j, textureObj);
		}
		bankObj["textures"] = texturesArray;
		banksArray.replace(i, bankObj);
	}
	dataAtlasObj["banks"] = banksArray;
	dataAtlasObj["$fileVersion"] = 4;
	dataAtlasDocRef.setObject(dataAtlasObj);
}

/*static*/ void VersionPatcher::Patch_4to5(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	// Meta Items
	QJsonObject metaItemsObj = metaItemsDocRef.object();
	metaItemsObj.insert("$fileVersion", 5);
	metaItemsDocRef.setObject(metaItemsObj);

	// Data Items
	QJsonObject dataItemsObj = dataItemsDocRef.object();
	dataItemsObj.insert("$fileVersion", 5);
	dataItemsDocRef.setObject(dataItemsObj);

	// Data atlas
	QJsonObject dataAtlasObj = dataAtlasDocRef.object();
	dataAtlasObj["$fileVersion"] = 5;
	dataAtlasDocRef.setObject(dataAtlasObj);

	// Meta atlas
	QJsonObject metaAtlasObj = metaAtlasDocRef.object();
	QJsonArray assetsArray = metaAtlasObj["assets"].toArray();
	QJsonArray banksArray = metaAtlasObj["banks"].toArray();
	for(int i = 0; i < assetsArray.size(); ++i)
	{
		QJsonObject assetObj = assetsArray.at(i).toObject();

		// Get texture format and filter from its bank
		int iBankId = assetObj["bankId"].toInt();
		bool bFound = false;
		for(int j = 0; j < banksArray.size(); ++j)
		{
			QJsonObject bankObj = banksArray.at(j).toObject();
			if(bankObj["bankId"].toInt() == iBankId)
			{
				assetObj.insert("textureFiltering", bankObj["textureFiltering"].toString());
				bFound = true;
				break;
			}
		}
		if(bFound == false)
			assetObj.insert("textureFiltering", "Bilinear");

		assetsArray.replace(i, assetObj);
	}
	metaAtlasObj.insert("assets", assetsArray);

	for(int i = 0; i < banksArray.size(); ++i)
	{
		QJsonObject bankObj = banksArray.at(i).toObject();
		bankObj.remove("textureFormat");
		bankObj.remove("textureFiltering");
		bankObj.remove("textureType");

		// Insert new "unfilledIndices" array into meta object
		QJsonArray unfilledIndicesArray;
		QJsonArray dataAtlasBankArray = dataAtlasObj["banks"].toArray();
		for(auto dataBank : dataAtlasBankArray)
		{
			QJsonObject dataBankObj = dataBank.toObject();
			if(dataBankObj["bankId"].toInt() == bankObj["bankId"].toInt())
			{
				// Take the last texture index in bank, since it's guarenteed that on this version all textures in bank have same format/filtering
				QJsonArray dataBankTexturesArray = dataBankObj["textures"].toArray();
				if(dataBankTexturesArray.empty())
					unfilledIndicesArray.append(0);
				else
					unfilledIndicesArray.append(dataBankTexturesArray.size() - 1);
			}
		}
		bankObj.insert("unfilledIndices", unfilledIndicesArray);

		banksArray.replace(i, bankObj);
	}
	metaAtlasObj.insert("banks", banksArray);
	
	metaAtlasObj["$fileVersion"] = 5;
	metaAtlasDocRef.setObject(metaAtlasObj);
}

/*static*/ void VersionPatcher::Patch_5to6(QJsonDocument &projDocRef)
{
	QJsonObject projObj = projDocRef.object();
	projObj.insert("BuildPath", "./build/");
	projDocRef.setObject(projObj);
}

/*static*/ void VersionPatcher::Patch_6to7(Project *pProj, QJsonDocument &projDocRef)
{
	QJsonObject projObj = projDocRef.object();

	// Copy source code into new <meta>/Source location
	QDir oldSrcDir(pProj->GetDirPath() + projObj["SourcePath"].toString());
	QDir newSrcDir(pProj->GetDirPath() + projObj["MetaDataPath"].toString() + HyGlobal::AssetName(ASSET_Source));
	newSrcDir.mkpath(".");

	QStringList sFoundSourceFileAppendList;
	HyGlobal::RecursiveFindFileOfExt("cpp", sFoundSourceFileAppendList, oldSrcDir);
	HyGlobal::RecursiveFindFileOfExt("h", sFoundSourceFileAppendList, oldSrcDir);
	for(auto sSrcFile : sFoundSourceFileAppendList)
	{
		QFileInfo srcFileInfo(sSrcFile);
		QFile::copy(srcFileInfo.absoluteFilePath(), newSrcDir.absoluteFilePath(srcFileInfo.fileName()));
	}

	// Rename GameName -> Title
	QString sTitle = projObj["GameName"].toString();
	projObj.remove("GameName");
	projObj.insert("Title", sTitle);

	// No longer used
	projObj.remove("ClassName");
	projObj.remove("PixelsPerMeter");

	projDocRef.setObject(projObj);

	QMessageBox::information(nullptr, "Version Patcher 6->7", "You will need to delete and regenerate any builds for project '" % sTitle % "'");
}

/*static*/ void VersionPatcher::Patch_7to8(QJsonDocument &projDocRef)
{
	QJsonObject projObj = projDocRef.object();

	// Rename MetaDataPath -> MetaPath
	QString sMetaPath = projObj["MetaDataPath"].toString();
	projObj.remove("MetaDataPath");
	projObj.insert("MetaPath", sMetaPath);

	QString sSourcePath = sMetaPath + HyGlobal::AssetName(ASSET_Source) + "/";
	projObj.insert("SourcePath", sSourcePath); // Re-add back 'SourcePath' from version 6 since that is needed to be separated from meta (to make clones/skins etc easier)

	projDocRef.setObject(projObj);
}

/*static*/ void VersionPatcher::Patch_8to9(QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef)
{
	QJsonObject metaAtlasObj = metaAtlasDocRef.object();
	QJsonArray assetsArray = metaAtlasObj["assets"].toArray();
	for(int iAssetIndex = 0; iAssetIndex < assetsArray.size(); ++iAssetIndex)
	{
		// Remove "textureFiltering" and "textureFormat" and replace with "textureInfo"
		QJsonObject assetObj = assetsArray.at(iAssetIndex).toObject();

		HyTextureFiltering eFiltering = HYTEXFILTER_BILINEAR;
		QString sFiltering = assetObj["textureFiltering"].toString();
		if(sFiltering.compare("Nearest", Qt::CaseInsensitive) == 0)
			eFiltering = HYTEXFILTER_NEAREST;

		HyTextureFormat eFormat = HYTEXTURE_Unknown;
		uint8 uiParam1 = 0;
		uint8 uiParam2 = 0;
		QString sFormat = assetObj["textureFormat"].toString();
		if(sFormat.compare("R8G8B8A8", Qt::CaseInsensitive) == 0)
		{
			eFormat = HYTEXTURE_Uncompressed;
			uiParam1 = 4; // Num channels
			uiParam2 = HyTextureInfo::UNCOMPRESSEDFILE_PNG;
		}
		else if(sFormat.compare("RGB_DTX1", Qt::CaseInsensitive) == 0)
		{
			eFormat = HYTEXTURE_DXT;
			uiParam1 = 3; // Num channels
			uiParam2 = 1; // DXT Type
		}
		else if(sFormat.compare("DTX5", Qt::CaseInsensitive) == 0)
		{
			eFormat = HYTEXTURE_DXT;
			uiParam1 = 4; // Num channels
			uiParam2 = 5; // DXT Type
		}

		assetObj.remove("textureFiltering");
		assetObj.remove("textureFormat");

		HyTextureInfo texInfo(eFiltering, eFormat, uiParam1, uiParam2);
		uint32 uiBucketId = texInfo.GetBucketId();
		assetObj.insert("textureInfo", QJsonValue(static_cast<qint64>(uiBucketId)));
		assetsArray.replace(iAssetIndex, assetObj);
	}
	metaAtlasObj.insert("assets", assetsArray);
	metaAtlasDocRef.setObject(metaAtlasObj);


	QJsonObject dataAtlasObj = dataAtlasDocRef.object();
	QJsonArray banksArray = dataAtlasObj["banks"].toArray();
	for(int iBankIndex = 0; iBankIndex < banksArray.size(); ++iBankIndex)
	{
		QJsonObject bankObj = banksArray.at(iBankIndex).toObject();

		QJsonArray texturesArray = bankObj["textures"].toArray();
		for(int iTextureIndex = 0; iTextureIndex < texturesArray.size(); ++iTextureIndex)
		{
			// Remove "textureFiltering" and "textureFormat" and replace with "textureInfo"
			QJsonObject textureObj = texturesArray.at(iTextureIndex).toObject();

			HyTextureFiltering eFiltering = HYTEXFILTER_BILINEAR;
			QString sFiltering = textureObj["filtering"].toString();
			if(sFiltering.compare("Nearest", Qt::CaseInsensitive) == 0)
				eFiltering = HYTEXFILTER_NEAREST;

			HyTextureFormat eFormat = HYTEXTURE_Unknown;
			uint8 uiParam1 = 0;
			uint8 uiParam2 = 0;
			QString sFormat = textureObj["format"].toString();
			if(sFormat.compare("R8G8B8A8", Qt::CaseInsensitive) == 0)
			{
				eFormat = HYTEXTURE_Uncompressed;
				uiParam1 = 4; // Num channels
				uiParam2 = HyTextureInfo::UNCOMPRESSEDFILE_PNG;
			}
			else if(sFormat.compare("RGB_DTX1", Qt::CaseInsensitive) == 0)
			{
				eFormat = HYTEXTURE_DXT;
				uiParam1 = 3; // Num channels
				uiParam2 = 1; // DXT Type
			}
			else if(sFormat.compare("DTX5", Qt::CaseInsensitive) == 0)
			{
				eFormat = HYTEXTURE_DXT;
				uiParam1 = 4; // Num channels
				uiParam2 = 5; // DXT Type
			}

			HyTextureInfo texInfo(eFiltering, eFormat, uiParam1, uiParam2);
			textureObj.remove("filtering");
			textureObj.remove("format");
			textureObj.insert("textureInfo", QJsonValue(static_cast<qint64>(texInfo.GetBucketId())));
			texturesArray.replace(iTextureIndex, textureObj);
		}
		bankObj.insert("textures", texturesArray);
		banksArray.replace(iBankIndex, bankObj);
	}
	dataAtlasObj.insert("banks", banksArray);
	dataAtlasDocRef.setObject(dataAtlasObj);
}

/*static*/ void VersionPatcher::RewriteFile(QString sFilePath, QJsonDocument &fileDocRef, bool bIsMeta)
{
	QFile dataFile(sFilePath);
	if(dataFile.exists() == false) // Don't create files that don't exist. That is handled by the the respective asset/manager/etc class
		return;

	QJsonObject obj = fileDocRef.object();
	obj.insert("$fileVersion", HYGUI_FILE_VERSION);
	fileDocRef.setObject(obj);

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
