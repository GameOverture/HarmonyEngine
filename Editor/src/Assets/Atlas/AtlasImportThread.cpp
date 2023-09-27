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

AtlasImportThread::AtlasImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{
}

/*virtual*/ AtlasImportThread::~AtlasImportThread()
{
}

/*virtual*/ QString AtlasImportThread::OnRun() /*override*/
{
	AtlasModel &atlasModelRef = static_cast<AtlasModel &>(m_ManagerModelRef);

	// Error check all the imported assets before adding them, and cancel entire import if any fail
	QList<QImage *> newImageList;
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		QFileInfo fileInfo(m_sImportAssetList[i]);

		QImage *pNewImage = new QImage(fileInfo.absoluteFilePath());
		newImageList.push_back(pNewImage);

		QSize atlasDimensions = atlasModelRef.GetMaxAtlasDimensions(atlasModelRef.GetBankIndexFromBankId(m_uiBankId));
		if(atlasModelRef.IsImageValid(*pNewImage, m_uiBankId) == false)
		{
			QString sCancelReason = "Importing image " % fileInfo.fileName() % " will not fit in atlas bank '" % QString::number(m_uiBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")";
			for(auto image : newImageList)
				delete image;

			return sCancelReason;
		}
	}

	// Passed error check: proceed with import
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		atlasModelRef.m_ImportedAssetList.append(atlasModelRef.ImportImage(QFileInfo(m_sImportAssetList[i]).baseName(), *newImageList[i], m_uiBankId, false, m_CorrespondingUuidList[i]));
		Q_EMIT ImportUpdate(i+1, m_sImportAssetList.size());
	}

	if(atlasModelRef.m_ImportedAssetList.empty() == false)
	{
		QSet<IAssetItemData *> returnListAsSet(atlasModelRef.m_ImportedAssetList.begin(), atlasModelRef.m_ImportedAssetList.end());
		atlasModelRef.AddAssetsToRepack(atlasModelRef.m_BanksModel.GetBank(atlasModelRef.GetBankIndexFromBankId(m_uiBankId)), returnListAsSet);
	}

	for(auto image : newImageList)
		delete image;

	return ""; // No error; Successful Import into Asset Manager's 'm_ImportedAssetList'
}
