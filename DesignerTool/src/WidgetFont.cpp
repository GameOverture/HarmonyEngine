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

#include <QDir>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMenu>

int WidgetFont::sm_iUniqueIdCounter = 0;

WidgetFont::WidgetFont(ItemFont *pOwner, QWidget *parent) : QWidget(parent),
                                                            m_pItemFont(pOwner),
                                                            m_pCurFontState(NULL),
                                                            m_pAtlas(NULL),
                                                            m_FontMetaDir(m_pItemFont->GetItemProject()->GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirFonts)),
                                                            ui(new Ui::WidgetFont)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemFont->GetName(true));

    QMenu *pEditMenu = m_pItemFont->GetEditMenu();
    pEditMenu->addAction(ui->actionAddState);
    pEditMenu->addAction(ui->actionRemoveState);
    pEditMenu->addAction(ui->actionRenameState);
    pEditMenu->addAction(ui->actionOrderStateBackwards);
    pEditMenu->addAction(ui->actionOrderStateForwards);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionAddLayer);
    pEditMenu->addAction(ui->actionRemoveLayer);
    pEditMenu->addAction(ui->actionOrderLayerUpwards);
    pEditMenu->addAction(ui->actionOrderLayerDownwards);
    
    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionAddLayer);
    m_StateActionsList.push_back(ui->actionRemoveLayer);
    m_StateActionsList.push_back(ui->actionOrderLayerUpwards);
    m_StateActionsList.push_back(ui->actionOrderLayerDownwards);

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
        on_actionAddState_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemFont->GetUndoStack()->clear();

    m_iPrevAtlasCmbIndex = ui->cmbAtlasGroups->currentIndex();

    UpdateActions();
}

WidgetFont::~WidgetFont()
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        delete m_MasterStageList[i];

    QMap<int, FontStage *>::iterator iter;
    for(iter = m_RemovedStageMap.begin(); iter != m_RemovedStageMap.end(); ++iter)
        delete iter.value();

    delete ui;
}

ItemFont *WidgetFont::GetItemFont()
{
    return m_pItemFont;
}

QString WidgetFont::GetFullItemName()
{
    return m_pItemFont->GetName(true);
}

int WidgetFont::RequestStage(QString sFullFontPath, rendermode_t eRenderMode, float fSize, float fOutlineThickness)
{
    // Look for an existing stage that matches the request first
    FontStage *pStage = NULL;
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        pStage = m_MasterStageList[i];

        QFileInfo stageFontPath(pStage->pTextureFont->filename);
        QFileInfo requestFontPath(sFullFontPath);

        if(QString::compare(stageFontPath.fileName(), requestFontPath.fileName(), Qt::CaseInsensitive) == 0 &&
           pStage->eMode == eRenderMode &&
           pStage->fSize == fSize &&
           pStage->fOutlineThickness == fOutlineThickness)
        {
            ui->cmbStates->currentData().value<WidgetFontState *>()->GetFontModel()->AddStage(pStage);
            m_MasterStageList.append(pStage);

            return pStage->iUNIQUE_ID;
        }
    }

    sm_iUniqueIdCounter++;

    pStage = new FontStage(sm_iUniqueIdCounter, eRenderMode, fSize, fOutlineThickness);
    ui->cmbStates->currentData().value<WidgetFontState *>()->GetFontModel()->AddStage(pStage);
    m_MasterStageList.append(pStage);

    return pStage->iUNIQUE_ID;
}

void WidgetFont::RequestStage(int iId)
{
    for(int i = 0; i < m_RemovedStageList.count(); ++i)
    {
        if(m_RemovedStageList[i].first->iUNIQUE_ID == iId)
        {
            beginInsertRows(QModelIndex(), m_RemovedStageList[i].second, m_RemovedStageList[i].second);
            m_MasterStageList.insert(m_RemovedStageList[i].second, m_RemovedStageList[i].first);
            m_RemovedStageList.removeAt(i);
            endInsertRows();

            break;
        }
    }
}

void WidgetFont::RemoveStage(int iId)
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        if(m_MasterStageList[i]->iUNIQUE_ID == iId)
        {
            m_RemovedStageList.append(QPair<FontStage *, int>(m_MasterStageList[i], i));
            m_MasterStageList.removeAt(i);

            return;
        }
    }
}

