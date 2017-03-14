/**************************************************************************
 *	WidgetFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontWidget.h"
#include "ui_WidgetFont.h"

#include "HyGuiGlobal.h"
#include "FontData.h"
#include "FontUndoCmds.h"
#include "UndoCmds.h"
#include "AtlasesWidget.h"
#include "FontModelView.h"
#include "DlgInputName.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QMenu>
#include <QColor>

FontWidget::FontWidget(FontData *pOwner, QWidget *parent) : QWidget(parent),
                                                            m_pItemFont(pOwner),
                                                            m_bGlyphsDirty(false),
                                                            m_bFontPreviewDirty(false),
                                                            m_pCurFontState(NULL),
                                                            m_pAtlas(NULL),
                                                            m_pTrueAtlasPixelData(NULL),
                                                            m_pTrueAtlasFrame(NULL),
                                                            m_FontMetaDir(m_pItemFont->GetItemProject()->GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirFonts)),
                                                            ui(new Ui::FontWidget)
{
    ui->setupUi(this);
    
    m_bBlockGeneratePreview = true; // Avoid generating the atlas preview multiple times during initialization

    m_PrevAtlasSize.setWidth(0);
    m_PrevAtlasSize.setHeight(0);

    ui->txtPrefixAndName->setText(m_pItemFont->GetName(true));
    
    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionAddLayer);
    m_StateActionsList.push_back(ui->actionRemoveLayer);
    m_StateActionsList.push_back(ui->actionOrderLayerUpwards);
    m_StateActionsList.push_back(ui->actionOrderLayerDownwards);

    // If item's init value is defined, parse and initalize with it, otherwise make default empty font
    if(m_pItemFont->GetInitValue().type() != QJsonValue::Null)
    {
        QJsonObject fontObj = m_pItemFont->GetInitValue().toObject();

        QJsonObject availGlyphsObj = fontObj["availableGlyphs"].toObject();
        ui->chk_09->setChecked(availGlyphsObj["0-9"].toBool());
        ui->chk_AZ->setChecked(availGlyphsObj["A-Z"].toBool());
        ui->chk_az->setChecked(availGlyphsObj["a-z"].toBool());
        ui->chk_symbols->setChecked(availGlyphsObj["symbols"].toBool());
        ui->txtAdditionalSymbols->setText(availGlyphsObj["additional"].toString());
        SetGlyphsDirty();

        QList<quint32> requestList;
        requestList.append(JSONOBJ_TOINT(fontObj, "checksum"));
        QList<AtlasFrame *> pRequestedList = m_pItemFont->GetItemProject()->GetAtlasesData().RequestFrames(m_pItemFont, requestList);
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

FontWidget::~FontWidget()
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        delete m_MasterStageList[i];

    delete ui;
}

void FontWidget::OnGiveMenuActions(QMenu *pMenu)
{
    pMenu->addAction(ui->actionAddState);
    pMenu->addAction(ui->actionRemoveState);
    pMenu->addAction(ui->actionRenameState);
    pMenu->addAction(ui->actionOrderStateBackwards);
    pMenu->addAction(ui->actionOrderStateForwards);
    pMenu->addSeparator();
    pMenu->addAction(ui->actionAddLayer);
    pMenu->addAction(ui->actionRemoveLayer);
    pMenu->addAction(ui->actionOrderLayerUpwards);
    pMenu->addAction(ui->actionOrderLayerDownwards);
}

FontData *FontWidget::GetItemFont()
{
    return m_pItemFont;
}

QString FontWidget::GetFullItemName()
{
    return m_pItemFont->GetName(true);
}

QComboBox *FontWidget::GetCmbStates()
{
    return ui->cmbStates;
}

FontTableModel *FontWidget::GetCurrentFontModel()
{
    return m_pCurFontState->GetFontModel();
}

void FontWidget::SetGlyphsDirty()
{
    m_sAvailableTypefaceGlyphs.clear();
    m_sAvailableTypefaceGlyphs += ' ';
    
    // Assemble glyph set
    if(ui->chk_09->isChecked())
        m_sAvailableTypefaceGlyphs += "0123456789";
    if(ui->chk_az->isChecked())
        m_sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";
    if(ui->chk_AZ->isChecked())
        m_sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(ui->chk_symbols->isChecked())
        m_sAvailableTypefaceGlyphs += "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@";
    m_sAvailableTypefaceGlyphs += ui->txtAdditionalSymbols->text();    // May contain duplicates as stated in freetype-gl documentation
    
    QString sRegularExp = m_sAvailableTypefaceGlyphs;
    sRegularExp.replace('\\', "\\\\");
    sRegularExp.replace('^', "\\^");
    sRegularExp.replace(']', "\\]");
    sRegularExp.replace('-', "\\-");
    
    m_PreviewValidator.setRegExp(QRegExp("[" % sRegularExp % "]*"));
    ui->txtPreviewString->setValidator(&m_PreviewValidator);

    m_bGlyphsDirty = true;
}

void FontWidget::GeneratePreview(bool bStoreIntoAtlasManager /*= false*/)
{
    if(m_bBlockGeneratePreview)
        return;
    
    // 'bIsDirty' will determine whether we actually need to regenerate this typeface atlas
    bool bIsDirty = false;
    
    // Iterating through every layer of every font, match with an item in 'm_MasterStageList' and count everytime it's referenced in using 'iTmpReferenceCount'
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        m_MasterStageList[i]->iTmpReferenceCount = 0;

    for(int i = 0; i < ui->cmbStates->count(); ++i) // Iterating each font
    {
        FontWidgetState *pFontState = ui->cmbStates->itemData(i).value<FontWidgetState *>();
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
                m_MasterStageList.append(new FontStagePass(pFontState->GetFontFilePath(), pFontModel->GetLayerRenderMode(j), pFontState->GetSize(), pFontModel->GetLayerOutlineThickness(j)));
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
    QSize atlasSize = m_pItemFont->GetItemProject()->GetAtlasesData().GetAtlasDimensions();
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
        HyGuiLog("Generated " % m_pItemFont->GetName(true) % " Preview", LOGTYPE_Info);
        HyGuiLog(QString::number(m_MasterStageList.count()) % " fonts with " % QString::number(m_sAvailableTypefaceGlyphs.size()) % " glyphs each (totaling " % QString::number(m_sAvailableTypefaceGlyphs.size() * m_MasterStageList.count()) % ").", LOGTYPE_Normal);
        HyGuiLog("Font Atlas size: " % QString::number(m_pAtlas->width) % "x" % QString::number(m_pAtlas->height) % " (Utilizing " % QString::number(100.0*m_pAtlas->used / (float)(m_pAtlas->width*m_pAtlas->height)) % "%) (Num Passes: " % QString::number(iNumPasses) % " - Dimensions Modifier: " % QString::number(fAtlasSizeModifier) % ")", LOGTYPE_Normal);
    }

    ui->lcdCurTexWidth->display(static_cast<int>(m_pAtlas->width));
    ui->lcdCurTexHeight->display(static_cast<int>(m_pAtlas->height));
    ui->lcdPercentageUsed->display(static_cast<double>(100.0 * m_pAtlas->used) / static_cast<double>(m_pAtlas->width * m_pAtlas->height));
    
    // Make a fully white texture in 'pBuffer', then using the single channel from 'texture_atlas_t', overwrite the alpha channel
    delete [] m_pTrueAtlasPixelData;
    uint uiNumPixels = static_cast<uint>(m_pAtlas->width * m_pAtlas->height);
    m_pTrueAtlasPixelData = new unsigned char[uiNumPixels * 4];
    memset(m_pTrueAtlasPixelData, 0xFF, uiNumPixels * 4);
    // Overwriting alpha channel
    for(uint i = 0; i < uiNumPixels; ++i)
        m_pTrueAtlasPixelData[i*4+3] = m_pAtlas->data[i];

    if(bStoreIntoAtlasManager)
    {
        QImage fontAtlasImage(m_pTrueAtlasPixelData, static_cast<int>(m_pAtlas->width), static_cast<int>(m_pAtlas->height), QImage::Format_RGBA8888);

        if(m_pTrueAtlasFrame)
            m_pItemFont->GetItemProject()->GetAtlasesData().ReplaceFrame(m_pTrueAtlasFrame, m_pItemFont->GetName(false), fontAtlasImage, true);
        else
            m_pTrueAtlasFrame = m_pItemFont->GetItemProject()->GetAtlasesData().GenerateFrame(m_pItemFont, m_pItemFont->GetName(false), fontAtlasImage, ATLAS_Font);
    }
    
    // Signals ItemFont to upload and refresh the preview texture
    m_pAtlas->id = 0;
    m_bFontPreviewDirty = true;
}

