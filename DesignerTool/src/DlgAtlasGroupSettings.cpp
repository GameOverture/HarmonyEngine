#include "DlgAtlasGroupSettings.h"
#include "ui_DlgAtlasGroupSettings.h"

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
    if(m_bSettingsDirty)
    {
        QMessageBox dlg(QMessageBox::Question, "Harmony Designer Tool", "Atlas texture settings have changed. Would you like to save settings and regenerate all textures?", QMessageBox::Yes | QMessageBox::Cancel);
        switch(dlg.exec())
        {
        case QMessageBox::Yes:
            // Save was clicked. Reload every texture with new settings, then show 'frames'
            SaveSettings();
            RepackFrames();
            break;
        case QMessageBox::No:
            // Don't Save was clicked. Restore the cached settings and show the 'frames'

            break;
        case QMessageBox::Cancel:
            // Cancel was clicked. Don't do anything and stay on the 'settings'
            return;
        default:
            // should never be reached
            break;
        }
    }

    ui->stackedWidget->setCurrentIndex(PAGE_Frames);
    m_bSettingsDirty = false;
}

void DlgAtlasGroupSettings::on_buttonBox_rejected()
{
    //LoadSettings();
}
