/**************************************************************************
 *	AudioAsset.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AudioAsset_H
#define AudioAsset_H

#include "IAssetItemData.h"
#include "ProjectItemData.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

struct WaveHeader
{
	/* RIFF Chunk Descriptor */
	uint8_t         RIFF[4];        // RIFF Header Magic header
	uint32_t        ChunkSize;      // RIFF Chunk Size
	uint8_t         WAVE[4];        // WAVE Header
	/* "fmt" sub-chunk */
	uint8_t         fmt[4];         // FMT header
	uint32_t        Subchunk1Size;  // Size of the fmt chunk
	uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM  ***Values other than 1 indicate some form of compression***
	uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
	uint32_t        BytesPerSec;    // bytes per second
	uint16_t        BlockAlign;     // 2=16-bit mono, 4=16-bit stereo
	uint16_t        BitsPerSample;  // Number of bits per sample
	/* "data" sub-chunk */
	uint8_t         Subchunk2ID[4]; // "data"  string
	uint32_t        Subchunk2Size;  // NumSamples * NumChannels * BitsPerSample/8

	WaveHeader() { }

	WaveHeader(QJsonObject headerObj)
	{
		RIFF[0] = 'R'; RIFF[1] = 'I'; RIFF[2] = 'F'; RIFF[3] = 'F';
		ChunkSize = headerObj["chunkSize"].toInt();
		WAVE[0] = 'W'; WAVE[1] = 'A'; WAVE[2] = 'V'; WAVE[3] = 'E';
		
		fmt[0] = 'f'; fmt[1] = 'm'; fmt[2] = 't'; fmt[3] = ' ';
		Subchunk1Size = 16;
		AudioFormat = headerObj["audioFormat"].toInt();
		NumOfChan = headerObj["numChannels"].toInt();
		SamplesPerSec = headerObj["sampleRate"].toInt();
		BytesPerSec = headerObj["byteRate"].toInt();
		BlockAlign = headerObj["blockAlign"].toInt();
		BitsPerSample = headerObj["bitsPerSample"].toInt();
		
		Subchunk2ID[0] = 'd'; Subchunk2ID[1] = 'a'; Subchunk2ID[2] = 't'; Subchunk2ID[3] = 'a';
		Subchunk2Size = headerObj["dataSize"].toInt();
	}
};

class AudioAsset : public AssetItemData
{
	Q_OBJECT

	WaveHeader		m_WaveHeader;

	bool			m_bIsMusic;
	bool			m_bExportMono;
	bool			m_bCompressed;
	float			m_fVbrQuality;
	int32			m_iGlobalLimit;

public:
	AudioAsset(IManagerModel &modelRef, HyGuiItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, const WaveHeader &wavHeaderRef, bool bIsMusic, bool bExportMono, int32 iGlobalLimit, bool bCompressed, float fVbrQuality, uint uiErrors);
	~AudioAsset();

	bool IsMusic() const;
	bool IsExportMono() const;
	int32 GetGlobalLimit() const;
	bool IsCompressed() const;
	float GetVbrQuality() const;

	QString ConstructDataFileName(bool bWithExt) const;

	void ReplaceAudio(QString sName, uint32 uiChecksum, const WaveHeader &wavHeaderRef);

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;
};

#endif // AudioAsset_H
