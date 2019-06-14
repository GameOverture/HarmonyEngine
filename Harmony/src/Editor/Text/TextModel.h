/**************************************************************************
*	TextModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include "IModel.h"
#include "ProjectItem.h"
#include "EntityTreeModel.h"
#include "GlobalWidgetMappers.h"
#include "ProjectItemMimeData.h"
#include "PropertiesTreeModel.h"
#include "SubWidgets/TextLayersModel.h"

#include <QObject>
#include <QJsonArray>

class TextStateData : public IStateData
{
	TextLayersModel			m_LayersModel;

public:
	TextStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~TextStateData();

	TextLayersModel &GetLayersModel();

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextModel : public IModel
{
	Q_OBJECT

	PropertiesTreeModel		m_GlyphsModel;
	AtlasFrame *			m_pAtlasFrame;
	QJsonArray				m_TypefaceArray;

public:
	TextModel(ProjectItem &itemRef, QJsonObject fontObj);
	virtual ~TextModel();

	TextLayersModel *GetLayersModel(uint uiIndex);
	PropertiesTreeModel *GetGlyphsModel();

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // TEXTMODEL_H
