/**************************************************************************
 *	SoundClip.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SoundClip.h"
#include "IManagerModel.h"
#include "_Dependencies/scriptum/imagepacker.h"

SoundClip::SoundClip(IManagerModel &modelRef,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   const WaveHeader &wavHeaderRef,
					   int32 iGroupId,
					   bool bIsStreaming,
					   bool bExportMono,
					   int32 iInstanceLimit,
					   bool bCompressed,
					   double dVbrQuality,
					   uint uiErrors) :
	IAssetItemData(modelRef, ITEM_SoundClip, uuid, uiChecksum, uiBankId, sName, ".wav", uiErrors),
	m_WaveHeader(wavHeaderRef),
	m_iGroupId(iGroupId),
	m_bIsStreaming(bIsStreaming),
	m_bExportMono(bExportMono),
	m_iInstanceLimit(iInstanceLimit),
	m_bCompressed(bCompressed),
	m_dVbrQuality(dVbrQuality)
{
}

SoundClip::~SoundClip()
{
}

int32 SoundClip::GetGroupId() const
{
	return m_iGroupId;
}

bool SoundClip::IsStreaming() const
{
	return m_bIsStreaming;
}

bool SoundClip::IsExportMono() const
{
	return m_bExportMono;
}

int32 SoundClip::GetInstanceLimit() const
{
	return m_iInstanceLimit;
}

bool SoundClip::IsCompressed() const
{
	return m_bCompressed;
}

double SoundClip::GetVbrQuality() const
{
	return m_dVbrQuality;
}

void SoundClip::SetGroupId(int32 iGroupId)
{
	m_iGroupId = iGroupId;
}

void SoundClip::SetIsStreaming(bool bIsStreaming)
{
	m_bIsStreaming = bIsStreaming;
}

void SoundClip::SetIsExportMono(bool bIsExportMono)
{
	m_bExportMono = bIsExportMono;
}

void SoundClip::SetInstanceLimit(int32 iInstanceLimit)
{
	m_iInstanceLimit = iInstanceLimit;
}

void SoundClip::SetIsCompressed(bool bIsCompressed)
{
	m_bCompressed = bIsCompressed;
}

void SoundClip::SetVbrQuality(double dVbrQuality)
{
	m_dVbrQuality = dVbrQuality;
}

QString SoundClip::ConstructDataFileName(bool bWithExt) const
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

void SoundClip::ReplaceAudio(QString sName, uint32 uiChecksum, const WaveHeader &wavHeaderRef)
{
	m_sName = sName;
	m_uiChecksum = uiChecksum;
	m_WaveHeader = wavHeaderRef;
}

/*virtual*/ QString SoundClip::GetPropertyInfo() /*override*/
{
	QString sInfo;
	sInfo = "Grp: " % QString::number(m_iGroupId);
	if(m_bIsStreaming)
	{
		if(sInfo.isEmpty() == false)
			sInfo += " | ";
		sInfo = "Stream";
	}
	if(m_bExportMono)
	{
		if(sInfo.isEmpty() == false)
			sInfo +=  " | ";
		sInfo += "Mono";
	}
	if(m_bCompressed)
	{
		if(sInfo.isEmpty() == false)
			sInfo +=  " | ";
		sInfo += "Compressed";
	}
	if(m_iInstanceLimit > 0)
	{
		if(sInfo.isEmpty() == false)
			sInfo +=  " | ";
		sInfo += "Limit:" % QString::number(m_iInstanceLimit);
	}

	return sInfo;
}

/*virtual*/ void SoundClip::InsertUniqueJson(QJsonObject &frameObj) /*override*/
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
	frameObj.insert("groupId", m_iGroupId);
	frameObj.insert("isStreaming", m_bIsStreaming);
	frameObj.insert("isExportMono", m_bExportMono);
	frameObj.insert("instanceLimit", m_iInstanceLimit);
	frameObj.insert("isCompressed", m_bCompressed);
	frameObj.insert("vbrQuality", m_dVbrQuality);
}
