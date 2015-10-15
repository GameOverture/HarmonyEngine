#include "HyGuiAtlasGroup.h"

HyGuiAtlasGroup::HyGuiAtlasGroup()
{
    
}

HyGuiAtlasGroup::~HyGuiAtlasGroup()
{
    
}


void HyGuiAtlasGroup::SaveSettings()
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
    
    
    QFile file(m_MetaDataFile.absoluteFilePath());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        HYLOG("Couldn't open atlas settings file for writing", LOGTYPE_Error);
        return;
    }

    QJsonDocument doc(settings);
    qint64 iBytesWritten = file.write(doc.toBinaryData());
    if(0 == iBytesWritten || -1 == iBytesWritten)
    {
        HYLOG("Could not write to atlas settings file: " % file.errorString(), LOGTYPE_Error);
        return;
    }
}

void HyGuiAtlasGroup::LoadSettings()
{
    if(m_MetaDataFile.exists() == false)
    {
        HYLOG("Atlas settings file not found. Generating new one.", LOGTYPE_Info);
        SaveSettings();
        ui->stackedWidget->setCurrentIndex(PAGE_Settings);
        
        return;
    }
    
    QFile file(m_MetaDataFile.absoluteFilePath());
    if(file.open(QIODevice::ReadOnly) == false)
    {
        HYLOG("Could not open atlas settings file for reading", LOGTYPE_Error);
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromBinaryData(data);
    file.close();
    
    QJsonObject settings = doc.object();
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
    
    //
    ui->stackedWidget->setCurrentIndex(PAGE_Frames);
}
