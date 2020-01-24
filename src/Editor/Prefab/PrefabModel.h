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
	PrefabStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~PrefabStateData();

	void GetStateInfo(QJsonObject &stateObjOut);

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
	PrefabModel(ProjectItem &itemRef, ItemFileData &itemFileDataRef);

	PropertiesTreeModel &GetPropertiesModel();

	virtual QJsonObject GetStateJson(uint32 uiIndex) const override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;

private:
	void AppendGltfValueProperty(QString sCategoryName, QString sPropertyName, const tinygltf::Value &valueRef, QString sToolTip);
};

#endif // PREFABMODEL_H
