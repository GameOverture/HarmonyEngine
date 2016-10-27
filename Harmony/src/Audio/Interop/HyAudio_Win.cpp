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
	#pragma comment(lib,"xaudio2.lib")
 #else
	#include <xaudio2.h>
 #endif

//typedef HRESULT (WINAPI *fpHyXAudio2Create)(_Outptr_ IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

//const DWORD uiSamplesPerSec = 48000;
//const DWORD uiBytesPerSample = sizeof(int16) * 2;
//const DWORD uiSecondaryBufferSize = uiSamplesPerSec * uiBytesPerSample;

HyAudio_Win::HyAudio_Win(HyRendererInterop &rendererRef) : IHyAudio(rendererRef)
{
	//// Load the library
	//HMODULE hXAudioLib = NULL;
	//int32 iVersion = 10;
	//while(hXAudioLib == NULL && iVersion >= 0)
	//{
	//	iVersion--;

	//	char szVersionDll[16];
	//	snprintf(szVersionDll, sizeof(szVersionDll), "XAudio2_%i.dll", iVersion);

	//	hXAudioLib = LoadLibraryA(szVersionDll);
	//}

	//if(hXAudioLib == false)
	//{
	//	HyLogError("Missing XAudio2 dll");
	//	return;
	//}

	//fpHyXAudio2Create fpXAudio2Create = (fpHyXAudio2Create)GetProcAddress(hXAudioLib, "XAudio2Create");

	CoInitializeEx(nullptr, 0);

	UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
 
	// Initialize the XAudio2 object
	IXAudio2 *pXAudio2 = NULL;
	HRESULT hr = XAudio2Create(&pXAudio2, flags);
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
	XAUDIO2_DEBUG_CONFIGURATION debug = {0};
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	pXAudio2->SetDebugConfiguration(&debug, 0);
 #endif


	//LPDIRECTSOUND pDirectSound;
	//if(fpDirectSoundCreate && SUCCEEDED(fpDirectSoundCreate(0, &pDirectSound, 0)))
	//{
	//	WAVEFORMATEX waveFormat = {};
	//	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	//	waveFormat.nChannels = 2;
	//	waveFormat.nSamplesPerSec = uiSamplesPerSec;
	//	waveFormat.wBitsPerSample = 16;	// CD quality
	//	waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
	//	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	//	waveFormat.cbSize = 0;

	//	if(SUCCEEDED(pDirectSound->SetCooperativeLevel(rendererRef.GetHWND(0), DSSCL_PRIORITY)))
	//	{
	//		DSBUFFERDESC bufferDesc;
	//		memset(&bufferDesc, 0, sizeof(DSBUFFERDESC));
	//		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	//		bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	//		// Create the "info" buffer
	//		LPDIRECTSOUNDBUFFER pInfoBuffer;
	//		if(SUCCEEDED(pDirectSound->CreateSoundBuffer(&bufferDesc, &pInfoBuffer, 0)))
	//		{
	//			if(SUCCEEDED(pInfoBuffer->SetFormat(&waveFormat)))
	//			{
	//			}
	//			else
	//				HyLogError("IDirectSoundBuffer::SetFormat failed");
	//		}
	//		else
	//			HyLogError("IDirectSound::CreateSoundBuffer (primary) failed");
	//	}
	//	else
	//		HyLogError("IDirectSound::SetCooperativeLevel failed");

	//	// Create the "write" buffer
	//	DSBUFFERDESC secondaryBufferDesc;
	//	memset(&secondaryBufferDesc, 0, sizeof(DSBUFFERDESC));
	//	secondaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	//	secondaryBufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
	//	secondaryBufferDesc.dwBufferBytes = uiSecondaryBufferSize;
	//	secondaryBufferDesc.lpwfxFormat = &waveFormat;

	//	LPDIRECTSOUNDBUFFER pWriteBuffer;
	//	if(SUCCEEDED(pDirectSound->CreateSoundBuffer(&secondaryBufferDesc, &pWriteBuffer, 0)))
	//	{
	//	}
	//	else
	//		HyLogError("IDirectSound::CreateSoundBuffer (secondary) failed");

	//	// Start it playing
	//}
	//else
	//	HyLogError("Could not GetProcAddress of DirectSoundCreate or the invokation failed");
}

HyAudio_Win::~HyAudio_Win()
{
}
