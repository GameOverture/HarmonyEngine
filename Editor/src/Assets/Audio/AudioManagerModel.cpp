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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAudioDecoder>
#include <QAudioDeviceInfo>
#include <QMimeData>

AudioGroupsModel::AudioGroupsModel()
{
}

/*virtual*/ AudioGroupsModel::~AudioGroupsModel()
{
}

void AudioGroupsModel::AddGroup(QString sName, quint32 uiId)
{
	m_GroupList.push_back(QPair<QString, quint32>(sName, uiId));
}

QString AudioGroupsModel::GetName(uint uiIndex) const
{
	return m_GroupList[uiIndex].first;
}

quint32 AudioGroupsModel::GetId(uint uiIndex) const
{
	return m_GroupList[uiIndex].second;
}

int AudioGroupsModel::GetIndex(quint32 uiId) const
{
	for(int i = 0; i < m_GroupList.size(); ++i)
	{
		if(uiId == m_GroupList[i].second)
			return i;
	}
	return -1;
}

/*virtual*/ int AudioGroupsModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_GroupList.size();
}

/*virtual*/ QVariant AudioGroupsModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if(role == Qt::TextAlignmentRole)
		return Qt::AlignLeft;

	if(role == Qt::DisplayRole || role == Qt::EditRole)
		return QString::number(index.row()) % " - " % m_GroupList[index.row()].first;

	return QVariant();
}

/*virtual*/ QVariant AudioGroupsModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	return QVariant();
}

AudioManagerModel::AudioManagerModel(Project &projRef) :
	IManagerModel(projRef, ASSETMAN_Audio),
	m_uiNextGroupId(2) // Defaults are SFX:0, Music:1
{
	m_DesiredRawFormat.setChannelCount(2);
	m_DesiredRawFormat.setCodec("audio/wav"); // also consider "audio/x-raw" or "audio/pcm"
	m_DesiredRawFormat.setSampleType(QAudioFormat::SignedInt);
	m_DesiredRawFormat.setSampleSize(16);
	m_DesiredRawFormat.setSampleRate(HY_DEFAULT_SAMPLE_RATE);
}

/*virtual*/ AudioManagerModel::~AudioManagerModel()
{
}

AudioGroupsModel &AudioManagerModel::GetGroupsModel()
{
	return m_AudioGroupsModel;
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

int AudioManagerModel::GetGroupIndexFromGroupId(quint32 uiGroupId) const
{
	return m_AudioGroupsModel.GetIndex(uiGroupId);
}

quint32 AudioManagerModel::GetGroupIdFromGroupIndex(uint uiGroupIndex) const
{
	return m_AudioGroupsModel.GetId(uiGroupIndex);
}

/*virtual*/ void AudioManagerModel::OnInit() /*override*/
{
	// Create data runtime file if one doesn't exist
	QFile runtimeFile(m_DataDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_DataExt));
	if(runtimeFile.exists() == false)
		SaveRuntime();

	// Initialize audio groups
	if(runtimeFile.exists() == false)
		HyGuiLog("audio runtime file doesn't exist!", LOGTYPE_Error);
	if(!runtimeFile.open(QIODevice::ReadOnly))
		HyGuiLog(QString("AudioManagerModel::OnInit() could not open ") % HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_DataExt, LOGTYPE_Error);

	QJsonDocument runtimeDoc = QJsonDocument::fromJson(runtimeFile.readAll());
	runtimeFile.close();

	QJsonObject runtimeObj = runtimeDoc.object();
	QJsonArray groupArray = runtimeObj["groups"].toArray();
	if(groupArray.empty())
	{
		// Add default groups
		m_AudioGroupsModel.AddGroup("SFX", 0);
		m_AudioGroupsModel.AddGroup("Music", 1);
	}
	else
	{
		// Parse saved groups
		for(int i = 0; i < groupArray.size(); ++i)
		{
			QJsonObject groupObj = groupArray[i].toObject();
			m_AudioGroupsModel.AddGroup(groupObj["groupName"].toString(), groupObj["groupId"].toInt());
		}
	}

	QFile settingsFile(m_MetaDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_MetaExt));
	if(settingsFile.exists() == false)
		HyGuiLog("audio meta file doesn't exist!", LOGTYPE_Error);
	
	if(!settingsFile.open(QIODevice::ReadOnly))
		HyGuiLog(QString("AudioManagerModel::OnInit() could not open ") % HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_MetaExt, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
	QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
	QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
	settingsFile.close();

	QJsonObject settingsObj = settingsDoc.object();
	m_uiNextGroupId = JSONOBJ_TOINT(settingsObj, "nextGroupId");
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
										   metaObj["groupId"].toInt(),
										   metaObj["isStreaming"].toBool(),
										   metaObj["isExportMono"].toBool(),
										   metaObj["instanceLimit"].toInt(),
										   metaObj["isCompressed"].toBool(),
										   metaObj["vbrQuality"].toDouble(),
										   metaObj["errors"].toInt(0));

	return pNewFrame;
}

/*virtual*/ void AudioManagerModel::OnGenerateAssetsDlg(const QModelIndex &indexDestination) /*override*/
{
}

