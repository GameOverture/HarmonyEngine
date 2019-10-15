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
	AudioModel(ProjectItem &itemRef);
	virtual ~AudioModel();

	virtual QJsonObject GetStateJson(uint32 uiIndex) const override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // AUDIOMODEL_H
