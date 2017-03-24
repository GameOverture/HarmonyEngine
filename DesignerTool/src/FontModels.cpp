/**************************************************************************
 *	FontModels.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontModels.h"

#include <QJsonArray>

FontStateData::FontStateData(IModel &modelRef, QJsonObject stateObj) : IStateData(modelRef, stateObj["name"].toString())
{
    m_pFontTableModel = new FontTableModel(&m_ModelRef);
    m_pTxtMapper_Font = new LineEditMapper(&m_ModelRef);
    m_pSbMapper_Size = new DoubleSpinBoxMapper(&m_ModelRef);
    
    if(stateObj.empty() == false)
    {
        QJsonArray layerArray = stateObj["layers"].toArray();
        for(int j = 0; j < layerArray.size(); ++j)
        {
            QJsonObject layerObj = layerArray.at(j).toObject();
            QJsonObject typefaceObj = static_cast<FontModel &>(m_ModelRef).GetTypefaceObj(layerObj["typefaceIndex"].toInt());
            
            if(j == 0) // Only need to set the state's font and size once
            {
                m_pTxtMapper_Font->SetString(typefaceObj["font"].toString());
                m_pSbMapper_Size->SetValue(typefaceObj["size"].toDouble());
            }
            
            QColor topColor, botColor;
            topColor.setRgbF(layerObj["topR"].toDouble(), layerObj["topG"].toDouble(), layerObj["topB"].toDouble());
            botColor.setRgbF(layerObj["botR"].toDouble(), layerObj["botG"].toDouble(), layerObj["botB"].toDouble());
            
            int iLayerId = m_pFontTableModel->AddNewLayer(static_cast<rendermode_t>(typefaceObj["mode"].toInt()), typefaceObj["size"].toDouble(), typefaceObj["outlineThickness"].toDouble());
            m_pFontTableModel->SetLayerColors(iLayerId, topColor, botColor);
        }
    }
    else
    {
    }
}

FontModel::FontModel(ProjectItem *pItem, QJsonObject fontObj) : IModel(pItem)
{
    m_pChkMapper_09 = new CheckBoxMapper(this);
    m_pChkMapper_AZ = new CheckBoxMapper(this);
    m_pChkMapper_az = new CheckBoxMapper(this);
    m_pChkMapper_Symbols = new CheckBoxMapper(this);
    m_pTxtMapper_AdditionalSymbols = new LineEditMapper(this);
    
    // If item's init value is defined, parse and initalize with it, otherwise make default empty font
    if(fontObj.empty() == false)
    {
        QJsonObject availGlyphsObj = fontObj["availableGlyphs"].toObject();
        
        m_pChkMapper_09->SetChecked(availGlyphsObj["0-9"].toBool());
        m_pChkMapper_AZ->SetChecked(availGlyphsObj["A-Z"].toBool());
        m_pChkMapper_az->SetChecked(availGlyphsObj["a-z"].toBool());
        m_pChkMapper_Symbols->SetChecked(availGlyphsObj["symbols"].toBool());
        m_pTxtMapper_AdditionalSymbols->SetString(availGlyphsObj["additional"].toString());
        

        QList<quint32> requestList;
        requestList.append(JSONOBJ_TOINT(fontObj, "checksum"));

        QList<AtlasFrame *> pRequestedList = RequestFrames(nullptr, requestList);
        m_pTrueAtlasFrame = pRequestedList[0];

        m_TypefaceArray = fontObj["typefaceArray"].toArray();
        
        QJsonArray stateArray = fontObj["stateArray"].toArray();
        for(int i = 0; i < stateArray.size(); ++i)
            AppendState<FontStateData>(stateArray[i].toObject());
    }
    else
    {
        AppendState<FontStateData>(QJsonObject());
    }
}

/*virtual*/ FontModel::~FontModel()
{
}

QJsonObject FontModel::GetTypefaceObj(int iTypefaceIndex)
{
    return m_TypefaceArray.at(iTypefaceIndex).toObject();
}

/*virtual*/ QJsonObject FontModel::PopStateAt(uint32 uiIndex)
{
}