/*virtual*/ QList<AssetItemData *> AudioManagerModel::OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, ItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) /*override*/
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
		QSet<AssetItemData *> newSet;
		for(auto pItem : returnListAsSet)
			newSet.insert(pItem);

		AddAssetsToRepack(m_BanksModel.GetBank(GetBankIndexFromBankId(uiBankId)), newSet);
	}

	return returnList;
}

/*virtual*/ bool AudioManagerModel::OnRemoveAssets(QStringList sPreviousFilterPaths, QList<AssetItemData *> assetList) /*override*/
{
	QSet<BankData *> affectedBankSet;
	for(int i = 0; i < assetList.count(); ++i)
	{
		affectedBankSet.insert(m_BanksModel.GetBank(GetBankIndexFromBankId(assetList[i]->GetBankId())));
		DeleteAsset(assetList[i]);
	}

	for(auto iter = affectedBankSet.begin(); iter != affectedBankSet.end(); ++iter)
		AddAssetsToRepack(*iter);
		
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

	QMap<uint, QSet<AssetItemData *>> affectedMap;
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

		affectedMap[GetBankIndexFromBankId(pAudio->GetBankId())].insert(pAudio);
	}

	for(auto iter = affectedMap.begin(); iter != affectedMap.end(); ++iter)
		AddAssetsToRepack(m_BanksModel.GetBank(iter.key()), iter.value());

	return true;
}

/*virtual*/ bool AudioManagerModel::OnUpdateAssets(QList<AssetItemData *> assetList) /*override*/
{
	for(int i = 0; i < assetList.count(); ++i)
		AddAssetsToRepack(m_BanksModel.GetBank(GetBankIndexFromBankId(assetList[i]->GetBankId())), assetList[i]);

	return true;
}

/*virtual*/ bool AudioManagerModel::OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
{
	QList<uint> affectedBankIndexList;			// old
	QSet<AssetItemData *> assetsGoingToNewBankSet;	// new

	for(int i = 0; i < assetsList.count(); ++i)
	{
		if(assetsList[i]->GetBankId() == uiNewBankId)
			continue;

		affectedBankIndexList.push_back(GetBankIndexFromBankId(assetsList[i]->GetBankId()));
		assetsGoingToNewBankSet.insert(assetsList[i]);

		MoveAsset(assetsList[i], uiNewBankId);
	}

	QList<QPair<BankData *, QSet<AssetItemData *>>> affectedAudioList;
	for(int i = 0; i < affectedBankIndexList.count(); ++i)
	{
		QPair<BankData *, QSet<AssetItemData *>> bankPair(m_BanksModel.GetBank(affectedBankIndexList[i]), QSet<AssetItemData *>());
		affectedAudioList.append(bankPair);
	}
	QPair<BankData *, QSet<AssetItemData *>> bankPair(m_BanksModel.GetBank(uiNewBankId), assetsGoingToNewBankSet);
	affectedAudioList.append(bankPair);

	for(auto repack : affectedAudioList)
		AddAssetsToRepack(repack.first, repack.second);

	return true;
}

/*virtual*/ void AudioManagerModel::OnFlushRepack() /*override*/
{

}

/*virtual*/ void AudioManagerModel::OnSaveMeta(QJsonObject &metaObjRef) /*override*/
{
	metaObjRef.insert("nextGroupId", static_cast<qint64>(m_uiNextGroupId));
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
			assetObj.insert("groupId", static_cast<AudioAsset *>(bankAssetListRef[i])->GetGroupId());
			assetObj.insert("isStreaming", static_cast<AudioAsset *>(bankAssetListRef[i])->IsStreaming());
			assetObj.insert("instanceLimit", static_cast<AudioAsset *>(bankAssetListRef[i])->GetInstanceLimit());

			assetsArray.append(assetObj);
		}
		bankObj.insert("assets", assetsArray);

		bankArray.append(bankObj);
	}

	QJsonArray groupArray;
	for(int i = 0; i < m_AudioGroupsModel.rowCount(); ++i)
	{
		QJsonObject groupObj;
		groupObj.insert("groupName", m_AudioGroupsModel.GetName(i));
		groupObj.insert("groupId", static_cast<qint64>(m_AudioGroupsModel.GetId(i)));

		groupArray.append(groupObj);
	}

	QJsonObject atlasInfoObj;
	atlasInfoObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	atlasInfoObj.insert("banks", bankArray);
	atlasInfoObj.insert("groups", groupArray);

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

AudioAsset *AudioManagerModel::ImportSound(QString sFilePath, quint32 uiBankId, ItemType eType, QUuid uuid, const WaveHeader &wavHeaderRef)
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
	
	AudioAsset *pNewAsset = new AudioAsset(*this, eType, uuid, uiChecksum, uiBankId, fileInfo.baseName(), wavHeaderRef, GetGroupIdFromGroupIndex(0), false, wavHeaderRef.NumOfChan == 1, 0, false, 1.0f, 0);

	if(QFile::copy(sFilePath, m_MetaDir.absoluteFilePath(pNewAsset->ConstructMetaFileName())) == false)
	{
		HyGuiLog("AudioManagerModel::ImportSound - Could not copy file to meta " % sFilePath, LOGTYPE_Error);
		delete pNewAsset;
		return nullptr;
	}

	RegisterAsset(pNewAsset);
	return pNewAsset;
}
