/**************************************************************************
 *	SourceModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SOURCEMODEL_H
#define SOURCEMODEL_H

#include "IManagerModel.h"

#include "AtlasFrame.h"

class SourceModel : public IManagerModel
{
	Q_OBJECT

	enum TemplateFileType
	{
		TEMPLATE_Main = 0,
		TEMPLATE_Pch,
		TEMPLATE_MainClassCpp,
		TEMPLATE_MainClassH,
		TEMPLATE_ClassCpp,
		TEMPLATE_ClassH,

		TEMPLATE_Entity
	};

public:
	SourceModel(Project &projRef);
	virtual ~SourceModel();
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() const override;

protected:
	quint32 ComputeFileChecksum(QString sFilterPath, QString sFileName) const;
	QString GenerateSrcFile(TemplateFileType eTemplate, QModelIndex destIndex, QString sClassName, QString sFileName, QString sBaseClass, bool bEntityBaseClass);
	void GatherSourceFiles(QStringList &srcFilePathListOut, QList<quint32> &checksumListOut) const;
	QString CleanEmscriptenCcall(QString sUserValue) const;

	virtual void OnInit() override;
	virtual void OnCreateNewBank(QJsonObject &newMetaBankObjRef) override;
	virtual IAssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;

	virtual void OnGenerateAssetsDlg(const QModelIndex &indexDestination) override;
	virtual QList<IAssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, ItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) override; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) override;
	virtual bool OnUpdateAssets(QList<IAssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset

	virtual void OnFlushRepack() override;

	virtual void OnSaveMeta(QJsonObject &metaObjRef) override;
	virtual QJsonObject GetSaveJson() override;
};

#endif // SOURCEMODEL_H