texture_atlas_t *FontWidget::GetAtlas()
{
    return m_pAtlas;
}

unsigned char *FontWidget::GetAtlasPixelData()
{
    return m_pTrueAtlasPixelData;
}

QDir FontWidget::GetFontMetaDir()
{
    return m_FontMetaDir;
}

void FontWidget::UpdateActions()
{
    bool bGeneratePreview = false;
    
    QSize curSize = m_pItemFont->GetItemProject()->GetAtlasesData().GetAtlasDimensions();
    if(m_PrevAtlasSize.width() < curSize.width() || m_PrevAtlasSize.height() < curSize.height())
        bGeneratePreview = true;

    m_PrevAtlasSize = curSize;
    
    QString sPrevSymbols = m_sAvailableTypefaceGlyphs;
    SetGlyphsDirty();
    if(sPrevSymbols != m_sAvailableTypefaceGlyphs)
        bGeneratePreview = true;
    
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
    
    FontWidgetState *pFontState = ui->cmbStates->currentData().value<FontWidgetState *>();
    if(pFontState)
    {
        FontTableView *pTableView = pFontState->GetFontLayerView();
        bool bFrameIsSelected = pFontState->GetFontModel()->rowCount() > 0 && pTableView->currentIndex().row() >= 0;
        
        ui->actionOrderLayerUpwards->setEnabled(bFrameIsSelected && pTableView->currentIndex().row() != 0);
        ui->actionOrderLayerDownwards->setEnabled(bFrameIsSelected && pTableView->currentIndex().row() != pFontState->GetFontModel()->rowCount() - 1);
    }
    else
    {
        ui->actionOrderLayerUpwards->setEnabled(false);
        ui->actionOrderLayerDownwards->setEnabled(false);
    }
    
    if(bGeneratePreview)
        GeneratePreview();

    m_bFontPreviewDirty = true;
}

