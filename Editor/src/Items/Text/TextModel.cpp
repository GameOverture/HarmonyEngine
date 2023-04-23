/**************************************************************************
*	TextModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TextModel.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "AtlasFrame.h"
#include "AtlasModel.h"
#include "ManagerWidget.h"

TextStateData::TextStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData),
	m_LayersModel(static_cast<TextModel &>(modelRef).GetFontManager(), static_cast<TextModel &>(modelRef).GetFontManager().RegisterLayers(stateFileData.m_Data))
{
}

/*virtual*/ TextStateData::~TextStateData()
{
}

TextLayersModel &TextStateData::GetLayersModel()
{
	return m_LayersModel;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TextModel::TextModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_FontManager(m_ItemRef, itemFileDataRef.m_Meta["availableGlyphs"].toObject(), itemFileDataRef.m_Data["fontArray"].toArray()),
	m_pAtlasFrame(nullptr),
	m_bGenerateRuntimeAtlas(false)
{
	InitStates<TextStateData>(itemFileDataRef);
		
	// Find existing AtlasFrame * to assign to 'm_pAtlasFrame'
	if(itemFileDataRef.m_Meta.contains("assetUUID"))
	{
		QList<QUuid> uuidRequestList;
		uuidRequestList.append(QUuid(itemFileDataRef.m_Meta["assetUUID"].toString()));

		QList<TreeModelItemData *> dependantList = m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, uuidRequestList);
		if(dependantList.size() == 1)
			m_pAtlasFrame = static_cast<AtlasFrame *>(dependantList[0]);
		else
			HyGuiLog("More than one frame returned for a font", LOGTYPE_Error);
	}

	if(m_pAtlasFrame)
		m_FontManager.SetAtlasGroup(m_pAtlasFrame->GetBankId());
}

/*virtual*/ TextModel::~TextModel()
{
}

TextFontManager &TextModel::GetFontManager()
{
	return m_FontManager;
}

TextLayersModel *TextModel::GetLayersModel(uint uiIndex) const
{
	if(uiIndex < static_cast<uint>(m_StateList.size()))
		return &static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel();

	return nullptr;
}

PropertiesTreeModel *TextModel::GetGlyphsModel()
{
	return m_FontManager.GetGlyphsModel();
}

QStringList TextModel::GetFontUrls()
{
	QStringList fontUrlList;
	for(int i = 0; i < m_StateList.size(); ++i)
	{
		TextLayersModel *pLayersModel = GetLayersModel(i);
		
		//QString sFontPath = m_FontManager.GetFontPath(sFontName);
		
		fontUrlList.append(pLayersModel->GetFontPath());
	}
	fontUrlList.removeDuplicates();

	return fontUrlList;
}

void TextModel::SetRuntimeAtlasDirty()
{
	HyGuiLog(m_ItemRef.GetName(true) % " runtime atlas is dirty.", LOGTYPE_Normal);
	m_bGenerateRuntimeAtlas = true;
}

