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
#include "AtlasWidget.h"

TextStateData::TextStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData),
	m_LayersModel(static_cast<TextModel &>(modelRef).GetFontManager(), static_cast<TextModel &>(modelRef).GetFontManager().RegisterLayers(stateFileData.m_Data), &modelRef)
{
}

/*virtual*/ TextStateData::~TextStateData()
{
}

TextLayersModel &TextStateData::GetLayersModel()
{
	return m_LayersModel;
}

/*virtual*/ int TextStateData::AddFrame(AtlasFrame *pFrame) /*override*/
{
	return 0;
}

/*virtual*/ void TextStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TextModel::TextModel(ProjectItem &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_FontManager(m_ItemRef, itemFileDataRef.m_Meta["availableGlyphs"].toObject(), itemFileDataRef.m_Data["fontArray"].toArray()),
	m_pAtlasFrame(nullptr)
{
	InitStates<TextStateData>(itemFileDataRef);
		
	// Find existing AtlasFrame * to assign to 'm_pAtlasFrame'
	if(itemFileDataRef.m_Meta.contains("frameUUID"))
	{
		int iAffectedFrameIndex = 0;
		QList<QUuid> uuidRequestList;
		uuidRequestList.append(QUuid(itemFileDataRef.m_Meta["frameUUID"].toString()));
		QList<AtlasFrame *> pRequestedList = RequestFramesByUuid(nullptr, uuidRequestList, iAffectedFrameIndex);
		if(pRequestedList.size() == 1)
			m_pAtlasFrame = pRequestedList[0];
		else
			HyGuiLog("More than one frame returned for a font", LOGTYPE_Error);
	}

	if(m_pAtlasFrame)
		m_FontManager.SetAtlasGroup(m_pAtlasFrame->GetAtlasGrpId());
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
	if(uiIndex < m_StateList.size())
		return &static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel();

	return nullptr;
}

PropertiesTreeModel *TextModel::GetGlyphsModel()
{
	return m_FontManager.GetGlyphsModel();
}

/*virtual*/ bool TextModel::OnSave() /*override*/
{
	m_FontManager.GenerateOptimizedAtlas();
	QSize atlasDimensionsOut; uint uiAtlasPixelDataSizeOut;
	unsigned char *pPixelData = m_FontManager.GetAtlasInfo(uiAtlasPixelDataSizeOut, atlasDimensionsOut);

	QImage fontAtlasImage(pPixelData, atlasDimensionsOut.width(), atlasDimensionsOut.height(), QImage::Format_RGBA8888);

	// If an atlas group has not been established (first time saving) then choose whichever 
	m_FontManager.GetGlyphsModel()->FindPropertyValue("Atlas Info", TEXTPROP_AtlasGroup);
	quint32 uiAtlasGrpIndex = 0;
	if(m_pAtlasFrame == nullptr)
	{
		if(m_ItemRef.GetProject().GetAtlasWidget())
			uiAtlasGrpIndex = m_ItemRef.GetProject().GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(m_ItemRef.GetProject().GetAtlasWidget()->GetSelectedAtlasGrpId());
	}
	else
		uiAtlasGrpIndex = m_ItemRef.GetProject().GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(m_pAtlasFrame->GetAtlasGrpId());

	// Ensure newly generated font sub-atlas will fit in atlas group dimensions
	QSize atlasDimensions = m_ItemRef.GetProject().GetAtlasModel().GetAtlasDimensions(uiAtlasGrpIndex);
	QSize atlasMargins = m_ItemRef.GetProject().GetAtlasModel().GetAtlasMargins(uiAtlasGrpIndex);
	if(fontAtlasImage.width() >= (atlasDimensions.width() - atlasMargins.width()) ||
		fontAtlasImage.height() >= (atlasDimensions.height() - atlasMargins.height()))
	{
		HyGuiLog("Cannot generate text sub-atlas for " % m_ItemRef.GetName(true) % " because it will not fit in atlas group '" % QString::number(m_ItemRef.GetProject().GetAtlasWidget()->GetSelectedAtlasGrpId()) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
		return false;
	}

	// Apply newly generated font sub-atlas
	if(m_pAtlasFrame)
		m_ItemRef.GetProject().GetAtlasModel().ReplaceFrame(m_pAtlasFrame, m_ItemRef.GetName(false), fontAtlasImage, true);
	else
		m_pAtlasFrame = m_ItemRef.GetProject().GetAtlasModel().GenerateFrame(&m_ItemRef, m_ItemRef.GetName(false), fontAtlasImage, uiAtlasGrpIndex, ITEM_Text);

	if(m_pAtlasFrame)
		m_FontManager.SetAtlasGroup(m_pAtlasFrame->GetAtlasGrpId());

	// Copy font files into the font meta directory
	QDir metaDir(m_ItemRef.GetProject().GetMetaDataAbsPath() % HYMETA_FontsDir);
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

	itemSpecificFileDataOut.m_Data.insert("checksum", m_pAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pAtlasFrame->GetImageChecksum())));
	itemSpecificFileDataOut.m_Meta.insert("frameUUID", m_pAtlasFrame == nullptr ? 0 : m_pAtlasFrame->GetId().toString());

	uint uiAtlasPixelDataSizeOut; QSize atlasDimensionsOut;
	unsigned char *pPixelData = m_FontManager.GetAtlasInfo(uiAtlasPixelDataSizeOut, atlasDimensionsOut);
	itemSpecificFileDataOut.m_Data.insert("subAtlasWidth", m_pAtlasFrame == nullptr ? atlasDimensionsOut.width() : QJsonValue(m_pAtlasFrame->GetSize().width()));
	itemSpecificFileDataOut.m_Data.insert("subAtlasHeight", m_pAtlasFrame == nullptr ? atlasDimensionsOut.height() : QJsonValue(m_pAtlasFrame->GetSize().height()));

	QJsonArray fontArray = m_FontManager.GetFontArray();
	itemSpecificFileDataOut.m_Data.insert("fontArray", fontArray);
}

