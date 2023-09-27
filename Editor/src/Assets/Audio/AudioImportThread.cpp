/**************************************************************************
 *	AudioImportThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioImportThread.h"
#include <AudioManagerModel.h>

AudioImportThread::AudioImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{

}

/*virtual*/ AudioImportThread::~AudioImportThread()
{
}

/*virtual*/ QString AudioImportThread::OnRun() /*override*/
{
	AudioManagerModel &audioModelRef = static_cast<AudioManagerModel &>(m_ManagerModelRef);

	QList<WaveHeader> headerList;

	// Error check all the imported assets before adding them, and cancel entire import if any fail
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		headerList.push_back(WaveHeader());
		if(audioModelRef.IsWaveValid(m_sImportAssetList[i], headerList.last()) == false)
			return "Importing Wave " % m_sImportAssetList[i] % " was invalid";
	}

	// Passed error check: proceed with import
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		// ImportSound calls RegisterAsset() on valid imports
		SoundClip *pNewAsset = audioModelRef.ImportSound(m_sImportAssetList[i], m_uiBankId, m_CorrespondingUuidList[i], headerList[i]);
		if(pNewAsset)
			audioModelRef.m_ImportedAssetList.append(pNewAsset);

		Q_EMIT ImportUpdate(i + 1, m_sImportAssetList.size());
	}

	// Repack asset bank with newly imported audio
	if(audioModelRef.m_ImportedAssetList.empty() == false)
	{
		QSet<IAssetItemData *> returnListAsSet(audioModelRef.m_ImportedAssetList.begin(), audioModelRef.m_ImportedAssetList.end());
		audioModelRef.AddAssetsToRepack(audioModelRef.m_BanksModel.GetBank(audioModelRef.GetBankIndexFromBankId(m_uiBankId)), returnListAsSet);
	}

	return ""; // No error; Successful Import into Asset Manager's 'm_ImportedAssetList'
}
