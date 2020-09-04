/**************************************************************************
*	AudioModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef AUDIOMODEL_H
#define AUDIOMODEL_H

#include "IModel.h"
#include "PropertiesTreeModel.h"

class AudioStateData : public IStateData
{
	
public:
	AudioStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~AudioStateData();
	
	virtual QVariant OnLinkAsset(AssetItemData *pAsset) override;
	virtual void OnUnlinkAsset(AssetItemData *pAsset) override;
};

class AudioModel : public IModel
{
	PropertiesTreeModel		m_PropertiesModel;

public:
	AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef);
	virtual ~AudioModel();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual FileDataPair GetStateFileData(uint32 uiIndex) const override;
	virtual QList<AssetItemData *> GetAssets(HyGuiItemType eType) const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // AUDIOMODEL_H
