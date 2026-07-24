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
#include "AudioManager.h"

AudioImportThread::AudioImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{

}

/*virtual*/ AudioImportThread::~AudioImportThread()
{
}

/*virtual*/ bool AudioImportThread::OnRun(QString &sReportOut) /*override*/
{
	AudioManager &audioManagerRef = static_cast<AudioManager &>(m_ManagerModelRef);
	QStringList invalidImportList;

	const int iEMIT_THROTTLE = m_sImportAssetList.size() / 100;

	WaveHeader tempHeader;
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		// Error check all the imported assets before adding them
		if(audioManagerRef.IsWaveValid(m_sImportAssetList[i], tempHeader))
		{
			// ImportSound calls RegisterAsset()
			SoundClip *pNewAsset = audioManagerRef.ImportSound(m_sImportAssetList[i], m_uiBankId, m_CorrespondingUuidList[i], tempHeader);
			if(pNewAsset)
			{
				audioManagerRef.m_ImportedAssetList.append(pNewAsset);

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
	if(audioManagerRef.m_ImportedAssetList.empty() == false)
	{
		QSet<IAssetItemData *> returnListAsSet(audioManagerRef.m_ImportedAssetList.begin(), audioManagerRef.m_ImportedAssetList.end());
		audioManagerRef.AddAssetsToRepack(audioManagerRef.m_BanksModel.GetBank(audioManagerRef.GetBankIndexFromBankId(m_uiBankId)), returnListAsSet);

		return true;
	}

	return false;
}
