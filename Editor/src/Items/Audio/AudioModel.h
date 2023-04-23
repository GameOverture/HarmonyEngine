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
#include "AudioPlayListModel.h"
#include "PropertiesTreeModel.h"

class AudioStateData : public IStateData
{
	AudioPlayListModel		m_PlayListModel;
	PropertiesTreeModel		m_PropertiesModel;

public:
	AudioStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~AudioStateData();

	AudioPlayListModel &GetPlayListModel();
	PropertiesTreeModel &GetPropertiesModel();

	QJsonArray GenPlayListArray() const;
	
	int Cmd_AddAudioAsset(AudioAsset *pAsset);
	void Cmd_RemoveAudioAsset(AudioAsset *pAsset);
};

class AudioModel : public IModel
{
public:
	AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef);
	virtual ~AudioModel();

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from this constructor and from UndoCmd's
	int Cmd_AddAudioAssets(int iStateIndex, QList<AudioAsset *> assetList);
	void Cmd_RemoveAudioAssets(int iStateIndex, QList<AudioAsset *> assetList);

	AudioPlayListModel &GetPlayListModel(uint uiStateIndex);
	PropertiesTreeModel &GetPropertiesModel(uint uiStateIndex);

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
};

#endif // AUDIOMODEL_H
