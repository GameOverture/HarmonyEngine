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
					   bool bExportMono,
					   int32 iGlobalLimit,
					   bool bCompressed,
					   double dVbrQuality,
					   uint uiErrors) :
	AssetItemData(modelRef, eType, uuid, uiChecksum, uiBankId, sName, ".wav", uiErrors),
	m_WaveHeader(wavHeaderRef),
	m_bIsMusic(bIsMusic),
	m_bExportMono(bExportMono),
	m_iGlobalLimit(iGlobalLimit),
	m_bCompressed(bCompressed),
	m_dVbrQuality(dVbrQuality)
{
}

AudioAsset::~AudioAsset()
{
}

bool AudioAsset::IsMusic() const
{
	return m_bIsMusic;
}

bool AudioAsset::IsExportMono() const
{
	return m_bExportMono;
}

int32 AudioAsset::GetGlobalLimit() const
{
	return m_iGlobalLimit;
}

bool AudioAsset::IsCompressed() const
{
	return m_bCompressed;
}

double AudioAsset::GetVbrQuality() const
{
	return m_dVbrQuality;
}

void AudioAsset::SetIsMusic(bool bIsMusic)
{
	m_bIsMusic = bIsMusic;
}

void AudioAsset::SetIsExportMono(bool bIsExportMono)
{
	m_bExportMono = bIsExportMono;
}

void AudioAsset::SetGlobalLimit(int32 iGlobalLimit)
{
	m_iGlobalLimit = iGlobalLimit;
}

void AudioAsset::SetIsCompressed(bool bIsCompressed)
{
	m_bCompressed = bIsCompressed;
}

void AudioAsset::SetVbrQuality(double dVbrQuality)
{
	m_dVbrQuality = dVbrQuality;
}

QString AudioAsset::ConstructDataFileName(bool bWithExt) const
{
	QString sDataName;
	sDataName = sDataName.asprintf("%010u", m_uiChecksum);

	if(bWithExt)
	{
		if(m_bCompressed)
			sDataName += ".ogg";
		else
			sDataName += ".wav";
	}

	return sDataName;
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
	frameObj.insert("isExportMono", m_bExportMono);
	frameObj.insert("globalLimit", m_iGlobalLimit);
	frameObj.insert("isCompressed", m_bCompressed);
	frameObj.insert("vbrQuality", m_dVbrQuality);
}
