/**************************************************************************
*	SpineModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINEMODEL_H
#define SPINEMODEL_H

#include "IModel.h"

class SpineStateData : public IStateData
{
public:
	SpineStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~SpineStateData();

	virtual QVariant OnLinkAsset(AssetItemData *pAsset) override;
	virtual void OnUnlinkAsset(AssetItemData *pAsset) override;
};
////////////////////////////////////////////////////////////////////////////////////////
class SpineModel : public IModel
{
	Q_OBJECT

	QList<AtlasFrame *>		m_AtlasFrameList;

public:
	SpineModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~SpineModel();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(HyGuiItemType eType) const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // SPINEMODEL_H
