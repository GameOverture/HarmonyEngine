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
class AudioModel : public IModel
{
public:
	AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef);
	virtual ~AudioModel();

	virtual bool OnPrepSave() override { return true; }
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual FileDataPair GetStateFileData(uint32 uiIndex) const override;
	virtual QList<AssetItemData *> GetAtlasAssets() const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // AUDIOMODEL_H
