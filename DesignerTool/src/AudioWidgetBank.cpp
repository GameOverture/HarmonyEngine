/**************************************************************************
 *	WidgetAudioBank.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioWidgetBank.h"
#include "ui_AudioWidgetBank.h"

#include "MainWindow.h"
#include "AudioWidgetManager.h"

#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

AudioWidgetBank::AudioWidgetBank(QWidget *parent) : QWidget(parent),
                                                    ui(new Ui::AudioWidgetBank)
{
    ui->setupUi(this);
    
    // Invalid constructor. This exists so Q_OBJECT can work.
    HyGuiLog("WidgetAudioBank::WidgetAudioBank() invalid constructor used", LOGTYPE_Error);
}

AudioWidgetBank::AudioWidgetBank(QDir metaDir, QDir dataDir, AudioWidgetManager *pManager, QWidget *pParent /*= 0*/) :  QWidget(pParent),
                                                                                                                        ui(new Ui::AudioWidgetBank),
                                                                                                                        m_pManager(pManager),
                                                                                                                        m_MetaDir(metaDir),
                                                                                                                        m_DataDir(dataDir)
{
    ui->setupUi(this);
    
    m_pModel = new AudioBankTableModel(this);
    ui->waveTable->setModel(m_pModel);
    ui->waveTable->setShowGrid(false);

    QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
    if(settingsFile.exists())
    {
        if(!settingsFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetAudioBank::WidgetAudioBank() could not open ") % HYGUIPATH_MetaSettings, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
        QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
        QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
        settingsFile.close();

        QJsonObject settingsObj = settingsDoc.object();
        
        SetName(settingsObj["name"].toString());

        QJsonArray wavesArray = settingsObj["waves"].toArray();
        for(int i = 0; i < wavesArray.size(); ++i)
        {
            QJsonObject waveObj = wavesArray[i].toObject();
            
            AudioWave *pNewWave = m_pManager->CreateWave(GetId(),
                                                         JSONOBJ_TOINT(waveObj, "checksum"),
                                                         waveObj["name"].toString(),
                                                         waveObj["formatType"].toInt(),
                                                         waveObj["numChannels"].toInt(),
                                                         waveObj["bitsPerSample"].toInt(),
                                                         waveObj["samplesPerSec"].toInt(),
                                                         waveObj["errors"].toInt(0));

            if(QFile::exists(m_MetaDir.absoluteFilePath(pNewWave->ConstructWaveFileName())) == false)
                pNewWave->SetError(GUIFRAMEERROR_CannotFindMetaImg);
            else
                pNewWave->ClearError(GUIFRAMEERROR_CannotFindMetaImg);

            m_pModel->AddWave(pNewWave);
        }
    }

}

AudioWidgetBank::~AudioWidgetBank()
{
    delete ui;
}

QString AudioWidgetBank::GetName()
{
    return m_pModel->GetName();
}

void AudioWidgetBank::SetName(QString sName)
{
    m_pModel->SetName(sName);
}

int AudioWidgetBank::GetId()
{
    return m_MetaDir.dirName().toInt();
}

void AudioWidgetBank::on_btnAddWaves_pressed()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    QString sSelectedFilter(tr("WAVE (*.wav)"));
    QStringList sImportWaveList = QFileDialog::getOpenFileNames(this,
                                                               "Import wave(s) into bank",
                                                               QString(),
                                                               tr("All files (*.*);;WAVE (*.wav)"),
                                                               &sSelectedFilter);

    if(sImportWaveList.empty() == false)
    {
        ImportWaves(sImportWaveList);
        Refresh();
    }
}

void AudioWidgetBank::on_btnAddDir_pressed()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    if(dlg.exec() == QDialog::Rejected)
        return;

    QStringList sDirs = dlg.selectedFiles();
    QStringList sImportWaveList;
    for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
    {
        QDir dirEntry(sDirs[iDirIndex]);
        HyGlobal::RecursiveFindOfFileExt("wav", sImportWaveList, dirEntry);
    }

    if(sImportWaveList.empty() == false)
    {
        ImportWaves(sImportWaveList);
        Refresh();
    }
}

