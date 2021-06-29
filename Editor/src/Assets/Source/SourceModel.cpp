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

		SaveMeta();
		bAccepted = true;
	}
	delete pDlg;
	
	return bAccepted;
}

/*virtual*/ QStringList SourceModel::GetSupportedFileExtList() const /*override*/
{
	return QStringList() << ".cpp" << ".h"; // TODO: Add shader file types eventually
}

/*virtual*/ void SourceModel::OnAllocateDraw(IManagerDraw *&pDrawOut) /*override*/
{
}

quint32 SourceModel::ComputeFileChecksum(QString sFilterPath, QString sFileName) const
{
	QString sCombinedPath;
	if(sFilterPath.isEmpty() || sFilterPath == ".")
		sCombinedPath = sFileName;
	else
		sCombinedPath = sFilterPath + "/" + sFileName;

	std::string sCleanPath = HyIO::CleanPath(sCombinedPath.toLocal8Bit(), nullptr, true);
	return HyGlobal::CRCData(0, reinterpret_cast<const uchar *>(sCleanPath.data()), sCleanPath.size());
}

QString SourceModel::GenerateSrcFile(TemplateFileType eTemplate, QModelIndex destIndex, QString sClassName, QString sFileName)
{
	QString sTemplateFilePath = MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "src/";
	switch(eTemplate)
	{
	case TEMPLATE_Main:			sTemplateFilePath += "main.cpp";		break;
	case TEMPLATE_Pch:			sTemplateFilePath += "pch.h";			break;
	case TEMPLATE_MainClassCpp:	sTemplateFilePath += "MainClass.cpp";	break;
	case TEMPLATE_MainClassH:	sTemplateFilePath += "MainClass.h";		break;
	case TEMPLATE_ClassCpp:		sTemplateFilePath += "Class.cpp";		break;
	case TEMPLATE_ClassH:		sTemplateFilePath += "Class.h";			break;
	}

	QFile file(sTemplateFilePath);
	if(!file.open(QFile::ReadOnly))
	{
		HyGuiLog("Error reading " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
		return QString();
	}
	QTextCodec *pCodec = QTextCodec::codecForLocale();
	QString sContents = pCodec->toUnicode(file.readAll());
	file.close();

	// Replace template variables
	sContents.replace("%HY_CLASS%", sClassName);
	sContents.replace("%HY_TITLE%", m_ProjectRef.GetTitle());
	sContents.replace("%HY_PROJECTNAME%", m_ProjectRef.GetName());

	// Save generated source file to destination
	QString sDestinationPath = m_MetaDir.absoluteFilePath(AssembleFilter(data(destIndex, Qt::UserRole).value<TreeModelItemData *>(), true));
	sDestinationPath += "/" + sFileName + "." + QFileInfo(sTemplateFilePath).suffix();
	file.setFileName(sDestinationPath);
	if(!file.open(QFile::WriteOnly))
	{
		HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
		return QString();
	}
	file.write(pCodec->fromUnicode(sContents));
	file.close();

	return sDestinationPath;
}

void SourceModel::GatherSourceFiles(QStringList &srcFilePathListOut, QList<quint32> &checksumListOut) const
{
	srcFilePathListOut.clear();
	checksumListOut.clear();

	QStack<QFileInfoList> dirStack;
	QFileInfoList metaInfoList = m_MetaDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	dirStack.push(metaInfoList);

	while(dirStack.isEmpty() == false)
	{
		QFileInfoList fileInfoList = dirStack.pop();

		for(int i = 0; i < fileInfoList.count(); i++)
		{
			QFileInfo info = fileInfoList[i];
			QString sTest = info.absoluteFilePath();
			if(info.isDir())
			{
				QDir subDir(info.filePath());
				dirStack.push(subDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot));
			}
			else
			{
				for(const auto &sExt : GetSupportedFileExtList())
				{
					if(QString('.' % info.suffix()).compare(sExt, Qt::CaseInsensitive) == 0)
					{
						srcFilePathListOut.push_back(info.filePath());
						
						QString sFilterPath = m_MetaDir.relativeFilePath(info.absoluteDir().absolutePath());
						QString sFileName = info.fileName();
						checksumListOut.push_back(ComputeFileChecksum(sFilterPath, sFileName));
						break;
					}
				}
			}
		}
	}
}

