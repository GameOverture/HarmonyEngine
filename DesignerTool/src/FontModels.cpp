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
        
        
        
        QList<AtlasFrame *> pRequestedList = RequestFrames(, QList<quint32> requestList);
        
        QList<AtlasFrame *> pRequestedList = m_pItemFont->GetProject().GetAtlasesData().RequestFrames(m_pItemFont, requestList);
        m_pTrueAtlasFrame = pRequestedList[0];
        
//        for(int i = 0; i < ui->cmbAtlasGroups->count(); ++i)
//        {
//            if(m_pItemFont->GetAtlasManager().GetAtlasIdFromIndex(i) == m_pTrueAtlasFrame->GetAtlasIndex())
//            {
//                ui->cmbAtlasGroups->setCurrentIndex(i);
//                m_iPrevAtlasCmbIndex = i;
//                break;
//            }
//        }
        
        QJsonArray stateArray = fontObj["stateArray"].toArray();
        QJsonArray typefaceArray = fontObj["typefaceArray"].toArray();
        
        for(int i = 0; i < stateArray.size(); ++i)
        {
            QJsonObject stateObj = stateArray.at(i).toObject();
            
            m_pItemFont->GetUndoStack()->push(new UndoCmd_AddState<FontWidget, FontWidgetState>("Add Font State", this, m_StateActionsList, ui->cmbStates));
            m_pItemFont->GetUndoStack()->push(new UndoCmd_RenameState<FontWidgetState>("Rename Font State", ui->cmbStates, stateObj["name"].toString()));
            
            QJsonArray layerArray = stateObj["layers"].toArray();
            for(int j = 0; j < layerArray.size(); ++j)
            {
                QJsonObject layerObj = layerArray.at(j).toObject();
                QJsonObject typefaceObj = typefaceArray.at(layerObj["typefaceIndex"].toInt()).toObject();
                FontTableModel *pCurFontModel = m_pCurFontState->GetFontModel();
                
                if(j == 0) // Only need to set the state's font and size once
                {
                    m_pCurFontState->SetSelectedFont(typefaceObj["font"].toString());
                    m_pCurFontState->SetSize(typefaceObj["size"].toDouble());
                }
                
                m_pItemFont->GetUndoStack()->push(new FontUndoCmd_AddLayer(*this,
                                                                                 ui->cmbStates,
                                                                                 static_cast<rendermode_t>(typefaceObj["mode"].toInt()),
                                                                                 typefaceObj["size"].toDouble(),
                                                                                 typefaceObj["outlineThickness"].toDouble()));
                
                QColor topColor, botColor;
                topColor.setRgbF(layerObj["topR"].toDouble(), layerObj["topG"].toDouble(), layerObj["topB"].toDouble());
                botColor.setRgbF(layerObj["botR"].toDouble(), layerObj["botG"].toDouble(), layerObj["botB"].toDouble());
                m_pItemFont->GetUndoStack()->push(new FontUndoCmd_LayerColors(*this,
                                                                                    ui->cmbStates,
                                                                                    pCurFontModel->GetLayerId(j),
                                                                                    pCurFontModel->GetLayerTopColor(j),
                                                                                    pCurFontModel->GetLayerBotColor(j),
                                                                                    topColor,
                                                                                    botColor));
            }
        }
    }
    else
    {
        SetGlyphsDirty();
        
        on_actionAddState_triggered();
        on_actionAddLayer_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemFont->GetUndoStack()->clear();

    UpdateActions();
    
    m_bBlockGeneratePreview = false;
    GeneratePreview();
}

/*virtual*/ FontModel::~FontModel()
{
}

/*virtual*/ int FontModel::AppendState(QJsonObject stateObj)
{
}

/*virtual*/ void FontModel::InsertState(int iStateIndex, QJsonObject stateObj)
{
}

/*virtual*/ QJsonObject FontModel::PopStateAt(uint32 uiIndex)
{
}

/*virtual*/ QJsonValue FontModel::GetSaveInfo()
{
}
