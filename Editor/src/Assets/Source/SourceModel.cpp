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
#include "DlgBuildSettings.h"
#include "SourceGenFileDlg.h"
#include "Project.h"
#include "EntityModel.h"
#include "WgtCodeEditor.h"
#include "MainWindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>

SourceModel::SourceModel(Project &projRef) :
	IManagerModel(projRef, ASSETMAN_Source),
	m_pEntityFolderItem(nullptr)
{
	m_bIsSingleBank = true;
	m_MetaDir.setPath(m_ProjectRef.GetSourceAbsPath());
	m_DataDir.setPath(m_ProjectRef.GetSourceAbsPath()); // SourceModel doesn't use a DataDir

	m_pEntityFolderItem = new TreeModelItemData(ITEM_Filter, QUuid(), HySrcEntityFilter);
	InsertTreeItem(m_ProjectRef, m_pEntityFolderItem, nullptr);
}

/*virtual*/ SourceModel::~SourceModel()
{
}

bool SourceModel::GenerateEntitySrcFiles(EntityModel &entityModelRef)
{
	m_ImportBaseClassList.clear();

	QString sClassName = entityModelRef.GetItem().GetName(false);

	// Generate the cpp and h file of the entity (it will overwrite the entity files if they already exist)
	QModelIndex entityFolderIndex = FindIndex<TreeModelItemData *>(m_pEntityFolderItem, 0);
	QStringList sImportList;

	QString sHeaderFile = GenerateSrcFile(TEMPLATE_EntityH, entityFolderIndex, sClassName, "hy_" % sClassName, "HyEntity2d", true, &entityModelRef);
	if(false == DoesAssetExist(ComputeFileChecksum(AssembleFilter(m_pEntityFolderItem, true), QFileInfo(sHeaderFile).fileName())))
	{
		sImportList << sHeaderFile;
		m_ImportBaseClassList << "HyEntity2d";
	}

	QString sSrcFile = GenerateSrcFile(TEMPLATE_EntityCpp, entityFolderIndex, sClassName, "hy_" % sClassName, "HyEntity2d", true, &entityModelRef);
	if(false == DoesAssetExist(ComputeFileChecksum(AssembleFilter(m_pEntityFolderItem, true), QFileInfo(sSrcFile).fileName())))
	{
		sImportList << sSrcFile;
		m_ImportBaseClassList << "HyEntity2d";
	}

	if(sImportList.empty())
		return true;

	QVector<TreeModelItemData *> correspondingParentList;
	QVector<QUuid> correspondingUuidList;
	for(int i = 0; i < sImportList.size(); ++i)
	{
		correspondingParentList << m_pEntityFolderItem;
		correspondingUuidList << QUuid::createUuid();
	}

	return ImportNewAssets(sImportList, 0, correspondingParentList, correspondingUuidList);
}

void SourceModel::DeleteEntitySrcFiles(EntityModel &entityModelRef)
{
	QList<IAssetItemData *> &assetListRef = m_BanksModel.GetBank(0)->m_AssetList;

	QList<IAssetItemData *> deleteList;
	QString sFileName = "hy_" + entityModelRef.GetItem().GetName(false);
	for(IAssetItemData *pSrcAsset : assetListRef)
	{
		QFileInfo srcFileInfo(pSrcAsset->GetAbsMetaFilePath());
		if(sFileName.startsWith(srcFileInfo.baseName()))
			deleteList.push_back(pSrcAsset);
	}

	RemoveItems(deleteList, QList<TreeModelItemData *>(), false);
}