/*virtual*/ bool TextModel::OnPrepSave() /*override*/
{
	if(m_bGenerateRuntimeAtlas == false)
		return true;

	m_FontManager.CleanupLayers(m_StateList);

	m_FontManager.GenerateOptimizedAtlas();
	QSize atlasDimensionsOut; uint uiAtlasPixelDataSizeOut;
	unsigned char *pPixelData = m_FontManager.GetAtlasInfo(uiAtlasPixelDataSizeOut, atlasDimensionsOut);

	QImage fontAtlasImage(pPixelData, atlasDimensionsOut.width(), atlasDimensionsOut.height(), QImage::Format_RGBA8888);

	// Best determine atlas bank to save generated texture in
	m_FontManager.GetGlyphsModel()->FindPropertyValue("Atlas Info", TEXTPROP_AtlasGroup);
	quint32 uiAtlasBankIndex = 0;
	if(m_pAtlasFrame == nullptr)
	{
		if(m_ItemRef.GetProject().GetAtlasWidget())
			uiAtlasBankIndex = m_ItemRef.GetProject().GetAtlasModel().GetBankIndexFromBankId(m_ItemRef.GetProject().GetAtlasWidget()->GetSelectedBankId());
	}
	else
		uiAtlasBankIndex = m_ItemRef.GetProject().GetAtlasModel().GetBankIndexFromBankId(m_pAtlasFrame->GetBankId());

	// Ensure newly generated font sub-atlas will fit in atlas bank dimensions
	QSize atlasDimensions = m_ItemRef.GetProject().GetAtlasModel().GetAtlasDimensions(uiAtlasBankIndex);
	quint32 uiNewAtlasBankId = m_ItemRef.GetProject().GetAtlasModel().GetBankIdFromBankIndex(uiAtlasBankIndex);
	if(m_ItemRef.GetProject().GetAtlasModel().IsImageValid(fontAtlasImage, uiNewAtlasBankId) == false)
	{
		HyGuiLog("Cannot generate text sub-atlas for " % m_ItemRef.GetName(true) % " because it will not fit in atlas group '" % QString::number(uiNewAtlasBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
		return false;
	}

	// Apply newly generated font sub-atlas
	if(m_pAtlasFrame)
	{
		if(m_ItemRef.GetProject().GetAtlasModel().ReplaceFrame(m_pAtlasFrame, m_ItemRef.GetName(false), fontAtlasImage) == false)
		{
			HyGuiLog("Cannot ReplaceFrame text sub-atlas for " % m_ItemRef.GetName(true), LOGTYPE_Error);
			return false;
		}
	}
	else
		m_pAtlasFrame = m_ItemRef.GetProject().GetAtlasModel().GenerateFrame(&m_ItemRef, m_ItemRef.GetName(false), fontAtlasImage, uiAtlasBankIndex, ITEM_Text);

	if(m_pAtlasFrame)
		m_FontManager.SetAtlasGroup(m_pAtlasFrame->GetBankId());
	else
		return false;

	// Copy font files into the font meta directory
	QDir metaDir(m_ItemRef.GetProject().GetMetaAbsPath() % HYMETA_FontsDir);
	if(metaDir.mkpath("."))
	{
		for(int i = 0; i < m_StateList.size(); ++i)
		{
			TextLayersModel *pLayersModel = GetLayersModel(i);

			QFileInfo tmpFontFile(pLayersModel->GetFontPath());
			QFileInfo metaFontFile(metaDir.absoluteFilePath(tmpFontFile.fileName()));

			if(metaFontFile.exists() == false)
			{
				if(QFile::copy(tmpFontFile.absoluteFilePath(), metaFontFile.absoluteFilePath()) == false)
					HyGuiLog("Could not copy font file (" % tmpFontFile.filePath() % ") to the meta directory", LOGTYPE_Error);
			}
		}
	}
	else
		HyGuiLog("Could not create font meta directory", LOGTYPE_Error);

	m_ItemRef.GetProject().GetAtlasModel().FlushRepack();
	m_bGenerateRuntimeAtlas = false;
	return true;
}

/*virtual*/ void TextModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	const PropertiesTreeModel *pGlyphsModel = m_FontManager.GetGlyphsModel();

	QJsonObject availableGlyphsObj;
	QVariant propValue;
	propValue = pGlyphsModel->FindPropertyValue("Uses Glyphs", TEXTPROP_09);
	availableGlyphsObj.insert("0-9", static_cast<bool>(propValue.toInt() == Qt::Checked));
	propValue = pGlyphsModel->FindPropertyValue("Uses Glyphs", TEXTPROP_az);
	availableGlyphsObj.insert("a-z", static_cast<bool>(propValue.toInt() == Qt::Checked));
	propValue = pGlyphsModel->FindPropertyValue("Uses Glyphs", TEXTPROP_AZ);
	availableGlyphsObj.insert("A-Z", static_cast<bool>(propValue.toInt() == Qt::Checked));
	propValue = pGlyphsModel->FindPropertyValue("Uses Glyphs", TEXTPROP_Symbols);
	availableGlyphsObj.insert("symbols", static_cast<bool>(propValue.toInt() == Qt::Checked));
	propValue = pGlyphsModel->FindPropertyValue("Uses Glyphs", TEXTPROP_AdditionalSyms);
	availableGlyphsObj.insert("additional", propValue.toString());
	
	itemSpecificFileDataOut.m_Meta["availableGlyphs"] = availableGlyphsObj;

	itemSpecificFileDataOut.m_Data.insert("checksum", m_pAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pAtlasFrame->GetChecksum())));
	itemSpecificFileDataOut.m_Meta.insert("assetUUID", m_pAtlasFrame == nullptr ? 0 : m_pAtlasFrame->GetUuid().toString());

	//uint uiAtlasPixelDataSizeOut;
	//unsigned char *pPixelData = m_FontManager.GetAtlasInfo(uiAtlasPixelDataSizeOut, atlasDimensionsOut);
	QSize atlasDimensionsOut;
	itemSpecificFileDataOut.m_Data.insert("subAtlasWidth", m_pAtlasFrame == nullptr ? atlasDimensionsOut.width() : QJsonValue(m_pAtlasFrame->GetSize().width()));
	itemSpecificFileDataOut.m_Data.insert("subAtlasHeight", m_pAtlasFrame == nullptr ? atlasDimensionsOut.height() : QJsonValue(m_pAtlasFrame->GetSize().height()));

	QJsonArray fontArray = m_FontManager.GetFontArray();
	itemSpecificFileDataOut.m_Data.insert("fontArray", fontArray);
}

/*virtual*/ void TextModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	float fLeftSideNudgeAmt, fLineAscender, fLineDescender, fLineHeight;
	GetLayersModel(uiIndex)->GetMiscInfo(fLeftSideNudgeAmt, fLineAscender, fLineDescender, fLineHeight);
	stateFileDataOut.m_Data.insert("leftSideNudgeAmt", fLeftSideNudgeAmt);
	stateFileDataOut.m_Data.insert("lineAscender", fLineAscender);
	stateFileDataOut.m_Data.insert("lineDescender", fLineDescender);
	stateFileDataOut.m_Data.insert("lineHeight", fLineHeight);

	QJsonArray layersArray = static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel().GetLayersArray();
	stateFileDataOut.m_Data.insert("layers", layersArray);
}
