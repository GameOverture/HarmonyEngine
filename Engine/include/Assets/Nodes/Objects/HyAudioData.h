/**************************************************************************
 *	HyAudioData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudioData_h__
#define HyAudioData_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"

class HyAudioData : public IHyNodeData
{
	class AudioState
	{
	public:
		HyAudioPlaylist			m_Playlist;

		HyPlaylistMode			m_ePlaylistMode;
		float					m_fVolume;
		float					m_fPitch;
		int32					m_iPriority;
		int32					m_iLoops;
		uint32					m_uiMaxDistance;
	};
	AudioState *				m_pAudioStates;

	std::vector<uint32> *		m_pSequentialCountList; // Needs to be dynambically allocated so we can update the vector within a const function

	HyFileAudio *				m_pAuxiliaryFile;

public:
	HyAudioData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	HyAudioData(uint32 uiChecksum, uint32 uiBankId, HyAssets &assetsRef);
	HyAudioData(HyAuxiliaryFileHandle hFileHandle, std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit, int32 iCategoryId, HyAssets &assetsRef);
	virtual ~HyAudioData(void);

	virtual IHyFile *GetAuxiliaryFile() const override;

	const HyAudioPlaylist &GetPlaylist(uint32 uiStateIndex) const;

	HyPlaylistMode GetPlaylistMode(uint32 uiStateIndex) const;
	int32 GetPriority(uint32 uiStateIndex) const;
	int32 GetLoops(uint32 uiStateIndex) const;
	uint32 GetMaxDistance(uint32 uiStateIndex) const;
	float GetVolume(uint32 uiStateIndex) const;
	float GetPitch(uint32 uiStateIndex) const;

	int32 WeightedEntryPull(const HyAudioPlaylist &entriesList) const; // Returns the index within 'entriesList' that was chosen. -1 if 'entriesList' is empty
	void WeightedShuffle(uint32 uiStateIndex, std::vector<HyAudioHandle> &soundOrderListOut) const;

	HyAudioHandle GetNextSequential(uint32 uiStateIndex) const;
};

#endif /* HyAudioData_h__ */
