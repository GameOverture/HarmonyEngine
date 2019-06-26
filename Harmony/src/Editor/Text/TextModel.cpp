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

TextStateData::TextStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj) :
	IStateData(iStateIndex, modelRef, stateObj["name"].toString()),
	m_LayersModel(static_cast<TextModel &>(modelRef).GetFontManager(), static_cast<TextModel &>(modelRef).GetFontManager().RegisterLayers(stateObj["layers"].toArray()), &modelRef),
	m_fLeftSideNudgeAmt(stateObj["leftSideNudgeAmt"].toDouble()),
	m_fLineAscender(stateObj["lineAscender"].toDouble()),
	m_fLineDescender(stateObj["lineDescender"].toDouble()),
	m_fLineHeight(stateObj["lineHeight"].toDouble())
{
}

/*virtual*/ TextStateData::~TextStateData()
{
}

TextLayersModel &TextStateData::GetLayersModel()
{
	return m_LayersModel;
}

void TextStateData::GetMiscInfo(float &fLeftSideNudgeAmtOut, float &fLineAscenderOut, float &fLineDescenderOut, float &fLineHeightOut)
{
	fLeftSideNudgeAmtOut = m_fLeftSideNudgeAmt;
	fLineAscenderOut = m_fLineAscender;
	fLineDescenderOut = m_fLineDescender;
	fLineHeightOut = m_fLineHeight;
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
TextModel::TextModel(ProjectItem &itemRef, QJsonObject textObj) :
	IModel(itemRef),
	m_pAtlasFrame(nullptr)
{
	// If item's init value is defined, parse and initialize with it, otherwise make default empty font
	if(textObj.empty() == false)
	{
		m_pFontManager = new TextFontManager(m_ItemRef, textObj["availableGlyphs"].toObject(), textObj["typefaceArray"].toArray());

		QJsonArray stateArray = textObj["stateArray"].toArray();
		for(int i = 0; i < stateArray.size(); ++i)
		{
			QJsonObject stateObj = stateArray[i].toObject();
			AppendState<TextStateData>(stateObj);
		}

		// Find existing AtlasFrame * to assign to 'm_pAtlasFrame'
		int iAffectedFrameIndex = 0;
		QList<quint32> idRequestList;
		idRequestList.append(JSONOBJ_TOINT(textObj, "id"));
		QList<AtlasFrame *> pRequestedList = RequestFramesById(nullptr, idRequestList, iAffectedFrameIndex);
		if(pRequestedList.size() == 1)
			m_pAtlasFrame = pRequestedList[0];
		else
			HyGuiLog("More than one frame returned for a font", LOGTYPE_Error);
	}
	else
	{
		m_pFontManager = new TextFontManager(m_ItemRef, QJsonObject(), QJsonArray());
		AppendState<TextStateData>(QJsonObject());
	}
}

/*virtual*/ TextModel::~TextModel()
{
	delete m_pFontManager;
}

TextFontManager &TextModel::GetFontManager()
{
	return *m_pFontManager;
}

TextLayersModel *TextModel::GetLayersModel(uint uiIndex)
{
	if(uiIndex < m_StateList.size())
		return &static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel();

	return nullptr;
}

PropertiesTreeModel *TextModel::GetGlyphsModel()
{
	return m_pFontManager->GetGlyphsModel();
}

/*virtual*/ void TextModel::OnSave() /*override*/
{
	QDir metaDir(m_ItemRef.GetProject().GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_Text, true));
	if(metaDir.mkpath(".") == false)
		HyGuiLog("Could not create font meta directory", LOGTYPE_Error);
	else
	{
		// Copy font files into the font meta directory
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

	uint uiAtlasPixelDataSizeOut;
	QSize atlasDimensionsOut;
	unsigned char *pPixelData = m_pFontManager->GetAtlasInfo(uiAtlasPixelDataSizeOut, atlasDimensionsOut);

	QImage fontAtlasImage(pPixelData, atlasDimensionsOut.width(), atlasDimensionsOut.height(), QImage::Format_RGBA8888);

	if(m_pAtlasFrame)
		m_ItemRef.GetProject().GetAtlasModel().ReplaceFrame(m_pAtlasFrame, m_ItemRef.GetName(false), fontAtlasImage, true);
	else
	{
		quint32 uiAtlasGrpIndex = 0;
		if(m_ItemRef.GetProject().GetAtlasWidget())
			uiAtlasGrpIndex = m_ItemRef.GetProject().GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(m_ItemRef.GetProject().GetAtlasWidget()->GetSelectedAtlasGrpId());

		m_pAtlasFrame = m_ItemRef.GetProject().GetAtlasModel().GenerateFrame(&m_ItemRef, m_ItemRef.GetName(false), fontAtlasImage, uiAtlasGrpIndex, ITEM_Text);
	}
}

/*virtual*/ QJsonObject TextModel::GetStateJson(uint32 uiIndex) const /*override*/
{
	float fLeftSideNudgeAmt, fLineAscender, fLineDescender, fLineHeight;
	static_cast<TextStateData *>(m_StateList[uiIndex])->GetMiscInfo(fLeftSideNudgeAmt, fLineAscender, fLineDescender, fLineHeight);

	QJsonObject stateObjOut;
	stateObjOut.insert("name", m_StateList[uiIndex]->GetName());
	stateObjOut.insert("leftSideNudgeAmt", fLeftSideNudgeAmt);//layersModelRef.GetLeftSideNudgeAmt(m_sAvailableTypefaceGlyphs));
	stateObjOut.insert("lineAscender", fLineAscender);
	stateObjOut.insert("lineDescender", fLineDescender);
	stateObjOut.insert("lineHeight", fLineHeight);

	QJsonArray layersArray = static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel().GetLayersArray();
	stateObjOut.insert("layers", layersArray);

	return stateObjOut;
}

/*virtual*/ QJsonValue TextModel::GetJson() const /*override*/
{
	QJsonObject textObj;

	const PropertiesTreeModel *pGlyphsModel = m_pFontManager->GetGlyphsModel();

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
	textObj.insert("availableGlyphs", availableGlyphsObj);

	textObj.insert("checksum", m_pAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pAtlasFrame->GetImageChecksum())));
	textObj.insert("id", m_pAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pAtlasFrame->GetId())));

	QJsonArray stateArray;
	for(int i = 0; i < GetNumStates(); ++i)
		stateArray.append(GetStateJson(i));
	textObj.insert("stateArray", stateArray);

	uint uiAtlasPixelDataSizeOut; QSize atlasDimensionsOut;
	unsigned char *pPixelData = m_pFontManager->GetAtlasInfo(uiAtlasPixelDataSizeOut, atlasDimensionsOut);

	textObj.insert("subAtlasWidth", m_pAtlasFrame == nullptr ? atlasDimensionsOut.width() : QJsonValue(m_pAtlasFrame->GetSize().width()));
	textObj.insert("subAtlasHeight", m_pAtlasFrame == nullptr ? atlasDimensionsOut.height() : QJsonValue(m_pAtlasFrame->GetSize().height()));

	QJsonArray fontArray = m_pFontManager->GetFontArray();
	textObj.insert("typefaceArray", fontArray);

	return textObj;
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
