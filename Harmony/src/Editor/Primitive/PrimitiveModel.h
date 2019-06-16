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
	PrimitiveModel(ProjectItem &itemRef);
	virtual ~PrimitiveModel();

	virtual void OnSave() override;
	virtual QJsonObject GetStateJson(uint32 uiIndex) const override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // PRIMITIVEMODEL_H
