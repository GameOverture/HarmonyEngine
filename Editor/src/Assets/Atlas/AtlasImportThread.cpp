/**************************************************************************
 *	AtlasImportThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasImportThread.h"

#include <AtlasModel.h>

AtlasImportThread::AtlasImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{
}

/*virtual*/ AtlasImportThread::~AtlasImportThread()
{
}

/*virtual*/ bool AtlasImportThread::OnRun(QString &sReportOut) /*override*/
{
	AtlasModel &atlasModelRef = static_cast<AtlasModel &>(m_ManagerModelRef);

	const int iEMIT_THROTTLE = m_sImportAssetList.size() / 100;

	QImage tempImage;
	QFileInfoList invalidImportList;
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		QFileInfo fileInfo(m_sImportAssetList[i]);
		tempImage.load(fileInfo.absoluteFilePath());
		
		// Error check the imported assets before adding them
		if(atlasModelRef.IsImageValid(tempImage, m_uiBankId))
		{
			// ImportImage calls RegisterAsset()
			atlasModelRef.m_ImportedAssetList.append(atlasModelRef.ImportImage(fileInfo.baseName(), tempImage, m_uiBankId, ITEM_None, m_CorrespondingUuidList[i]));

			if(iEMIT_THROTTLE == 0 || (i % iEMIT_THROTTLE) == 0)
				Q_EMIT ImportUpdate(i + 1, m_sImportAssetList.size());
		}
		else
			invalidImportList.append(fileInfo);
	}

	sReportOut.clear();
	if(invalidImportList.empty() == false)
	{
		QSize atlasDimensions = atlasModelRef.GetMaxAtlasDimensions(atlasModelRef.GetBankIndexFromBankId(m_uiBankId));
		sReportOut += "The following images were not imported because they were too large to fit in the atlas bank '" % QString::number(m_uiBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % "):";
		for(int i = 0; i < invalidImportList.size(); ++i)
			sReportOut += "\n" % invalidImportList[i].fileName();
	}

	// Repack asset bank with newly imported images
	if(atlasModelRef.m_ImportedAssetList.empty() == false)
	{
		QSet<IAssetItemData *> returnListAsSet(atlasModelRef.m_ImportedAssetList.begin(), atlasModelRef.m_ImportedAssetList.end());
		atlasModelRef.AddAssetsToRepack(atlasModelRef.m_BanksModel.GetBank(atlasModelRef.GetBankIndexFromBankId(m_uiBankId)), returnListAsSet);

		return true;
	}
	
	return false;
}
