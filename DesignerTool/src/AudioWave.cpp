/**************************************************************************
 *	HyGuiWave.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioWave.h"

AudioWave::AudioWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint uiErrors) :  m_uiWAVE_BANK_ID(uiWaveBankId),
                                                                                                                                                                                        m_uiChecksum(uiChecksum),
                                                                                                                                                                                        m_sName(sName),
                                                                                                                                                                                        m_uiFormatType(uiFormatType),
                                                                                                                                                                                        m_uiNumChannels(uiNumChannels),
                                                                                                                                                                                        m_uiBitsPerSample(uiBitsPerSample),
                                                                                                                                                                                        m_uiSamplesPerSec(uiSamplesPerSec),
                                                                                                                                                                                        m_uiErrors(uiErrors)
{
}

AudioWave::~AudioWave()
{
}

quint32 AudioWave::GetChecksum()
{
    return m_uiChecksum;
}

QString AudioWave::GetName()
{
    return m_sName;
}

QIcon AudioWave::GetIcon()
{
    if(m_uiErrors == 0)
        return HyGlobal::AudioIcon(AUDIO_Wave);
    else
        return HyGlobal::AudioIcon(AUDIO_Wave_Warning);
}

QString AudioWave::ConstructWaveFileName()
{
    QString sMetaImgName;
    sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiChecksum, m_sName.toStdString().c_str());
    sMetaImgName += ".wav";

    return sMetaImgName;
}

uint32 AudioWave::GetDataSize()
{
    return m_uiDataSize;
}

QString AudioWave::GetDescription()
{
    QString sDesc;
    sDesc = QString::number(m_uiBitsPerSample) % "-bit ";
    
    switch(m_uiFormatType)
    {
    case 1:     sDesc += "PCM ";        break;
    case 3:     sDesc += "IEEE Float "; break;
    case 0x161: sDesc += "WMAUDIO2 ";   break;
    case 0x162: sDesc += "WMAUDIO3 ";   break;
    case 0x166: sDesc += "XMA2 ";       break;  // XMA2 is supported by Xbox One
    case 0x2:   sDesc += "ADPCM ";      break;
    case 0xFFFE:sDesc += "EXTENSIBLE "; break;
    }
    
    sDesc += QString::number(m_uiSamplesPerSec) % " Hz, ";
    
    switch(m_uiNumChannels)
    {
    case 1:     sDesc += "Mono ";       break;
    case 2:     sDesc += "Stereo ";     break;
    default:    sDesc += "Multi Channel";break;
    }
    
    return sDesc;
}

QString AudioWave::GetSizeDescription()
{
    return m_sName % " - " % QString::number(m_uiDataSize) % "bytes";
}

void AudioWave::GetJsonObj(QJsonObject &waveObj)
{
    waveObj.insert("checksum", QJsonValue(static_cast<qint64>(m_uiChecksum)));
    waveObj.insert("name", m_sName);
    waveObj.insert("formatType", m_uiFormatType);
    waveObj.insert("numChannels", m_uiNumChannels);
    waveObj.insert("bitsPerSample", m_uiBitsPerSample);
    waveObj.insert("samplesPerSec", QJsonValue(static_cast<int>(m_uiSamplesPerSec)));
    waveObj.insert("dataSize", QJsonValue(static_cast<int>(m_uiDataSize)));
    waveObj.insert("errors", QJsonValue(static_cast<int>(m_uiErrors)));
}

void AudioWave::SetError(eGuiFrameError eError)
{
    m_uiErrors |= (1 << eError);

//    if(m_pTreeWidgetItem)
//    {
//        m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
//        m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
//    }
}

void AudioWave::ClearError(eGuiFrameError eError)
{
    m_uiErrors &= ~(1 << eError);

//    if(m_pTreeWidgetItem)
//    {
//        if(m_uiErrors == 0)
//            m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(m_eType));
//        else
//        {
//            m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
//            m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
//        }
//    }
}

uint AudioWave::GetErrors()
{
    return m_uiErrors;
}

// Below structures fit in aligned memory of a file type, and should be tightly packed
#pragma pack(push,1)

struct RIFFChunk
{
    uint32_t tag;
    uint32_t size;
};

struct RIFFChunkHeader
{
    uint32_t tag;
    uint32_t size;
    uint32_t riff;
};

// OLD general waveform format structure (information common to all formats)
struct HY_WAVEFORMAT
{
    uint16_t    uiFormatTag;        // format type
    uint16_t    uiChannels;         // number of channels (i.e. mono, stereo, etc.)
    uint32_t    uiSamplesPerSec;    // sample rate
    uint32_t    uiAvgBytesPerSec;   // for buffer estimation
    uint16_t    uiBlockAlign;       // block size of data
};

// Extended waveform format structure used for all non-PCM formats. This structure is common to all non-PCM formats.
struct HY_WAVEFORMATEX
{
    uint16_t    uiFormatTag;        // format type
    uint16_t    uiChannels;         // number of channels (i.e. mono, stereo...)
    uint32_t    uiSamplesPerSec;    // sample rate
    uint32_t    uiAvgBytesPerSec;   // for buffer estimation
    uint16_t    uiBlockAlign;       // block size of data
    uint16_t    uiBitsPerSample;    // number of bits per sample of mono data
    uint16_t    uiSize;             // the count in bytes of the size of
                                    // extra information (after cbSize)
};

// New wave format development should be based on the HY_WAVEFORMATEXTENSIBLE structure.
// HY_WAVEFORMATEXTENSIBLE allows you to avoid having to register a new format tag with Microsoft.
// Simply define a new HY_WAVEGUID value for the HY_WAVEFORMATEXTENSIBLE.SubFormat field and use WAVE_FORMAT_EXTENSIBLE in the WAVEFORMATEXTENSIBLE.Format.wFormatTag field.
struct HY_WAVEGUID
{
    uint32_t            Data1;
    uint16_t            Data2;
    uint16_t            Data3;
    uint8_t             Data4[8];
};
struct HY_WAVEFORMATEXTENSIBLE
{
    HY_WAVEFORMATEX     Format;
    union
    {
        uint16_t    wValidBitsPerSample;    // bits of precision
        uint16_t    wSamplesPerBlock;       // valid if wBitsPerSample==0
        uint16_t    wReserved;              // If neither applies, set to zero.
    } Samples;
    
    uint32_t            dwChannelMask;      // which channels are present in stream
    
    HY_WAVEGUID         SubFormat;
};

// TODO: Support big-endian as well?
//       QSysInfo::ByteOrder == QSysInfo::BigEndian
//       
//       LittleEndian is below
const uint32_t FOURCC_RIFF_TAG      = 'FFIR';
const uint32_t FOURCC_FORMAT_TAG    = ' tmf';
const uint32_t FOURCC_DATA_TAG      = 'atad';
const uint32_t FOURCC_WAVE_FILE_TAG = 'EVAW';
const uint32_t FOURCC_XWMA_FILE_TAG = 'AMWX';
const uint32_t FOURCC_DLS_SAMPLE    = 'pmsw';
const uint32_t FOURCC_MIDI_SAMPLE   = 'lpms';
const uint32_t FOURCC_XWMA_DPDS     = 'sdpd';
const uint32_t FOURCC_XMA_SEEK      = 'kees';

//--------------------------------------------------------------------------------------

const RIFFChunk *FindChunk(const uint8_t *pData, uint32 uiDataSize, uint32_t uiTagToFind)
{
    if(uiDataSize == 0)
        return NULL;

    const uint8_t *pCurrentPtr = pData;
    const uint8_t *pEndOfData = pData + uiDataSize;

    while(pEndOfData > (pCurrentPtr + sizeof(RIFFChunk)))
    {
        const RIFFChunk *pHeader = reinterpret_cast<const RIFFChunk *>(pCurrentPtr);
        if (pHeader->tag == uiTagToFind)
            return pHeader;

        ptrdiff_t offset = pHeader->size + sizeof(RIFFChunk);
        pCurrentPtr += offset;
    }

    return NULL;
}

//--------------------------------------------------------------------------------------

/*static*/ bool AudioWave::ParseWaveFile(QFileInfo waveFileInfo, quint32 &uiChecksumOut, QString &sNameOut, uint16 &uiFormatTagOut, uint16 &uiNumChannelsOut, uint16 &uiBitsPerSampleOut, uint32 &uiSamplesPerSecOut)
{
    if(waveFileInfo.exists() == false)
    {
        HyGuiLog(waveFileInfo.filePath() % " file does not exist", LOGTYPE_Error);
        return false;
    }
    
    sNameOut = waveFileInfo.baseName();
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the wave file into memory
    QFile waveFile(waveFileInfo.absoluteFilePath());
    if(!waveFile.open(QIODevice::ReadOnly))
    {
        HyGuiLog("HyGuiWave::ParseWaveFile could not open the wave file: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    QByteArray waveData = waveFile.readAll();
    waveFile.close();
    
    if(waveData.size() == 0)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile readAll returned 0: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    if(waveData.size() < (sizeof(RIFFChunk)*2 + sizeof(uint32_t) + 14/*sizeof(WAVEFORMAT)*/))
    {
        HyGuiLog("HyGuiWave::ParseWaveFile readAll returned a value smaller than the header info: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    const uint8_t *pWavEnd = reinterpret_cast<uint8_t *>(waveData.data() + waveData.size());
    uiChecksumOut = HyGlobal::CRCData(0, reinterpret_cast<uchar *>(waveData.data()), waveData.size());
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Locate RIFF 'WAVE'
    const RIFFChunk *pRiffChunk = FindChunk(reinterpret_cast<uint8_t *>(waveData.data()), waveData.size(), FOURCC_RIFF_TAG);
    if(!pRiffChunk || pRiffChunk->size < 4)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile FindChunk could not locate FOURCC_RIFF_TAG: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    const RIFFChunkHeader *pRiffHeader = reinterpret_cast<const RIFFChunkHeader *>(pRiffChunk);
    if (pRiffHeader->riff != FOURCC_WAVE_FILE_TAG && pRiffHeader->riff != FOURCC_XWMA_FILE_TAG)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile FindChunk (FOURCC_RIFF_TAG) returned invalid chunk: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Locate 'fmt '
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(pRiffHeader) + sizeof(RIFFChunkHeader);
    if((ptr + sizeof(RIFFChunk)) > pWavEnd)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile parsing next chunk reached EOF: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    const RIFFChunk *pFmtChunk = FindChunk(ptr, pRiffHeader->size, FOURCC_FORMAT_TAG);
    if(!pFmtChunk || pFmtChunk->size < 16/*sizeof(PCMWAVEFORMAT)*/)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile FindChunk could not locate FOURCC_FORMAT_TAG: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    ptr = reinterpret_cast<const uint8_t *>(pFmtChunk) + sizeof(RIFFChunk);
    if(ptr + pFmtChunk->size > pWavEnd)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile parsing chunk reached EOF: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Validate WAVEFORMAT (focused on chunk size and format tag, not other data that XAUDIO2 will validate)
    const HY_WAVEFORMAT *pWaveFormat = reinterpret_cast<const HY_WAVEFORMAT *>(ptr);
    
    bool bIsDPDS = false;
    bool bIsSeek = false;
    
    // Can be a PCMWAVEFORMAT (8 bytes) or WAVEFORMATEX (10 bytes)
    if(pWaveFormat->uiFormatTag != 1 /*WAVE_FORMAT_PCM*/ && pWaveFormat->uiFormatTag != 3 /*WAVE_FORMAT_IEEE_FLOAT*/)
    {
        // We validated chunk as at least sizeof(PCMWAVEFORMAT) above
        if(pFmtChunk->size < sizeof(HY_WAVEFORMATEX))
        {
            HyGuiLog("HyGuiWave::ParseWaveFile non-PCMWAVEFORMAT format chunk is not large enough to hold WAVEFORMATEX: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
            return false;
        }

        const HY_WAVEFORMATEX *pWaveFormatEx = reinterpret_cast<const HY_WAVEFORMATEX*>(ptr);

        if(pFmtChunk->size < (sizeof(HY_WAVEFORMATEX) + pWaveFormatEx->uiSize))
        {
            HyGuiLog("HyGuiWave::ParseWaveFile non-PCMWAVEFORMAT format chunk is not large enough to hold WAVEFORMATEX and its appended extra format information: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
            return false;
        }

        switch(pWaveFormatEx->uiFormatTag)
        {
        case 0x161: // WAVE_FORMAT_WMAUDIO2
        case 0x162: // WAVE_FORMAT_WMAUDIO3
            bIsDPDS = true;
            break;

        case 0x166: // WAVE_FORMAT_XMA2 - XMA2 is supported by Xbox One
            if((pFmtChunk->size < 52 /*sizeof(XMA2WAVEFORMATEX)*/) || (pWaveFormatEx->uiSize < 34 /*(sizeof(XMA2WAVEFORMATEX) - sizeof(WAVEFORMATEX))*/))
            {
                HyGuiLog("HyGuiWave::ParseWaveFile WAVE_FORMAT_XMA2 format chunk is not large enough: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
                return false;
            }
            bIsSeek = true;
            break;

        case 0x2:   // WAVE_FORMAT_ADPCM
            if((pFmtChunk->size < (sizeof(HY_WAVEFORMATEX) + 32)) || (pWaveFormatEx->uiSize < 32 /*MSADPCM_FORMAT_EXTRA_BYTES*/))
            {
                HyGuiLog("HyGuiWave::ParseWaveFile WAVE_FORMAT_ADPCM format chunk is not large enough: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
                return false;
            }
            break;

        case 0xFFFE: // WAVE_FORMAT_EXTENSIBLE
            if((pFmtChunk->size < sizeof(HY_WAVEFORMATEXTENSIBLE)) || ( pWaveFormatEx->uiSize < (sizeof(HY_WAVEFORMATEXTENSIBLE) - sizeof(HY_WAVEFORMATEX))))
            {
                HyGuiLog("HyGuiWave::ParseWaveFile WAVE_FORMAT_EXTENSIBLE format chunk is not large enough: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
                return false;
            }
            else
            {
                const HY_WAVEFORMATEXTENSIBLE *pWaveFormatExtensible = reinterpret_cast<const HY_WAVEFORMATEXTENSIBLE *>(ptr);
                
                // Check against standard KSDATAFORMAT_SUBTYPE_PCM GUID (ignore first 4-byte field)
                const HY_WAVEGUID standardPCM_GUID = {0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
                if(memcmp(reinterpret_cast<const unsigned char *>(&pWaveFormatExtensible->SubFormat) + sizeof(uint32_t),
                          reinterpret_cast<const unsigned char *>(&standardPCM_GUID) + sizeof(uint32_t),
                          sizeof(HY_WAVEGUID) - sizeof(uint32_t)) != 0)
                {
                    HyGuiLog("HyGuiWave::ParseWaveFile HY_WAVEFORMATEXTENSIBLE is not using the KSDATAFORMAT_SUBTYPE_PCM GUID: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
                    return false;
                }
                
                switch(pWaveFormatExtensible->SubFormat.Data1)
                {
                case 1: // WAVE_FORMAT_PCM
                case 3: // WAVE_FORMAT_IEEE_FLOAT
                    break;
                
                // MS-ADPCM and XMA2 are not supported as WAVEFORMATEXTENSIBLE
                
                case 0x161: // WAVE_FORMAT_WMAUDIO2
                case 0x162: // WAVE_FORMAT_WMAUDIO3
                    bIsDPDS = true;
                    break;
                
                default:
                    HyGuiLog("HyGuiWave::ParseWaveFile HY_WAVEFORMATEXTENSIBLE is using an unsupported GUID: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
                    return false;
                }

            }
            break;

        default:
            HyGuiLog("HyGuiWave::ParseWaveFile HY_WAVEFORMATEX is using an unsupported format tag: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
            return false;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: Move this code elsewhere for when we want the raw audio data
    // Locate 'data'
    ptr = reinterpret_cast<const uint8_t *>(pRiffHeader) + sizeof(RIFFChunkHeader);
    if((ptr + sizeof(RIFFChunk)) > pWavEnd)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile parsing riff chunk reached EOF: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }

    const RIFFChunk *pDataChunk = FindChunk(ptr, pRiffChunk->size, FOURCC_DATA_TAG);
    if(!pDataChunk || !pDataChunk->size)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile FindChunk (FOURCC_DATA_TAG) returned invalid chunk: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }

    ptr = reinterpret_cast<const uint8_t *>(pDataChunk) + sizeof(RIFFChunk);
    if(ptr + pDataChunk->size > pWavEnd)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile parsing data chunk reached EOF: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }

    // TODO: Move this code elsewhere for when we want the raw audio data
//    *pdata = ptr;
//    *dataSize = pDataChunk->size;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    const HY_WAVEFORMATEX *pWaveFormatEx = reinterpret_cast<const HY_WAVEFORMATEX*>(pWaveFormat);
                
    uiFormatTagOut = pWaveFormatEx->uiFormatTag;
    uiNumChannelsOut = pWaveFormatEx->uiChannels;
    uiBitsPerSampleOut = pWaveFormatEx->uiBitsPerSample;
    uiSamplesPerSecOut = pWaveFormatEx->uiSamplesPerSec;



//    hr = WaveFindLoopInfo( wavData.get(), bytesRead, &result.loopStart, &result.loopLength );
//    if ( FAILED(hr) )
//        return hr;

    if(bIsDPDS)
    {
        int asdf = 0;
//        if(WaveFindTable(wavData.get(), bytesRead, FOURCC_XWMA_DPDS, &result.seek, &result.seekCount) == false)
//        {
//            HyGuiLog("HyGuiWave::ParseWaveFile WaveFindTable for FOURCC_XWMA_DPDS failed: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
//            return false;
//        }
    }
    else if(bIsSeek)
    {
        int asdf = 0;
//        if(WaveFindTable(wavData.get(), bytesRead, FOURCC_XMA_SEEK, &result.seek, &result.seekCount) == false)
//        {
//            HyGuiLog("HyGuiWave::ParseWaveFile WaveFindTable for FOURCC_XMA_SEEK failed: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
//            return false;
//        }
    }
    
    return true;
}
