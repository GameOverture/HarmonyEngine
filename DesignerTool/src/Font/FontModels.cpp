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
#include "Project.h"

#include <QJsonArray>
#include <QStandardPaths>

FontStateData::FontStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj) : IStateData(iStateIndex, modelRef, stateObj["name"].toString())
{
    m_pLayersModel = new FontStateLayersModel(&m_ModelRef);
    m_pSbMapper_Size = new SpinBoxMapper(&m_ModelRef);

    // Populate the font list combo box
    m_pCmbMapper_Fonts = new ComboBoxMapper(&m_ModelRef);
    //
    // Start with the stored meta dir fonts
    QStringList sFilterList;
    sFilterList << "*.ttf" << "*.otf";
    QFileInfoList metaFontFileInfoList = static_cast<FontModel &>(modelRef).GetMetaDir().entryInfoList(sFilterList);
    for(int i = 0; i < metaFontFileInfoList.count(); ++i)
        m_pCmbMapper_Fonts->AddItem(metaFontFileInfoList[i].fileName(), QVariant(metaFontFileInfoList[i].absoluteFilePath()));
    //
    // Now add system fonts, while not adding any duplicates found in the meta directory
    QStringList sSystemFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for(int i = 0; i < sSystemFontPaths.count(); ++i)
    {
        QDir fontDir(sSystemFontPaths[i]);

        QFileInfoList fontFileInfoList = fontDir.entryInfoList(sFilterList);
        for(int j = 0; j < fontFileInfoList.count(); ++j)
        {
            bool bIsDuplicate = false;
            for(int k = 0; k < metaFontFileInfoList.count(); ++k)
            {
                if(metaFontFileInfoList[k].fileName().compare(fontFileInfoList[j].fileName(), Qt::CaseInsensitive) == 0)
                {
                    bIsDuplicate = true;
                    break;
                }
            }

            if(bIsDuplicate == false)
                m_pCmbMapper_Fonts->AddItem(fontFileInfoList[j].fileName(), QVariant(fontFileInfoList[j].absoluteFilePath()));
        }
    }

    
    if(stateObj.empty() == false)
    {
        QJsonArray layerArray = stateObj["layers"].toArray();
        for(int j = 0; j < layerArray.size(); ++j)
        {
            QJsonObject layerObj = layerArray.at(j).toObject();
            QJsonObject typefaceObj = static_cast<FontModel &>(m_ModelRef).GetTypefaceObj(layerObj["typefaceIndex"].toInt());
            
            if(j == 0) // Only need to set the state's font and size once
            {
                m_pCmbMapper_Fonts->SetIndex(typefaceObj["font"].toString());
                m_pSbMapper_Size->SetValue(typefaceObj["size"].toDouble());
            }
            
            QColor topColor, botColor;
            topColor.setRgbF(layerObj["topR"].toDouble(), layerObj["topG"].toDouble(), layerObj["topB"].toDouble());
            botColor.setRgbF(layerObj["botR"].toDouble(), layerObj["botG"].toDouble(), layerObj["botB"].toDouble());
            
            int iLayerId = m_pLayersModel->AddNewLayer(static_cast<rendermode_t>(typefaceObj["mode"].toInt()), typefaceObj["size"].toInt(), typefaceObj["outlineThickness"].toDouble());
            m_pLayersModel->SetLayerColors(iLayerId, topColor, botColor);
        }
    }
    else
    {
        m_pCmbMapper_Fonts->SetIndex(0);
    }
}

/*virtual*/ FontStateData::~FontStateData()
{
}

FontStateLayersModel *FontStateData::GetFontLayersModel()
{
    return m_pLayersModel;
}

SpinBoxMapper *FontStateData::GetSizeMapper()
{
    return m_pSbMapper_Size;
}

ComboBoxMapper *FontStateData::GetFontMapper()
{
    return m_pCmbMapper_Fonts;
}

