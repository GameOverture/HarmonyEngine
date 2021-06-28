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
#include "SourceSettingsDlg.h"
#include "Project.h"
#include "MainWindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>
#include <QTextCodec>

SourceModel::SourceModel(Project &projRef) :
	IManagerModel(projRef, ASSET_Source)
{
	m_bIsSingleBank = true;
	m_DataDir.setPath(m_MetaDir.absolutePath()); // SourceModel doesn't use a DataDir
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
	if(uiBankIndex != 0)
	{
		HyGuiLog("SourceModel::OnBankSettingsDlg was invoked with invalid bank index: " % QString::number(uiBankIndex), LOGTYPE_Error);
		uiBankIndex = 0;
	}

	bool bAccepted = false;
	SourceSettingsDlg *pDlg = new SourceSettingsDlg(m_ProjectRef, m_BanksModel.GetBank(uiBankIndex)->m_MetaObj);
	if(QDialog::Accepted == pDlg->exec())
	{
		m_BanksModel.GetBank(uiBankIndex)->m_MetaObj = pDlg->GetMetaObj();

		WriteCMakeLists();
		bAccepted = true;
	}
	delete pDlg;
	
	return bAccepted;
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

	QFile file(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "src/CMakeLists.txt");
	if(!file.open(QFile::ReadOnly))
	{
		HyGuiLog("Error reading " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
		return;
	}
	QTextCodec *pCodec = QTextCodec::codecForLocale();
	QString sContents = pCodec->toUnicode(file.readAll());
	file.close();

	// Replace the template CMakeLists %HY_% variables
	const BankData *pSourceBank = m_BanksModel.GetBank(0);

	sContents.replace("%HY_PROJECTNAME%", m_ProjectRef.GetName());
	sContents.replace("%HY_RELPROJPATH%", m_MetaDir.relativeFilePath(m_ProjectRef.GetDirPath()));
	sContents.replace("%HY_RELDATADIR%", m_MetaDir.relativeFilePath(m_ProjectRef.GetAssetsAbsPath()));
	sContents.replace("%HY_RELHARMONYDIR%", m_MetaDir.relativeFilePath(MainWindow::EngineSrcLocation()));

	sContents.replace("%HY_OUTPUTNAME%", pSourceBank->m_MetaObj["OutputName"].toString());

	
	QString sSrcFiles;
	for(int32 i = 0; i < pSourceBank->m_AssetList.size(); ++i)
	{
		sSrcFiles += "\t\"" + static_cast<SourceFile *>(pSourceBank->m_AssetList[i])->ConstructMetaFileName();
		if(i == pSourceBank->m_AssetList.size() - 1)
			sSrcFiles += "\"";
		else
			sSrcFiles += "\"\n";
	}
	sContents.replace("%HY_SRCFILES%", sSrcFiles);

	QStringList srcFolderList;
	for(int32 i = 0; i < pSourceBank->m_AssetList.size(); ++i)
		srcFolderList.push_back(pSourceBank->m_AssetList[i]->GetFilter());
	srcFolderList = srcFolderList.toSet().toList(); // Remove duplicates
	QString sIncludeDirs;
	for(auto sFolder : srcFolderList)
		sIncludeDirs += "list(APPEND GAME_INCLUDE_DIRS \"${CMAKE_CURRENT_SOURCE_DIR}/" + sFolder + ")\n";
	sContents.replace("%HY_INCLUDEDIRS%", sIncludeDirs);

	sContents.replace("%HY_TITLE%", m_ProjectRef.GetTitle());

	QString sDependAdd;
	QJsonArray srcDependsArray = pSourceBank->m_MetaObj["SrcDepends"].toArray();
	for(auto srcDep : srcDependsArray)
	{
		QJsonObject srcDepObj = srcDep.toObject();

		sDependAdd += "add_subdirectory(\"";
		sDependAdd += srcDepObj["RelPath"].toString();// srcDep->GetRelPath();
		sDependAdd += "\" \"";
		sDependAdd += srcDepObj["ProjectName"].toString();// srcDep->GetProjectName();
		sDependAdd += "\")\n";
	}
	sContents.replace("%HY_DEPENDENCIES_ADD%", sDependAdd);

	QString sDependLink;
	for(auto srcDep : srcDependsArray)
	{
		QJsonObject srcDepObj = srcDep.toObject();

		sDependLink += "\"";
		sDependLink += srcDepObj["ProjectName"].toString();//srcDep->GetProjectName();
		sDependLink += "\" ";
	}
	sContents.replace("%HY_DEPENDENCIES_LINK%", sDependLink);

	// Save generated CMakeLists file to destination
	file.setFileName(m_MetaDir.absoluteFilePath("CMakeLists.txt"));
	if(!file.open(QFile::WriteOnly))
	{
		HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
		return;
	}
	file.write(pCodec->fromUnicode(sContents));
	file.close();

	//QDir projGenSrcDir();
	//fileInfoList = projGenSrcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//for(int i = 0; i < fileInfoList.size(); ++i)
	//	QFile::copy(fileInfoList[i].absoluteFilePath(), srcDir.absoluteFilePath(fileInfoList[i].fileName()));

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//// Rename the copied source files if needed
	//fileInfoList = QDir(GetProjDirPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//fileInfoList += srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//for(int i = 0; i < fileInfoList.size(); ++i)
	//{
	//	if(fileInfoList[i].fileName().contains("%HY_CLASS%"))
	//	{
	//		QFile file(fileInfoList[i].absoluteFilePath());
	//		QString sNewFileName = fileInfoList[i].fileName().replace("%HY_CLASS%", ui->txtClassName->text());
	//		file.rename(fileInfoList[i].absoluteDir().absolutePath() % "/Game/" % sNewFileName);
	//		file.close();
	//	}
	//}
	//// Then replace the variable contents of the copied source files
	//fileInfoList = QDir(GetProjDirPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//fileInfoList += srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//fileInfoList += QDir(srcDir.absoluteFilePath("Game")).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//QTextCodec *pCodec = QTextCodec::codecForLocale();
	//for(int i = 0; i < fileInfoList.size(); ++i)
	//{
	//	
	//}
}

/*virtual*/ void SourceModel::OnInit() /*override*/
{
	if(m_BanksModel.GetBank(0)->m_AssetList.empty())
	{
		// If asset list is empty try importing (refreshing) everything in m_MetaDir (source) directory.
		// If that is also empty, then generate a brand new project
		QStringList sImportList;
		QList<TreeModelItemData *> correspondingParentList;
		QList<QUuid> correspondingUuidList;

		// Dig recursively through this directory and grab all the source files (while creating filters that resemble the folder structure they're stored in)
		QDir dirEntry(m_MetaDir);
		TreeModelItemData *pCurFilter = nullptr;

		QStack<QPair<QFileInfoList, TreeModelItemData *>> dirStack;
		dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(dirEntry.entryInfoList(QDir::NoDotAndDotDot), pCurFilter));

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
					for(const auto &sExt : GetSupportedFileExtList())
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

/*virtual*/ void SourceModel::OnCreateNewBank(QJsonObject &newMetaBankObjRef) /*override*/
{
	newMetaBankObjRef["OutputName"] = m_ProjectRef.GetName();
	newMetaBankObjRef["SrcDepends"] = QJsonArray();
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
		sNewFilePath = m_MetaDir.filePath(sNewFilePath);
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
		QString sFileToDelete = m_MetaDir.filePath(pFile->GetFilter() % pFile->GetName());
		
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
