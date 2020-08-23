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

AudioManagerModel::AudioManagerModel(Project &projRef) :
	IManagerModel(projRef, ITEM_Audio)
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

/*virtual*/ QStringList AudioManagerModel::GetSupportedFileExtList() /*override*/
{
	return QStringList() << ".wav";
}

/*virtual*/ void AudioManagerModel::OnAllocateDraw(IManagerDraw *&pDrawOut) /*override*/
{
}

void AudioManagerModel::Repack(uint uiBankIndex, QSet<AudioAsset *> newAssetSet)
{

}

/*virtual*/ AssetItemData *AudioManagerModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	AudioAsset *pNewFrame = new AudioAsset(*this,
										   HyGlobal::GetTypeFromString(metaObj["itemType"].toString()),
										   QUuid(metaObj["assetUUID"].toString()),
										   JSONOBJ_TOINT(metaObj, "checksum"),
										   JSONOBJ_TOINT(metaObj, "bankId"),
										   metaObj["name"].toString(),
										   metaObj["format"].toString(),
										   metaObj["errors"].toInt(0));

	return pNewFrame;
}

/*virtual*/ QList<AssetItemData *> AudioManagerModel::OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<QUuid> correspondingUuidList) /*override*/
{
	// Error check all the imported assets before adding them, and cancel entire import if any fail
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		QFileInfo fileInfo(sImportAssetList[i]);

		// TODO: Error check wav format, or convert if possible

		// TODO: Maybe support importing lossless compressed file types
		//QStringList sCodecList = QAudioDeviceInfo::defaultOutputDevice().supportedCodecs();
		//QAudioDecoder *decoder = new QAudioDecoder(this);
		//decoder->setAudioFormat(m_DesiredRawFormat);
		//decoder->setSourceFilename("level1.mp3");
		//connect(decoder, SIGNAL(bufferReady()), this, SLOT(readBuffer()));
		//decoder->start();
	}

	// Passed error check: proceed with import
	QList<AssetItemData *> returnList;
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		// ImportSound calls RegisterAsset() on valid imports
		AudioAsset *pNewAsset = ImportSound(QFileInfo(sImportAssetList[i]).baseName(), uiBankId, eType, correspondingUuidList[i]);
		if(pNewAsset)
			returnList.append(pNewAsset);
	}

	// Repack asset bank with newly imported audio
	if(returnList.empty() == false)
	{
		QSet<AudioAsset *> newSet;
		for(auto pItem : returnList.toSet())
			newSet.insert(static_cast<AudioAsset *>(pItem));
		Repack(GetBankIndexFromBankId(uiBankId), newSet);
	}

	return returnList;
}

/*virtual*/ bool AudioManagerModel::OnRemoveAssets(QList<AssetItemData *> assetList) /*override*/
{
	QList <uint> affectedBankIndexList;
	for(int i = 0; i < assetList.count(); ++i)
	{
		affectedBankIndexList.push_back(GetBankIndexFromBankId(assetList[i]->GetBankId()));
		DeleteAsset(assetList[i]);
	}

	for(int i = 0; i < affectedBankIndexList.count(); ++i)
		Repack(affectedBankIndexList[i], QSet<AudioAsset *>());
	
	return true;
}

/*virtual*/ bool AudioManagerModel::OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) /*override*/
{
	// Error check all the replacement assets before adding them, and cancel entire replace if any fail
	for(int i = 0; i < assetList.count(); ++i)
	{
		QFileInfo fileInfo(sImportAssetList[i]);

		// TODO: Error check wav format, or convert if possible

		// TODO: Maybe support importing lossless compressed file types
		//QStringList sCodecList = QAudioDeviceInfo::defaultOutputDevice().supportedCodecs();
		//QAudioDecoder *decoder = new QAudioDecoder(this);
		//decoder->setAudioFormat(m_DesiredRawFormat);
		//decoder->setSourceFilename("level1.mp3");
		//connect(decoder, SIGNAL(bufferReady()), this, SLOT(readBuffer()));
		//decoder->start();
	}

	QList <uint> affectedBankIndexList;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AudioAsset *pAudio = static_cast<AudioAsset *>(assetList[i]);
		HyGuiLog("Replacing: " % pAudio->GetName() % " -> " % sImportAssetList[i], LOGTYPE_Info);

		affectedBankIndexList.push_back(GetBankIndexFromBankId(pAudio->GetBankId()));

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
		pAudio->ReplaceAudio(fileInfo.baseName(), uiChecksum);
		if(QFile::copy(fileInfo.absoluteFilePath(), m_MetaDir.absoluteFilePath(pAudio->ConstructMetaFileName())) == false)
		{
			HyGuiLog("AudioManagerModel::OnReplaceAssets - Could not copy file to meta " % fileInfo.absoluteFilePath(), LOGTYPE_Error);
			HyGuiLog(fileInfo.baseName() % " will not have a valid meta file!", LOGTYPE_Error);
		}

		// Re-enter the audio into the registry
		RegisterAsset(pAudio);
	}

	for(int i = 0; i < affectedBankIndexList.count(); ++i)
		Repack(affectedBankIndexList[i], QSet<AudioAsset *>());
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

	// Repack all old affected banks
	for(int i = 0; i < affectedBankIndexList.count(); ++i)
		Repack(affectedBankIndexList[i], QSet<AudioAsset *>());

	// Repack new affected atlas group
	Repack(GetBankIndexFromBankId(uiNewBankId), assetsGoingToNewBankSet);

	return true;
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
			assetObj.insert("fileName", QJsonValue(bankAssetListRef[i]->ConstructMetaFileName()));

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

AudioAsset *AudioManagerModel::ImportSound(QString sFilePath, quint32 uiBankId, HyGuiItemType eType, QUuid uuid)
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
	
	AudioAsset *pNewAsset = new AudioAsset(*this, eType, uuid/*QUuid::createUuid()*/, uiChecksum, uiBankId, fileInfo.baseName(), "wav", 0);

	if(QFile::copy(sFilePath, m_MetaDir.absoluteFilePath(pNewAsset->ConstructMetaFileName())) == false)
	{
		HyGuiLog("AudioManagerModel::ImportSound - Could not copy file to meta " % sFilePath, LOGTYPE_Error);
		delete pNewAsset;
		return nullptr;
	}

	RegisterAsset(pNewAsset);
	return pNewAsset;
}
