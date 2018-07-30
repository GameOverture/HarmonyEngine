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

class PrefabModel : public IModel
{
	QList<quint32>			m_ChecksumList;
	PropertiesTreeModel		m_PropertiesModel;

public:
	PrefabModel(ProjectItem &itemRef, QJsonValue initValue);

	PropertiesTreeModel &GetPropertiesModel();

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;

private:
	void AppendGltfValueProperty(QString sCategoryName, QString sPropertyName, const tinygltf::Value &valueRef, QString sToolTip);
};

#endif // PREFABMODEL_H
