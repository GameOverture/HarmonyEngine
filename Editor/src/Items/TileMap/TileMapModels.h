/**************************************************************************
 *	TileMapModels.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEMAPMODELS_H
#define TILEMAPMODELS_H

#include "IModel.h"
#include "AtlasFrame.h"

#include <QObject>
#include <QJsonArray>

class TileMapStateData : public IStateData
{
public:
	TileMapStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~TileMapStateData();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileMapModel : public IModel
{
	Q_OBJECT

public:
	TileMapModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~TileMapModel();

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from this constructor and from UndoCmd's
	//int Cmd_AddFrames(int iStateIndex, QList<AtlasFrame *> frameList);
	//void Cmd_RemoveFrames(int iStateIndex, QList<AtlasFrame *> frameList);
	//
	virtual void OnPopState(int iPoppedStateIndex) override;
	virtual bool OnPrepSave() override { return true; }
	virtual void InsertItemSpecificData(FileDataPair &itemFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual void OnItemDeleted() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TILEMAPMODELS_H