void FontStateData::GetStateInfo(QJsonObject &stateObjOut)
{
    stateObjOut.insert("name", GetName());
    stateObjOut.insert("lineHeight", m_pLayersModel->GetLineHeight());
    stateObjOut.insert("lineAscender", m_pLayersModel->GetLineAscender());
    stateObjOut.insert("lineDescender", m_pLayersModel->GetLineDescender());
    stateObjOut.insert("leftSideNudgeAmt", 0);//m_pFontTableModel->GetLeftSideNudgeAmt(m_sAvailableTypefaceGlyphs));

    QJsonArray layersArray;
    for(int j = 0; j < m_pLayersModel->rowCount(); ++j)
    {
        QJsonObject layerObj;

        int iIndex = 0;
        QList<FontTypeface *> masterStageList = static_cast<FontModel &>(m_ModelRef).GetMasterStageList();
        FontTypeface *pFontStage = m_pLayersModel->GetStageRef(j);
        for(; iIndex < masterStageList.count(); ++iIndex)
        {
            if(masterStageList[iIndex] == pFontStage)
                break;
        }
        layerObj.insert("typefaceIndex", iIndex);
        layerObj.insert("topR", m_pLayersModel->GetLayerTopColor(j).redF());
        layerObj.insert("topG", m_pLayersModel->GetLayerTopColor(j).greenF());
        layerObj.insert("topB", m_pLayersModel->GetLayerTopColor(j).blueF());
        layerObj.insert("botR", m_pLayersModel->GetLayerBotColor(j).redF());
        layerObj.insert("botG", m_pLayersModel->GetLayerBotColor(j).greenF());
        layerObj.insert("botB", m_pLayersModel->GetLayerBotColor(j).blueF());

        layersArray.append(layerObj);
    }
    stateObjOut.insert("layers", layersArray);
}

QString FontStateData::GetFontFilePath()
{
    return m_pCmbMapper_Fonts->GetCurrentData().toString();
}

float FontStateData::GetSize()
{
    return static_cast<float>(m_pSbMapper_Size->GetValue());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontModel::FontModel(ProjectItem &itemRef, QJsonObject fontObj) :   IModel(itemRef),
                                                                    m_FontMetaDir(m_ItemRef.GetProject().GetMetaDataAbsPath() % HyGlobal::ItemName(DIR_Fonts)),
                                                                    m_pTrueAtlasFrame(nullptr),
                                                                    m_pFtglAtlas(nullptr),
                                                                    m_pTrueAtlasPixelData(nullptr),
                                                                    m_uiTrueAtlasPixelDataSize(0)
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
        
        QList<quint32> idRequestList;
        idRequestList.append(JSONOBJ_TOINT(fontObj, "id"));

        QList<AtlasFrame *> pRequestedList = RequestFramesById(nullptr, idRequestList);
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
    for(int i = 0; i < m_MasterLayerList.count(); ++i)
        delete m_MasterLayerList[i];
}

QDir FontModel::GetMetaDir()
{
    return m_FontMetaDir;
}

CheckBoxMapper *FontModel::Get09Mapper()
{
    return m_pChkMapper_09;
}

CheckBoxMapper *FontModel::GetAZMapper()
{
    return m_pChkMapper_AZ;
}

CheckBoxMapper *FontModel::GetazMapper()
{
    return m_pChkMapper_az;
}

CheckBoxMapper *FontModel::GetSymbolsMapper()
{
    return m_pChkMapper_Symbols;
}

LineEditMapper *FontModel::GetAdditionalSymbolsMapper()
{
    return m_pTxtMapper_AdditionalSymbols;
}

QList<FontTypeface *> FontModel::GetMasterStageList()
{
    return m_MasterLayerList;
}

QJsonObject FontModel::GetTypefaceObj(int iTypefaceIndex)
{
    return m_TypefaceArray.at(iTypefaceIndex).toObject();
}

texture_atlas_t *FontModel::GetFtglAtlas()
{
    return m_pFtglAtlas;
}

AtlasFrame *FontModel::GetAtlasFrame()
{
    return m_pTrueAtlasFrame;
}

unsigned char *FontModel::GetAtlasPixelData()
{
    return m_pTrueAtlasPixelData;
}

uint FontModel::GetAtlasPixelDataSize()
{
    return m_uiTrueAtlasPixelDataSize;
}

