/**************************************************************************
 *	HyAudioCore_miniaudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudioCore_miniaudio_h__
#define HyAudioCore_miniaudio_h__

#include "Afx/HyStdAfx.h"
#include "Audio/IHyAudioCore.h"

#define MINIAUDIO_IMPLEMENTATION
#include "vendor/miniaudio/miniaudio.h"

class IHyFileAudioImpl;
#define HYMAX_AUDIOCHANNELS 32

class HyAudioCore_miniaudio : public IHyAudioCore
{
	std::vector<std::string>					m_sDeviceList;

	ma_device_config							m_DevConfig;
	ma_device									m_Device;

	ma_engine_config							m_EngConfig;
	ma_engine									m_Engine;

	ma_resource_manager_config					m_ResConfig;
	ma_resource_manager							m_ResourceManager;

public:
	HyAudioCore_miniaudio();
	virtual ~HyAudioCore_miniaudio(void);

	const char *GetAudioDriver();

	ma_engine &GetEngine();
	ma_sound_group &GetGroup(uint32 uiGrpId);

protected:
	virtual void OnSetSfxVolume(float fGlobalSfxVolume) override;
	virtual void OnSetMusicVolume(float fGlobalMusicVolume) override;

	virtual void OnCue_Play(PlayInfo &playInfoRef) override;
	virtual void OnCue_Stop(PlayInfo &playInfoRef) override;
	virtual void OnCue_Pause(PlayInfo &playInfoRef) override;
	virtual void OnCue_Unpause(PlayInfo &playInfoRef) override;
	virtual void OnCue_Volume(PlayInfo &playInfoRef) override;

	static void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);
};

#endif /* HyAudioCore_miniaudio_h__ */
