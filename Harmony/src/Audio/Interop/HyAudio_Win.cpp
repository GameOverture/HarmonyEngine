/**************************************************************************
*	HyAudio_Win.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
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
	//#pragma warning(push)
	//#pragma warning(disable : 4005)
	#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>
	//#pragma warning(pop)
#endif

// TODO: Are multi-character literals compiler specific to work?
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

X3DAUDIO_HANDLE g_x3DInstance;

HyAudio_Win::HyAudio_Win(std::vector<HyWindow *> &windowListRef) :	IHyAudio(windowListRef),
																	m_pXAudio2(NULL),
																	m_pMasterVoice(NULL)
{
	HRESULT hr = 0;
#ifndef HY_PLATFORM_GUI
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if(FAILED(hr))
	{
		HyLogError("HyAudio_Win - CoInitializeEx failed");
		return;
	}
#endif

	UINT32 flags = 0;
#if ( _WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
	
	#ifdef _DEBUG
		m_hXAudioDLL = LoadLibraryExW(L"XAudioD2_7.DLL", nullptr, 0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */);	// Workaround for XAudio 2.7 known issue
		flags |= XAUDIO2_DEBUG_ENGINE;
	#else
		m_hXAudioDLL = LoadLibraryExW(L"XAudio2_7.DLL", nullptr, 0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */);	// Workaround for XAudio 2.7 known issue
	#endif
	if (!m_hXAudioDLL)
	{
		HyLogError("Could not load XAudio dll");
		return;
	}
#endif
 
	// Initialize the XAudio2 object
	hr = XAudio2Create(&m_pXAudio2, flags);
	if(FAILED(hr))
	{
		HyLogError("HyAudio_Win - XAudio2Create failed");
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
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;

		return;
	}

	// Get information about the audio output device
	//
#if(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
	XAUDIO2_VOICE_DETAILS voiceDetails;
	m_pMasterVoice->GetVoiceDetails(&voiceDetails);

	hr = m_pMasterVoice->GetChannelMask(&m_uiChannelMask);
	if(FAILED(hr))
	{
		HyLogError("XAudio2 - GetChannelMask failed");
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
		return;
	}

	m_uiSamplesPerSecond = voiceDetails.InputSampleRate;
	m_uiNumChannels = voiceDetails.InputChannels;

#else
	XAUDIO2_DEVICE_DETAILS deviceDetails;
	hr = m_pXAudio2->GetDeviceDetails(0, &deviceDetails);
	if(FAILED(hr))
	{
		HyLogError("XAudio2 - GetDeviceDetails failed");
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
		return;
	}

	m_uiSamplesPerSecond = deviceDetails.OutputFormat.Format.nSamplesPerSec;
	m_uiChannelMask = deviceDetails.OutputFormat.dwChannelMask;
	m_uiNumChannels = deviceDetails.OutputFormat.Format.nChannels;

#endif

	for(int i = 0; i < NUMEFFECTS; ++i)
	{
		switch(i)
		{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case EFFECT_MasterVolume:
			m_Effects[EFFECT_MasterVolume].uiNumOutputChannels = 1;
			m_Effects[EFFECT_MasterVolume].pEffect = NULL;
			hr = m_pXAudio2->CreateSubmixVoice(&m_Effects[EFFECT_MasterVolume].pSubmixVoice, 1, m_uiSamplesPerSecond, 0, 0, 0, 0);
			if(FAILED(hr))
			{
				HyLogError("XAudio2 - CreateSubmixVoice (Master Volume) failed");
				m_pXAudio2->Release();
				m_pXAudio2 = NULL;
			}
			break;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case EFFECT_Reverb:
			m_Effects[EFFECT_Reverb].uiNumOutputChannels = 1;
			UINT32 uiReverbFlags = 0;
			#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
				uiReverbFlags |= XAUDIO2FX_DEBUG;
			#endif
			if(FAILED(hr = XAudio2CreateReverb(&m_Effects[EFFECT_Reverb].pEffect, uiReverbFlags)))
			{
				HyLogError("XAudio2 - XAudio2CreateReverb failed");
				m_pXAudio2->Release();
				m_pXAudio2 = NULL;
			}
			else
			{
				XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { m_Effects[EFFECT_Reverb].pEffect, TRUE, m_Effects[EFFECT_Reverb].uiNumOutputChannels } };
				XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };
				hr = m_pXAudio2->CreateSubmixVoice(&m_Effects[EFFECT_Reverb].pSubmixVoice, 1, m_uiSamplesPerSecond, 0, 0, nullptr, &effectChain);
				if(FAILED(hr))
				{
					HyLogError("XAudio2 - CreateSubmixVoice (Reverb) failed");
					m_pXAudio2->Release();
					m_pXAudio2 = NULL;
				}
			}

			// Set default FX params
			XAUDIO2FX_REVERB_I3DL2_PARAMETERS defaultReverbPreset = XAUDIO2FX_I3DL2_PRESET_DEFAULT;
			XAUDIO2FX_REVERB_PARAMETERS native;
			ReverbConvertI3DL2ToNative(&defaultReverbPreset, &native);
			m_Effects[EFFECT_Reverb].pSubmixVoice->SetEffectParameters(0, &native, sizeof(native), XAUDIO2_COMMIT_NOW);
			
			break;
		}
	} // effects

	X3DAudioInitialize(m_uiChannelMask, X3DAUDIO_SPEED_OF_SOUND, g_x3DInstance);

	X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI*5.0f/6.0f, X3DAUDIO_PI*11.0f/6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };
}

HyAudio_Win::~HyAudio_Win()
{
#ifndef HY_PLATFORM_GUI
	#if ( _WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
		if(m_hXAudioDLL)
			FreeLibrary(m_hXAudioDLL);
	#endif

		// TODO: Call this without it crashing the program (make CoInit and CoUninit into an outer scope?)
		CoUninitialize();
#endif
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

