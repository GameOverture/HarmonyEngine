/**************************************************************************
*	HySoundBuffers.cpp
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Audio/HySoundBuffers.h"
#include "Audio/HyAudioCore.h"

HySoundBuffers::HySoundBuffers(HyAudioCore &coreRef, std::string sFilePath, int32 iGroupId, bool bIsStreaming, int32 iInstanceLimit) :
	m_CoreRef(coreRef),
	m_sFILE_PATH(sFilePath),
	m_iGROUP_ID(iGroupId),
	m_bIS_STREAMING(bIsStreaming),
	m_iINSTANCE_LIMIT(iInstanceLimit)
{
	HyAssert(m_iINSTANCE_LIMIT >= 0, "Invalid instance limit in HySoundBuffers");
	if(m_iINSTANCE_LIMIT == 0)
		m_SoundList.push_back(HY_NEW ma_sound());
	else
	{
		m_SoundList.resize(m_iINSTANCE_LIMIT);
		for(int i = 0; i < m_SoundList.size(); ++i)
			m_SoundList[i] = HY_NEW ma_sound();
	}
}

HySoundBuffers::~HySoundBuffers()
{
	//Unload();

	for(int i = 0; i < m_SoundList.size(); ++i)
		delete m_SoundList[i];
}

std::string HySoundBuffers::GetFilePath() const
{
	return m_sFILE_PATH;
}

int32 HySoundBuffers::GetGroupId() const
{
	return m_iGROUP_ID;
}

bool HySoundBuffers::IsStreaming() const
{
	return m_bIS_STREAMING;
}

int32 HySoundBuffers::GetInstanceLimit() const
{
	return m_iINSTANCE_LIMIT;
}

bool HySoundBuffers::Load()
{
	ma_uint32 uiFlags = m_bIS_STREAMING ? MA_SOUND_FLAG_STREAM : 0;
	ma_sound_group *pGroup = m_CoreRef.GetGroup(GetGroupId());

	for(uint32 i = 0; i < static_cast<uint32>(m_SoundList.size()); ++i)
	{
		ma_result eResult = ma_sound_init_from_file(m_CoreRef.GetEngine(),
			m_sFILE_PATH.c_str(),
			uiFlags,
			pGroup,
			nullptr,
			m_SoundList[i]);

		if(eResult != MA_SUCCESS)
		{
			HyLogError("Load() - ma_sound_init_from_file failed: " << eResult);
			return false;  // Failed to load sound.
		}
	}
	
	return true;
}

void HySoundBuffers::Unload()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundList.size()); ++i)
		ma_sound_uninit(m_SoundList[i]);
}

ma_sound *HySoundBuffers::GetFreshBuffer()
{
	while(true)
	{
		for(int32 i = 0; i < static_cast<int32>(m_SoundList.size()); ++i)
		{
			if(ma_sound_is_playing(m_SoundList[i]) == false)
				return m_SoundList[i];
		}

		if(m_iINSTANCE_LIMIT == 0) // Allows dynamic resizing
			AppendBuffer();
		else
			return nullptr; // No available buffer
	}
}

void HySoundBuffers::AppendBuffer()
{
	m_SoundList.push_back(HY_NEW ma_sound());

	ma_uint32 uiFlags = m_bIS_STREAMING ? MA_SOUND_FLAG_STREAM : 0;
	uiFlags |= MA_SOUND_FLAG_ASYNC;

	ma_sound_group *pGroup = m_CoreRef.GetGroup(GetGroupId());

	//ma_result eResult = ma_sound_init_copy(m_CoreRef.GetEngine(),
	//	&m_SoundList[0],
	//	uiFlags,
	//	pGroup,
	//	&m_SoundList[m_SoundList.size() - 1]);

	ma_result eResult = ma_sound_init_from_file(m_CoreRef.GetEngine(),
		m_sFILE_PATH.c_str(),
		uiFlags,
		pGroup,
		nullptr,
		m_SoundList.back());

	if(eResult != MA_SUCCESS)
		HyLogError("AppendBuffer() - ma_sound_init_from_file failed: " << eResult);
}
