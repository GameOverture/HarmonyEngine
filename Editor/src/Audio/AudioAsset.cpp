/**************************************************************************
 *	AudioAsset.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioAsset.h"
#include "IManagerModel.h"
#include "_Dependencies/scriptum/imagepacker.h"

AudioAsset::AudioAsset(IManagerModel &modelRef,
					   HyGuiItemType eType,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   const WaveHeader &wavHeaderRef,
					   bool bIsMusic,
					   int32 iInstanceLimit,
					   uint uiErrors) :
	AssetItemData(modelRef, eType, uuid, uiChecksum, uiBankId, sName, ".wav", uiErrors),
	m_WaveHeader(wavHeaderRef),
	m_bIsMusic(bIsMusic),
	m_iInstanceLimit(iInstanceLimit)
{
}

AudioAsset::~AudioAsset()
{
}

bool AudioAsset::IsMusic() const
{
	return m_bIsMusic;
}

int32 AudioAsset::GetInstanceLimit() const
{
	return m_iInstanceLimit;
}

void AudioAsset::ReplaceAudio(QString sName, uint32 uiChecksum, const WaveHeader &wavHeaderRef)
{
	m_sName = sName;
	m_uiChecksum = uiChecksum;
	m_WaveHeader = wavHeaderRef;
}

/*virtual*/ void AudioAsset::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	QJsonObject wavHeaderObj;
	wavHeaderObj.insert("chunkSize", QJsonValue(static_cast<qint64>(m_WaveHeader.ChunkSize)));
	wavHeaderObj.insert("audioFormat", m_WaveHeader.AudioFormat);
	wavHeaderObj.insert("numChannels", m_WaveHeader.NumOfChan);
	wavHeaderObj.insert("sampleRate", QJsonValue(static_cast<qint64>(m_WaveHeader.SamplesPerSec)));
	wavHeaderObj.insert("byteRate", QJsonValue(static_cast<qint64>(m_WaveHeader.BytesPerSec)));
	wavHeaderObj.insert("blockAlign", m_WaveHeader.BlockAlign);
	wavHeaderObj.insert("bitsPerSample", m_WaveHeader.BitsPerSample);
	wavHeaderObj.insert("dataSize", QJsonValue(static_cast<qint64>(m_WaveHeader.Subchunk2Size)));

	frameObj.insert("wavHeader", wavHeaderObj);
	frameObj.insert("isMusic", m_bIsMusic);
	frameObj.insert("instanceLimit", m_iInstanceLimit);
}
