/**************************************************************************
*	HyAudio_Win.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Audio/Interop/HyAudio_Win.h"

#include "Diagnostics/HyGuiComms.h"

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
	#include <xaudio2.h>
	#include <xaudio2fx.h>
	#include <x3daudio.h>
	#pragma comment(lib,"xaudio2.lib")
#else
	#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
	#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
	#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
	#pragma warning(push)
	#pragma warning( disable : 4005 )
	#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>
	#pragma warning(pop)
	#pragma comment(lib,"x3daudio.lib")
#endif

#ifdef HY_ENDIAN_BIG
	#define fourccRIFF 'RIFF'
	#define fourccDATA 'data'
	#define fourccFMT 'fmt '
	#define fourccWAVE 'WAVE'
	#define fourccXWMA 'XWMA'
	#define fourccDPDS 'dpds'
#endif
#ifdef HY_ENDIAN_LITTLE
	#define fourccRIFF 'FFIR'
	#define fourccDATA 'atad'
	#define fourccFMT ' tmf'
	#define fourccWAVE 'EVAW'
	#define fourccXWMA 'AMWX'
	#define fourccDPDS 'sdpd'
#endif

//typedef HRESULT (WINAPI *fpHyXAudio2Create)(_Outptr_ IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

//const DWORD uiSamplesPerSec = 48000;
//const DWORD uiBytesPerSample = sizeof(int16) * 2;
//const DWORD uiSecondaryBufferSize = uiSamplesPerSec * uiBytesPerSample;


HyAudio_Win::HyAudio_Win() :	IHyAudio(),
								m_pXAudio2(NULL),
								m_pMasterVoice(NULL)
{
	//CoInitializeEx(nullptr, 0);

	UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
 
	// Initialize the XAudio2 object
	HRESULT hr = XAudio2Create(&m_pXAudio2, flags);
	if(FAILED(hr))
	{
		HyLogError("XAudio2Create failed");
		return;
	}
	
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	// To see the debug output, you need to view ETW logs for this application:
	// Go to Control Panel, Administrative Tools, Event Viewer.
	// View->Show Analytic and Debug Logs.
	// Applications and Services Logs / Microsoft / Windows / XAudio2.
	// Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK
	XAUDIO2_DEBUG_CONFIGURATION debugCfg = {0};
	debugCfg.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	m_pXAudio2->SetDebugConfiguration(&debugCfg, 0);
 #endif

	if(FAILED(hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice)))
	{
		HyLogError("XAudio2 - CreateMasteringVoice failed");
		return;
	}
}

HyAudio_Win::~HyAudio_Win()
{

}

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
			hr = HRESULT_FROM_WIN32( GetLastError() );

		if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
			hr = HRESULT_FROM_WIN32( GetLastError() );

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
				hr = HRESULT_FROM_WIN32( GetLastError() );
			break;

		default:
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
			return HRESULT_FROM_WIN32( GetLastError() );            
		}

		dwOffset += sizeof(DWORD) * 2;
		
		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		
		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );
	DWORD dwRead;
	if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
		hr = HRESULT_FROM_WIN32( GetLastError() );
	return hr;
}

//DWORD HyAudio_Win::PlaySoundTest()
//{
//	TCHAR * strFileName = _TEXT("media\\MusicMono.wav");
//
//	// Open the file
//	HANDLE hFile = CreateFile(
//		strFileName,
//		GENERIC_READ,
//		FILE_SHARE_READ,
//		NULL,
//		OPEN_EXISTING,
//		0,
//		NULL );
//
//	if( INVALID_HANDLE_VALUE == hFile )
//		return HRESULT_FROM_WIN32( GetLastError() );
//
//	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
//		return HRESULT_FROM_WIN32( GetLastError() );
//
//	return S_OK
//
//	//DWORD dwChunkSize;
//	//DWORD dwChunkPosition;
//	////check the file type, should be fourccWAVE or 'XWMA'
//	//FindChunk(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
//	//DWORD filetype;
//	//ReadChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);
//	//if (filetype != fourccWAVE)
//	//	return S_FALSE;
//
//	//FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition);
//	//ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
//
//	////fill out the audio data buffer with the contents of the fourccDATA chunk
//	//FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
//	//BYTE * pDataBuffer = new BYTE[dwChunkSize];
//	//ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
//
//	//buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
//	//buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
//	//buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
//}
