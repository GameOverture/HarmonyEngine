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

	virtual QVariant OnLinkAsset(AssetItemData *pAsset) override;
	virtual void OnUnlinkAsset(AssetItemData *pAsset) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextModel : public IModel
{
	Q_OBJECT

	TextFontManager 		m_FontManager;
	AtlasFrame *			m_pAtlasFrame;

public:
	TextModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~TextModel();

	TextFontManager &GetFontManager();
	TextLayersModel *GetLayersModel(uint uiIndex) const;
	PropertiesTreeModel *GetGlyphsModel();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(HyGuiItemType eType) const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // TEXTMODEL_H