QStringList SourceModel::GetEditorEntityList() const
{
	QStringList sEntityList;
	
	QModelIndex entityFolderIndex = FindIndex<TreeModelItemData *>(m_pEntityFolderItem, 0);
	QList<TreeModelItemData *> editorEntityList = GetItemsRecursively(entityFolderIndex);
	for(int i = 0; i < editorEntityList.size(); ++i)
	{
		if(editorEntityList[i]->GetType() == ITEM_Source)
		{
			QString sEntCodeName = QFileInfo(static_cast<SourceFile *>(editorEntityList[i])->GetName()).baseName();
			if(sEntCodeName.startsWith("hy_"))
				sEntCodeName = sEntCodeName.mid(3);
			sEntityList << sEntCodeName;
		}
	}

	return sEntityList;
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
	DlgBuildSettings *pDlg = new DlgBuildSettings(m_ProjectRef, m_BanksModel.GetBank(uiBankIndex)->m_MetaObj);
	if(QDialog::Accepted == pDlg->exec())
	{
		pDlg->UpdateMetaObj(m_BanksModel.GetBank(uiBankIndex)->m_MetaObj);

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

quint32 SourceModel::ComputeFileChecksum(QString sFilterPath, QString sFileName) const
{
	QString sCombinedPath;
	if(sFilterPath.isEmpty() || sFilterPath == ".")
		sCombinedPath = sFileName;
	else
		sCombinedPath = sFilterPath + "/" + sFileName;

	std::string sCleanPath = HyIO::CleanPath(std::string(sCombinedPath.toLocal8Bit()));
	HyIO::MakeLowercase(sCleanPath);
	return HyGlobal::CRCData(0, reinterpret_cast<const uchar *>(sCleanPath.data()), sCleanPath.size());
}

QString SourceModel::GenerateSrcFile(TemplateFileType eTemplate, QModelIndex destIndex, QString sClassName, QString sFileName, QString sBaseClass, bool bEntityBaseClass, EntityModel *pEntityModel)
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
	case TEMPLATE_EntityCpp:	sTemplateFilePath += "Entity.cpp";		break;
	case TEMPLATE_EntityH:		sTemplateFilePath += "Entity.h";		break;
	}

	QFile file(sTemplateFilePath);
	if(!file.open(QFile::ReadOnly))
	{
		HyGuiLog("Error reading template file " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
		return QString();
	}
	
	QString sContents = file.readAll();
	file.close();

	// Strip return characters because they are causing problems
	sContents.remove('\r');

	// Replace template variables
	sContents.replace("%HY_FILENAME%", sFileName);
	sContents.replace("%HY_CLASS%", sClassName);
	sContents.replace("%HY_PROJECTNAME%", m_ProjectRef.GetName());

	QString sBaseClassDecl;
	QString sClassCtorSignature;
	QString sMemberInitializerList;
	QString sClassFuncs; // protected class methods
	if(sBaseClass.isEmpty() == false)
	{
		sBaseClassDecl = " : public " + sBaseClass;
		if(bEntityBaseClass)
		{
			switch(eTemplate)
			{
			case TEMPLATE_Main:
				sContents.replace("%HY_TITLE%", m_ProjectRef.GetTitle());
				break;

			case TEMPLATE_ClassH:
				if(sBaseClass == "HyEntity2d" || sBaseClass == "HyEntity3d")
					sContents.replace("%HY_INCLUDES%", "");
				else
				{
					QString sFileName = sBaseClass;
					sFileName.replace("hy::", "hy_");
					sContents.replace("%HY_INCLUDES%", "#include \"" + sFileName + ".h\"\n");
				}
				sClassCtorSignature = "HyEntity2d *pParent = nullptr";
				sClassFuncs = "virtual void OnUpdate() override;";
				break;

			case TEMPLATE_ClassCpp:
				sClassCtorSignature = "HyEntity2d *pParent /*= nullptr*/";
				sMemberInitializerList = " :\n\t" + sBaseClass + "(pParent)";
				sClassFuncs = "/*virtual*/ void " + sClassName + "::OnUpdate() /*override*/\n{\n}";
				break;

			case TEMPLATE_EntityH:
				if(pEntityModel == nullptr)
					HyGuiLog("SourceModel::GenerateSrcFile() is TEMPLATE_EntityH and was passed a nullptr 'pEntityModel'", LOGTYPE_Error);
				sContents.replace("%HY_INCLUDES%", pEntityModel->GenerateSrc_FileIncludes());
				sContents.replace("%HY_STATEENUMS%", pEntityModel->GenerateSrc_StateEnums());
				sClassCtorSignature = "HyEntity2d *pParent = nullptr";
				sContents.replace("%HY_MEMBERVARIABLES%", pEntityModel->GenerateSrc_MemberVariables());
				sContents.replace("%HY_ACCESSORDECL%", pEntityModel->GenerateSrc_AccessorDecl());
				sContents.replace("%HY_CALLBACKSDECL%", pEntityModel->GenerateSrc_CallbacksDecl());
				break;

			case TEMPLATE_EntityCpp:
				if(pEntityModel == nullptr)
					HyGuiLog("SourceModel::GenerateSrcFile() is TEMPLATE_EntityCpp and was passed a nullptr 'pEntityModel'", LOGTYPE_Error);
				sClassCtorSignature = "HyEntity2d *pParent /*= nullptr*/";
				sMemberInitializerList = pEntityModel->GenerateSrc_MemberInitializerList();
				sContents.replace("%HY_CTORIMPL%", pEntityModel->GenerateSrc_Ctor());
				sContents.replace("%HY_NUMSTATES%", QString::number(pEntityModel->GetNumStates()));
				sContents.replace("%HY_SETSTATEIMPL%", pEntityModel->GenerateSrc_SetStateImpl());
				sContents.replace("%HY_ACCESSORDEFINITION%", pEntityModel->GenerateSrc_AccessorDefinition(sClassName));
				sContents.replace("%HY_TIMELINEADVANCEIMPL%", pEntityModel->GenerateSrc_TimelineAdvance());
				break;
			}
		}
	}

	sContents.replace("%HY_NAMESPACE%", HySrcEntityNamespace);
	sContents.replace("%HY_BASECLASSDECL%", sBaseClassDecl);
	sContents.replace("%HY_BASECLASS%", sBaseClass);
	sContents.replace("%HY_CLASSCTORSIG%", sClassCtorSignature);
	sContents.replace("%HY_CLASSMEMBERINITIALIZERLIST%", sMemberInitializerList);
	sContents.replace("%HY_CLASSFUNCS%", sClassFuncs);

	// Save generated source file to destination
	QString sDestinationPath = m_MetaDir.absoluteFilePath(AssembleFilter(data(destIndex, Qt::UserRole).value<TreeModelItemData *>(), true));
	sDestinationPath += "/" + sFileName + "." + QFileInfo(sTemplateFilePath).suffix();
	file.setFileName(sDestinationPath);

	// Make sure destination directory exists
	QDir dir;
	if(dir.mkpath(QFileInfo(file).absolutePath()) == false)
		HyGuiLog("SourceModel::GenerateSrcFile - QDir::mkpath failed", LOGTYPE_Error);

	QIODevice::OpenMode eOpenMode = QFile::WriteOnly | QFile::Text;
	if(pEntityModel == nullptr)
		eOpenMode |= QFile::NewOnly;
	if(!file.open(eOpenMode))
	{
		HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
		return QString();
	}
	file.write(sContents.toUtf8());
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

QString SourceModel::CleanEmscriptenCcall(QString sUserValue) const
{
	sUserValue = sUserValue.simplified();
	sUserValue.replace(' ', "");
	if(sUserValue.isEmpty())
		return QString();

	QStringList sFuncList = sUserValue.split(',', Qt::SkipEmptyParts);
	if(sFuncList.empty())
		return QString();

	// list(APPEND HYEM_LINK_FLAGS "-sEXPORTED_RUNTIME_METHODS=ccall")
	// list(APPEND HYEM_LINK_FLAGS "-sEXPORTED_FUNCTIONS=_main,_foo,_bar")
	QString sLinkFlags = "list(APPEND HYEM_LINK_FLAGS \"-sEXPORTED_RUNTIME_METHODS=ccall\")\n\tlist(APPEND HYEM_LINK_FLAGS \"-sEXPORTED_FUNCTIONS=_main";

	for(int i = 0; i < sFuncList.size(); ++i)
	{
		if(sFuncList[i].startsWith('_') == false)
			sFuncList[i].prepend('_');

		if(sFuncList[i] == "_main")
			continue;

		sLinkFlags += ",";
		sLinkFlags += sFuncList[i];
	}
	sLinkFlags += "\")";

	return sLinkFlags;
}

/*virtual*/ void SourceModel::OnInit() /*override*/
{
	if(m_BanksModel.GetBank(0)->m_AssetList.empty())
	{
		m_ImportBaseClassList.clear();

		// If asset list is empty try importing (refreshing) everything in m_MetaDir (source) directory.
		// If this is also empty, then generate a brand new project
		QStringList sImportList;
		QVector<TreeModelItemData *> correspondingParentList;
		QVector<QUuid> correspondingUuidList;

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
					dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(subDir.entryInfoList(), CreateNewFilter(subDir.dirName(), curDir.second, false)));
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
							m_ImportBaseClassList.push_back(QString());
							break;
						}
					}
				}
			}
		}

		// If nothing exists, generate a brand new project
		if(sImportList.empty())
		{
			sImportList << GenerateSrcFile(TEMPLATE_Main, QModelIndex(), m_ProjectRef.GetName(), "main", QString(), false, nullptr);
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();
			m_ImportBaseClassList.push_back(QString());

			sImportList << GenerateSrcFile(TEMPLATE_Pch, QModelIndex(), m_ProjectRef.GetName(), "pch", QString(), false, nullptr);
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();
			m_ImportBaseClassList.push_back(QString());

			sImportList << GenerateSrcFile(TEMPLATE_MainClassCpp, QModelIndex(), m_ProjectRef.GetName(), m_ProjectRef.GetName(), QString(), false, nullptr);
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();
			m_ImportBaseClassList.push_back(QString());

			sImportList << GenerateSrcFile(TEMPLATE_MainClassH, QModelIndex(), m_ProjectRef.GetName(), m_ProjectRef.GetName(), QString(), false, nullptr);
			correspondingParentList << nullptr;
			correspondingUuidList << QUuid::createUuid();
			m_ImportBaseClassList.push_back(QString());
		}
		
		ImportNewAssets(sImportList,
						0,
						correspondingParentList,
						correspondingUuidList);

		SaveMeta();
	}
}