void FontModel::GeneratePreview()
{
    uint uiAtlasGrpIndex = 0;
    if(m_pTrueAtlasFrame != nullptr)
        uiAtlasGrpIndex = m_ItemRef.GetProject().GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(m_pTrueAtlasFrame->GetAtlasGrpId());

    QSize atlasSize = m_ItemRef.GetProject().GetAtlasModel().GetAtlasDimensions(uiAtlasGrpIndex);
    QSize curAtlasSize = atlasSize;
    bool bDoInitialShrink = true;
    size_t iNumMissedGlyphs = 0;
    int iNumPasses = 0;
    do
    {
        iNumPasses++;

        if(bDoInitialShrink && curAtlasSize != atlasSize)
            bDoInitialShrink = false;

        iNumMissedGlyphs = 0;

        if(m_pFtglAtlas)
            texture_atlas_delete(m_pFtglAtlas);
        m_pFtglAtlas = texture_atlas_new(static_cast<size_t>(curAtlasSize.width()), static_cast<size_t>(curAtlasSize.height()), 1);

        for(int i = 0; i < m_MasterLayerList.count(); ++i)
        {
            texture_font_t *pFtglFont = texture_font_new_from_file(m_pFtglAtlas, m_MasterLayerList[i]->fSize, m_MasterLayerList[i]->sFontPath.toStdString().c_str());
            if(pFtglFont == NULL)
            {
                HyGuiLog("Could not create freetype font from: " % m_MasterLayerList[i]->sFontPath, LOGTYPE_Error);
                return;
            }

            m_MasterLayerList[i]->SetFont(pFtglFont);   // Applies its attributes like size, render mode, outline thickness
            iNumMissedGlyphs += texture_font_load_glyphs(pFtglFont, m_sAvailableTypefaceGlyphs.toUtf8().data());
        }

        if(iNumMissedGlyphs && curAtlasSize == atlasSize)
            break; // Failure; will be printed below

        if(iNumMissedGlyphs)
        {
            curAtlasSize.setWidth(HyClamp(curAtlasSize.width() + 25, 0, atlasSize.width()));
            curAtlasSize.setHeight(HyClamp(curAtlasSize.height() + 25, 0, atlasSize.height()));
        }
        else if(bDoInitialShrink)
        {
            double dNewSize = sqrt(static_cast<double>(m_pFtglAtlas->used)) + 25.0;
            curAtlasSize.setWidth(static_cast<int>(dNewSize));
            curAtlasSize.setHeight(static_cast<int>(dNewSize));
        }
    }
    while(bDoInitialShrink || (iNumMissedGlyphs != 0));

    if(iNumMissedGlyphs)
    {
        HyGuiLog("Failed to generate font preview. Number of missed glyphs: " % QString::number(iNumMissedGlyphs), LOGTYPE_Info);
    }
    else
    {
        HyGuiLog("Generated " % m_ItemRef.GetName(true) % " Preview", LOGTYPE_Info);
        HyGuiLog(QString::number(m_MasterLayerList.count()) % " fonts with " % QString::number(m_sAvailableTypefaceGlyphs.size()) % " glyphs each (totaling " % QString::number(m_sAvailableTypefaceGlyphs.size() * m_MasterLayerList.count()) % ").", LOGTYPE_Normal);
        HyGuiLog("Font Atlas size: " % QString::number(m_pFtglAtlas->width) % "x" % QString::number(m_pFtglAtlas->height) % " (Utilizing " % QString::number(100.0*m_pFtglAtlas->used / (float)(m_pFtglAtlas->width*m_pFtglAtlas->height)) % "%) (Num Passes: " % QString::number(iNumPasses) % ")", LOGTYPE_Normal);
    }

    // Make a fully white texture in 'pBuffer', then using the single channel from 'texture_atlas_t', overwrite the alpha channel
    delete [] m_pTrueAtlasPixelData;
    uint uiNumPixels = static_cast<uint>(m_pFtglAtlas->width * m_pFtglAtlas->height);
    m_uiTrueAtlasPixelDataSize = uiNumPixels * 4;
    m_pTrueAtlasPixelData = new unsigned char[m_uiTrueAtlasPixelDataSize];
    memset(m_pTrueAtlasPixelData, 0xFF, m_uiTrueAtlasPixelDataSize);
    // Overwriting alpha channel
    for(uint i = 0; i < uiNumPixels; ++i)
        m_pTrueAtlasPixelData[i*4+3] = m_pFtglAtlas->data[i];

    // Signals ItemFont to upload and refresh the preview texture
    m_pFtglAtlas->id = 0;
}

