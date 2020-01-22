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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir metaDir(pProj->GetMetaDataAbsPath());
	if(metaDir.exists() == false)
	{
		HyGuiLog("Meta directory is missing, recreating", LOGTYPE_Info);
		metaDir.mkpath(metaDir.absolutePath());
	}
	uiMetaItemsVersion = GetFileVersion(metaDir.absoluteFilePath(HYMETA_DataFile), true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir dataDir(pProj->GetAssetsAbsPath());
	if(dataDir.exists() == false)
	{
		HyGuiLog("Data directory is missing, recreating", LOGTYPE_Info);
		dataDir.mkpath(dataDir.absolutePath());
	}
	uiDataItemsVersion = GetFileVersion(dataDir.absoluteFilePath(HYASSETS_DataFile), true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir metaAtlasDir(pProj->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	if(metaAtlasDir.exists() == false)
	{
		HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
		metaAtlasDir.mkpath(metaAtlasDir.absolutePath());
	}
	uiMetaAtlasVersion = GetFileVersion(metaAtlasDir.absoluteFilePath(HYMETA_AtlasFile), true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir dataAtlasDir(pProj->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true));
	if(dataAtlasDir.exists() == false)
	{
		HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
		dataAtlasDir.mkpath(dataAtlasDir.absolutePath());
	}
	uiDataAtlasVersion = GetFileVersion(dataAtlasDir.absoluteFilePath(HYASSETS_AtlasFile), false);

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
		HyGuiLog("Mismatching file versions found between data and meta", LOGTYPE_Error);
	}

	// Note: Each patch 'falls through' each case statement
	switch(iFileVersion)
	{
	case 0:
		if(Patch_0to1(pProj) == false)
			return;
	case 1:
		break; // current version

	default:
		HyGuiLog("File version unknown: " % QString::number(iFileVersion), LOGTYPE_Error);
	}
}

/*static*/ int VersionPatcher::GetFileVersion(QString sFilePath, bool bIsMeta)
{
	QFile file(sFilePath);
	if(file.exists())
	{
		if(!file.open(QIODevice::ReadOnly))
			HyGuiLog(QString("VersionPatcher::GetFileVersion() could not open ") % sFilePath, LOGTYPE_Error);

		QJsonDocument fileDoc;
		if(bIsMeta)
		{
#ifdef HYGUI_UseBinaryMetaFiles
			fileDoc = QJsonDocument::fromBinaryData(file.readAll());
#else
			fileDoc = QJsonDocument::fromJson(file.readAll());
#endif
		}
		else
			fileDoc = QJsonDocument::fromJson(file.readAll());

		file.close();

		if(fileDoc.isArray()) // All files use object as root as of version 1
			return 0;

		QJsonObject fileObj = fileDoc.object();
		if(fileObj.keys().contains("_fileVersion") == false)
			return 0;
		else
			return fileObj["_fileVersion"].toInt();
	}
	else
		return -1;
}

/*static*/ bool VersionPatcher::Patch_0to1(Project *pProj)
{
	QDir dataItemsDir(pProj->GetAssetsAbsPath());
	QFile dataItemsFile(dataItemsDir.absoluteFilePath(HYASSETS_DataFile));
	if(dataItemsFile.open(QIODevice::ReadWrite) == false)
	{
		HyGuiLog(QString("Couldn't open ") % HYASSETS_DataFile % " for reading/writing: " % dataItemsFile.errorString(), LOGTYPE_Error);

		dataItemsFile.close();
		return false;
	}
	QJsonDocument dataItemsDoc = QJsonDocument::fromJson(dataItemsFile.readAll());
	QJsonObject dataItemsObj = dataItemsDoc.object();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QDir metaItemsDir(pProj->GetMetaDataAbsPath());
	QFile metaItemsFile(metaItemsDir.absoluteFilePath(HYMETA_DataFile));
	if(metaItemsFile.open(QIODevice::ReadWrite) == false)
	{
		HyGuiLog(QString("Couldn't open ") % HYMETA_DataFile % " for reading/writing: " % metaItemsFile.errorString(), LOGTYPE_Error);

		dataItemsFile.close();
		metaItemsFile.close();
		return false;
	}
	QJsonDocument metaItemsDoc;
#ifdef HYGUI_UseBinaryMetaFiles
	metaItemsDoc = QJsonDocument::fromBinaryData(metaItemsFile.readAll());
#else
	metaItemsDoc = QJsonDocument::fromJson(metaItemsFile.readAll());
#endif
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
