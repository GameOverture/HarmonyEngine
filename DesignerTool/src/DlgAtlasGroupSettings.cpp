/**************************************************************************
 *	DlgAtlasGroupSettings.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgAtlasGroupSettings.h"
#include "ui_DlgAtlasGroupSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

#include "HyGlobal.h"

DlgAtlasGroupSettings::DlgAtlasGroupSettings(QJsonObject packerSettingsObj, QWidget *parent) :  QDialog(parent),
                                                                                                ui(new Ui::DlgAtlasGroupSettings),
                                                                                                m_bSettingsDirty(false),
                                                                                                m_bNameChanged(false)
{
    ui->setupUi(this);

    ui->txtName->setText(m_sName);

    ui->cmbSortOrder->setCurrentIndex(packerSettingsObj["cmbSortOrder"].toInt());
    ui->sbFrameMarginTop->setValue(packerSettingsObj["sbFrameMarginTop"].toInt());
    ui->sbFrameMarginLeft->setValue(packerSettingsObj["sbFrameMarginLeft"].toInt());
    ui->sbFrameMarginRight->setValue(packerSettingsObj["sbFrameMarginRight"].toInt());
    ui->sbFrameMarginBottom->setValue(packerSettingsObj["sbFrameMarginBottom"].toInt());
    ui->extrude->setValue(packerSettingsObj["extrude"].toInt());
    ui->chkMerge->setChecked(packerSettingsObj["chkMerge"].toBool());
    ui->chkSquare->setChecked(packerSettingsObj["chkSquare"].toBool());
    ui->chkAutosize->setChecked(packerSettingsObj["chkAutosize"].toBool());
    ui->minFillRate->setValue(packerSettingsObj["minFillRate"].toInt());

    ui->sbTextureWidth->setValue(packerSettingsObj["sbTextureWidth"].toInt());
    ui->sbTextureHeight->setValue(packerSettingsObj["sbTextureHeight"].toInt());
    ui->cmbHeuristic->setCurrentIndex(packerSettingsObj["cmbHeuristic"].toInt());

    m_bSettingsDirty = m_bNameChanged = false;
}

DlgAtlasGroupSettings::~DlgAtlasGroupSettings()
{
    delete ui;
}

void DlgAtlasGroupSettings::WidgetsToData()
{
    m_sName = ui->txtName->text();

    m_iTextureWidth = ui->sbTextureWidth->value();
    m_iTextureHeight = ui->sbTextureHeight->value();
    m_iHeuristicIndex = ui->cmbHeuristic->currentIndex();
    m_iSortOrderIndex = ui->cmbSortOrder->currentIndex();
    m_iFrameMarginTop = ui->sbFrameMarginTop->value();
    m_iFrameMarginLeft = ui->sbFrameMarginLeft->value();
    m_iFrameMarginRight = ui->sbFrameMarginRight->value();
    m_iFrameMarginBottom = ui->sbFrameMarginBottom->value();
    m_iExtrude = ui->extrude->value();
    m_bMerge = ui->chkMerge->isChecked();
    m_bAutoSize = ui->chkAutosize->isChecked();
    m_bSquare = ui->chkSquare->isChecked();
    m_iFillRate = ui->minFillRate->value();
    
    // NOTE: Removing rotation from atlas packing
    //m_iRotationStrategyIndex = ui->cmbRotationStrategy->currentIndex();
}

void DlgAtlasGroupSettings::DataToWidgets()
{

}

QString DlgAtlasGroupSettings::GetName()
{
    return m_sName;
}

void DlgAtlasGroupSettings::SetName(QString sName)
{
    m_sName = sName;
}

int DlgAtlasGroupSettings::TextureWidth()
{
    return m_iTextureWidth;
}

int DlgAtlasGroupSettings::TextureHeight()
{
    return m_iTextureHeight;
}

int DlgAtlasGroupSettings::GetHeuristic()
{
    return m_iHeuristicIndex;
}

QJsonObject DlgAtlasGroupSettings::GetSettings()
{
    QJsonObject settings;

    settings.insert("txtName", QJsonValue(m_sName));

    settings.insert("cmbSortOrder", QJsonValue(m_iSortOrderIndex/*ui->cmbSortOrder->currentIndex()*/));
    settings.insert("sbFrameMarginTop", QJsonValue(m_iFrameMarginTop/*ui->sbFrameMarginTop->value()*/));
    settings.insert("sbFrameMarginLeft", QJsonValue(m_iFrameMarginLeft/*ui->sbFrameMarginLeft->value()*/));
    settings.insert("sbFrameMarginRight", QJsonValue(m_iFrameMarginRight/*ui->sbFrameMarginRight->value()*/));
    settings.insert("sbFrameMarginBottom", QJsonValue(m_iFrameMarginBottom/*ui->sbFrameMarginBottom->value()*/));
    settings.insert("extrude", QJsonValue(m_iExtrude/*ui->extrude->value()*/));
    settings.insert("chkMerge", QJsonValue(m_bMerge/*ui->chkMerge->isChecked()*/));
    settings.insert("chkSquare", QJsonValue(m_bSquare/*ui->chkSquare->isChecked()*/));
    settings.insert("chkAutosize", QJsonValue(m_bAutoSize/*ui->chkAutosize->isChecked()*/));
    settings.insert("minFillRate", QJsonValue(m_iFillRate/*ui->minFillRate->value()*/));
    
    //settings.insert("cmbRotationStrategy", QJsonValue(m_iRotationStrategyIndex/*ui->cmbRotationStrategy->currentIndex()*/));

    settings.insert("sbTextureWidth", QJsonValue(m_iTextureWidth/*ui->sbTextureWidth->value()*/));
    settings.insert("sbTextureHeight", QJsonValue(m_iTextureHeight/*ui->sbTextureHeight->value()*/));
    settings.insert("cmbHeuristic", QJsonValue(m_iHeuristicIndex/*ui->cmbHeuristic->currentIndex()*/));

    return settings;
}

