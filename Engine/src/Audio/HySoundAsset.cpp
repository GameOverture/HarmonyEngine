/**************************************************************************
*	HySoundAsset.cpp
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Audio/HySoundAsset.h"
#include "Audio/HyAudioCore.h"

// TODO: Specify in Editor whether a sound supports pitching, and use 'MA_SOUND_FLAG_NO_PITCH' if not
// TODO: 'MA_SOUND_FLAG_NO_SPATIALIZATION' is set post-initialization with 'ma_sound_set_spatialization_enabled'

HySoundAsset::HySoundAsset(HyAudioCore &coreRef, std::string sFilePath, int32 iCategoryId, bool bIsStreaming, int32 iInstanceLimit) :
	m_CoreRef(coreRef),
	m_sFILE_PATH(sFilePath),
	m_iCATEGORY_ID(iCategoryId),
	m_bIS_STREAMING(bIsStreaming),
	m_iINSTANCE_LIMIT(iInstanceLimit)
{
	HyAssert(m_iINSTANCE_LIMIT >= 0, "Invalid instance limit in HySoundBuffers");
	if(m_iINSTANCE_LIMIT == 0)
		m_SoundBufferList.push_back(HY_NEW ma_sound());
	else
	{
		m_SoundBufferList.resize(m_iINSTANCE_LIMIT);
		for(int i = 0; i < m_SoundBufferList.size(); ++i)
			m_SoundBufferList[i] = HY_NEW ma_sound();
	}
}

HySoundAsset::~HySoundAsset()
{
	//Unload();
	for(int i = 0; i < m_SoundBufferList.size(); ++i)
		delete m_SoundBufferList[i];
}

std::string HySoundAsset::GetFilePath() const
{
	return m_sFILE_PATH;
}

int32 HySoundAsset::GetCategoryId() const
{
	return m_iCATEGORY_ID;
}

bool HySoundAsset::IsStreaming() const
{
	return m_bIS_STREAMING;
}

int32 HySoundAsset::GetInstanceLimit() const
{
	return m_iINSTANCE_LIMIT;
}

bool HySoundAsset::Load()
{
#ifdef HY_PLATFORM_BROWSER
	if(m_CoreRef.GetEngine() == nullptr)
	{
		m_CoreRef.DeferLoading(this);
		return true;
	}
#endif

	// Constructor guarantees at least one buffer exists, load it with 'ma_sound_init_from_file'
	HyAssert(m_SoundBufferList.size() >= 1, "HySoundAsset::Load() - Constructor needs to allocate at least 1 buffer");

	ma_result eResult = ma_sound_init_from_file(m_CoreRef.GetEngine(),
		m_sFILE_PATH.c_str(),
		m_bIS_STREAMING ? MA_SOUND_FLAG_STREAM : 0,
		m_CoreRef.GetCategory(GetCategoryId()),
		nullptr,
		m_SoundBufferList[0]);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("Load() - ma_sound_init_from_file failed: " << eResult);
		return false;  // Failed to load sound.
	}

	// Initialize the rest of the buffers with InitSoundBuffer(), which optimially copies from the first buffer if able to
	for(uint32 i = 1; i < static_cast<uint32>(m_SoundBufferList.size()); ++i)
		InitSoundBuffer(m_SoundBufferList[i], 0);
	
	return true;
}

void HySoundAsset::Unload()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBufferList.size()); ++i)
		ma_sound_uninit(m_SoundBufferList[i]);
}

ma_sound *HySoundAsset::GetFreshBuffer()
{
	while(true)
	{
		for(int32 i = 0; i < static_cast<int32>(m_SoundBufferList.size()); ++i)
		{
			if(ma_sound_is_playing(m_SoundBufferList[i]) == false)
				return m_SoundBufferList[i];
		}

		if(m_iINSTANCE_LIMIT == 0) // Allows dynamic resizing
		{
			m_SoundBufferList.push_back(HY_NEW ma_sound());
			InitSoundBuffer(m_SoundBufferList.back(), MA_SOUND_FLAG_ASYNC);
		}
		else
			return nullptr; // No available buffer
	}
}

void HySoundAsset::InitSoundBuffer(ma_sound *pSound, ma_uint32 uiAdditionalFlags)
{
	if(m_bIS_STREAMING)
	{
		// Cannot use 'ma_sound_init_copy' because it doesn't support 'MA_SOUND_FLAG_STREAM'
		ma_result eResult = ma_sound_init_from_file(m_CoreRef.GetEngine(),
			m_sFILE_PATH.c_str(),
			MA_SOUND_FLAG_STREAM | uiAdditionalFlags,
			m_CoreRef.GetCategory(GetCategoryId()),
			nullptr,
			pSound);

		if(eResult != MA_SUCCESS)
			HyLogError("HySoundAsset::GetFreshBuffer() - ma_sound_init_from_file failed: " << eResult);
	}
	else
	{
		ma_result eResult = ma_sound_init_copy(m_CoreRef.GetEngine(),
			m_SoundBufferList[0],
			uiAdditionalFlags,
			m_CoreRef.GetCategory(GetCategoryId()),
			pSound);

		if(eResult != MA_SUCCESS)
			HyLogError("HySoundAsset::GetFreshBuffer() - ma_sound_init_copy failed: " << eResult);
	}
}
