/**************************************************************************
 *	WidgetFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFont.h"
#include "ui_WidgetFont.h"

#include "HyGlobal.h"
#include "ItemFont.h"
#include "ItemFontCmds.h"
#include "WidgetAtlasManager.h"

#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>

WidgetFont::WidgetFont(ItemFont *pOwner, QWidget *parent) : QWidget(parent),
                                                            m_pItemFont(pOwner),
                                                            m_pAtlas(NULL),
                                                            ui(new Ui::WidgetFont)
{
    ui->setupUi(this);
    
    ui->cmbAtlasGroups->setModel(m_pItemFont->GetAtlasManager().AllocateAtlasModelView());
    
    // If a .hyfnt file exists, parse and initalize with it, otherwise make default empty font
    QFile fontFile(m_pItemFont->GetAbsPath());
    if(fontFile.exists())
    {
        if(!fontFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetFont::WidgetFont() could not open ") % m_pItemFont->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument fontJsonDoc = QJsonDocument::fromJson(fontFile.readAll());
        fontFile.close();

//        QJsonArray stateArray = fontJsonDoc.array();
//        for(int i = 0; i < stateArray.size(); ++i)
//        {
//            QJsonObject stateObj = stateArray[i].toObject();

//            m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_AddState(this, m_StateActionsList, ui->cmbStates));
//            m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_RenameState(ui->cmbStates, stateObj["name"].toString()));
            
//            QJsonArray spriteFrameArray = stateObj["frames"].toArray();
//            for(int j = 0; j < spriteFrameArray.size(); ++j)
//            {
//                QJsonObject spriteFrameObj = spriteFrameArray[j].toObject();
                
//                QList<quint32> requestList;
//                requestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
//                QList<HyGuiFrame *> pRequestedList = m_pItemSprite->GetAtlasManager().RequestFrames(m_pItemSprite, requestList);

//                WidgetSpriteState *pSpriteState = GetCurSpriteState();

//                QPoint vOffset(spriteFrameObj["offsetX"].toInt() - pRequestedList[0]->GetCrop().left(),
//                               spriteFrameObj["offsetY"].toInt() - (pRequestedList[0]->GetSize().height() - pRequestedList[0]->GetCrop().bottom()));
//                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_OffsetFrame(pSpriteState->GetFrameView(), j, vOffset));
//                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_DurationFrame(pSpriteState->GetFrameView(), j, spriteFrameObj["duration"].toDouble()));
//            }
//        }
    }
    else
    {
        ui->cmbAtlasGroups->setCurrentIndex(m_pItemFont->GetAtlasManager().CurrentAtlasGroupIndex());
        
        //on_actionAddState_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemFont->GetUndoStack()->clear();
}

WidgetFont::~WidgetFont()
{
    delete ui;
}

void WidgetFont::GeneratePreview()
{
    QSize atlasDimensions = m_pItemFont->GetAtlasManager().GetAtlasDimensions(ui->cmbAtlasGroups->currentIndex());
    
    texture_atlas_delete(m_pAtlas);
    m_pAtlas = texture_atlas_new(atlasDimensions.width(), atlasDimensions.height(), 4);
    
    // Get path to the font file
    QString sFontFamily = ui->cmbFonts->currentFont().family();
    QStringList sFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    bool bFound = false;
    foreach(QString sPath, sFontPaths)
    {
        QDir fontDir(sPath);
        if(fontDir.exists() == false)
        {
            HyGuiLog("WidgetFont::GeneratePreivew() font path '" % sPath % "' was not found", LOGTYPE_Error);
            return;
        }
        
        QFileInfoList fontFileInfoList = fontDir.entryInfoList();
        foreach(QFileInfo fontFileInfo, fontFileInfoList)
        {
            if(QString::compare(sFontFamily, fontFileInfo.baseName(), Qt::CaseInsensitive) == 0)
            {
                sFontFamily = fontFileInfo.absoluteFilePath();
                bFound = true;
            }
        }
        
        if(bFound)
            break;
    }
    
    // Assemble glyph set
    QString sGlyphs;
    if(ui->chk_09->isChecked())
        sGlyphs += "0123456789";
    if(ui->chk_az->isChecked())
        sGlyphs += "abcdefghijklmnopqrstuvwxyz";
    if(ui->chk_AZ->isChecked())
        sGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(ui->chk_symbols->isChecked())
        sGlyphs += "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@";
    sGlyphs += ui->txtAdditionalSymbols->text();    // May contain duplicates as stated in freetype-gl documentation
    
    // Get all the type sizes for this font
    QList<float> sizeList;
    for(int i = 0; i < ui->cmbSizes->count(); ++i)
        sizeList.append(ui->cmbSizes->itemText(i).toFloat());
    
    // Clear old texture fonts
    for(int i = 0; i < m_TextureFontList.count(); ++i)
        texture_font_delete(m_TextureFontList[i]);
    m_TextureFontList.clear();
    
    size_t iNumMissedGlyphs = 0;
    for(int i = 0; i < sizeList.count(); ++i)
    {
        texture_font_t *pFont = texture_font_new_from_file(m_pAtlas, sizeList[i], sFontFamily.toStdString().c_str());
        
        // TODO: implement all the outline stages
        iNumMissedGlyphs += texture_font_load_glyphs(pFont, sGlyphs.toStdString().c_str());
        
        m_TextureFontList.append(pFont);
    }
    
    HyGuiLog("Generating New Font Preview", LOGTYPE_Title);
    HyGuiLog("Matched font               : " % sFontFamily, LOGTYPE_Normal);
    HyGuiLog("Number of fonts            : " % QString::number(m_TextureFontList.count()), LOGTYPE_Normal);
    HyGuiLog("Number of glyphs per font  : " % QString::number(sGlyphs.size()), LOGTYPE_Normal);
    HyGuiLog("Number of missed glyphs    : " % QString::number(iNumMissedGlyphs), LOGTYPE_Normal);
    HyGuiLog("Total number of glyphs     : " % QString::number(sGlyphs.size() * m_TextureFontList.count() - iNumMissedGlyphs) % "/" % QString::number(sGlyphs.size() * m_TextureFontList.count()), LOGTYPE_Normal);
    HyGuiLog("Texture size               : " % QString::number(m_pAtlas->width) % "x" % QString::number(m_pAtlas->height), LOGTYPE_Normal);
    HyGuiLog("Texture occupancy          : " % QString::number(100.0*m_pAtlas->used / (float)(m_pAtlas->width*m_pAtlas->height)), LOGTYPE_Normal);
}

void WidgetFont::on_cmbAtlasGroups_currentIndexChanged(int index)
{
    if(ui->cmbAtlasGroups->currentIndex() == index)
        return;

    QUndoCommand *pCmd = new ItemFontCmd_AtlasGroupChanged(ui->cmbAtlasGroups, index);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_cmbFonts_currentIndexChanged(int index)
{
    
}

void WidgetFont::on_actionAddFontSize_triggered()
{
    
}

void WidgetFont::on_actionRemoveFontSize_triggered()
{
    
}

void WidgetFont::on_chk_09_stateChanged(int arg1)
{
    
}

void WidgetFont::on_chk_az_stateChanged(int arg1)
{
    
}

void WidgetFont::on_chk_AZ_stateChanged(int arg1)
{
    
}

void WidgetFont::on_chk_symbols_stateChanged(int arg1)
{
    
}

void WidgetFont::on_txtAdditionalSymbols_editingFinished()
{
    
}
