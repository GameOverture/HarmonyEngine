/**************************************************************************
 *	PrefabModel.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PREFABMODEL_H
#define PREFABMODEL_H

#include "IModel.h"

#include <QObject>

class PrefabModel : public IModel
{
public:
	PrefabModel(ProjectItem &itemRef, QJsonValue initValue);

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // PREFABMODEL_H