void DlgAtlasGroupSettings::LoadSettings(QJsonObject settings)
{
    m_sName = settings["txtName"].toString();

    m_iSortOrderIndex = JSONOBJ_TOINT(settings, "cmbSortOrder");
    m_iFrameMarginTop = JSONOBJ_TOINT(settings, "sbFrameMarginTop");
    m_iFrameMarginLeft = JSONOBJ_TOINT(settings, "sbFrameMarginLeft");
    m_iFrameMarginRight = JSONOBJ_TOINT(settings, "sbFrameMarginRight");
    m_iFrameMarginBottom = JSONOBJ_TOINT(settings, "sbFrameMarginBottom");
    m_iExtrude = JSONOBJ_TOINT(settings, "extrude");
    m_bMerge = settings["chkMerge"].toBool();
    m_bAutoSize = settings["chkAutosize"].toBool();
    m_bSquare = settings["chkSquare"].toBool();
    m_iFillRate = JSONOBJ_TOINT(settings, "minFillRate");
    
    //m_iRotationStrategyIndex = JSONOBJ_TOINT(settings, "cmbRotationStrategy");

    m_iTextureWidth = JSONOBJ_TOINT(settings, "sbTextureWidth");
    m_iTextureHeight = JSONOBJ_TOINT(settings, "sbTextureHeight");
    m_iHeuristicIndex = JSONOBJ_TOINT(settings, "cmbHeuristic");
}

void DlgAtlasGroupSettings::on_btnTexSize256_clicked()
{
    ui->sbTextureWidth->setValue(256);
    ui->sbTextureHeight->setValue(256);
}

void DlgAtlasGroupSettings::on_btnTexSize512_clicked()
{
    ui->sbTextureWidth->setValue(512);
    ui->sbTextureHeight->setValue(512);
}

void DlgAtlasGroupSettings::on_btnTexSize1024_clicked()
{
    ui->sbTextureWidth->setValue(1024);
    ui->sbTextureHeight->setValue(1024);
}

void DlgAtlasGroupSettings::on_btnTexSize2048_clicked()
{
    ui->sbTextureWidth->setValue(2048);
    ui->sbTextureHeight->setValue(2048);
}

void DlgAtlasGroupSettings::on_buttonBox_accepted()
{
    m_bSettingsDirty = m_bNameChanged = false;

    if(m_sName != ui->txtName->text())
        m_bNameChanged = true;
    
    if(m_iTextureWidth != ui->sbTextureWidth->value() ||
       m_iTextureHeight != ui->sbTextureHeight->value() ||
       m_iHeuristicIndex != ui->cmbHeuristic->currentIndex() ||
       m_iSortOrderIndex != ui->cmbSortOrder->currentIndex() ||
       m_iFrameMarginTop != ui->sbFrameMarginTop->value() ||
       m_iFrameMarginLeft != ui->sbFrameMarginLeft->value() ||
       m_iFrameMarginRight != ui->sbFrameMarginRight->value() ||
       m_iFrameMarginBottom != ui->sbFrameMarginBottom->value() ||
       m_iExtrude != ui->extrude->value() ||
       m_bMerge != ui->chkMerge->isChecked() ||
       m_bAutoSize != ui->chkAutosize->isChecked() ||
       m_bSquare != ui->chkSquare->isChecked() ||
       m_iFillRate != ui->minFillRate->value()/* ||
       m_iRotationStrategyIndex != ui->cmbRotationStrategy->currentIndex()*/)
    {
        if(QMessageBox::Ok == QMessageBox::warning(NULL, QString("Save Atlas Group Settings?"), QString("By modifying the Atlas Group settings, it is required to regenerate the entire Atlas Group."), QMessageBox::Ok, QMessageBox::Cancel))
            m_bSettingsDirty = true;
    }
}

void DlgAtlasGroupSettings::on_buttonBox_rejected()
{
    DataToWidgets();
}
