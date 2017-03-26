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
#include "ui_FontWidget.h"

#include "HyGuiGlobal.h"
#include "FontUndoCmds.h"
#include "UndoCmds.h"
#include "AtlasWidget.h"
#include "FontModelView.h"
#include "FontModels.h"
#include "DlgInputName.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QMenu>
#include <QColor>

FontWidget::FontWidget(ProjectItem &itemRef, IHyApplication &hyApp, QWidget *parent) :  QWidget(parent),
                                                                                        ui(new Ui::FontWidget),
                                                                                        m_Draw(*static_cast<FontModel *>(itemRef.GetModel()), hyApp),
                                                                                        m_ItemRef(itemRef)
{
    ui->setupUi(this);

    m_PrevAtlasSize.setWidth(0);
    m_PrevAtlasSize.setHeight(0);

    ui->txtPrefixAndName->setText(m_ItemRef.GetName(true));
    
    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    ui->btnAddLayer->setDefaultAction(ui->actionAddLayer);
    ui->btnRemoveLayer->setDefaultAction(ui->actionRemoveLayer);
    ui->btnOrderLayerUp->setDefaultAction(ui->actionOrderLayerUpwards);
    ui->btnOrderLayerDown->setDefaultAction(ui->actionOrderLayerDownwards);

    ui->stagesView->resize(ui->stagesView->size());
    ui->stagesView->setItemDelegate(new FontDelegate(&m_ItemRef, ui->cmbStates, this));
    QItemSelectionModel *pSelModel = ui->stagesView->selectionModel();
    connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_layersView_selectionChanged(const QItemSelection &, const QItemSelection &)));

    // ...set models
    SetSelectedState(0);
    
    

    UpdateActions();
    
    static_cast<FontModel *>(m_ItemRef.GetModel())->GeneratePreview();
}

FontWidget::~FontWidget()
{
    delete ui;
}

void FontWidget::SetSelectedState(int iIndex)
{
    FontStateData *pCurStateData = static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(iIndex));

    ui->stagesView->setModel(pCurStateData->GetFontModel());
    if(ui->stagesView->currentIndex().row() < 0 && ui->stagesView->model()->rowCount() > 0)
        ui->stagesView->selectRow(0);

    pCurStateData->GetSizeMapper()->AddSpinBoxMapping(ui->sbSize);
    pCurStateData->GetRenderModeMapper()->AddComboBoxMapping(ui->cmbRenderMode);
    pCurStateData->GetFontMapper()->AddComboBoxMapping(ui->cmbFontList);

    ui->stagesView->setModel(pCurStateData->GetFontModel());


    UpdateActions();
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

ProjectItem &FontWidget::GetItem()
{
    return m_ItemRef;
}

QString FontWidget::GetFullItemName()
{
    return m_ItemRef.GetName(true);
}

QComboBox *FontWidget::GetCmbStates()
{
    return ui->cmbStates;
}

void FontWidget::Refresh(QVariant param)
{
    bool bParamOk = false;
    int iStateAffected = param.toInt(&bParamOk);
    if(bParamOk && iStateAffected >= 0)
        SetSelectedState(iStateAffected);

    static_cast<FontModel *>(m_ItemRef.GetModel())->GeneratePreview();
    UpdateActions();
}

void FontWidget::UpdateActions()
{
    bool bGeneratePreview = false;

    QSize curSize = m_ItemRef.GetProject().GetAtlasModel().GetAtlasDimensions();
    if(m_PrevAtlasSize.width() < curSize.width() || m_PrevAtlasSize.height() < curSize.height())
        bGeneratePreview = true;

    m_PrevAtlasSize = curSize;

//    QString sPrevSymbols = m_sAvailableTypefaceGlyphs;
//    SetGlyphsDirty();
//    if(sPrevSymbols != m_sAvailableTypefaceGlyphs)
//        bGeneratePreview = true;

    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));

    bool bFrameIsSelected = ui->stagesView->model()->rowCount() > 0 && ui->stagesView->currentIndex().row() >= 0;
    ui->actionOrderLayerUpwards->setEnabled(bFrameIsSelected && ui->stagesView->currentIndex().row() != 0);
    ui->actionOrderLayerDownwards->setEnabled(bFrameIsSelected && ui->stagesView->currentIndex().row() != ui->stagesView->model()->rowCount() - 1);

    if(bGeneratePreview)
        static_cast<FontModel *>(m_ItemRef.GetModel())->GeneratePreview();

    //m_bFontPreviewDirty = true;
}

FontStateData *FontWidget::GetCurStateData()
{
    return static_cast<FontStateData *>(static_cast<FontModel *>(m_ItemRef.GetModel())->GetStateData(ui->cmbStates->currentIndex()));
}

