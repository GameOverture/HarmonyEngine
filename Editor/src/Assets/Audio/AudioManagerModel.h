/**************************************************************************
 *	AudioManagerModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AudioManagerModel_H
#define AudioManagerModel_H

#include "IManagerModel.h"
#include "SoundClip.h"

#include <QAudioFormat>
#include <QSoundEffect>

class AudioCategoriesModel : public QAbstractListModel
{
	Q_OBJECT

	QList<QPair<QString, quint32>>			m_CategoryList;			// QPair<Name, ID>

public:
	AudioCategoriesModel();
	virtual ~AudioCategoriesModel();

	void AddCategory(QString sName, quint32 uiId);

	QString GetName(uint uiIndex) const;
	quint32 GetId(uint uiIndex) const;
	int GetIndex(quint32 uiId) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

class AudioManagerModel : public IManagerModel
{
	Q_OBJECT

	friend class AudioImportThread;

	QAudioFormat							m_DesiredRawFormat;
	AudioCategoriesModel					m_AudioCategoriesModel;
	quint32									m_uiNextCategoryId;

public:
	AudioManagerModel(Project &projRef);
	virtual ~AudioManagerModel();

	AudioCategoriesModel &GetCategoriesModel();

	bool IsWaveValid(QString sFilePath, WaveHeader &wavHeaderOut);
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() const override;

	virtual void UpdateInspectorScene(const QList<IAssetItemData *> &selectedAssetsList) override;

	int GetCategoryIndexFromCategoryId(quint32 uiCategoryId) const;
	quint32 GetGroupIdFromGroupIndex(uint uiCategoryIndex) const;
	QString GetCategoryName(quint32 uiCategoryId) const;

protected:
	virtual void OnInit() override;
	virtual void OnCreateNewBank(QJsonObject &newMetaBankObjRef) override;

	virtual IAssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;

	virtual void OnGenerateAssetsDlg(const QModelIndex &indexDestination) override;
	virtual bool OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) override;
	virtual bool OnUpdateAssets(QList<IAssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset

	virtual void OnFlushRepack() override;

	virtual void OnSaveMeta(QJsonObject &metaObjRef) override;
	virtual QJsonObject GetSaveJson() override;

private:
	SoundClip *ImportSound(QString sFilePath, quint32 uiBankIndex, QUuid uuid, const WaveHeader &wavHeaderRef);
};

#endif // AudioManagerModel_H
