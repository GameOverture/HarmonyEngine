/**************************************************************************
 *	HyGuiWave.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiWave.h"

HyGuiWave::HyGuiWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint uiErrors) :  m_uiWAVE_BANK_ID(uiWaveBankId),
                                                                                                                                                                                        m_uiChecksum(uiChecksum),
                                                                                                                                                                                        m_sName(sName),
                                                                                                                                                                                        m_uiFormatType(uiFormatType),
                                                                                                                                                                                        m_uiNumChannels(uiNumChannels),
                                                                                                                                                                                        m_uiBitsPerSample(uiBitsPerSample),
                                                                                                                                                                                        m_uiSamplesPerSec(uiSamplesPerSec),
                                                                                                                                                                                        m_uiErrors(uiErrors)
{
}

HyGuiWave::~HyGuiWave()
{
}

QString HyGuiWave::ConstructImageFileName()
{
    QString sMetaImgName;
    sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiChecksum, m_sName.toStdString().c_str());
    sMetaImgName += ".wav";

    return sMetaImgName;
}

void HyGuiWave::SetError(eGuiFrameError eError)
{
    m_uiErrors |= (1 << eError);

//    if(m_pTreeWidgetItem)
//    {
//        m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
//        m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
//    }
}

void HyGuiWave::ClearError(eGuiFrameError eError)
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

uint HyGuiWave::GetErrors()
{
    return m_uiErrors;
}


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

const RIFFChunk *FindChunk(const char *pData, uint32 uiDataSize, uint32_t uiTagToFind)
{
    if(waveData.size() == 0)
        return NULL;

    const char *pCurrentPtr = pData;
    const char *pEndOfData = pData + uiDataSize;

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

static void HyGuiWave::ParseWaveFile(QFileInfo waveFileInfo, quint32 &uiChecksumOut, QString &sNameOut, uint16 &uiFormatTypeOut, uint16 &uiNumChannelsOut, uint16 &uiBitsPerSampleOut, uint32 &uiSamplesPerSecOut)
{
    if(waveFileInfo.exists() == false)
    {
        HyGuiLog(waveFileInfo.filePath() % " file does not exist", LOGTYPE_Error);
        return false;
    }
    
    sNameOut = waveFileInfo.baseName();
    
    QFile waveFile(waveFileInfo.absoluteFilePath());
    if(!waveFile.open(QIODevice::ReadOnly))
    {
        HyGuiLog("HyGuiWave::ParseWaveFile could not open the wave file: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    QByteArray waveData = waveFile.readAll();
    waveFile.close();
    
    uiChecksumOut = HyGlobal::CRCData(0, waveData.data(), waveData.size());
    
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
    
    
    // Locate RIFF 'WAVE'
    const RIFFChunk *pRiffChunk = FindChunk(waveData.data(), waveData.size(), FOURCC_RIFF_TAG);
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
    
    const char *pWavEnd = waveData.data() + waveData.size();
    
    // Locate 'fmt '
    const char *ptr = reinterpret_cast<const char *>(pRiffHeader) + sizeof(RIFFChunkHeader);
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
    
    ptr = reinterpret_cast<const char *>(pFmtChunk) + sizeof(RIFFChunk);
    if(ptr + pFmtChunk->size > pWavEnd)
    {
        HyGuiLog("HyGuiWave::ParseWaveFile parsing chunk reached EOF: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
        return false;
    }
    
    auto wf = reinterpret_cast<const WAVEFORMAT*>( ptr );
    
    

//    bool dpds, seek;
//    hr = WaveFindFormatAndData( wavData.get(), bytesRead, &result.wfx, &result.startAudio, &result.audioBytes, dpds, seek );
//    if ( FAILED(hr) )
//        return hr;

//    hr = WaveFindLoopInfo( wavData.get(), bytesRead, &result.loopStart, &result.loopLength );
//    if ( FAILED(hr) )
//        return hr;

//    if ( dpds )
//    {
//        hr = WaveFindTable( wavData.get(), bytesRead, FOURCC_XWMA_DPDS, &result.seek, &result.seekCount );
//        if ( FAILED(hr) )
//            return hr;
//    }
//    else if ( seek )
//    {
//        hr = WaveFindTable( wavData.get(), bytesRead, FOURCC_XMA_SEEK, &result.seek, &result.seekCount );
//        if ( FAILED(hr) )
//            return hr;
//    }
}
