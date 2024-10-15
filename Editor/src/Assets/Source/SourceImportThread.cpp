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

SourceImportThread::SourceImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList) :
	IImportThread(managerModelRef, sImportAssetList, uiBankId, correspondingParentList, correspondingUuidList)
{
}

/*virtual*/ SourceImportThread::~SourceImportThread()
{
}

/*virtual*/ bool SourceImportThread::OnRun(QString &sReportOut) /*override*/
{
	SourceModel &sourceModelRef = static_cast<SourceModel &>(m_ManagerModelRef);

	QStringList sErrorList;

	const int iEMIT_THROTTLE = m_sImportAssetList.size() / 100;

	// Passed error check: proceed with import
	for(int i = 0; i < m_sImportAssetList.size(); ++i)
	{
		// Error check the imported assets before adding them
		QFileInfo origFileInfo(m_sImportAssetList[i]);
		if(origFileInfo.exists() == false)
		{
			sErrorList.append("Could not find imported file: " % m_sImportAssetList[i]);
			continue;
		}
		auto srcFilesInFilter = sourceModelRef.FindByChecksum(sourceModelRef.ComputeFileChecksum(sourceModelRef.AssembleFilter(m_CorrespondingParentList[i], true), origFileInfo.fileName()));
		if(srcFilesInFilter.isEmpty() == false)
		{
			sErrorList.append("A file with the name: " % origFileInfo.fileName() % "\nalready exists in this location.");
			continue;
		}

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
				sErrorList.append("QFile::copy failed: " % origFileInfo.absoluteFilePath() % " -> " % sNewFilePath);
				continue;
			}
		}

		QString sBaseClass;
		if(sourceModelRef.m_ImportBaseClassList.size() > i)
			sBaseClass = sourceModelRef.m_ImportBaseClassList[i];

		SourceFile *pNewFile = new SourceFile(sourceModelRef, m_CorrespondingUuidList[i], sBaseClass, sourceModelRef.ComputeFileChecksum(sNewFilterPath, sNewFileName), sNewFileName, 0);
		sourceModelRef.m_ImportedAssetList.append(pNewFile);
		sourceModelRef.RegisterAsset(pNewFile);

		if(iEMIT_THROTTLE == 0 || (i % iEMIT_THROTTLE) == 0)
			Q_EMIT ImportUpdate(i + 1, m_sImportAssetList.size());
	}

	sReportOut.clear();
	if(sErrorList.empty() == false)
	{
		sReportOut = "The following errors occurred during import:\n";
		for(int i = 0; i < sErrorList.size(); ++i)
			sReportOut += sErrorList[i] % "\n";
	}

	return sourceModelRef.m_ImportedAssetList.empty() == false;
}
