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
	QString					m_sFontName;
	uint					m_uiSize;

	TextLayersModel			m_LayersModel;

public:
	TextStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~TextStateData();

	void SetInfo(QString sFontName, uint uiSize);

	TextLayersModel &GetLayersModel();

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextModel : public IModel
{
	Q_OBJECT

	struct Font
	{
		QString				m_sName;
		uint				m_uiSize;
		rendermode_t		m_eMode;

		bool operator==(const Font &rightRef) { return m_sName.compare(rightRef.m_sName, Qt::CaseInsensitive) == 0 && m_uiSize == rightRef.m_uiSize && m_eMode == rightRef.m_eMode; }
	};
	QList<Font>				m_FontList;

	PropertiesTreeModel		m_GlyphsModel;
	QDataWidgetMapper		m_FontsWidgetMapper;

	AtlasFrame *			m_pAtlasFrame;

public:
	TextModel(ProjectItem &itemRef, QJsonObject fontObj);
	virtual ~TextModel();

	void MapFontComboBox(QComboBox *pComboBox);

	TextLayersModel *GetLayersModel(uint uiIndex);
	PropertiesTreeModel *GetGlyphsModel();

	virtual void OnSave() override;
	virtual QJsonObject GetStateJson(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // TEXTMODEL_H