QString FontWidget::GetPreviewString()
{
    return ui->txtPreviewString->text();
}

bool FontWidget::ClearFontDirtyFlag()
{
    if(m_bFontPreviewDirty)
    {
        m_bFontPreviewDirty = false;
        return true;
    }

    return false;
}

bool FontWidget::SaveFontFilesToMetaDir()
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
            {
                HyGuiLog("Could not copy font file (" % tmpFontFile.filePath() % ") to the meta directory", LOGTYPE_Error);
                return false;
            }
        }
    }
    
    return true;
}

void FontWidget::GetSaveInfo(QJsonObject &fontObj)
{
    fontObj.insert("checksum", QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetChecksum())));

    fontObj.insert("atlasIndex", m_pTrueAtlasFrame->GetTextureIndex());

    QJsonObject availableGlyphsObj;
    availableGlyphsObj.insert("0-9", ui->chk_09->isChecked());
    availableGlyphsObj.insert("A-Z", ui->chk_AZ->isChecked());
    availableGlyphsObj.insert("a-z", ui->chk_az->isChecked());
    availableGlyphsObj.insert("symbols", ui->chk_symbols->isChecked());
    availableGlyphsObj.insert("additional", ui->txtAdditionalSymbols->text());

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
            FontStagePass *pFontStage = pFontModel->GetStageRef(j);
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

