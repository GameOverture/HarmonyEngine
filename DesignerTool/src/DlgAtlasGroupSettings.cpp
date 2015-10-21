#include "DlgAtlasGroupSettings.h"
#include "ui_DlgAtlasGroupSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

#include "HyGlobal.h"

DlgAtlasGroupSettings::DlgAtlasGroupSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAtlasGroupSettings)
{
    ui->setupUi(this);
}

DlgAtlasGroupSettings::~DlgAtlasGroupSettings()
{
    delete ui;
}

int DlgAtlasGroupSettings::TextureWidth()
{
    return ui->sbTextureWidth->value();
}

int DlgAtlasGroupSettings::TextureHeight()
{
    return ui->sbTextureHeight->value();
}

int DlgAtlasGroupSettings::GetHeuristic()
{
    return ui->cmbHeuristic->currentIndex();
}

void DlgAtlasGroupSettings::SetPackerSettings(ImagePacker *pPacker)
{
    pPacker->sortOrder = ui->cmbSortOrder->currentIndex();
    pPacker->border.t = ui->sbFrameMarginTop->value();
    pPacker->border.l = ui->sbFrameMarginLeft->value();
    pPacker->border.r = ui->sbFrameMarginRight->value();
    pPacker->border.b = ui->sbFrameMarginBottom->value();
    pPacker->extrude = ui->extrude->value();
    pPacker->merge = ui->chkMerge->isChecked();
    pPacker->square = ui->chkSquare->isChecked();
    pPacker->autosize = ui->chkAutosize->isChecked();
    pPacker->minFillRate = ui->minFillRate->value();
    pPacker->mergeBF = false;
    pPacker->rotate = ui->cmbRotationStrategy->currentIndex();
}

QJsonObject DlgAtlasGroupSettings::GetSettings()
{
    QJsonObject settings;
    settings.insert("cmbSortOrder", QJsonValue(ui->cmbSortOrder->currentIndex()));
    settings.insert("sbFrameMarginTop", QJsonValue(ui->sbFrameMarginTop->value()));
    settings.insert("sbFrameMarginLeft", QJsonValue(ui->sbFrameMarginLeft->value()));
    settings.insert("sbFrameMarginRight", QJsonValue(ui->sbFrameMarginRight->value()));
    settings.insert("sbFrameMarginBottom", QJsonValue(ui->sbFrameMarginBottom->value()));
    settings.insert("extrude", QJsonValue(ui->extrude->value()));
    settings.insert("chkMerge", QJsonValue(ui->chkMerge->isChecked()));
    settings.insert("chkSquare", QJsonValue(ui->chkSquare->isChecked()));
    settings.insert("chkAutosize", QJsonValue(ui->chkAutosize->isChecked()));
    settings.insert("minFillRate", QJsonValue(ui->minFillRate->value()));
    settings.insert("cmbRotationStrategy", QJsonValue(ui->cmbRotationStrategy->currentIndex()));

    settings.insert("sbTextureWidth", QJsonValue(ui->sbTextureWidth->value()));
    settings.insert("sbTextureHeight", QJsonValue(ui->sbTextureHeight->value()));
    settings.insert("cmbHeuristic", QJsonValue(ui->cmbHeuristic->currentIndex()));

    return settings;
}

void DlgAtlasGroupSettings::LoadSettings(QJsonObject settings)
{
    ui->cmbSortOrder->setCurrentIndex(JSONOBJ_TOINT(settings, "cmbSortOrder"));
    ui->sbFrameMarginTop->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginTop"));
    ui->sbFrameMarginLeft->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginLeft"));
    ui->sbFrameMarginRight->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginRight"));
    ui->sbFrameMarginBottom->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginBottom"));
    ui->extrude->setValue(JSONOBJ_TOINT(settings, "extrude"));
    ui->chkMerge->setChecked(settings["chkMerge"].toBool());
    ui->chkSquare->setChecked(settings["chkSquare"].toBool());
    ui->chkAutosize->setChecked(settings["chkAutosize"].toBool());
    ui->minFillRate->setValue(JSONOBJ_TOINT(settings, "minFillRate"));
    ui->cmbRotationStrategy->setCurrentIndex(JSONOBJ_TOINT(settings, "cmbRotationStrategy"));

    ui->sbTextureWidth->setValue(JSONOBJ_TOINT(settings, "sbTextureWidth"));
    ui->sbTextureHeight->setValue(JSONOBJ_TOINT(settings, "sbTextureHeight"));
    ui->cmbHeuristic->setCurrentIndex(JSONOBJ_TOINT(settings, "cmbHeuristic"));
}

void DlgAtlasGroupSettings::on_cmbSortOrder_currentIndexChanged(int index)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_cmbRotationStrategy_currentIndexChanged(int index)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_cmbHeuristic_currentIndexChanged(int index)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_alphaThreshold_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_extrude_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_minFillRate_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_chkMerge_stateChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_chkAutosize_stateChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_chkSquare_stateChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginTop_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginLeft_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginRight_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginBottom_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbTextureWidth_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbTextureHeight_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnMatchTextureWidthHeight_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize256_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize512_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize1024_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize2048_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_buttonBox_accepted()
{
//    if(m_bSettingsDirty)
//    {
//        QMessageBox dlg(QMessageBox::Question, "Harmony Designer Tool", "Atlas texture settings have changed. Would you like to save settings and regenerate all textures?", QMessageBox::Yes | QMessageBox::Cancel);
//        switch(dlg.exec())
//        {
//        case QMessageBox::Yes:
//            // Save was clicked. Reload every texture with new settings, then show 'frames'
//            SaveSettings();
//            RepackFrames();
//            break;
//        case QMessageBox::No:
//            // Don't Save was clicked. Restore the cached settings and show the 'frames'

//            break;
//        case QMessageBox::Cancel:
//            // Cancel was clicked. Don't do anything and stay on the 'settings'
//            return;
//        default:
//            // should never be reached
//            break;
//        }
//    }

//    ui->stackedWidget->setCurrentIndex(PAGE_Frames);
//    m_bSettingsDirty = false;
}

void DlgAtlasGroupSettings::on_buttonBox_rejected()
{
    //LoadSettings();
}