/*virtual*/ QJsonValue FontModel::GetSaveInfo()
{
    QJsonObject fontObj;
    
    fontObj.insert("checksum", QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetChecksum())));

    QJsonObject availableGlyphsObj;
    availableGlyphsObj.insert("0-9", m_pChkMapper_09->IsChecked());
    availableGlyphsObj.insert("A-Z", m_pChkMapper_AZ->IsChecked());
    availableGlyphsObj.insert("a-z", m_pChkMapper_az->IsChecked());
    availableGlyphsObj.insert("symbols", m_pChkMapper_Symbols->IsChecked());
    availableGlyphsObj.insert("additional", m_pTxtMapper_AdditionalSymbols->GetString());

    fontObj.insert("availableGlyphs", availableGlyphsObj);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QJsonArray typefaceArray;
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        QJsonObject stageObj;
        QFileInfo fontFileInfo(m_MasterStageList[i]->pTextureFont->filename);
        
        stageObj.insert("font", fontFileInfo.fileName());
        stageObj.insert("size", m_MasterStageList[i]->fSize);
        stageObj.insert("mode", m_MasterStageList[i]->eMode);
        stageObj.insert("outlineThickness", m_MasterStageList[i]->fOutlineThickness);
        
        QJsonArray glyphsArray;
        for(int j = 0; j < m_sAvailableTypefaceGlyphs.count(); ++j)
        {
            // NOTE: Assumes LITTLE ENDIAN
            QString sSingleChar = m_sAvailableTypefaceGlyphs[j];
            texture_glyph_t *pGlyph = texture_font_get_glyph(m_MasterStageList[i]->pTextureFont, sSingleChar.toUtf8().data());

            QJsonObject glyphInfoObj;
            if(pGlyph == nullptr)
            {
                HyGuiLog("Could not find glyph: '" % sSingleChar % "'\nPlace a breakpoint and walk into texture_font_get_glyph() below before continuing", LOGTYPE_Error);

                pGlyph = texture_font_get_glyph(m_MasterStageList[i]->pTextureFont, sSingleChar.toUtf8().data());
            }
            else
            {
                glyphInfoObj.insert("code", QJsonValue(static_cast<qint64>(pGlyph->codepoint)));
                glyphInfoObj.insert("advance_x", pGlyph->advance_x);
                glyphInfoObj.insert("advance_y", pGlyph->advance_y);
                glyphInfoObj.insert("width", static_cast<int>(pGlyph->width));
                glyphInfoObj.insert("height", static_cast<int>(pGlyph->height));
                glyphInfoObj.insert("offset_x", pGlyph->offset_x);
                glyphInfoObj.insert("offset_y", pGlyph->offset_y);
                glyphInfoObj.insert("left", pGlyph->s0);
                glyphInfoObj.insert("top", pGlyph->t0);
                glyphInfoObj.insert("right", pGlyph->s1);
                glyphInfoObj.insert("bottom", pGlyph->t1);
            }
            
            QJsonObject kerningInfoObj;
            for(int k = 0; k < m_sAvailableTypefaceGlyphs.count(); ++k)
            {
                char cTmpChar = m_sAvailableTypefaceGlyphs.toStdString().c_str()[k];
                float fKerningAmt = texture_glyph_get_kerning(pGlyph, &cTmpChar);
                
                if(fKerningAmt != 0.0f)
                    kerningInfoObj.insert(QString(m_sAvailableTypefaceGlyphs[k]), fKerningAmt);
            }
            glyphInfoObj.insert("kerning", kerningInfoObj);
            
            glyphsArray.append(glyphInfoObj);
        }
        stageObj.insert("glyphs", glyphsArray);
        
        typefaceArray.append(QJsonValue(stageObj));
    }
    fontObj.insert("typefaceArray", typefaceArray);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QJsonArray stateArray;
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        FontWidgetState *pState = ui->cmbStates->itemData(i).value<FontWidgetState *>();
        FontTableModel *pFontModel = pState->GetFontModel();
        
        QJsonObject stateObj;
        stateObj.insert("name", pState->GetName());
        stateObj.insert("lineHeight", pFontModel->GetLineHeight());
        stateObj.insert("lineAscender", pFontModel->GetLineAscender());
        stateObj.insert("lineDescender", pFontModel->GetLineDescender());
        stateObj.insert("leftSideNudgeAmt", pFontModel->GetLeftSideNudgeAmt(m_sAvailableTypefaceGlyphs));
        
        QJsonArray layersArray;
        for(int j = 0; j < pFontModel->rowCount(); ++j)
        {
            QJsonObject layerObj;
            
            int iIndex = 0;
            FontTypeface *pFontStage = pFontModel->GetStageRef(j);
            for(; iIndex < m_MasterStageList.count(); ++iIndex)
            {
                if(m_MasterStageList[iIndex] == pFontStage)
                    break;
            }
            layerObj.insert("typefaceIndex", iIndex);
            layerObj.insert("topR", pFontModel->GetLayerTopColor(j).redF());
            layerObj.insert("topG", pFontModel->GetLayerTopColor(j).greenF());
            layerObj.insert("topB", pFontModel->GetLayerTopColor(j).blueF());
            layerObj.insert("botR", pFontModel->GetLayerBotColor(j).redF());
            layerObj.insert("botG", pFontModel->GetLayerBotColor(j).greenF());
            layerObj.insert("botB", pFontModel->GetLayerBotColor(j).blueF());
            
            layersArray.append(layerObj);
        }
        stateObj.insert("layers", layersArray);
        
        stateArray.append(stateObj);
    }
    fontObj.insert("stateArray", stateArray);
}