/*virtual*/ FileDataPair TextModel::GetStateFileData(uint32 uiIndex) const /*override*/
{
	FileDataPair stateFileData;
	stateFileData.m_Meta.insert("name", m_StateList[uiIndex]->GetName());

	float fLeftSideNudgeAmt, fLineAscender, fLineDescender, fLineHeight;
	GetLayersModel(uiIndex)->GetMiscInfo(fLeftSideNudgeAmt, fLineAscender, fLineDescender, fLineHeight);
	stateFileData.m_Data.insert("leftSideNudgeAmt", fLeftSideNudgeAmt);
	stateFileData.m_Data.insert("lineAscender", fLineAscender);
	stateFileData.m_Data.insert("lineDescender", fLineDescender);
	stateFileData.m_Data.insert("lineHeight", fLineHeight);

	QJsonArray layersArray = static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel().GetLayersArray();
	stateFileData.m_Data.insert("layers", layersArray);

	return stateFileData;
}

/*virtual*/ QList<AtlasFrame *> TextModel::GetAtlasFrames() const /*override*/
{
	QList<AtlasFrame *> retAtlasFrameList;
	if(m_pAtlasFrame)
		retAtlasFrameList.push_back(m_pAtlasFrame);

	return retAtlasFrameList;
}

/*virtual*/ QStringList TextModel::GetFontUrls() const /*override*/
{
	QStringList fontUrlList;
	//for(int i = 0; i < m_StateList.size(); ++i)
	//{
	//	FontStateData *pState = static_cast<FontStateData *>(m_StateList[i]);
	//	fontUrlList.append(pState->GetFontFilePath());
	//}

	//fontUrlList.removeDuplicates();
	return fontUrlList;
}
