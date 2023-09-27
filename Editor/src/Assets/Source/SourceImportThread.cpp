/**************************************************************************
 *	SourceImportThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SourceImportThread.h"
#include "SourceModel.h"
#include "SourceFile.h"

SourceImportThread::SourceImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{
}

/*virtual*/ SourceImportThread::~SourceImportThread()
{
}

/*virtual*/ QString SourceImportThread::OnRun() /*override*/
{
	SourceModel &sourceModelRef = static_cast<SourceModel &>(m_ManagerModelRef);

	// Error check all the imported assets before adding them, and cancel entire import if any fail
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		QFileInfo fileInfo(m_sImportAssetList[i]);
		if(fileInfo.exists() == false)
			return QString("Could not find imported file: " % m_sImportAssetList[i]);

		quint32 uiChecksum = sourceModelRef.ComputeFileChecksum(sourceModelRef.AssembleFilter(m_CorrespondingParentList[i], true), fileInfo.fileName());
		auto srcFilesInFilter = sourceModelRef.FindByChecksum(uiChecksum);
		if(srcFilesInFilter.isEmpty() == false)
			return QString("A file with the name: " % fileInfo.fileName() % "\nalready exists in this location.");
	}

	// Passed error check: proceed with import
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		QFileInfo origFileInfo(m_sImportAssetList[i]);
		QString sNewFilterPath = sourceModelRef.AssembleFilter(m_CorrespondingParentList[i], true);
		QString sNewFileName = origFileInfo.fileName();

		QString sNewFilePath;
		if(sNewFilterPath.isEmpty())
			sNewFilePath = sNewFileName;
		else
			sNewFilePath = sNewFilterPath + "/" + sNewFileName;

		// Copy file into source location
		sNewFilePath = sourceModelRef.m_MetaDir.filePath(sNewFilePath);
		if(origFileInfo.absoluteFilePath().compare(sNewFilePath, Qt::CaseInsensitive) != 0 && QFile::exists(sNewFilePath) == false)
		{
			if(QFile::copy(origFileInfo.absoluteFilePath(), sNewFilePath) == false)
			{
				HyGuiLog("QFile::copy failed: " % origFileInfo.absoluteFilePath() % " -> " % sNewFilePath, LOGTYPE_Error);
				continue;
			}
		}

		quint32 uiChecksum = sourceModelRef.ComputeFileChecksum(sNewFilterPath, sNewFileName);
		SourceFile *pNewFile = new SourceFile(sourceModelRef, m_CorrespondingUuidList[i], uiChecksum, sNewFileName, 0);
		sourceModelRef.m_ImportedAssetList.append(pNewFile);
		sourceModelRef.RegisterAsset(pNewFile);

		Q_EMIT ImportUpdate(i + 1, m_sImportAssetList.size());
	}

	return ""; // No error; Successful Import into Asset Manager's 'm_ImportedAssetList'
}