void AudioWidgetBank::ImportWaves(QStringList sWaveFileList)
{
    for(int i = 0; i < sWaveFileList.size(); ++i)
    {
        QFileInfo waveFileInfo(sWaveFileList[i]);

        quint32 uiChecksum;
        QString sName;
        uint16 uiFormatType;
        uint16 uiNumChannels;
        uint16 uiBitsPerSample;
        uint32 uiSamplesPerSec;
        
        if(AudioWave::ParseWaveFile(waveFileInfo, uiChecksum, sName, uiFormatType, uiNumChannels, uiBitsPerSample, uiSamplesPerSec))
        {
            AudioWave *pNewWave = m_pManager->CreateWave(GetId(), uiChecksum, sName, uiFormatType, uiNumChannels, uiBitsPerSample, uiSamplesPerSec, 0);
            if(pNewWave)
            {
                if(QFile::copy(waveFileInfo.absoluteFilePath(), m_MetaDir.absoluteFilePath(pNewWave->ConstructWaveFileName())))
                {
                    m_pModel->AddWave(pNewWave);
                }
                else
                {
                    HyGuiLog("WidgetAudioBank::ImportWaves could not copy wave to meta dir meta: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
                }

//                QFile waveFile(waveFileInfo.absoluteFilePath());
//                if(!waveFile.open(QIODevice::ReadOnly))
//                {
//                    HyGuiLog("HyGuiWave::ParseWaveFile could not open the wave file: " % waveFileInfo.absoluteFilePath(), LOGTYPE_Error);
//                    return false;
//                }

//                QByteArray waveData = waveFile.readAll();
//                waveFile.close();
            }
        }
    }
}

void AudioWidgetBank::Refresh()
{
//    // Gather wave files
//    std::unique_ptr<uint8_t[]> entries;
//    std::unique_ptr<char[]> entryNames;
//    std::vector<WaveFile> waves;
//    MINIWAVEFORMAT compactFormat = { 0 };

//    size_t index = 0;
//    for (auto pConv = conversion.begin(); pConv != conversion.end(); ++pConv, ++index)
//    {
//        wchar_t ext[_MAX_EXT];
//        wchar_t fname[_MAX_FNAME];
//        _wsplitpath_s(pConv->szSrc, nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);

//        // Load source image
//        if (pConv != conversion.begin())
//            wprintf(L"\n");
//        else if (!*szOutputFile)
//        {
//            if (_wcsicmp(ext, L".xwb") == 0)
//            {
//                wprintf(L"ERROR: Need to specify output file via -o\n");
//                return 1;
//            }

//            _wmakepath_s(szOutputFile, nullptr, nullptr, fname, L".xwb");
//        }

//        wprintf(L"reading %ls", pConv->szSrc);
//        fflush(stdout);

//        WaveFile wave;
//        wave.conv = index;
//        std::unique_ptr<uint8_t[]> waveData;

//        HRESULT hr = DirectX::LoadWAVAudioFromFileEx(pConv->szSrc, waveData, wave.data);
//        if (FAILED(hr))
//        {
//            wprintf(L"\nERROR: Failed to load file (%08X)\n", hr);
//            return 1;
//        }

//        wave.waveData = std::move(waveData);

//        PrintInfo(wave);

//        waves.emplace_back(std::move(wave));
//    }

//    wprintf(L"\n");

//    DWORD dwAlignment = ALIGNMENT_MIN;
//    if (dwOptions & (1 << OPT_STREAMING))
//        dwAlignment = ALIGNMENT_DVD;

//    // Convert wave format to miniformat, failing if any won't map
//    // Check to see if we can use the compact wave bank format
//    bool compact = (dwOptions & (1 << OPT_NOCOMPACT)) ? false : true;
//    int reason = 0;
//    uint64_t waveOffset = 0;

//    for (auto it = waves.begin(); it != waves.end(); ++it)
//    {
//        if (!ConvertToMiniFormat(it->data.wfx, it->data.seek != 0, it->miniFmt))
//        {
//            auto cit = conversion.cbegin();
//            advance(cit, it->conv);
//            wprintf(L"ERROR: Failed encoding %ls\n", cit->szSrc);
//            return 1;
//        }

//        if (it == waves.begin())
//        {
//            memcpy(&compactFormat, &it->miniFmt, sizeof(MINIWAVEFORMAT));
//        }
//        else if (memcmp(&compactFormat, &it->miniFmt, sizeof(MINIWAVEFORMAT)) != 0)
//        {
//            compact = false;
//            reason |= 0x1;
//        }

//        if (it->data.loopLength > 0)
//        {
//            compact = false;
//            reason |= 0x2;
//        }

//        DWORD alignedSize = BLOCKALIGNPAD(it->data.audioBytes, dwAlignment);
//        waveOffset += alignedSize;
//    }

//    if (waveOffset > UINT32_MAX)
//    {
//        wprintf(L"ERROR: Audio wave data is too large to encode into wavebank (offset %I64u)", waveOffset);
//        return 1;
//    }
//    else if (waveOffset > (MAX_COMPACT_DATA_SEGMENT_SIZE * dwAlignment))
//    {
//        compact = false;
//        reason |= 0x4;
//    }

//    if ((dwOptions & (1 << OPT_COMPACT)) && !compact)
//    {
//        wprintf(L"ERROR: Cannot create compact wave bank:\n");
//        if (reason & 0x1)
//        {
//            wprintf(L"- Mismatched formats. All formats must be identical for a compact wavebank.\n");
//        }
//        if (reason & 0x2)
//        {
//            wprintf(L"- Found loop points. Compact wavebanks do not support loop points.\n");
//        }
//        if (reason & 0x4)
//        {
//            wprintf(L"- Audio wave data is too large to encode in compact wavebank (%I64u > %I64u).\n", waveOffset, uint64_t(MAX_COMPACT_DATA_SEGMENT_SIZE * dwAlignment));
//        }
//        return 1;
//    }

//    // Build entry metadata (and assign wave offset within data segment)
//    // Build entry friendly names if requested
//    entries.reset(new uint8_t[(compact ? sizeof(ENTRYCOMPACT) : sizeof(ENTRY)) * waves.size()]);

//    if (dwOptions & (1 << OPT_FRIENDLY_NAMES))
//    {
//        entryNames.reset(new char[waves.size() * ENTRYNAME_LENGTH]);
//        memset(entryNames.get(), 0, sizeof(char) * waves.size() * ENTRYNAME_LENGTH);
//    }

//    waveOffset = 0;
//    size_t count = 0;
//    size_t seekEntries = 0;
//    for (auto it = waves.begin(); it != waves.end(); ++it, ++count)
//    {
//        DWORD alignedSize = BLOCKALIGNPAD(it->data.audioBytes, dwAlignment);

//        auto wfx = it->data.wfx;

//        uint64_t duration = 0;

//        switch (it->miniFmt.wFormatTag)
//        {
//        case MINIWAVEFORMAT::TAG_ADPCM:
//        {
//            auto adpcmFmt = reinterpret_cast<const ADPCMEWAVEFORMAT*>(wfx);
//            duration = (it->data.audioBytes / wfx->nBlockAlign) * adpcmFmt->wSamplesPerBlock;
//            int partial = it->data.audioBytes % wfx->nBlockAlign;
//            if (partial)
//            {
//                if (partial >= (7 * wfx->nChannels))
//                    duration += (partial * 2 / wfx->nChannels - 12);
//            }
//        }
//        break;

//        case MINIWAVEFORMAT::TAG_WMA:
//            if (it->data.seekCount > 0)
//            {
//                seekEntries += it->data.seekCount + 1;
//                duration = it->data.seek[it->data.seekCount - 1] / uint32_t(2 * wfx->nChannels);
//            }
//            break;

//        default: // MINIWAVEFORMAT::TAG_PCM
//            duration = (uint64_t(it->data.audioBytes) * 8) / uint64_t(wfx->wBitsPerSample * wfx->nChannels);
//            break;
//        }

//        if (compact)
//        {
//            auto entry = reinterpret_cast<ENTRYCOMPACT*>(entries.get() + count * sizeof(ENTRYCOMPACT));
//            memset(entry, 0, sizeof(ENTRYCOMPACT));

//            assert(waveOffset <= (MAX_COMPACT_DATA_SEGMENT_SIZE * dwAlignment));
//            entry->dwOffset = uint32_t(waveOffset / dwAlignment);

//            assert(dwAlignment <= 2048);
//            entry->dwLengthDeviation = alignedSize - it->data.audioBytes;
//        }
//        else
//        {
//            auto entry = reinterpret_cast<ENTRY*>(entries.get() + count * sizeof(ENTRY));
//            memset(entry, 0, sizeof(ENTRY));

//            if (duration > 268435455)
//            {
//                wprintf(L"ERROR: Duration of audio too long to encode into wavebank (%I64u > 2^28))\n", duration);
//                return 1;
//            }

//            entry->Duration = uint32_t(duration);
//            memcpy(&entry->Format, &it->miniFmt, sizeof(MINIWAVEFORMAT));
//            entry->PlayRegion.dwOffset = uint32_t(waveOffset);
//            entry->PlayRegion.dwLength = it->data.audioBytes;

//            if (it->data.loopLength > 0)
//            {
//                entry->LoopRegion.dwStartSample = it->data.loopStart;
//                entry->LoopRegion.dwTotalSamples = it->data.loopLength;
//            }
//        }

//        if (dwOptions & (1 << OPT_FRIENDLY_NAMES))
//        {
//            auto cit = conversion.cbegin();
//            advance(cit, it->conv);

//            wchar_t wEntryName[_MAX_FNAME];
//            _wsplitpath_s(cit->szSrc, nullptr, 0, nullptr, 0, wEntryName, _MAX_FNAME, nullptr, 0);

//            int result = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, wEntryName, -1, &entryNames[count * ENTRYNAME_LENGTH], ENTRYNAME_LENGTH, nullptr, FALSE);
//            if (result <= 0)
//            {
//                memset(&entryNames[count * ENTRYNAME_LENGTH], 0, ENTRYNAME_LENGTH);
//            }
//        }

//        waveOffset += alignedSize;
//    }

//    assert(count > 0 && count == waves.size());

//    // Create wave bank
//    assert(*szOutputFile != 0);

//    wprintf(L"writing %ls%ls wavebank %ls w/ %Iu entries\n", (compact) ? L"compact " : L"", (dwOptions & (1 << OPT_STREAMING)) ? L"streaming" : L"in-memory", szOutputFile, waves.size());
//    fflush(stdout);

//    if (dwOptions & (1 << OPT_NOOVERWRITE))
//    {
//        if (FileExists(szOutputFile))
//        {
//            wprintf(L"ERROR: Output file %ls already exists!\n", szOutputFile);
//            return 1;
//        }

//        if (*szHeaderFile)
//        {
//            if (FileExists(szHeaderFile))
//            {
//                wprintf(L"ERROR: Output header file %ls already exists!\n", szHeaderFile);
//                return 1;
//            }
//        }
//    }

//    hFile.reset(safe_handle(CreateFileW(szOutputFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)));
//    if (!hFile)
//    {
//        wprintf(L"ERROR: Failed opening output file %ls, %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    // Setup wave bank header
//    HEADER header;
//    memset(&header, 0, sizeof(header));
//    header.dwSignature = HEADER::SIGNATURE;
//    header.dwHeaderVersion = HEADER::VERSION;
//    header.dwVersion = XACT_CONTENT_VERSION;

//    DWORD segmentOffset = sizeof(HEADER);

//    // Write bank metadata
//    assert((segmentOffset % 4) == 0);

//    BANKDATA data;
//    memset(&data, 0, sizeof(data));

//    data.dwEntryCount = uint32_t(waves.size());
//    data.dwAlignment = dwAlignment;

//    GetSystemTimeAsFileTime(&data.BuildTime);

//    data.dwFlags = (dwOptions & (1 << OPT_STREAMING)) ? BANKDATA::TYPE_STREAMING : BANKDATA::TYPE_BUFFER;

//    data.dwFlags |= BANKDATA::FLAGS_SEEKTABLES;

//    if (dwOptions & (1 << OPT_FRIENDLY_NAMES))
//    {
//        data.dwFlags |= BANKDATA::FLAGS_ENTRYNAMES;
//        data.dwEntryNameElementSize = ENTRYNAME_LENGTH;
//    }

//    if (compact)
//    {
//        data.dwFlags |= BANKDATA::FLAGS_COMPACT;
//        data.dwEntryMetaDataElementSize = sizeof(ENTRYCOMPACT);
//        memcpy(&data.CompactFormat, &compactFormat, sizeof(MINIWAVEFORMAT));
//    }
//    else
//    {
//        data.dwEntryMetaDataElementSize = sizeof(ENTRY);
//    }

//    {
//        wchar_t wBankName[_MAX_FNAME];
//        _wsplitpath_s(szOutputFile, nullptr, 0, nullptr, 0, wBankName, _MAX_FNAME, nullptr, 0);

//        int result = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, wBankName, -1, data.szBankName, BANKDATA::BANKNAME_LENGTH, nullptr, FALSE);
//        if (result <= 0)
//        {
//            memset(data.szBankName, 0, BANKDATA::BANKNAME_LENGTH);
//        }
//    }

//    if (SetFilePointer(hFile.get(), segmentOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//    {
//        wprintf(L"ERROR: Failed writing bank data to %ls, SFP %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    if (!WriteFile(hFile.get(), &data, sizeof(data), nullptr, nullptr))
//    {
//        wprintf(L"ERROR: Failed writing bank data to %ls, %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    header.Segments[HEADER::SEGIDX_BANKDATA].dwOffset = segmentOffset;
//    header.Segments[HEADER::SEGIDX_BANKDATA].dwLength = sizeof(BANKDATA);
//    segmentOffset += sizeof(BANKDATA);

//    // Write entry metadata
//    assert((segmentOffset % 4) == 0);

//    if (SetFilePointer(hFile.get(), segmentOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//    {
//        wprintf(L"ERROR: Failed writing entry metadata to %ls, SFP %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    uint32_t entryBytes = uint32_t(waves.size() * data.dwEntryMetaDataElementSize);
//    if (!WriteFile(hFile.get(), entries.get(), entryBytes, nullptr, nullptr))
//    {
//        wprintf(L"ERROR: Failed writing entry metadata to %ls, %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwOffset = segmentOffset;
//    header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwLength = entryBytes;
//    segmentOffset += entryBytes;

//    // Write seek tables
//    assert((segmentOffset % 4) == 0);

//    header.Segments[HEADER::SEGIDX_SEEKTABLES].dwOffset = segmentOffset;

//    if (seekEntries > 0)
//    {
//        seekEntries += waves.size(); // Room for an offset per entry

//        std::unique_ptr<uint32_t[]> seekTables(new uint32_t[seekEntries]);

//        if (SetFilePointer(hFile.get(), segmentOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//        {
//            wprintf(L"ERROR: Failed writing seek tables to %ls, SFP %u\n", szOutputFile, GetLastError());
//            return 1;
//        }

//        uint32_t seekoffset = 0;
//        uint32_t index = 0;
//        for (auto it = waves.begin(); it != waves.end(); ++it, ++index)
//        {
//            if (it->miniFmt.wFormatTag == MINIWAVEFORMAT::TAG_WMA)
//            {
//                seekTables[index] = seekoffset * sizeof(uint32_t);

//                uint32_t baseoffset = uint32_t(waves.size() + seekoffset);
//                seekTables[baseoffset] = it->data.seekCount;

//                for (uint32_t j = 0; j < it->data.seekCount; ++j)
//                {
//                    seekTables[baseoffset + j + 1] = it->data.seek[j];
//                }

//                seekoffset += it->data.seekCount + 1;
//            }
//            else
//            {
//                seekTables[index] = uint32_t(-1);
//            }
//        }

//        uint32_t seekLen = uint32_t(sizeof(uint32_t) * seekEntries);

//        if (!WriteFile(hFile.get(), seekTables.get(), seekLen, nullptr, nullptr))
//        {
//            wprintf(L"ERROR: Failed writing seek tables to %ls, %u\n", szOutputFile, GetLastError());
//            return 1;
//        }

//        segmentOffset += seekLen;

//        header.Segments[HEADER::SEGIDX_SEEKTABLES].dwLength = seekLen;
//    }
//    else
//    {
//        header.Segments[HEADER::SEGIDX_SEEKTABLES].dwLength = 0;
//    }

//    // Write entry names
//    if (dwOptions & (1 << OPT_FRIENDLY_NAMES))
//    {
//        assert((segmentOffset % 4) == 0);

//        if (SetFilePointer(hFile.get(), segmentOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//        {
//            wprintf(L"ERROR: Failed writing friendly entry names to %ls, SFP %u\n", szOutputFile, GetLastError());
//            return 1;
//        }

//        uint32_t entryNamesBytes = uint32_t(count * data.dwEntryNameElementSize);
//        if (!WriteFile(hFile.get(), entryNames.get(), entryNamesBytes, nullptr, nullptr))
//        {
//            wprintf(L"ERROR: Failed writing friendly entry names to %ls, %u\n", szOutputFile, GetLastError());
//            return 1;
//        }

//        header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwOffset = segmentOffset;
//        header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwLength = entryNamesBytes;
//        segmentOffset += entryNamesBytes;
//    }

//    // Write wave data
//    segmentOffset = BLOCKALIGNPAD(segmentOffset, dwAlignment);

//    header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwOffset = segmentOffset;
//    header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength = uint32_t(waveOffset);

//    for (auto it = waves.begin(); it != waves.end(); ++it)
//    {
//        if (SetFilePointer(hFile.get(), segmentOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//        {
//            wprintf(L"ERROR: Failed writing audio data to %ls, SFP %u\n", szOutputFile, GetLastError());
//            return 1;
//        }

//        if (!WriteFile(hFile.get(), it->data.startAudio, it->data.audioBytes, nullptr, nullptr))
//        {
//            wprintf(L"ERROR: Failed writing audio data to %ls, %u\n", szOutputFile, GetLastError());
//            return 1;
//        }

//        DWORD alignedSize = BLOCKALIGNPAD(it->data.audioBytes, dwAlignment);

//        if ((uint64_t(segmentOffset) + alignedSize) > UINT32_MAX)
//        {
//            wprintf(L"ERROR: Data exceeds maximum size for wavebank\n");
//            return 1;
//        }

//        segmentOffset += alignedSize;
//    }

//    assert(segmentOffset == (header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwOffset + waveOffset));

//    // Commit wave bank
//    if (SetFilePointer(hFile.get(), segmentOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//    {
//        wprintf(L"ERROR: Failed committing output file %ls, EOF %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    if (!SetEndOfFile(hFile.get()))
//    {
//        wprintf(L"ERROR: Failed committing output file %ls, EOF %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    if (SetFilePointer(hFile.get(), 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
//    {
//        wprintf(L"ERROR: Failed committing output file %ls, HDR %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    if (!WriteFile(hFile.get(), &header, sizeof(header), nullptr, nullptr))
//    {
//        wprintf(L"ERROR: Failed committing output file %ls, HDR %u\n", szOutputFile, GetLastError());
//        return 1;
//    }

//    // Write C header if requested
//    if (*szHeaderFile)
//    {
//        wprintf(L"writing C header %ls\n", szHeaderFile);
//        fflush(stdout);

//        FILE* file = nullptr;
//        if (!_wfopen_s(&file, szHeaderFile, L"wt"))
//        {
//            wchar_t wBankName[_MAX_FNAME];
//            _wsplitpath_s(szOutputFile, nullptr, 0, nullptr, 0, wBankName, _MAX_FNAME, nullptr, 0);

//            FileNameToIdentifier(wBankName, _MAX_FNAME);

//            fprintf_s(file, "#pragma once\n\nenum XACT_WAVEBANK_%ls\n{\n", wBankName);

//            size_t index = 0;
//            for (auto it = waves.begin(); it != waves.end(); ++it, ++index)
//            {
//                auto cit = conversion.cbegin();
//                advance(cit, it->conv);

//                wchar_t wEntryName[_MAX_FNAME];
//                _wsplitpath_s(cit->szSrc, nullptr, 0, nullptr, 0, wEntryName, _MAX_FNAME, nullptr, 0);

//                FileNameToIdentifier(wEntryName, _MAX_FNAME);

//                fprintf_s(file, "    XACT_WAVEBANK_%ls_%ls = %Iu,\n", wBankName, wEntryName, index);
//            }

//            fprintf_s(file, "};\n\n#define XACT_WAVEBANK_%ls_ENTRY_COUNT %Iu\n", wBankName, count);

//            fclose(file);
//        }
//        else
//        {
//            wprintf(L"ERROR: Failed writing wave bank C header %ls\n", szHeaderFile);
//            return 1;
//        }
//    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WRITE SETTINGS FILE TO AUDIO META DIR
    QJsonObject settingsObj;
    m_pModel->GetJsonObj(settingsObj);

    QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
    if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
       HyGuiLog("Couldn't open audio settings file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonDocument settingsDoc(settingsObj);

#ifdef HYGUI_UseBinaryMetaFiles
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toBinaryData());
#else
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
#endif
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to audio settings file: " % settingsFile.errorString(), LOGTYPE_Error);
        }

        settingsFile.close();
    }
}