/*virtual*/ void SourceModel::OnCreateNewBank(QJsonObject &newMetaBankObjRef) /*override*/
{
	newMetaBankObjRef["OutputName"] = m_ProjectRef.GetName();
	newMetaBankObjRef["SrcDepends"] = QJsonArray();
	newMetaBankObjRef["UseSdl2"] = false;
	newMetaBankObjRef["UseSdlAudio"] = false;
	newMetaBankObjRef["UseSdlNet"] = false;
	newMetaBankObjRef["UseNlohmannJson"] = false;
	newMetaBankObjRef["UseSpine"] = false;
	newMetaBankObjRef["UseIcu"] = false;
	newMetaBankObjRef["EmscriptenCcall"] = "";
}

/*virtual*/ IAssetItemData *SourceModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	SourceFile *pNewFile = new SourceFile(*this,
										  QUuid(metaObj["assetUUID"].toString()),
										  metaObj["baseClass"].toString(),
										  JSONOBJ_TOINT(metaObj, "checksum"),
										  metaObj["name"].toString(),
										  metaObj["errors"].toInt(0));

	return pNewFile;
}

/*virtual*/ void SourceModel::OnGenerateAssetsDlg(const QModelIndex &indexDestination) /*override*/
{
	SourceGenFileDlg *pDlg = new SourceGenFileDlg(GetEditorEntityList());
	if(QDialog::Accepted == pDlg->exec())
	{
		m_ImportBaseClassList.clear();

		QStringList sImportList;
		QVector<TreeModelItemData *> correspondingParentList;
		QVector<QUuid> correspondingUuidList;

		TreeModelItemData *pParentLocation = FindTreeItemFilter(data(indexDestination, Qt::UserRole).value<TreeModelItemData *>());

		sImportList << GenerateSrcFile(TEMPLATE_ClassCpp, indexDestination, pDlg->GetCodeClassName(), pDlg->GetCppFileName(), pDlg->GetBaseClassName(), pDlg->IsEntityBaseClass(), nullptr);
		correspondingParentList << pParentLocation;
		correspondingUuidList << QUuid::createUuid();
		m_ImportBaseClassList.push_back(pDlg->GetBaseClassName());

		sImportList << GenerateSrcFile(TEMPLATE_ClassH, indexDestination, pDlg->GetCodeClassName(), pDlg->GetHeaderFileName(), pDlg->GetBaseClassName(), pDlg->IsEntityBaseClass(), nullptr);
		correspondingParentList << pParentLocation;
		correspondingUuidList << QUuid::createUuid();
		m_ImportBaseClassList.push_back(pDlg->GetBaseClassName());
		
		ImportNewAssets(sImportList,
						0,
						correspondingParentList,
						correspondingUuidList);

		SaveMeta();
	}
	delete pDlg;
}

