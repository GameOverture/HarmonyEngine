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
#include "WidgetFontModelView.h"

#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QFileDialog>

WidgetFont::WidgetFont(ItemFont *pOwner, QWidget *parent) : QWidget(parent),
                                                            m_pItemFont(pOwner),
                                                            m_pAtlas(NULL),
                                                            m_FontMetaDir(m_pItemFont->GetItemProject()->GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirFonts)),
                                                            ui(new Ui::WidgetFont)
{
    ui->setupUi(this);
    
    ui->btnAddStage->setDefaultAction(ui->actionAddStage);
    ui->btnRemoveStage->setDefaultAction(ui->actionRemoveStage);

    m_pFontStageModel = new WidgetFontModel(this);

    ui->stagesView->setModel(m_pFontStageModel);
    ui->stagesView->resize(ui->stagesView->size());
    ui->stagesView->setItemDelegate(new WidgetFontDelegate(m_pItemFont, this));
    //QItemSelectionModel *pSelModel = ui->stagesView->selectionModel();
    //connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

    
    ui->cmbAtlasGroups->setModel(m_pItemFont->GetAtlasManager().AllocateAtlasModelView());
    
    // Populate the font list combo box
    QStringList sFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    sFontPaths.append(m_FontMetaDir.absolutePath());
    ui->cmbFontList->clear();
    ui->cmbFontList->blockSignals(true);
    for(int i = 0; i < sFontPaths.count(); ++i)
    {
        QDir fontDir(sFontPaths[i]);
        QStringList sFilterList;
        sFilterList << "*.ttf" << "*.otf";
        QFileInfoList fontFileInfoList = fontDir.entryInfoList(sFilterList);
        
        for(int j = 0; j < fontFileInfoList.count(); ++j)
        {
            ui->cmbFontList->findText(fontFileInfoList[j].fileName(), Qt::MatchFixedString);
            ui->cmbFontList->addItem(fontFileInfoList[j].fileName(), QVariant(fontFileInfoList[j].absoluteFilePath()));
        }
    }
    
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
        
        // Try to find Arial as default font
        int iArialIndex = ui->cmbFontList->findText("Arial.ttf", Qt::MatchFixedString);
        if(iArialIndex != -1)
            ui->cmbFontList->setCurrentIndex(iArialIndex);

        on_actionAddStage_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemFont->GetUndoStack()->clear();
    ui->cmbFontList->blockSignals(false);

    m_iPrevFontIndex = ui->cmbFontList->currentIndex();
}

WidgetFont::~WidgetFont()
{
    delete ui;
}

QString WidgetFont::GetFullItemName()
{
    return m_pItemFont->GetName(true);
}