/*virtual*/ void SourceModel::OnInit() /*override*/
{
	if(m_BanksModel.GetBank(0)->m_AssetList.empty())
	{
		// If asset list is empty try importing (refreshing) everything in m_MetaDir (source) directory.
		// If this is also empty, then generate a brand new project
		QStringList sImportList;
		QList<TreeModelItemData *> correspondingParentList;
		QList<QUuid> correspondingUuidList;

		// Dig recursively through m_MetaDir and grab all the source files (while creating filters that resemble the folder structure they're stored in)
		QDir dirEntry(m_MetaDir);
		TreeModelItemData *pCurFilter = nullptr;

		QStack<QPair<QFileInfoList, TreeModelItemData *>> dirStack;
		dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(dirEntry.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot), pCurFilter));

		while(dirStack.isEmpty() == false)
		{
			QPair<QFileInfoList, TreeModelItemData *> curDir = dirStack.pop();
			QFileInfoList list = curDir.first;

			for(int i = 0; i < list.count(); i++)
			{
				QFileInfo info = list[i];
				if(info.isDir())// && info.fileName() != ".." && info.fileName() != ".")
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

		// If nothing exists, generate a brand new project
		if(sImportList.empty())
		{
			sImportList << GenerateSrcFile(TEMPLATE_Main, QModelIndex(), m_ProjectRef.GetName(), "main");
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();

			sImportList << GenerateSrcFile(TEMPLATE_Pch, QModelIndex(), m_ProjectRef.GetName(), "pch");
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();

			sImportList << GenerateSrcFile(TEMPLATE_MainClassCpp, QModelIndex(), m_ProjectRef.GetName(), m_ProjectRef.GetName());
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();

			sImportList << GenerateSrcFile(TEMPLATE_MainClassH, QModelIndex(), m_ProjectRef.GetName(), m_ProjectRef.GetName());
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();
		}
		
		ImportNewAssets(sImportList,
						0,
						ITEM_Source,
						correspondingParentList,
						correspondingUuidList);

		SaveMeta();
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
	// This function doesn't make sense with how source files are kept (they're updated via some IDE)
	return false;
}

/*virtual*/ bool SourceModel::OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
{
	// This function doesn't make sense with how source files are kept (single bank)
	return false;
}

/*virtual*/ void SourceModel::OnSaveMeta() /*override*/
{
	const BankData *pSourceBank = m_BanksModel.GetBank(0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// First ensure all source files are residing in their proper folder on disk (represented here with filters)
	QStringList srcFilePathList;
	QList<quint32> currentChecksumList;
	GatherSourceFiles(srcFilePathList, currentChecksumList);

	for(int32 i = 0; i < pSourceBank->m_AssetList.size(); ++i)
	{
		if(pSourceBank->m_AssetList[i]->GetFilter().isEmpty() == false)
		{
			if(m_MetaDir.mkpath(pSourceBank->m_AssetList[i]->GetFilter()) == false)
				HyGuiLog("SourceModel::OnSaveMeta mkpath() failed", LOGTYPE_Error);
		}

		for(int32 iSrcFileIndex = 0; iSrcFileIndex < currentChecksumList.size(); ++iSrcFileIndex)
		{
			if(pSourceBank->m_AssetList[i]->GetChecksum() == currentChecksumList[iSrcFileIndex])
			{
				QFileInfo oldFileInfo(srcFilePathList[iSrcFileIndex]);
				QFileInfo newFileInfo(m_MetaDir.absoluteFilePath(pSourceBank->m_AssetList[i]->ConstructMetaFileName()));
				if(oldFileInfo != newFileInfo)
				{
					QString sOldFilePath = oldFileInfo.absoluteFilePath();
					QString sNewFilePath = newFileInfo.absoluteFilePath();
					QFile::rename(sOldFilePath, sNewFilePath);
					static_cast<SourceFile *>(pSourceBank->m_AssetList[i])->UpdateChecksum(ComputeFileChecksum(pSourceBank->m_AssetList[i]->GetFilter(), pSourceBank->m_AssetList[i]->GetName()));
				}
				
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Now re-generate the CMakeLists.txt
	QFile file(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "src/CMakeLists.txt");
	if(!file.open(QFile::ReadOnly))
	{
		HyGuiLog("Error reading " % file.fileName() % " when generating CMakeLists.txt: " % file.errorString(), LOGTYPE_Error);
		return;
	}
	QTextCodec *pCodec = QTextCodec::codecForLocale();
	QString sContents = pCodec->toUnicode(file.readAll());
	file.close();

	// Replace the template CMakeLists %HY_% variables
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
	{
		QString sFilter = pSourceBank->m_AssetList[i]->GetFilter();
		if(sFilter.isEmpty() == false)
			srcFolderList.push_back(sFilter);
	}
	srcFolderList = srcFolderList.toSet().toList(); // Remove duplicates
	QString sIncludeDirs;
	for(auto sFolder : srcFolderList)
		sIncludeDirs += "list(APPEND GAME_INCLUDE_DIRS \"${CMAKE_CURRENT_SOURCE_DIR}/" + sFolder + "\")\n";
	sContents.replace("%HY_INCLUDEDIRS%", sIncludeDirs);

	sContents.replace("%HY_TITLE%", m_ProjectRef.GetTitle());

	QString sDependAdd;
	QJsonArray srcDependsArray = pSourceBank->m_MetaObj["SrcDepends"].toArray();
	for(auto srcDep : srcDependsArray)
	{
		QJsonObject srcDepObj = srcDep.toObject();

		sDependAdd += "add_subdirectory(\"";
		sDependAdd += srcDepObj["RelPath"].toString();
		sDependAdd += "\" \"";
		sDependAdd += srcDepObj["ProjectName"].toString();
		sDependAdd += "\")\n";
	}
	sContents.replace("%HY_DEPENDENCIES_ADD%", sDependAdd);

	QString sDependLink;
	for(auto srcDep : srcDependsArray)
	{
		QJsonObject srcDepObj = srcDep.toObject();

		sDependLink += "\"";
		sDependLink += srcDepObj["ProjectName"].toString();
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
}

/*virtual*/ QJsonObject SourceModel::GetSaveJson() /*override*/
{
	// This function doesn't make sense with m_bHasRuntimeMantifest == false
	return QJsonObject();
}