/*virtual*/ bool SourceModel::OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) /*override*/
{
	for(int i = 0; i < assetList.count(); ++i)
	{
		SourceFile *pFile = static_cast<SourceFile *>(assetList[i]);
		QString sPrefix = sPreviousFilterPaths[i];
		if(sPrefix.isEmpty() == false)
			sPrefix += "/";
		QString sFileToDelete = m_MetaDir.absoluteFilePath(sPrefix % pFile->GetName());
		
		// Instead of DeleteAsset(), do it manually here
		RemoveLookup(pFile);
		if(QFile::remove(sFileToDelete) == false)
		{
			HyGuiLog("QFile::remove failed: " % sFileToDelete, LOGTYPE_Error);
			continue;
		}
	}

	SaveMeta();

	return true;
}

/*virtual*/ bool SourceModel::OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) /*override*/
{
	// This function doesn't make sense with how source files are kept
	return false;
}

/*virtual*/ bool SourceModel::OnUpdateAssets(QList<IAssetItemData *> assetList) /*override*/
{
	// This function doesn't make sense with how source files are kept (they're updated via some IDE)
	return false;
}

/*virtual*/ bool SourceModel::OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
{
	// This function doesn't make sense with how source files are kept (single bank)
	return false;
}

/*virtual*/ void SourceModel::OnFlushRepack() /*override*/
{
}

