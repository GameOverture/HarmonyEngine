/**************************************************************************
 *	AudioManagerModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioManagerModel.h"
#include "Project.h"
#include "MainWindow.h"
#include "AudioRepackThread.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAudioDecoder>
#include <QAudioDeviceInfo>
#include <QMimeData>

AudioManagerModel::AudioManagerModel(Project &projRef) :
	IManagerModel(projRef, ASSET_Audio)
{
	m_DesiredRawFormat.setChannelCount(2);
	m_DesiredRawFormat.setCodec("audio/wav"); // also consider "audio/x-raw" or "audio/pcm"
	m_DesiredRawFormat.setSampleType(QAudioFormat::SignedInt);
	m_DesiredRawFormat.setSampleSize(16);
	m_DesiredRawFormat.setSampleRate(48000);
}

/*virtual*/ AudioManagerModel::~AudioManagerModel()
{
}

bool AudioManagerModel::IsWaveValid(QString sFilePath, WaveHeader &wavHeaderOut)
{
	QFile file(sFilePath);
	if(file.open(QIODevice::ReadOnly) == false)
	{
		HyGuiLog("Could not open file: " % sFilePath, LOGTYPE_Warning);
		return false;
	}

	
	file.read(reinterpret_cast<char *>(&wavHeaderOut), sizeof(WaveHeader));
	file.close();

	if(wavHeaderOut.AudioFormat != 1)
	{
		HyGuiLog("Only PCM (i.e. Linear quantization) wave files are supported", LOGTYPE_Warning);
		return false;
	}

	if(wavHeaderOut.NumOfChan > 2)
	{
		HyGuiLog("Only mono and stereo wave files are supported", LOGTYPE_Warning);
		return false;
	}

	return true;
}

/*virtual*/ QString AudioManagerModel::OnBankInfo(uint uiBankIndex) /*override*/
{
	auto assetList = GetBankAssets(uiBankIndex);
	QString sInfo = "Num Assets: " % QString::number(assetList.size());
	return sInfo;
}

/*virtual*/ bool AudioManagerModel::OnBankSettingsDlg(uint uiBankIndex) /*override*/
{
	QList<AssetItemData *> assetList = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
	bool bBankHasAssets = assetList.size() > 0;
	bool bAccepted = true;
	
	return bAccepted;
}

/*virtual*/ QStringList AudioManagerModel::GetSupportedFileExtList() const /*override*/
{
	return QStringList() << ".wav";
}

/*virtual*/ void AudioManagerModel::OnAllocateDraw(IManagerDraw *&pDrawOut) /*override*/
{
}

void AudioManagerModel::Repack(QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAudioList)
{
	AudioRepackThread *pWorkerThread = new AudioRepackThread(affectedAudioList, m_MetaDir);
	StartRepackThread("Repacking Audio", pWorkerThread);
}

/*virtual*/ void AudioManagerModel::OnInit() /*override*/
{
	// Create data manifest file if one doesn't exist
	QFile manifestFile(m_DataDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_DataExt));
	if(manifestFile.exists() == false)
		SaveRuntime();
}

/*virtual*/ void AudioManagerModel::OnCreateNewBank(QJsonObject &newMetaBankObjRef) /*override*/
{
}

/*virtual*/ AssetItemData *AudioManagerModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	WaveHeader wavHeader(metaObj["wavHeader"].toObject());
	AudioAsset *pNewFrame = new AudioAsset(*this,
										   HyGlobal::GetTypeFromString(metaObj["itemType"].toString()),
										   QUuid(metaObj["assetUUID"].toString()),
										   JSONOBJ_TOINT(metaObj, "checksum"),
										   JSONOBJ_TOINT(metaObj, "bankId"),
										   metaObj["name"].toString(),
										   wavHeader,
										   metaObj["isMusic"].toBool(),
										   metaObj["isExportMono"].toBool(),
										   metaObj["globalLimit"].toInt(),
										   metaObj["isCompressed"].toBool(),
										   metaObj["vbrQuality"].toDouble(),
										   metaObj["errors"].toInt(0));

	return pNewFrame;
}

