/**************************************************************************
 *	WidgetFontState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontWidgetState.h"
#include "ui_FontWidgetState.h"

#include "FontWidget.h"
#include "UndoCmds.h"

#include <QStandardPaths>

FontWidgetState::FontWidgetState(FontWidget *pOwner, QList<QAction *> stateActionList, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                                                    m_pOwner(pOwner),
                                                                                                                    m_sName("Unnamed"),
                                                                                                                    m_iPrevFontCmbIndex(0),
                                                                                                                    ui(new Ui::FontWidgetState)
{
    ui->setupUi(this);
    
    m_pFontModel = new FontTableModel(this);
    
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
    ui->stagesView->setItemDelegate(new FontDelegate(&m_pOwner->GetItem(), m_pOwner->GetCmbStates(), this));
    QItemSelectionModel *pSelModel = ui->stagesView->selectionModel();
    connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_layersView_selectionChanged(const QItemSelection &, const QItemSelection &)));
    
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
    SetSelectedFont("Arial.ttf");
    
    UpdateActions();
}

/*virtual*/ FontWidgetState::~FontWidgetState()
{
    delete ui;
}

QString FontWidgetState::GetName()
{
    return m_sName;
}

void FontWidgetState::SetName(QString sNewName)
{
    m_sName = sNewName;
}

void FontWidgetState::SetSelectedFont(QString sFontName)
{
    int iFontIndex = ui->cmbFontList->findText(sFontName, Qt::MatchFixedString);
    if(iFontIndex != -1)
        ui->cmbFontList->setCurrentIndex(iFontIndex);

    m_iPrevFontCmbIndex = ui->cmbFontList->currentIndex();
}

FontTableModel *FontWidgetState::GetFontModel()
{
    return m_pFontModel;
}

FontTableView *FontWidgetState::GetFontLayerView()
{
    return ui->stagesView;
}

QString FontWidgetState::GetFontFilePath()
{
    return ui->cmbFontList->currentData().toString();
}

rendermode_t FontWidgetState::GetCurSelectedRenderMode()
{
    return static_cast<rendermode_t>(ui->cmbRenderMode->currentData().toInt());
}

float FontWidgetState::GetSize()
{
    return static_cast<float>(ui->sbSize->value());
}

void FontWidgetState::SetSize(double dSize)
{
    ui->sbSize->setValue(dSize);
    on_sbSize_editingFinished();
}

float FontWidgetState::GetThickness()
{
    return static_cast<float>(ui->sbThickness->value());
}

int FontWidgetState::GetSelectedStageId()
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

void FontWidgetState::UpdateActions()
{
    if(m_iPrevFontCmbIndex != ui->cmbFontList->currentIndex())
        m_pOwner->GeneratePreview();
    
    m_iPrevFontCmbIndex = ui->cmbFontList->currentIndex();
    m_dPrevFontSize = ui->sbSize->value();
    
    QComboBox *pCmbStates = m_pOwner->GetCmbStates();
    for(int i = 0; i < pCmbStates->count(); ++i)
    {
        if(pCmbStates->itemData(i).value<FontWidgetState *>() == this)
        {
            pCmbStates->setCurrentIndex(i);
            break;
        }
    }
}

void FontWidgetState::on_cmbFontList_currentIndexChanged(int index)
{
//    QUndoCommand *pCmd = new UndoCmd_ComboBox<FontWidgetState>("Font Selection", this, ui->cmbFontList, m_iPrevFontCmbIndex, index);
//    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void FontWidgetState::on_cmbRenderMode_currentIndexChanged(int index)
{
    switch(index)
    {
    case RENDER_NORMAL:
    case RENDER_SIGNED_DISTANCE_FIELD:
        ui->sbThickness->setRange(0.0, 0.0);
        ui->sbThickness->setValue(0.0);
        ui->sbThickness->setEnabled(false);
        break;
        
    case RENDER_OUTLINE_EDGE:
    case RENDER_OUTLINE_POSITIVE:
    case RENDER_OUTLINE_NEGATIVE:
        ui->sbThickness->setRange(1.0, 1024.0);
        ui->sbThickness->setValue(1.0);
        ui->sbThickness->setEnabled(true);
        break;
    }
}

void FontWidgetState::on_sbSize_editingFinished()
{
    if(m_dPrevFontSize == ui->sbSize->value())
        return;
    
    QUndoCommand *pCmd = new UndoCmd_DoubleSpinBox<FontWidgetState>("Font Size", this, ui->sbSize, m_dPrevFontSize, ui->sbSize->value());
    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void FontWidgetState::on_layersView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
    m_pOwner->UpdateActions();
}