/*virtual*/ void SourceModel::OnSaveMeta(QJsonObject &metaObjRef) /*override*/
{
	const BankData *pSourceBank = m_BanksModel.GetBank(0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// First ensure all source files are residing in their proper folder on disk (represented here with filters)
	// If any changes need to be made, the file's checksum will also be updated and the .meta file will need to be resaved by calling IManagerModel::SaveMeta()
	bool bResaveMeta = false;
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

					bResaveMeta = true;
				}
				
				break;
			}
		}
	}

	// Clear out any empty directories that are no longer used
	QFileInfoList metaInfoList = m_MetaDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int i = 0; i < metaInfoList.count(); i++)
	{
		QFileInfo info = metaInfoList[i];
		if(info.isDir())
		{
			QDir subDir(info.filePath());
			QStringList sFoundFilesAppendList;

			QStringList sExtList = GetSupportedFileExtList();
			for(auto sExt : sExtList)
				HyGlobal::RecursiveFindFileOfExt(sExt, sFoundFilesAppendList, subDir);

			if(sFoundFilesAppendList.empty())
				subDir.removeRecursively();
		}
	}

	if(bResaveMeta)
	{
		SaveMeta(); // Recursive call here, that shouldn't ever occur twice in a row
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Now re-generate the CMakeLists.txt
	QFile file(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "src/CMakeLists.txt");
	if(!file.open(QFile::ReadOnly))
	{
		HyGuiLog("Error reading " % file.fileName() % " when generating CMakeLists.txt: " % file.errorString(), LOGTYPE_Error);
		return;
	}
	QString sContents = file.readAll();
	file.close();

	// Strip return characters because they are causing problems
	sContents.remove('\r');

	// Replace the template CMakeLists %HY_% variables
	sContents.replace("%HY_PROJECTNAME%", m_ProjectRef.GetName());
	sContents.replace("%HY_RELPROJPATH%", m_MetaDir.relativeFilePath(m_ProjectRef.GetDirPath()));
	sContents.replace("%HY_RELDATADIR%", m_MetaDir.relativeFilePath(m_ProjectRef.GetAssetsAbsPath()));
	sContents.replace("%HY_RELHARMONYDIR%", m_MetaDir.relativeFilePath(MainWindow::EngineSrcLocation()));
	sContents.replace("%HY_OUTPUTNAME%", pSourceBank->m_MetaObj["OutputName"].toString());

	// SDL or GLFW
	if(pSourceBank->m_MetaObj["UseSdl2"].toBool() == false)
		sContents.replace("%HY_USEGLFW%", "set(HYBUILD_GLFW ON)");
	else
		sContents.replace("%HY_USEGLFW%", "set(HYBUILD_GLFW OFF)");
	if(pSourceBank->m_MetaObj["UseSdl2"].toBool())
		sContents.replace("%HY_USESDL2%", "set(HYBUILD_SDL2 ON)");
	else
		sContents.replace("%HY_USESDL2%", "set(HYBUILD_SDL2 OFF)");

	// SDL AUDIO
	if(pSourceBank->m_MetaObj["UseSdlAudio"].toBool())
		sContents.replace("%HY_USESDLAUDIO%", "set(HYBUILD_SDL_AUDIO ON)");
	else
		sContents.replace("%HY_USESDLAUDIO%", "set(HYBUILD_SDL_AUDIO OFF)");

	// SDL_net
	if(pSourceBank->m_MetaObj["UseSdlNet"].toBool())
		sContents.replace("%HY_USESDLNET%", "set(HYBUILD_SDL_NET ON)");
	else
		sContents.replace("%HY_USESDLNET%", "set(HYBUILD_SDL_NET OFF)");

	// nlohmann json
	if(pSourceBank->m_MetaObj["UseNlohmannJson"].toBool())
		sContents.replace("%HY_USENLOHMANNJSON%", "set(HYBUILD_NLOHMANN_JSON ON)");
	else
		sContents.replace("%HY_USENLOHMANNJSON%", "set(HYBUILD_NLOHMANN_JSON OFF)");

	// Spine
	if(pSourceBank->m_MetaObj["UseSpine"].toBool())
		sContents.replace("%HY_USESPINE%", "set(HYBUILD_SPINE ON)");
	else
		sContents.replace("%HY_USESPINE%", "set(HYBUILD_SPINE OFF)");

	// ICU
	if(pSourceBank->m_MetaObj["UseIcu"].toBool())
		sContents.replace("%HY_USEICU%", "set(HYBUILD_ICU ON)");
	else
		sContents.replace("%HY_USEICU%", "set(HYBUILD_ICU OFF)");

	// Emscripten ccall runtime methods
	QString sEmscriptenCcall = CleanEmscriptenCcall(pSourceBank->m_MetaObj["EmscriptenCcall"].toString());
	sContents.replace("%HY_EMSCRIPTEN_CCALL%", sEmscriptenCcall);

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
	QSet<QString> uniqueSet(srcFolderList.begin(), srcFolderList.end());
	srcFolderList = uniqueSet.values(); // Remove duplicates
	srcFolderList.sort(); // The type of sort doesn't matter, just want to be consistent

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

		sDependAdd += srcDepObj["Options"].toString();
		sDependAdd += "\n";

		sDependAdd += "add_subdirectory(\"";
		sDependAdd += srcDepObj["RelPath"].toString();
		sDependAdd += "\" \"";
		sDependAdd += srcDepObj["ProjectName"].toString();
		sDependAdd += "\")\n";

		sDependAdd += "set_target_properties(\"";
		sDependAdd += srcDepObj["ProjectName"].toString();
		sDependAdd += "\" PROPERTIES FOLDER \"";
		sDependAdd += m_ProjectRef.GetName();
		sDependAdd += " Libs\")\n\n";
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
	if(!file.open(QFile::WriteOnly | QFile::Text))
	{
		HyGuiLog("OnSaveMeta Error writing to " % file.fileName() % " when generating CMakeLists.txt: " % file.errorString(), LOGTYPE_Error);
		return;
	}
	file.write(sContents.toUtf8());
	file.close();
}

/*virtual*/ void SourceModel::OnSaveData(QJsonObject &dataObjRef) /*override*/
{
	// This function doesn't make sense with m_bHasRuntimeMantifest == false
}