/*virtual*/ QList<AssetItemData *> AudioManagerModel::OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) /*override*/
{
	QList<AssetItemData *> returnList;
	QList<WaveHeader> headerList;

	// Error check all the imported assets before adding them, and cancel entire import if any fail
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		headerList.push_back(WaveHeader());
		if(IsWaveValid(sImportAssetList[i], headerList.last()) == false)
			return returnList;
	}

	// Passed error check: proceed with import
	
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		// ImportSound calls RegisterAsset() on valid imports
		AudioAsset *pNewAsset = ImportSound(sImportAssetList[i], uiBankId, eType, correspondingUuidList[i], headerList[i]);
		if(pNewAsset)
			returnList.append(pNewAsset);
	}

	// Repack asset bank with newly imported audio
	if(returnList.empty() == false)
	{
		QSet<AssetItemData *> returnListAsSet(returnList.begin(), returnList.end());
		QSet<AudioAsset *> newSet;
		for(auto pItem : returnListAsSet)
			newSet.insert(static_cast<AudioAsset *>(pItem));

		QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAudioList;
		QPair<BankData *, QSet<AudioAsset *>> bankPair(m_BanksModel.GetBank(GetBankIndexFromBankId(uiBankId)), newSet);
		affectedAudioList.append(bankPair);

		Repack(affectedAudioList);
	}

	return returnList;
}

/*virtual*/ bool AudioManagerModel::OnRemoveAssets(QList<AssetItemData *> assetList) /*override*/
{
	// NOTE: there's no error checking on duplicates in 'assetList', will crash

	QList <uint> affectedBankIndexList;
	for(int i = 0; i < assetList.count(); ++i)
	{
		affectedBankIndexList.push_back(GetBankIndexFromBankId(assetList[i]->GetBankId()));
		DeleteAsset(assetList[i]);
	}


	QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAudioList;
	for(int i = 0; i < affectedBankIndexList.count(); ++i)
	{
		QPair<BankData *, QSet<AudioAsset *>> bankPair(m_BanksModel.GetBank(affectedBankIndexList[i]), QSet<AudioAsset *>());
		affectedAudioList.append(bankPair);
	}
	
	Repack(affectedAudioList);
	
	return true;
}

/*virtual*/ bool AudioManagerModel::OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) /*override*/
{
	QList<WaveHeader> headerList;

	// Error check all the replacement assets before adding them, and cancel entire replace if any fail
	for(int i = 0; i < assetList.count(); ++i)
	{
		headerList.push_back(WaveHeader());
		if(IsWaveValid(sImportAssetList[i], headerList.last()) == false)
			return false;
	}

	QMap<uint, QSet<AudioAsset *>> affectedMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AudioAsset *pAudio = static_cast<AudioAsset *>(assetList[i]);
		HyGuiLog("Replacing: " % pAudio->GetName() % " -> " % sImportAssetList[i], LOGTYPE_Info);

		// Audio asset's checksum will change, so it needs to be removed and readded to the manager's registry
		// First remove the audio from the map
		if(RemoveLookup(pAudio))
			pAudio->DeleteMetaFile();

		// Determine the new checksum
		QFile file(sImportAssetList[i]);
		if(!file.open(QIODevice::ReadOnly))
		{
			HyGuiLog("AudioManagerModel::OnReplaceAssets - Could not open file " % sImportAssetList[i], LOGTYPE_Error);
			return false;
		}
		QByteArray pBinaryData(file.readAll());
		file.close();
		quint32 uiChecksum = HyGlobal::CRCData(0, reinterpret_cast<const uchar *>(pBinaryData.constData()), pBinaryData.size());

		QFileInfo fileInfo(sImportAssetList[i]);
		pAudio->ReplaceAudio(fileInfo.baseName(), uiChecksum, headerList[i]);
		if(QFile::copy(fileInfo.absoluteFilePath(), m_MetaDir.absoluteFilePath(pAudio->ConstructMetaFileName())) == false)
		{
			HyGuiLog("AudioManagerModel::OnReplaceAssets - Could not copy file to meta " % fileInfo.absoluteFilePath(), LOGTYPE_Error);
			HyGuiLog(fileInfo.baseName() % " will not have a valid meta file!", LOGTYPE_Error);
		}

		// Re-enter the audio into the registry
		RegisterAsset(pAudio);

		QSet<AudioAsset *> &affectedSetRef = affectedMap[GetBankIndexFromBankId(pAudio->GetBankId())];
		affectedSetRef.insert(pAudio);
	}

	QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAudioList;
	for(auto iter = affectedMap.begin(); iter != affectedMap.end(); ++iter)
	{
		QPair<BankData *, QSet<AudioAsset *>> bankPair(m_BanksModel.GetBank(iter.key()), iter.value());
		affectedAudioList.append(bankPair);
	}

	Repack(affectedAudioList);

	return true;
}

/*virtual*/ bool AudioManagerModel::OnUpdateAssets(QList<AssetItemData *> assetList) /*override*/
{
	QMap<uint, QSet<AudioAsset *>> affectedMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AudioAsset *pAudio = static_cast<AudioAsset *>(assetList[i]);

		QSet<AudioAsset *> &affectedSetRef = affectedMap[GetBankIndexFromBankId(pAudio->GetBankId())];
		affectedSetRef.insert(pAudio);
	}

	QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAudioList;
	for(auto iter = affectedMap.begin(); iter != affectedMap.end(); ++iter)
	{
		QPair<BankData *, QSet<AudioAsset *>> bankPair(m_BanksModel.GetBank(iter.key()), iter.value());
		affectedAudioList.append(bankPair);
	}

	Repack(affectedAudioList);

	return true;
}

