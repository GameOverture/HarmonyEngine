/**************************************************************************
 *	SourceModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SourceModel.h"
#include "SourceFile.h"
#include "Project.h"
#include "MainWindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>

SourceModel::SourceModel(Project &projRef) :
	IManagerModel(projRef, ASSET_Source)
{
	m_bIsSingleBank = true;
	m_MetaDir.setPath(m_ProjectRef.GetSourceAbsPath());
	m_DataDir.setPath(m_ProjectRef.GetSourceAbsPath());
}

/*virtual*/ SourceModel::~SourceModel()
{

}

/*virtual*/ QString SourceModel::OnBankInfo(uint uiBankIndex) /*override*/
{
	return "";
}

/*virtual*/ bool SourceModel::OnBankSettingsDlg(uint uiBankIndex) /*override*/
{
	return false;
}

/*virtual*/ QStringList SourceModel::GetSupportedFileExtList() /*override*/
{
	return QStringList() << ".cpp" << ".h"; // TODO: Add shader file types eventually
}

/*virtual*/ void SourceModel::OnAllocateDraw(IManagerDraw *&pDrawOut) /*override*/
{
}

quint32 SourceModel::ComputeFileChecksum(QString sFilterPath, QString sFileName) const
{
	QString sCombinedPath;
	if(sFilterPath.isEmpty())
		sCombinedPath = sFileName;
	else
		sCombinedPath = sFilterPath + "/" + sFileName;

	std::string sCleanPath = HyIO::CleanPath(sCombinedPath.toLocal8Bit(), nullptr, true);
	return HyGlobal::CRCData(0, reinterpret_cast<const uchar *>(sCleanPath.data()), sCleanPath.size());
}

void SourceModel::WriteCMakeLists()
{
	SaveMeta();

	//m_ProjectRef.Get
}

/*virtual*/ void SourceModel::OnInit() /*override*/
{
	if(m_BanksModel.GetBank(0)->m_AssetList.empty())
	{
		// Try importing (refreshing) everything in m_DataDir (source) directory if no source files exist:

		QStringList sImportList;
		QList<TreeModelItemData *> correspondingParentList;
		QList<QUuid> correspondingUuidList;


		// Dig recursively through this directory and grab all the image files (while creating filters that resemble the folder structure they're stored in)
		QDir dirEntry(m_DataDir);
		TreeModelItemData *pCurFilter = nullptr;// m_pModel->CreateNewFilter(dirEntry.dirName(), pImportParent);

		QStack<QPair<QFileInfoList, TreeModelItemData *>> dirStack;
		dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(dirEntry.entryInfoList(), pCurFilter));

		while(dirStack.isEmpty() == false)
		{
			QPair<QFileInfoList, TreeModelItemData *> curDir = dirStack.pop();
			QFileInfoList list = curDir.first;

			for(int i = 0; i < list.count(); i++)
			{
				QFileInfo info = list[i];
				if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
				{
					QDir subDir(info.filePath());
					dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(subDir.entryInfoList(), CreateNewFilter(subDir.dirName(), curDir.second)));
				}
				else
				{
					for(auto sExt : GetSupportedFileExtList())
					{
						if(QString('.' % info.suffix()).compare(sExt, Qt::CaseInsensitive) == 0)
						{
							sImportList.push_back(info.filePath());
							correspondingParentList.push_back(curDir.second);
							correspondingUuidList.append(QUuid::createUuid());
							break;
						}
					}
				}
			}
		}
		
		ImportNewAssets(sImportList,
						0,
						ITEM_Source,
						correspondingParentList,
						correspondingUuidList);

		WriteCMakeLists();
	}
}

/*virtual*/ AssetItemData *SourceModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	SourceFile *pNewFile = new SourceFile(*this,
										  QUuid(metaObj["assetUUID"].toString()),
										  JSONOBJ_TOINT(metaObj, "checksum"),
										  metaObj["name"].toString(),
										  metaObj["errors"].toInt(0));

	return pNewFile;
}

/*virtual*/ QList<AssetItemData *> SourceModel::OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) /*override*/
{
	QList<AssetItemData *> returnList;

	// Error check all the imported assets before adding them, and cancel entire import if any fail
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		QFileInfo fileInfo(sImportAssetList[i]);
		if(fileInfo.exists() == false)
		{
			HyGuiLog("Could not find imported file: " % sImportAssetList[i], LOGTYPE_Warning);
			return returnList;
		}
		
		quint32 uiChecksum = ComputeFileChecksum(AssembleFilter(correspondingParentList[i], true), fileInfo.fileName());
		auto srcFilesInFilter = FindByChecksum(uiChecksum);
		if(srcFilesInFilter.isEmpty() == false)
		{
			HyGuiLog("A file with the name: " % fileInfo.fileName() % "\nalready exists in this location.", LOGTYPE_Warning);
			return returnList;
		}
	}
	
	// Passed error check: proceed with import
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		QFileInfo origFileInfo(sImportAssetList[i]);
		QString sNewFilterPath = AssembleFilter(correspondingParentList[i], true);
		QString sNewFileName = origFileInfo.fileName();

		QString sNewFilePath;
		if(sNewFilterPath.isEmpty())
			sNewFilePath = sNewFileName;
		else
			sNewFilePath = sNewFilterPath + "/" + sNewFileName;
		
		// Copy file into source location
		sNewFilePath = m_DataDir.filePath(sNewFilePath);
		if(origFileInfo.absoluteFilePath().compare(sNewFilePath, Qt::CaseInsensitive) != 0 && QFile::exists(sNewFilePath) == false)
		{
			if(QFile::copy(origFileInfo.absoluteFilePath(), sNewFilePath) == false)
			{
				HyGuiLog("QFile::copy failed: " % origFileInfo.absoluteFilePath() % " -> " % sNewFilePath, LOGTYPE_Error);
				continue;
			}
		}

		quint32 uiChecksum = ComputeFileChecksum(sNewFilterPath, sNewFileName);
		SourceFile *pNewFile = new SourceFile(*this, correspondingUuidList[i], uiChecksum, sNewFileName, 0);
		returnList.append(pNewFile);
		RegisterAsset(pNewFile);
	}

	return returnList;
}

/*virtual*/ bool SourceModel::OnRemoveAssets(QList<AssetItemData *> assetList) /*override*/
{
	for(int i = 0; i < assetList.count(); ++i)
	{
		SourceFile *pFile = static_cast<SourceFile *>(assetList[i]);
		QString sFileToDelete = m_DataDir.filePath(pFile->GetFilter() % pFile->GetName());
		
		if(QFile::remove(sFileToDelete) == false)
		{
			HyGuiLog("QFile::remove failed: " % sFileToDelete, LOGTYPE_Error);
			continue;
		}

		DeleteAsset(pFile);
	}

	return true;
}

/*virtual*/ bool SourceModel::OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) /*override*/
{
	// This function doesn't make sense with how source files are kept
	return false;
}

/*virtual*/ bool SourceModel::OnUpdateAssets(QList<AssetItemData *> assetList) /*override*/
{
	// This function doesn't make sense with how source files are kept
	return false;
}

/*virtual*/ bool SourceModel::OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
{
	// This function doesn't make sense with how source files are kept
	return false;
}

/*virtual*/ QJsonObject SourceModel::GetSaveJson() /*override*/
{
	// This function doesn't make sense with m_bHasRuntimeMantifest == false
	return QJsonObject();
}
