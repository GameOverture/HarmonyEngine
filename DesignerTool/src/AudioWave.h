/**************************************************************************
 *	HyGuiWave.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWAVE_H
#define AUDIOWAVE_H

#include "IData.h"

#include <QWidget>
#include <QJsonObject>

class AudioWave
{
    friend class AudioWidgetManager;
    
    const uint                          m_uiWAVE_BANK_ID;
    
    quint32                             m_uiChecksum;
    QString                             m_sName;
    
    uint16                              m_uiFormatType;
    uint16                              m_uiNumChannels;
    uint16                              m_uiBitsPerSample; // 16-bit, etc.
    uint32                              m_uiSamplesPerSec;
    
    uint32                              m_uiDataSize;
    
    uint                                m_uiErrors; // '0' when there is no error
    
    // Private ctor as WidgetAudioManager should only construct these
    AudioWave(uint uiWaveBankId, quint32 uiChecksum, QString sName, uint16 uiFormatType, uint16 uiNumChannels, uint16 uiBitsPerSample, uint32 uiSamplesPerSec, uint uiErrors);
    ~AudioWave();
    
public:
    quint32 GetChecksum();
    QString GetName();
    
    QIcon GetIcon();
    
    QString ConstructWaveFileName();
    
    uint32 GetDataSize();
    
    QString GetDescription();
    QString GetSizeDescription();
    
    static bool ParseWaveFile(QFileInfo waveFileInfo, quint32 &uiChecksumOut, QString &sNameOut, uint16 &uiFormatTagOut, uint16 &uiNumChannelsOut, uint16 &uiBitsPerSampleOut, uint32 &uiSamplesPerSecOut);

    void GetJsonObj(QJsonObject &waveObj);

    void SetError(eGuiFrameError eError);
    void ClearError(eGuiFrameError eError);
    uint GetErrors();
};

#endif // AUDIOWAVE_H
