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

class AudioManagerModel : public IManagerModel
{
	Q_OBJECT

	QAudioFormat	m_DesiredRawFormat;

public:
	AudioManagerModel(Project &projRef);
	virtual ~AudioManagerModel();
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() override;

	// Draw occur when the mouse hovers over the manager widget. ManagerWidget holds the ptr to IManagerDraw, but IManagerModel init/updates the actual concrete IDraw object
	virtual void OnAllocateDraw(IManagerDraw *&pDrawOut) override;

protected:
	void Repack(uint uiBankIndex, QSet<AudioAsset *> newAssetSet);

	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;
	virtual QList<AssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<QUuid> correspondingUuidList) override; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QList<AssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset
	virtual QJsonObject GetSaveJson() override;

private Q_SLOTS:
	void OnLoadUpdate(QString sMsg, int iPercComplete);
	void OnRepackFinished();

private:
	AudioAsset *ImportSound(QString sFilePath, quint32 uiBankIndex, HyGuiItemType eType, QUuid uuid);
};

#endif // AudioManagerModel_H
