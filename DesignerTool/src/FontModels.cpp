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

FontStateData::FontStateData(IModel &modelRef, QJsonObject stateObj) : IStateData(modelRef, stateObj["name"].toString())
{
    m_pFontTableModel = new FontTableModel(&m_ModelRef);
    m_pSbMapper_Size = new DoubleSpinBoxMapper(&m_ModelRef);

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
            
            int iLayerId = m_pFontTableModel->AddNewLayer(static_cast<rendermode_t>(typefaceObj["mode"].toInt()), typefaceObj["size"].toDouble(), typefaceObj["outlineThickness"].toDouble());
            m_pFontTableModel->SetLayerColors(iLayerId, topColor, botColor);
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

FontTableModel *FontStateData::GetFontModel()
{
    return m_pFontTableModel;
}

DoubleSpinBoxMapper *FontStateData::GetSizeMapper()
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
    stateObjOut.insert("lineHeight", m_pFontTableModel->GetLineHeight());
    stateObjOut.insert("lineAscender", m_pFontTableModel->GetLineAscender());
    stateObjOut.insert("lineDescender", m_pFontTableModel->GetLineDescender());
    stateObjOut.insert("leftSideNudgeAmt", 0);//m_pFontTableModel->GetLeftSideNudgeAmt(m_sAvailableTypefaceGlyphs));

    QJsonArray layersArray;
    for(int j = 0; j < m_pFontTableModel->rowCount(); ++j)
    {
        QJsonObject layerObj;

        int iIndex = 0;
        QList<FontTypeface *> masterStageList = static_cast<FontModel &>(m_ModelRef).GetMasterStageList();
        FontTypeface *pFontStage = m_pFontTableModel->GetStageRef(j);
        for(; iIndex < masterStageList.count(); ++iIndex)
        {
            if(masterStageList[iIndex] == pFontStage)
                break;
        }
        layerObj.insert("typefaceIndex", iIndex);
        layerObj.insert("topR", m_pFontTableModel->GetLayerTopColor(j).redF());
        layerObj.insert("topG", m_pFontTableModel->GetLayerTopColor(j).greenF());
        layerObj.insert("topB", m_pFontTableModel->GetLayerTopColor(j).blueF());
        layerObj.insert("botR", m_pFontTableModel->GetLayerBotColor(j).redF());
        layerObj.insert("botG", m_pFontTableModel->GetLayerBotColor(j).greenF());
        layerObj.insert("botB", m_pFontTableModel->GetLayerBotColor(j).blueF());

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

FontModel::FontModel(ProjectItem *pItem, QJsonObject fontObj) : IModel(pItem),
                                                                m_FontMetaDir(m_pItem->GetProject().GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirFonts)),
                                                                m_pTrueAtlasFrame(nullptr),
                                                                m_bGlyphsDirty(false),
                                                                m_bFontPreviewDirty(false),
                                                                m_pAtlas(nullptr),
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

    SetGlyphsDirty();
}

/*virtual*/ FontModel::~FontModel()
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        delete m_MasterStageList[i];
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
    return m_MasterStageList;
}

QJsonObject FontModel::GetTypefaceObj(int iTypefaceIndex)
{
    return m_TypefaceArray.at(iTypefaceIndex).toObject();
}

texture_atlas_t *FontModel::GetAtlas()
{
    return m_pAtlas;
}

unsigned char *FontModel::GetAtlasPixelData()
{
    return m_pTrueAtlasPixelData;
}

uint FontModel::GetAtlasPixelDataSize()
{
    return m_uiTrueAtlasPixelDataSize;
}

