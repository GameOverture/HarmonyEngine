/**************************************************************************
 *	WidgetFontState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFontState.h"
#include "ui_WidgetFontState.h"

#include "WidgetFont.h"
#include "ItemFontCmds.h"

#include <QStandardPaths>

WidgetFontState::WidgetFontState(WidgetFont *pOwner, QList<QAction *> stateActionList, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                                                    m_pOwner(pOwner),
                                                                                                                    m_sName("Unnamed"),
                                                                                                                    m_iPrevFontCmbIndex(0),
                                                                                                                    ui(new Ui::WidgetFontState)
{
    ui->setupUi(this);
    
    m_pFontModel = new WidgetFontModel(this);
    
    ui->cmbRenderMode->addItem("Normal", QVariant(static_cast<int>(RENDER_NORMAL)));
    ui->cmbRenderMode->addItem("Outline Edge", QVariant(static_cast<int>(RENDER_OUTLINE_EDGE)));
    ui->cmbRenderMode->addItem("Outline Positive", QVariant(static_cast<int>(RENDER_OUTLINE_POSITIVE)));
    ui->cmbRenderMode->addItem("Outline Negative", QVariant(static_cast<int>(RENDER_OUTLINE_NEGATIVE)));
    ui->cmbRenderMode->addItem("Signed Distance Field", QVariant(static_cast<int>(RENDER_SIGNED_DISTANCE_FIELD)));

    ui->btnAddLayer->setDefaultAction(FindAction(stateActionList, "actionAddLayer"));
    ui->btnRemoveLayer->setDefaultAction(FindAction(stateActionList, "actionRemoveLayer"));
    ui->btnOrderLayerUp->setDefaultAction(FindAction(stateActionList, "actionOrderLayerUpwards"));
    ui->btnOrderLayerDown->setDefaultAction(FindAction(stateActionList, "actionOrderLayerDownwards"));

    ui->stagesView->setModel(m_pFontModel);
    ui->stagesView->resize(ui->stagesView->size());
    ui->stagesView->setItemDelegate(new WidgetFontDelegate(m_pOwner->GetItemFont(), this));

    // Populate the font list combo box
    ui->cmbFontList->blockSignals(true);
    ui->cmbFontList->clear();
    QStringList sFilterList;
    sFilterList << "*.ttf" << "*.otf";
    //
    // Start with the stored meta dir fonts
    QFileInfoList metaFontFileInfoList = m_pOwner->GetFontMetaDir().entryInfoList(sFilterList);
    for(int i = 0; i < metaFontFileInfoList.count(); ++i)
    {
        ui->cmbFontList->findText(metaFontFileInfoList[i].fileName(), Qt::MatchFixedString);
        ui->cmbFontList->addItem(metaFontFileInfoList[i].fileName(), QVariant(metaFontFileInfoList[i].absoluteFilePath()));
    }
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
            {
                ui->cmbFontList->findText(fontFileInfoList[j].fileName(), Qt::MatchFixedString);
                ui->cmbFontList->addItem(fontFileInfoList[j].fileName(), QVariant(fontFileInfoList[j].absoluteFilePath()));
            }
        }
    }
    ui->cmbFontList->blockSignals(false);

    // Try to find Arial as default font
    int iArialIndex = ui->cmbFontList->findText("Arial.ttf", Qt::MatchFixedString);
    if(iArialIndex != -1)
        ui->cmbFontList->setCurrentIndex(iArialIndex);

    m_iPrevFontCmbIndex = ui->cmbFontList->currentIndex();
}

/*virtual*/ WidgetFontState::~WidgetFontState()
{
    delete ui;
}

QString WidgetFontState::GetName()
{
    return m_sName;
}

void WidgetFontState::SetName(QString sNewName)
{
    m_sName = sNewName;
}

WidgetFontModel *WidgetFontState::GetFontModel()
{
    return m_pFontModel;
}

QString WidgetFontState::GetFontFilePath()
{
    return ui->cmbFontList->currentData().toString();
}

rendermode_t WidgetFontState::GetCurSelectedRenderMode()
{
    return static_cast<rendermode_t>(ui->cmbRenderMode->currentData().toInt());
}

float WidgetFontState::GetSize()
{
    return static_cast<float>(ui->sbSize->value());
}

float WidgetFontState::GetThickness()
{
    return static_cast<float>(ui->sbThickness->value());
}

int WidgetFontState::GetSelectedStageId()
{
    int iRowIndex = ui->stagesView->currentIndex().row();
    
    if(m_pFontModel->rowCount() == 0 ||
       iRowIndex < 0 ||
       iRowIndex >= m_pFontModel->rowCount())
    {
        return -1;
    }
    
    return m_pFontModel->GetLayerId(iRowIndex);
}

void WidgetFontState::on_cmbFontList_currentIndexChanged(int index)
{
    ItemFont *pItemFont = m_pOwner->GetItemFont();
    
    QUndoCommand *pCmd = new ItemFontCmd_FontSelection(*m_pOwner, ui->cmbFontList, m_iPrevFontCmbIndex, index);
    pItemFont->GetUndoStack()->push(pCmd);

    m_iPrevFontCmbIndex = index;
}