/*virtual*/ void FontModel::OnSave() /*override*/
{
    QImage fontAtlasImage(m_pTrueAtlasPixelData, static_cast<int>(m_pFtglAtlas->width), static_cast<int>(m_pFtglAtlas->height), QImage::Format_RGBA8888);

    if(m_pTrueAtlasFrame)
        m_ItemRef.GetProject().GetAtlasModel().ReplaceFrame(m_pTrueAtlasFrame, m_ItemRef.GetName(false), fontAtlasImage, true);
    else
        m_pTrueAtlasFrame = m_ItemRef.GetProject().GetAtlasModel().GenerateFrame(&m_ItemRef, m_ItemRef.GetName(false), fontAtlasImage, 0, ITEM_Font);
}

/*virtual*/ QJsonObject FontModel::PopStateAt(uint32 uiIndex) /*override*/
{
    QJsonObject retObj;
    static_cast<FontStateData *>(m_StateList[uiIndex])->GetStateInfo(retObj);

    return retObj;
}

/*virtual*/ QJsonValue FontModel::GetJson() const /*override*/
{
    if(m_FontMetaDir.mkpath(".") == false) {
        HyGuiLog("Could not create font meta directory", LOGTYPE_Error);
    }
    else
    {
        for(int i = 0; i < m_MasterLayerList.count(); ++i)
        {
            QFileInfo tmpFontFile(m_MasterLayerList[i]->pTextureFont->filename);
            QFileInfo metaFontFile(m_FontMetaDir.absoluteFilePath(tmpFontFile.fileName()));

            if(metaFontFile.exists() == false)
            {
                if(QFile::copy(tmpFontFile.absoluteFilePath(), metaFontFile.absoluteFilePath()) == false)
                    HyGuiLog("Could not copy font file (" % tmpFontFile.filePath() % ") to the meta directory", LOGTYPE_Error);
            }
        }
    }

    QJsonObject fontObj;
    
    fontObj.insert("checksum", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetImageChecksum())));
    fontObj.insert("id", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetId())));

    fontObj.insert("subAtlasWidth", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(m_pTrueAtlasFrame->GetSize().width()));
    fontObj.insert("subAtlasHeight", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(m_pTrueAtlasFrame->GetSize().height()));
    
    QJsonObject availableGlyphsObj;
    availableGlyphsObj.insert("0-9", m_pChkMapper_09->IsChecked());
    availableGlyphsObj.insert("A-Z", m_pChkMapper_AZ->IsChecked());
    availableGlyphsObj.insert("a-z", m_pChkMapper_az->IsChecked());
    availableGlyphsObj.insert("symbols", m_pChkMapper_Symbols->IsChecked());
    availableGlyphsObj.insert("additional", m_pTxtMapper_AdditionalSymbols->GetString());

    fontObj.insert("availableGlyphs", availableGlyphsObj);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QJsonArray typefaceArray;
    for(int i = 0; i < m_MasterLayerList.count(); ++i)
    {
        QJsonObject stageObj;
        QFileInfo fontFileInfo(m_MasterLayerList[i]->pTextureFont->filename);
        
        stageObj.insert("font", fontFileInfo.fileName());
        stageObj.insert("size", m_MasterLayerList[i]->fSize);
        stageObj.insert("mode", m_MasterLayerList[i]->eMode);
        stageObj.insert("outlineThickness", m_MasterLayerList[i]->fOutlineThickness);
        
        QJsonArray glyphsArray;
        for(int j = 0; j < m_sAvailableTypefaceGlyphs.count(); ++j)
        {
            // NOTE: Assumes LITTLE ENDIAN
            QString sSingleChar = m_sAvailableTypefaceGlyphs[j];
            texture_glyph_t *pGlyph = texture_font_get_glyph(m_MasterLayerList[i]->pTextureFont, sSingleChar.toUtf8().data());

            QJsonObject glyphInfoObj;
            if(pGlyph == nullptr)
            {
                HyGuiLog("Could not find glyph: '" % sSingleChar % "'\nPlace a breakpoint and walk into texture_font_get_glyph() below before continuing", LOGTYPE_Error);

                pGlyph = texture_font_get_glyph(m_MasterLayerList[i]->pTextureFont, sSingleChar.toUtf8().data());
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
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        QJsonObject stateObj;
        static_cast<FontStateData *>(m_StateList[i])->GetStateInfo(stateObj);
        
        stateArray.append(stateObj);
    }
    fontObj.insert("stateArray", stateArray);

    return fontObj;
}

/*virtual*/ QList<AtlasFrame *> FontModel::GetAtlasFrames() const /*override*/
{
    //QList<AtlasFrame *> retAtlasFrameList;
    //retAtlasFrameList.push_back(m_pTrueAtlasFrame);

    return QList<AtlasFrame *>();//retAtlasFrameList;
}

/*virtual*/ QStringList FontModel::GetFontUrls() const /*override*/
{
    QStringList fontUrlList;
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        FontStateData *pState = static_cast<FontStateData *>(m_StateList[i]);
        fontUrlList.append(pState->GetFontFilePath());
    }

    fontUrlList.removeDuplicates();
    return fontUrlList;
}

