#include "DlgProjectSettings.h"
#include "ui_DlgProjectSettings.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

DlgProjectSettings::DlgProjectSettings(Project &projectRef, QWidget *parent) :  QDialog(parent),
                                                                                ui(new Ui::DlgProjectSettings),
                                                                                m_ProjectRef(projectRef)
{
    ui->setupUi(this);




//    QFile projFile(m_ProjectRef.GetPath());
//    if(projFile.exists())
//    {
//        if(!projFile.open(QIODevice::ReadOnly))
//        {
//            HyGuiLog("DlgProjectSettings::DlgProjectSettings() could not open " % m_ProjectRef.GetPath() % ": " % projFile.errorString(), LOGTYPE_Error);
//            return;
//        }
//    }
//    else
//    {
//        HyGuiLog("DlgProjectSettings::DlgProjectSettings() could not find the project file: " % m_ProjectRef.GetPath(), LOGTYPE_Error);
//        return;
//    }


//    QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
//    projFile.close();

    QJsonObject projObj = m_ProjectRef.GetSettingsObj();

    ui->txtTitleName->setText(projObj["GameName"].toString());
    ui->txtClassName->setText(projObj["ClassName"].toString());

    ui->txtAssetsLocation->setText(projObj["DataPath"].toString());
    ui->txtMetaDataLocation->setText(projObj["MetaDataPath"].toString());
    ui->txtSourceLocation->setText(projObj["SourcePath"].toString());

    ui->sbInputMaps->setValue(projObj["NumInputMappings"].toInt());
    ui->sbPixelsPerMeter->setValue(projObj["PixelsPerMeter"].toInt());
    ui->chkShowCursor->setChecked(projObj["ShowCursor"].toBool());
}

DlgProjectSettings::~DlgProjectSettings()
{
    delete ui;
}

void DlgProjectSettings::SaveSettings()
{

}