void WidgetFont::GeneratePreview(bool bFindBestFit /*= false*/)
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

    // if 'bFindBestFit' == true, adjust atlas dimentions until we utilize efficient space on the smallest texture
    QSize atlasSize = GetAtlasDimensions(ui->cmbAtlasGroups->currentIndex());
    float fAtlasSizeModifier = 1.0f;
    bool bDoInitialShrink = true;
    size_t iNumMissedGlyphs = 0;
    int iNumPasses = 0;
    int iNumFonts = 0;
    do
    {
        iNumPasses++;

        if(bDoInitialShrink && fAtlasSizeModifier != 1.0f)
            bDoInitialShrink = false;

        iNumMissedGlyphs = 0;

        if(m_pAtlas)
            texture_atlas_delete(m_pAtlas);
        m_pAtlas = texture_atlas_new(static_cast<size_t>(atlasSize.width() * fAtlasSizeModifier), static_cast<size_t>(atlasSize.height() * fAtlasSizeModifier), 1);

        for(int i = 0; i < ui->cmbStates->count(); ++i)
        {
            WidgetFontState *pFontState = ui->cmbStates->itemData(i).value<WidgetFontState *>();
            
            WidgetFontModel *pModel = static_cast<WidgetFontModel *>(pFontState->GetFontModel());
            for(int j = 0; j < pModel->rowCount(); ++j)
            {
                iNumFonts++;
                
                texture_font_t *pFont = texture_font_new_from_file(m_pAtlas, pModel->GetSize(), pFontState->GetFontFilePath().toStdString().c_str());
                if(pFont == NULL)
                {
                    HyGuiLog("Could not create freetype font from: " % pFontState->GetFontFilePath(), LOGTYPE_Error);
                    return;
                }
    
                pModel->SetTextureFont(j, pFont);
                iNumMissedGlyphs += texture_font_load_glyphs(pFont, sGlyphs.toStdString().c_str());
            }
        }

        if(iNumMissedGlyphs && fAtlasSizeModifier == 1.0f)
            break; // Failure

        if(iNumMissedGlyphs)
            fAtlasSizeModifier = HyClamp(fAtlasSizeModifier + 0.05f, 0.0f, 1.0f);
        else if(bDoInitialShrink)
            fAtlasSizeModifier = static_cast<float>(m_pAtlas->used) / static_cast<float>(m_pAtlas->width * m_pAtlas->height);
    }
    while(bFindBestFit && iNumMissedGlyphs != 0 || bDoInitialShrink);

    if(iNumMissedGlyphs)
    {
        HyGuiLog("Failed to generate font preview. Number of missed glyphs: " % QString::number(iNumMissedGlyphs), LOGTYPE_Info);
    }
    else if(bFindBestFit)
    {
        HyGuiLog("Generated " % m_pItemFont->GetName(true) % " Preview", LOGTYPE_Info);
        HyGuiLog(QString::number(iNumFonts) % " fonts with " % QString::number(sGlyphs.size()) % " glyphs each (totaling " % QString::number(sGlyphs.size() * iNumFonts) % ").", LOGTYPE_Normal);
        HyGuiLog("Font Atlas size: " % QString::number(m_pAtlas->width) % "x" % QString::number(m_pAtlas->height) % " (Utilizing " % QString::number(100.0*m_pAtlas->used / (float)(m_pAtlas->width*m_pAtlas->height)) % "%) (Num Passes: " % QString::number(iNumPasses) % " - Dimensions Modifier: " % QString::number(fAtlasSizeModifier) % ")", LOGTYPE_Normal);
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

QDir WidgetFont::GetFontMetaDir()
{
    return m_FontMetaDir;
}

QSize WidgetFont::GetAtlasDimensions(int iAtlasGrpIndex)
{
    return m_pItemFont->GetAtlasManager().GetAtlasDimensions(iAtlasGrpIndex);
}

void WidgetFont::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
}

void WidgetFont::on_cmbAtlasGroups_currentIndexChanged(int index)
{
    if(ui->cmbAtlasGroups->currentIndex() == index)
        return;

    // TODO: This will break when an atlas group is removed from the Atlas Manager and is apart of the UndoStack
    QUndoCommand *pCmd = new ItemFontCmd_AtlasGroupChanged(*this, ui->cmbAtlasGroups, m_iPrevAtlasCmbIndex, index);
    m_pItemFont->GetUndoStack()->push(pCmd);

    m_iPrevAtlasCmbIndex = index;
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

void WidgetFont::on_txtAdditionalSymbols_editingFinished()
{
    QUndoCommand *pCmd = new ItemFontCmd_LineEditSymbols(*this, ui->txtAdditionalSymbols);
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_cmbStates_currentIndexChanged(int index)
{
    WidgetFontState *pFontState = ui->cmbStates->itemData(index).value<WidgetFontState *>();
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

    //UpdateActions();
}

void WidgetFont::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_AddState(*this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}


void WidgetFont::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_RemoveState(*this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionRenameState_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_RenameState(*this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_MoveStateBack(*this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new ItemFontCmd_MoveStateForward(*this, m_StateActionsList, ui->cmbStates);
    m_pItemFont->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetFont::on_actionAddLayer_triggered()
{
    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();
    
    QUndoCommand *pCmd = new ItemFontCmd_AddLayer(*this, m_pFontModel, pFontState->GetFontFilePath(), pFontState->GetCurSelectedRenderMode(), pFontState->GetSize(), pFontState->GetThickness());
    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionRemoveLayer_triggered()
{
//    WidgetFontState *pFontState = ui->cmbStates->currentData().value<WidgetFontState *>();

//    QUndoCommand *pCmd = new ItemFontCmd_RemoveLayer(*this, m_pFontModel, pFontState->GetFontFilePath(), pFontState->GetCurSelectedRenderMode(), pFontState->GetSize(), pFontState->GetThickness());
//    m_pItemFont->GetUndoStack()->push(pCmd);
}

void WidgetFont::on_actionOrderLayerDownwards_triggered()
{

}

void WidgetFont::on_actionOrderLayerUpwards_triggered()
{

}
