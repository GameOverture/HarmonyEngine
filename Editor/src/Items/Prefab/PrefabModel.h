/**************************************************************************
 *	PrefabModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PREFABMODEL_H
#define PREFABMODEL_H

#include "IModel.h"
#include "PropertiesTreeModel.h"
#include "Assets/Files/HyGLTF.h"

#include <QObject>

class PrefabStateData : public IStateData
{
public:
	PrefabStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~PrefabStateData();

	void Refresh();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PrefabModel : public IModel
{
	QList<quint32>			m_ChecksumList;
	PropertiesTreeModel		m_PropertiesModel;

public:
	PrefabModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);

	PropertiesTreeModel &GetPropertiesModel();

	virtual bool OnPrepSave() override { return true; }
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;

private:
	//void AppendGltfValueProperty(QString sCategoryName, QString sPropertyName, const tinygltf::Value &valueRef, QString sToolTip);
};

#endif // PREFABMODEL_H
