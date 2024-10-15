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

AudioImportThread::AudioImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{

}

/*virtual*/ AudioImportThread::~AudioImportThread()
{
}

/*virtual*/ bool AudioImportThread::OnRun(QString &sReportOut) /*override*/
{
	AudioManagerModel &audioModelRef = static_cast<AudioManagerModel &>(m_ManagerModelRef);
	QStringList invalidImportList;

	const int iEMIT_THROTTLE = m_sImportAssetList.size() / 100;

	WaveHeader tempHeader;
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		// Error check all the imported assets before adding them
		if(audioModelRef.IsWaveValid(m_sImportAssetList[i], tempHeader))
		{
			// ImportSound calls RegisterAsset()
			SoundClip *pNewAsset = audioModelRef.ImportSound(m_sImportAssetList[i], m_uiBankId, m_CorrespondingUuidList[i], tempHeader);
			if(pNewAsset)
			{
				audioModelRef.m_ImportedAssetList.append(pNewAsset);

				if(iEMIT_THROTTLE == 0 || (i % iEMIT_THROTTLE) == 0)
					Q_EMIT ImportUpdate(i + 1, m_sImportAssetList.size());
			}
			else
				invalidImportList.append(m_sImportAssetList[i]);
		}
		else
			invalidImportList.append(m_sImportAssetList[i]);
	}

	sReportOut.clear();
	if(invalidImportList.empty() == false)
	{
		sReportOut = "The following assets were invalid: ";
		for(int i = 0; i < invalidImportList.size(); ++i)
			sReportOut += invalidImportList[i] + ", ";
		sReportOut.chop(2);
	}

	// Repack asset bank with newly imported audio
	if(audioModelRef.m_ImportedAssetList.empty() == false)
	{
		QSet<IAssetItemData *> returnListAsSet(audioModelRef.m_ImportedAssetList.begin(), audioModelRef.m_ImportedAssetList.end());
		audioModelRef.AddAssetsToRepack(audioModelRef.m_BanksModel.GetBank(audioModelRef.GetBankIndexFromBankId(m_uiBankId)), returnListAsSet);

		return true;
	}

	return false;
}
