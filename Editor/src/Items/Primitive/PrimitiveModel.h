/**************************************************************************
 *	PrimitiveModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PRIMITIVEMODEL_H
#define PRIMITIVEMODEL_H

#include "IModel.h"
class PrimitiveModel : public IModel
{
public:
	PrimitiveModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef);
	virtual ~PrimitiveModel();

	virtual bool OnPrepSave() override { return true; }
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
};

#endif // PRIMITIVEMODEL_H