/*virtual*/ void FontModel::Refresh() /*override*/
{
    // Clear all references counts and re-determine which stages are being used
    for(int i = 0; i < m_MasterLayerList.count(); ++i)
        m_MasterLayerList[i]->uiReferenceCount = 0;

    // Each font state will utilize 1 or more layers from the master list. Mark those layers by incrementing their uiReferenceCount
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        FontStateData *pFontState = static_cast<FontStateData *>(m_StateList[i]);
        FontStateLayersModel *pLayersModel = pFontState->GetFontLayersModel();

        // Iterating each layer of this font
        for(int j = 0; j < pLayersModel->rowCount(); ++j)
        {
            bool bMatched = false;

            // Find the match in the master list
            for(int k = 0; k < m_MasterLayerList.count(); ++k)
            {
                if(m_MasterLayerList[k]->pTextureFont == nullptr)
                    continue;

                QFileInfo stageFontPath(m_MasterLayerList[k]->pTextureFont->filename);
                QFileInfo stateFontPath(pFontState->GetFontFilePath());

                if(QString::compare(stageFontPath.fileName(), stateFontPath.fileName(), Qt::CaseInsensitive) == 0 &&
                   m_MasterLayerList[k]->eMode == pLayersModel->GetLayerRenderMode(j) &&
                   m_MasterLayerList[k]->fSize == pFontState->GetSize() &&
                   m_MasterLayerList[k]->fOutlineThickness == pLayersModel->GetLayerOutlineThickness(j))
                {
                    // Match found, incrementing reference
                    m_MasterLayerList[k]->uiReferenceCount++;

                    pLayersModel->SetFontStageReference(j, m_MasterLayerList[k]);
                    bMatched = true;
                    break;
                }
            }

            // Could not find a match, so adding a new layer to 'm_MasterLayerList'
            if(bMatched == false)
            {
                m_MasterLayerList.append(new FontTypeface(pFontState->GetFontFilePath(), pLayersModel->GetLayerRenderMode(j), pFontState->GetSize(), pLayersModel->GetLayerOutlineThickness(j)));
                m_MasterLayerList[m_MasterLayerList.count() - 1]->uiReferenceCount = 1;

                pLayersModel->SetFontStageReference(j, m_MasterLayerList[m_MasterLayerList.count() - 1]);
            }
        }
    }

    // Delete any layer that is no longer used
    for(int i = 0; i < m_MasterLayerList.count(); ++i)
    {
        if(m_MasterLayerList[i]->uiReferenceCount == 0)
        {
            delete m_MasterLayerList[i];
            m_MasterLayerList.removeAt(i);
        }
    }
    
    // Assemble glyph set
    m_sAvailableTypefaceGlyphs.clear();
    m_sAvailableTypefaceGlyphs += ' ';

    if(m_pChkMapper_09->IsChecked())
        m_sAvailableTypefaceGlyphs += "0123456789";
    if(m_pChkMapper_az->IsChecked())
        m_sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";
    if(m_pChkMapper_AZ->IsChecked())
        m_sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(m_pChkMapper_Symbols->IsChecked())
        m_sAvailableTypefaceGlyphs += "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@";
    m_sAvailableTypefaceGlyphs += m_pTxtMapper_AdditionalSymbols->GetString();    // May contain duplicates as stated in freetype-gl documentation
    
    GeneratePreview();
}