void WidgetFont::GeneratePreview()
{
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

    // Get the file path to the font
    QString sFontFilePath = ui->cmbFontList->currentData().toString();

    // Try to find the perfect fit. Adjust atlas dimentions until we utilize efficient space on the smallest texture
    QSize maxAtlasDimensions = m_pItemFont->GetAtlasManager().GetAtlasDimensions(ui->cmbAtlasGroups->currentIndex());
    ui->lcdMaxTexWidth->display(maxAtlasDimensions.width());
    ui->lcdMaxTexHeight->display(maxAtlasDimensions.height());

    float fModifier = 1.0f;
    bool bDoInitialShrink = true;
    size_t iNumMissedGlyphs = 0;
    int iNumPasses = 0;
    do
    {
        iNumPasses++;

        if(bDoInitialShrink && fModifier != 1.0f)
            bDoInitialShrink = false;

        iNumMissedGlyphs = 0;

        if(m_pAtlas)
            texture_atlas_delete(m_pAtlas);
        m_pAtlas = texture_atlas_new(static_cast<size_t>(maxAtlasDimensions.width() * fModifier), static_cast<size_t>(maxAtlasDimensions.height() * fModifier), 1);

        // Get model from tableview and iterate each FontStage, assigning a new 'texture_font_t'
        WidgetFontModel *pModel = static_cast<WidgetFontModel *>(ui->stagesView->model());
        for(int i = 0; i < pModel->rowCount(); i++)
        {
            texture_font_t *pFont = texture_font_new_from_file(m_pAtlas, pModel->GetStageSize(i), sFontFilePath.toStdString().c_str());
            if(pFont == NULL)
            {
                HyGuiLog("Could not create freetype font from: " % sFontFilePath, LOGTYPE_Error);
                return;
            }

            pModel->SetTextureFont(i, pFont);
            iNumMissedGlyphs += texture_font_load_glyphs(pFont, sGlyphs.toStdString().c_str());
        }

        if(iNumMissedGlyphs && fModifier == 1.0f)
            break; // Failure

        if(iNumMissedGlyphs)
        {
            fModifier = HyClamp(fModifier + 0.05f, 0.0f, 1.0f);
        }
        else if(bDoInitialShrink)
        {
            fModifier = static_cast<float>(m_pAtlas->used) / static_cast<float>(m_pAtlas->width * m_pAtlas->height);
            //fModifier += 0.025f; // Give some additional space initially, in case packing next time isn't 100% efficient
        }
    }
    while(iNumMissedGlyphs != 0 || bDoInitialShrink);

    if(iNumMissedGlyphs)
    {
        HyGuiLog("Failed to generate font preview. Number of missed glyphs: " % QString::number(iNumMissedGlyphs), LOGTYPE_Info);
    }
    else
    {
        HyGuiLog("Generated " % sFontFilePath % " Preview", LOGTYPE_Info);
        //HyGuiLog(QString::number(m_TextureFontList.count()) % " fonts with " % QString::number(sGlyphs.size()) % " glyphs each (totaling " % QString::number(sGlyphs.size() * m_TextureFontList.count()) % ").", LOGTYPE_Normal);
        HyGuiLog("Font Atlas size: " % QString::number(m_pAtlas->width) % "x" % QString::number(m_pAtlas->height) % " (Utilizing " % QString::number(100.0*m_pAtlas->used / (float)(m_pAtlas->width*m_pAtlas->height)) % "%) (Num Passes: " % QString::number(iNumPasses) % " - Dimensions Modifier: " % QString::number(fModifier) % ")", LOGTYPE_Normal);
    }

    ui->lcdCurTexWidth->display(static_cast<int>(m_pAtlas->width));
    ui->lcdCurTexHeight->display(static_cast<int>(m_pAtlas->height));
    
    // Signals ItemFont to upload and refresh the preview texture
    m_pAtlas->id = 0;
}

texture_atlas_t *WidgetFont::GetAtlas()
{
    return m_pAtlas;
}

WidgetFontModel *WidgetFont::GetFontModel()
{
    return m_pFontStageModel;
}

void WidgetFont::on_cmbAtlasGroups_currentIndexChanged(int index)
{
    if(ui->cmbAtlasGroups->currentIndex() == index)
        return;

    QUndoCommand *pCmd = new ItemFontCmd_AtlasGroupChanged(*this, ui->cmbAtlasGroups, index);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_txtAdditionalSymbols_editingFinished()
{
    QUndoCommand *pCmd = new ItemFontCmd_LineEditSymbols(*this, ui->txtAdditionalSymbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_cmbFontList_currentIndexChanged(int index)
{
    QUndoCommand *pCmd = new ItemFontCmd_FontSelection(*this, ui->cmbFontList, m_iPrevFontIndex, index, m_FontMetaDir);
    m_pItemFont->GetUndoStack()->push(pCmd);

    m_iPrevFontIndex = index;
}

void WidgetFont::on_chk_09_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_09);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_chk_az_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_az);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_chk_AZ_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_AZ);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_chk_symbols_clicked()
{
    QUndoCommand *pCmd = new ItemFontCmd_CheckBox(*this, ui->chk_symbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionAddStage_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_AddStage(*this, ui->stagesView, static_cast<float>(ui->sbSize->value()));
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionRemoveStage_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_RemoveStage(*this, ui->stagesView, ui->stagesView->currentIndex().row());
    m_pItemFont->GetUndoStack()->push(pCmd);
}
