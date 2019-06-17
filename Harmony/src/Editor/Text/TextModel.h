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
#include "SubWidgets/TextFontManager.h"
#include "SubWidgets/TextLayersModel.h"

#include <QObject>
#include <QJsonArray>

class TextStateData : public IStateData
{
	static TextLayersModelId	sm_hHandleCount;
	TextLayersModel				m_LayersModel;

	float						m_fLeftSideNudgeAmt;
	float						m_fLineAscender;
	float						m_fLineDescender;
	float						m_fLineHeight;

public:
	TextStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~TextStateData();

	TextLayersModel &GetLayersModel();
	void GetMiscInfo(float &fLeftSideNudgeAmtOut, float &fLineAscenderOut, float &fLineDescenderOut, float &fLineHeightOut);

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextModel : public IModel
{
	Q_OBJECT

	TextFontManager				m_FontManager;
	AtlasFrame *				m_pAtlasFrame;
	QDataWidgetMapper			m_FontsWidgetMapper;

public:
	TextModel(ProjectItem &itemRef, QJsonObject fontObj);
	virtual ~TextModel();

	void MapFontComboBox(QComboBox *pComboBox);

	TextFontManager &GetFontManager();
	TextLayersModel *GetLayersModel(uint uiIndex);
	PropertiesTreeModel *GetGlyphsModel();

	virtual void OnSave() override;
	virtual QJsonObject GetStateJson(uint32 uiIndex) const override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // TEXTMODEL_H
