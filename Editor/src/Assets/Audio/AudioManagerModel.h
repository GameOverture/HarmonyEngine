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
#include "AudioAsset.h"

#include <QAudioFormat>
#include <QSoundEffect>

class AudioGroupsModel : public QAbstractListModel
{
	Q_OBJECT

	QList<QPair<QString, quint32>>			m_GroupList;			// QPair<Name, ID>

public:
	AudioGroupsModel();
	virtual ~AudioGroupsModel();

	void AddGroup(QString sName, quint32 uiId);

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

	QAudioFormat							m_DesiredRawFormat;
	AudioGroupsModel						m_AudioGroupsModel;
	quint32									m_uiNextGroupId;

public:
	AudioManagerModel(Project &projRef);
	virtual ~AudioManagerModel();

	AudioGroupsModel &GetGroupsModel();

	bool IsWaveValid(QString sFilePath, WaveHeader &wavHeaderOut);
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() const override;

	int GetGroupIndexFromGroupId(quint32 uiGroupId) const;
	quint32 GetGroupIdFromGroupIndex(uint uiGroupIndex) const;

protected:
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

private Q_SLOTS:
	void OnLoadUpdate(QString sMsg, int iPercComplete);
	void OnRepackFinished();

private:
	AudioAsset *ImportSound(QString sFilePath, quint32 uiBankIndex, ItemType eType, QUuid uuid, const WaveHeader &wavHeaderRef);
};

#endif // AudioManagerModel_H