/*virtual*/ bool AudioManagerModel::OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
{
	QList<uint> affectedBankIndexList;			// old
	QSet<AudioAsset *> assetsGoingToNewBankSet;	// new

	for(int i = 0; i < assetsList.count(); ++i)
	{
		if(assetsList[i]->GetBankId() == uiNewBankId)
			continue;

		affectedBankIndexList.push_back(GetBankIndexFromBankId(assetsList[i]->GetBankId()));
		assetsGoingToNewBankSet.insert(static_cast<AudioAsset *>(assetsList[i]));

		MoveAsset(assetsList[i], uiNewBankId);
	}

	QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAudioList;
	for(int i = 0; i < affectedBankIndexList.count(); ++i)
	{
		QPair<BankData *, QSet<AudioAsset *>> bankPair(m_BanksModel.GetBank(affectedBankIndexList[i]), QSet<AudioAsset *>());
		affectedAudioList.append(bankPair);
	}
	QPair<BankData *, QSet<AudioAsset *>> bankPair(m_BanksModel.GetBank(uiNewBankId), assetsGoingToNewBankSet);
	affectedAudioList.append(bankPair);

	Repack(affectedAudioList);

	return true;
}

/*virtual*/ void AudioManagerModel::OnSaveMeta() /*override*/
{
}

/*virtual*/ QJsonObject AudioManagerModel::GetSaveJson() /*override*/
{
	QJsonArray bankArray;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		QJsonObject bankObj;
		bankObj.insert("name", m_BanksModel.GetBank(i)->GetName());
		bankObj.insert("bankId", static_cast<qint64>(m_BanksModel.GetBank(i)->GetId()));

		QJsonArray assetsArray;
		QList<AssetItemData *> &bankAssetListRef = m_BanksModel.GetBank(i)->m_AssetList;
		for(int i = 0; i < bankAssetListRef.size(); ++i)
		{
			QJsonObject assetObj;
			assetObj.insert("checksum", QJsonValue(static_cast<qint64>(bankAssetListRef[i]->GetChecksum())));
			assetObj.insert("fileName", static_cast<AudioAsset *>(bankAssetListRef[i])->ConstructDataFileName(true));
			assetObj.insert("isMusic", static_cast<AudioAsset *>(bankAssetListRef[i])->IsMusic());
			assetObj.insert("globalLimit", static_cast<AudioAsset *>(bankAssetListRef[i])->GetGlobalLimit());

			assetsArray.append(assetObj);
		}
		bankObj.insert("assets", assetsArray);

		bankArray.append(bankObj);
	}

	QJsonObject atlasInfoObj;
	atlasInfoObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	atlasInfoObj.insert("banks", bankArray);

	return atlasInfoObj;
}

/*slot*/ void AudioManagerModel::OnLoadUpdate(QString sMsg, int iPercComplete)
{
	MainWindow::SetLoading(sMsg, iPercComplete);
}

/*slot*/ void AudioManagerModel::OnRepackFinished()
{
	SaveRuntime();
}

AudioAsset *AudioManagerModel::ImportSound(QString sFilePath, quint32 uiBankId, HyGuiItemType eType, QUuid uuid, const WaveHeader &wavHeaderRef)
{
	QFile file(sFilePath);
	if(!file.open(QIODevice::ReadOnly))
	{
		HyGuiLog("AudioManagerModel::ImportSound - Could not open file " % sFilePath, LOGTYPE_Error);
		return nullptr;
	}

	QByteArray pBinaryData(file.readAll());
	file.close();

	quint32 uiChecksum = HyGlobal::CRCData(0, reinterpret_cast<const uchar *>(pBinaryData.constData()), pBinaryData.size());
	QFileInfo fileInfo(sFilePath);
	
	AudioAsset *pNewAsset = new AudioAsset(*this, eType, uuid, uiChecksum, uiBankId, fileInfo.baseName(), wavHeaderRef, false, wavHeaderRef.NumOfChan == 1, -1, false, 1.0f, 0);

	if(QFile::copy(sFilePath, m_MetaDir.absoluteFilePath(pNewAsset->ConstructMetaFileName())) == false)
	{
		HyGuiLog("AudioManagerModel::ImportSound - Could not copy file to meta " % sFilePath, LOGTYPE_Error);
		delete pNewAsset;
		return nullptr;
	}

	RegisterAsset(pNewAsset);
	return pNewAsset;
}
