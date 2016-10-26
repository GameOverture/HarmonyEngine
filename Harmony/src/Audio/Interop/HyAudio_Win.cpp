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

#include <Mmreg.h>
#include <dsound.h>

typedef HRESULT (WINAPI *fpHyDirectSoundCreate)(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);

const DWORD uiSamplesPerSec = 48000;
const DWORD uiBytesPerSample = sizeof(int16) * 2;
const DWORD uiSecondaryBufferSize = uiSamplesPerSec * uiBytesPerSample;

HyAudio_Win::HyAudio_Win(HyRendererInterop &rendererRef) : IHyAudio(rendererRef)
{
	// Load the library
	HMODULE hDirectSoundLib = LoadLibraryA("dsound.dll");
	if(hDirectSoundLib)
	{
		// Get a DirectSound object - cooperative
		fpHyDirectSoundCreate fpDirectSoundCreate = (fpHyDirectSoundCreate) GetProcAddress(hDirectSoundLib, "DirectSoundCreate");

		LPDIRECTSOUND pDirectSound;
		if(fpDirectSoundCreate && SUCCEEDED(fpDirectSoundCreate(0, &pDirectSound, 0)))
		{
			WAVEFORMATEX waveFormat = {};
			waveFormat.wFormatTag = WAVE_FORMAT_PCM;
			waveFormat.nChannels = 2;
			waveFormat.nSamplesPerSec = uiSamplesPerSec;
			waveFormat.wBitsPerSample = 16;	// CD quality
			waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
			waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
			waveFormat.cbSize = 0;

			if(SUCCEEDED(pDirectSound->SetCooperativeLevel(rendererRef.GetHWND(0), DSSCL_PRIORITY)))
			{
				DSBUFFERDESC bufferDesc;
				memset(&bufferDesc, 0, sizeof(DSBUFFERDESC));
				bufferDesc.dwSize = sizeof(DSBUFFERDESC);
				bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

				// Create the "info" buffer
				LPDIRECTSOUNDBUFFER pInfoBuffer;
				if(SUCCEEDED(pDirectSound->CreateSoundBuffer(&bufferDesc, &pInfoBuffer, 0)))
				{
					if(SUCCEEDED(pInfoBuffer->SetFormat(&waveFormat)))
					{
					}
					else
						HyLogError("IDirectSoundBuffer::SetFormat failed");
				}
				else
					HyLogError("IDirectSound::CreateSoundBuffer (primary) failed");
			}
			else
				HyLogError("IDirectSound::SetCooperativeLevel failed");

			// Create the "write" buffer
			DSBUFFERDESC secondaryBufferDesc;
			memset(&secondaryBufferDesc, 0, sizeof(DSBUFFERDESC));
			secondaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);
			secondaryBufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
			secondaryBufferDesc.dwBufferBytes = uiSecondaryBufferSize;
			secondaryBufferDesc.lpwfxFormat = &waveFormat;

			LPDIRECTSOUNDBUFFER pWriteBuffer;
			if(SUCCEEDED(pDirectSound->CreateSoundBuffer(&secondaryBufferDesc, &pWriteBuffer, 0)))
			{
			}
			else
				HyLogError("IDirectSound::CreateSoundBuffer (secondary) failed");

			// Start it playing
		}
		else
			HyLogError("Could not GetProcAddress of DirectSoundCreate or the invokation failed");
	}
	else
		HyLogError("Could not load direct sound library");
}

HyAudio_Win::~HyAudio_Win()
{
}