void FontWidget::on_chk_09_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox<FontWidget>(this, ui->chk_09);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_chk_az_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox<FontWidget>(this, ui->chk_az);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_chk_AZ_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox<FontWidget>(this, ui->chk_AZ);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_chk_symbols_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox<FontWidget>(this, ui->chk_symbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_txtAdditionalSymbols_editingFinished()
{
    QUndoCommand *pCmd = new UndoCmd_LineEdit<FontWidget>("Additional Symbols", this, ui->txtAdditionalSymbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_cmbStates_currentIndexChanged(int index)
{
    FontWidgetState *pFontState = ui->cmbStates->itemData(index).value<FontWidgetState *>();
    if(m_pCurFontState == pFontState)
        return;

    if(m_pCurFontState)
        m_pCurFontState->hide();

    ui->grpFontStateLayout->addWidget(pFontState);

#if _DEBUG
    int iDebugTest = ui->grpFontStateLayout->count(); // TODO: test to see if duplicates keep appending if you switch between the same two font states
#endif

    m_pCurFontState = pFontState;
    m_pCurFontState->show();

    UpdateActions();
}

void FontWidget::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddState<FontWidget, FontWidgetState>("Add Font State", this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_RemoveState<FontWidget, FontWidgetState>("Remove Font State", this, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Font State", ui->cmbStates->currentData().value<FontWidgetState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new UndoCmd_RenameState<FontWidgetState>("Rename Font State", ui->cmbStates, pDlg->GetName());
        m_pItemFont->GetUndoStack()->push(pCmd);
    }
    delete pDlg;
}

void FontWidget::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateBack<FontWidget, FontWidgetState>("Shift Font State Index <-", this, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateForward<FontWidget, FontWidgetState>("Shift Font State Index ->", this, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionAddLayer_triggered()
{
    FontWidgetState *pFontState = ui->cmbStates->currentData().value<FontWidgetState *>();
    
    QUndoCommand *pCmd = new FontUndoCmd_AddLayer(*this, ui->cmbStates, pFontState->GetCurSelectedRenderMode(), pFontState->GetSize(), pFontState->GetThickness());
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionRemoveLayer_triggered()
{
    FontWidgetState *pFontState = ui->cmbStates->currentData().value<FontWidgetState *>();
    
    int iSelectedId = pFontState->GetSelectedStageId();
    if(iSelectedId == -1)
        return;

    QUndoCommand *pCmd = new FontUndoCmd_RemoveLayer(*this, ui->cmbStates, iSelectedId);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionOrderLayerDownwards_triggered()
{
    FontWidgetState *pFontState = ui->cmbStates->currentData().value<FontWidgetState *>();
    
    QUndoCommand *pCmd = new FontUndoCmd_LayerOrder(*this, ui->cmbStates, pFontState->GetFontLayerView(), pFontState->GetFontLayerView()->currentIndex().row(), pFontState->GetFontLayerView()->currentIndex().row() + 1);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionOrderLayerUpwards_triggered()
{
    FontWidgetState *pFontState = ui->cmbStates->currentData().value<FontWidgetState *>();
    
    QUndoCommand *pCmd = new FontUndoCmd_LayerOrder(*this, ui->cmbStates, pFontState->GetFontLayerView(), pFontState->GetFontLayerView()->currentIndex().row(), pFontState->GetFontLayerView()->currentIndex().row() - 1);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void FontWidget::on_txtPreviewString_editingFinished()
{
    // TODO: Only allow typeface glyphs to be typed
    m_bFontPreviewDirty = true;
}