int FontWidget::GetSelectedStageId()
{
    int iRowIndex = ui->stagesView->currentIndex().row();

    if(ui->stagesView->model()->rowCount() == 0 ||
       iRowIndex < 0 ||
       iRowIndex >= ui->stagesView->model()->rowCount())
    {
        return -1;
    }

    return static_cast<FontTableModel *>(ui->stagesView->model())->GetLayerId(iRowIndex);
}

void FontWidget::on_chk_09_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox("Glyphs 0-9", m_ItemRef, static_cast<FontModel *>(m_ItemRef.GetModel())->Get09Mapper(), ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_chk_az_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox("Glyphs a-z", m_ItemRef, static_cast<FontModel *>(m_ItemRef.GetModel())->GetazMapper(), ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_chk_AZ_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox("Glyphs A-Z", m_ItemRef, static_cast<FontModel *>(m_ItemRef.GetModel())->GetAZMapper(), ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_chk_symbols_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox("Glyphs symbols", m_ItemRef, static_cast<FontModel *>(m_ItemRef.GetModel())->GetSymbolsMapper(), ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_txtAdditionalSymbols_editingFinished()
{
    QUndoCommand *pCmd = new UndoCmd_LineEdit("Additional Symbols",
                                              m_ItemRef,
                                              static_cast<FontModel *>(m_ItemRef.GetModel())->GetAdditionalSymbolsMapper(),
                                              ui->cmbStates->currentIndex(), ui->txtAdditionalSymbols->text(),
                                              static_cast<FontModel *>(m_ItemRef.GetModel())->GetAdditionalSymbolsMapper()->GetString());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_cmbStates_currentIndexChanged(int index)
{
    SetSelectedState(index);

//    FontWidgetState *pFontState = ui->cmbStates->itemData(index).value<FontWidgetState *>();
//    if(m_pCurFontState == pFontState)
//        return;

//    if(m_pCurFontState)
//        m_pCurFontState->hide();

//    ui->grpFontStateLayout->addWidget(pFontState);

//#if _DEBUG
//    int iDebugTest = ui->grpFontStateLayout->count(); // TODO: test to see if duplicates keep appending if you switch between the same two font states
//#endif

//    m_pCurFontState = pFontState;
//    m_pCurFontState->show();

    UpdateActions();
}

void FontWidget::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddState<FontStateData>("Add Font State", m_ItemRef);
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_RemoveState<FontStateData>("Remove Font State", m_ItemRef, ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Font State", GetCurStateData()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new UndoCmd_RenameState("Rename Font State", m_ItemRef, pDlg->GetName(), ui->cmbStates->currentIndex());
        m_ItemRef.GetUndoStack()->push(pCmd);
    }
    delete pDlg;
}

void FontWidget::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateBack("Shift Font State Index <-", m_ItemRef, ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateForward("Shift Font State Index ->", m_ItemRef, ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionAddLayer_triggered()
{
    QUndoCommand *pCmd = new FontUndoCmd_AddLayer(m_ItemRef,
                                                  ui->cmbStates->currentIndex(),
                                                  static_cast<ftgl::rendermode_t>(GetCurStateData()->GetRenderModeMapper()->GetCurrentData().toInt()),
                                                  GetCurStateData()->GetSizeMapper()->GetValue(),
                                                  GetCurStateData()->GetThicknessMapper()->GetValue());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionRemoveLayer_triggered()
{
    int iSelectedId = GetSelectedStageId();
    if(iSelectedId == -1)
        return;

    QUndoCommand *pCmd = new FontUndoCmd_RemoveLayer(m_ItemRef, ui->cmbStates->currentIndex(), iSelectedId);
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionOrderLayerDownwards_triggered()
{
    QUndoCommand *pCmd = new FontUndoCmd_LayerOrder(m_ItemRef,
                                                    ui->cmbStates->currentIndex(),
                                                    ui->stagesView,
                                                    ui->stagesView->currentIndex().row(),
                                                    ui->stagesView->currentIndex().row() + 1);
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_actionOrderLayerUpwards_triggered()
{
    QUndoCommand *pCmd = new FontUndoCmd_LayerOrder(m_ItemRef,
                                                    ui->cmbStates->currentIndex(),
                                                    ui->stagesView,
                                                    ui->stagesView->currentIndex().row(),
                                                    ui->stagesView->currentIndex().row() - 1);
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void FontWidget::on_cmbRenderMode_currentIndexChanged(int index)
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

void FontWidget::on_sbSize_editingFinished()
{
    QUndoCommand *pCmd = new UndoCmd_DoubleSpinBox("Font Size",
                                                   m_ItemRef,
                                                   GetCurStateData()->GetSizeMapper(),
                                                   ui->cmbStates->currentIndex(),
                                                   ui->sbSize->value(),
                                                   GetCurStateData()->GetSizeMapper()->GetValue());
    m_ItemRef.GetUndoStack()->push(pCmd);
}
