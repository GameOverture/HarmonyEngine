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
#include "ProjectItemData.h"
#include "GlobalWidgetMappers.h"
#include "ProjectItemMimeData.h"
#include "SubWidgets/TextFontManager.h"
#include "SubWidgets/TextLayersModel.h"

#include <QObject>
#include <QJsonArray>

#define TEXTPROP_AtlasGroup "Atlas Group"
#define TEXTPROP_Dimensions "Dimensions"
#define TEXTPROP_UsedPercent "Used Percent"
#define TEXTPROP_09 "0-9"
#define TEXTPROP_AZ "A-Z"
#define TEXTPROP_az "a-z"
#define TEXTPROP_Symbols "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@"
#define TEXTPROP_AdditionalSyms "Additional glyphs"

#define TEXTGLYPHS_SubStateId -5 // Some negative value that indicates the Glyphs property changed (gets sent to TextWidget::FocusState)

class TextStateData : public IStateData
{
	TextLayersModel			m_LayersModel;

public:
	TextStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~TextStateData();

	TextLayersModel &GetLayersModel();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextModel : public IModel
{
	Q_OBJECT

	TextFontManager 		m_FontManager;
	AtlasFrame *			m_pAtlasFrame;

	bool					m_bGenerateRuntimeAtlas;

public:
	TextModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~TextModel();

	TextFontManager &GetFontManager();
	TextLayersModel *GetLayersModel(uint uiIndex) const;
	PropertiesTreeModel *GetGlyphsModel();
	QStringList GetFontUrls();

	void SetRuntimeAtlasDirty();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
};

#endif // TEXTMODEL_H
