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

#include <QObject>

class PrefabStateData : public IStateData
{
public:
	PrefabStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~PrefabStateData();

	void Refresh();

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PrefabModel : public IModel
{
	QList<quint32>			m_ChecksumList;
	PropertiesTreeModel		m_PropertiesModel;

public:
	PrefabModel(ProjectItem &itemRef, const FileDataPair &itemFileDataRef);

	PropertiesTreeModel &GetPropertiesModel();

	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual FileDataPair GetStateFileData(uint32 uiIndex) const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;

private:
	void AppendGltfValueProperty(QString sCategoryName, QString sPropertyName, const tinygltf::Value &valueRef, QString sToolTip);
};

#endif // PREFABMODEL_H