void FontModel::GeneratePreview(bool bStoreIntoAtlasManager /*= false*/)
{
    // 'bIsDirty' will determine whether we actually need to regenerate this typeface atlas
    bool bIsDirty = false;

    // Iterating through every layer of every font, match with an item in 'm_MasterStageList' and count everytime it's referenced in using 'iTmpReferenceCount'
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        m_MasterStageList[i]->iTmpReferenceCount = 0;

    for(int i = 0; i < m_StateList.size(); ++i) // Iterating each font state
    {
        FontStateData *pFontState = static_cast<FontStateData *>(m_StateList[i]);
        FontTableModel *pFontModel = pFontState->GetFontModel();

        // Iterating each layer of this font
        for(int j = 0; j < pFontModel->rowCount(); ++j)
        {
            bool bMatched = false;

            for(int k = 0; k < m_MasterStageList.count(); ++k)
            {
                if(m_MasterStageList[k]->pTextureFont == NULL)
                    continue;

                QFileInfo stageFontPath(m_MasterStageList[k]->pTextureFont->filename);
                QFileInfo stateFontPath(pFontState->GetFontFilePath());

                if(QString::compare(stageFontPath.fileName(), stateFontPath.fileName(), Qt::CaseInsensitive) == 0 &&
                   m_MasterStageList[k]->eMode == pFontModel->GetLayerRenderMode(j) &&
                   m_MasterStageList[k]->fSize == pFontState->GetSize() &&
                   m_MasterStageList[k]->fOutlineThickness == pFontModel->GetLayerOutlineThickness(j))
                {
                    // Match found, incrementing reference
                    m_MasterStageList[k]->iTmpReferenceCount++;

                    pFontModel->SetFontStageReference(j, m_MasterStageList[k]);

                    bMatched = true;
                }
            }

            // Could not find a match, so adding a new FontStagePass to 'm_MasterStageList'
            if(bMatched == false)
            {
                m_MasterStageList.append(new FontTypeface(pFontState->GetFontFilePath(), pFontModel->GetLayerRenderMode(j), pFontState->GetSize(), pFontModel->GetLayerOutlineThickness(j)));
                m_MasterStageList[m_MasterStageList.count() - 1]->iTmpReferenceCount = 1;

                pFontModel->SetFontStageReference(j, m_MasterStageList[m_MasterStageList.count() - 1]);

                bIsDirty = true;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        if(m_MasterStageList[i]->iTmpReferenceCount != m_MasterStageList[i]->iReferenceCount)
            bIsDirty = true;

        if(m_MasterStageList[i]->iTmpReferenceCount == 0)
        {
            delete m_MasterStageList[i];
            m_MasterStageList.removeAt(i);

            bIsDirty = true;
        }
    }

    if(m_bGlyphsDirty)
    {
        bIsDirty = true;
        m_bGlyphsDirty = false;
    }

    if(bIsDirty == false && bStoreIntoAtlasManager == false)  // If 'bStoreIntoAtlasManager' is true, always try generating the best fit atlas
        return;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // 'bIsDirty' == true so reset ref count and generate atlas
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        m_MasterStageList[i]->iReferenceCount = m_MasterStageList[i]->iTmpReferenceCount;

    // if 'bFindBestFit' == true, adjust atlas dimentions until we utilize efficient space on the smallest texture
    QSize atlasSize = m_pItem->GetProject().GetAtlasModel().GetAtlasDimensions(TEMP_FONT_ATLAS_INDEX);
    float fAtlasSizeModifier = 1.0f;
    bool bDoInitialShrink = true;
    size_t iNumMissedGlyphs = 0;
    int iNumPasses = 0;
    do
    {
        iNumPasses++;

        if(bDoInitialShrink && fAtlasSizeModifier != 1.0f)
            bDoInitialShrink = false;

        iNumMissedGlyphs = 0;

        if(m_pAtlas)
            texture_atlas_delete(m_pAtlas);
        m_pAtlas = texture_atlas_new(static_cast<size_t>(atlasSize.width() * fAtlasSizeModifier), static_cast<size_t>(atlasSize.height() * fAtlasSizeModifier), 1);

        for(int i = 0; i < m_MasterStageList.count(); ++i)
        {
            texture_font_t *pFont = texture_font_new_from_file(m_pAtlas, m_MasterStageList[i]->fSize, m_MasterStageList[i]->sFontPath.toStdString().c_str());
            if(pFont == NULL)
            {
                HyGuiLog("Could not create freetype font from: " % m_MasterStageList[i]->sFontPath, LOGTYPE_Error);
                return;
            }

            m_MasterStageList[i]->SetFont(pFont);
            iNumMissedGlyphs += texture_font_load_glyphs(pFont, m_sAvailableTypefaceGlyphs.toUtf8().data());
        }

        if(iNumMissedGlyphs && fAtlasSizeModifier == 1.0f)
            break; // Failure

        if(iNumMissedGlyphs)
            fAtlasSizeModifier = HyClamp(fAtlasSizeModifier + 0.05f, 0.0f, 1.0f);
        else if(bDoInitialShrink)
            fAtlasSizeModifier = static_cast<float>(m_pAtlas->used) / static_cast<float>(m_pAtlas->width * m_pAtlas->height);
    }
    while(bStoreIntoAtlasManager && (iNumMissedGlyphs != 0 || bDoInitialShrink));

    if(iNumMissedGlyphs)
    {
        HyGuiLog("Failed to generate font preview. Number of missed glyphs: " % QString::number(iNumMissedGlyphs), LOGTYPE_Info);
    }
    else if(bStoreIntoAtlasManager)
    {
        HyGuiLog("Generated " % m_pItem->GetName(true) % " Preview", LOGTYPE_Info);
        HyGuiLog(QString::number(m_MasterStageList.count()) % " fonts with " % QString::number(m_sAvailableTypefaceGlyphs.size()) % " glyphs each (totaling " % QString::number(m_sAvailableTypefaceGlyphs.size() * m_MasterStageList.count()) % ").", LOGTYPE_Normal);
        HyGuiLog("Font Atlas size: " % QString::number(m_pAtlas->width) % "x" % QString::number(m_pAtlas->height) % " (Utilizing " % QString::number(100.0*m_pAtlas->used / (float)(m_pAtlas->width*m_pAtlas->height)) % "%) (Num Passes: " % QString::number(iNumPasses) % " - Dimensions Modifier: " % QString::number(fAtlasSizeModifier) % ")", LOGTYPE_Normal);
    }

//    ui->lcdCurTexWidth->display(static_cast<int>(m_pAtlas->width));
//    ui->lcdCurTexHeight->display(static_cast<int>(m_pAtlas->height));
//    ui->lcdPercentageUsed->display(static_cast<double>(100.0 * m_pAtlas->used) / static_cast<double>(m_pAtlas->width * m_pAtlas->height));

    // Make a fully white texture in 'pBuffer', then using the single channel from 'texture_atlas_t', overwrite the alpha channel
    delete [] m_pTrueAtlasPixelData;
    uint uiNumPixels = static_cast<uint>(m_pAtlas->width * m_pAtlas->height);
    m_uiTrueAtlasPixelDataSize = uiNumPixels * 4;
    m_pTrueAtlasPixelData = new unsigned char[m_uiTrueAtlasPixelDataSize];
    memset(m_pTrueAtlasPixelData, 0xFF, m_uiTrueAtlasPixelDataSize);
    // Overwriting alpha channel
    for(uint i = 0; i < uiNumPixels; ++i)
        m_pTrueAtlasPixelData[i*4+3] = m_pAtlas->data[i];

    if(bStoreIntoAtlasManager)
    {
        QImage fontAtlasImage(m_pTrueAtlasPixelData, static_cast<int>(m_pAtlas->width), static_cast<int>(m_pAtlas->height), QImage::Format_RGBA8888);

        if(m_pTrueAtlasFrame)
            m_pItem->GetProject().GetAtlasModel().ReplaceFrame(m_pTrueAtlasFrame, m_pItem->GetName(false), fontAtlasImage, true);
        else
            m_pTrueAtlasFrame = m_pItem->GetProject().GetAtlasModel().GenerateFrame(m_pItem, m_pItem->GetName(false), fontAtlasImage, TEMP_FONT_ATLAS_INDEX, ATLAS_Font);
    }

    // Signals ItemFont to upload and refresh the preview texture
    m_pAtlas->id = 0;
    m_bFontPreviewDirty = true;
}

void FontModel::SetGlyphsDirty()
{
    m_sAvailableTypefaceGlyphs.clear();
    m_sAvailableTypefaceGlyphs += ' ';

    // Assemble glyph set
    if(m_pChkMapper_09->IsChecked())
        m_sAvailableTypefaceGlyphs += "0123456789";
    if(m_pChkMapper_az->IsChecked())
        m_sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";
    if(m_pChkMapper_AZ->IsChecked())
        m_sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(m_pChkMapper_Symbols->IsChecked())
        m_sAvailableTypefaceGlyphs += "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@";
    m_sAvailableTypefaceGlyphs += m_pTxtMapper_AdditionalSymbols->GetString();    // May contain duplicates as stated in freetype-gl documentation

//    QString sRegularExp = m_sAvailableTypefaceGlyphs;
//    sRegularExp.replace('\\', "\\\\");
//    sRegularExp.replace('^', "\\^");
//    sRegularExp.replace(']', "\\]");
//    sRegularExp.replace('-', "\\-");

//    m_PreviewValidator.setRegExp(QRegExp("[" % sRegularExp % "]*"));
//    ui->txtPreviewString->setValidator(&m_PreviewValidator);

    m_bGlyphsDirty = true;
}

bool FontModel::ClearFontDirtyFlag()
{
    if(m_bFontPreviewDirty)
    {
        m_bFontPreviewDirty = false;
        return true;
    }

    return false;
}


/*virtual*/ QJsonObject FontModel::PopStateAt(uint32 uiIndex)
{
    QJsonObject retObj;
    static_cast<FontStateData *>(m_StateList[uiIndex])->GetStateInfo(retObj);

    return retObj;
}

/*virtual*/ QJsonValue FontModel::GetSaveInfo()
{
    if(m_FontMetaDir.mkpath(".") == false)
    {
        HyGuiLog("Could not create font meta directory", LOGTYPE_Error);
        return false;
    }

    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        QFileInfo tmpFontFile(m_MasterStageList[i]->pTextureFont->filename);
        QFileInfo metaFontFile(m_FontMetaDir.absoluteFilePath(tmpFontFile.fileName()));

        if(metaFontFile.exists() == false)
        {
            if(QFile::copy(tmpFontFile.absoluteFilePath(), metaFontFile.absoluteFilePath()) == false)
                HyGuiLog("Could not copy font file (" % tmpFontFile.filePath() % ") to the meta directory", LOGTYPE_Error);
        }
    }


    GeneratePreview(true);


    QJsonObject fontObj;
    
    fontObj.insert("checksum", QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetImageChecksum())));
    fontObj.insert("id", QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetId())));

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
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        FontStateData *pState = static_cast<FontStateData *>(m_StateList[i]);
        FontTableModel *pFontModel = pState->GetFontModel();
        
        QJsonObject stateObj;
        static_cast<FontStateData *>(m_StateList[i])->GetStateInfo(stateObj);
        
        stateArray.append(stateObj);
    }
    fontObj.insert("stateArray", stateArray);

    return fontObj;
}

/*virtual*/ void FontModel::Refresh()
{
}
