#include "DlgProjectSettings.h"
#include "ui_DlgProjectSettings.h"

#include "MainWindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

DlgProjectSettings::DlgProjectSettings(const QString sProjectFilePath, QWidget *parent) :   QDialog(parent),
                                                                                            ui(new Ui::DlgProjectSettings),
                                                                                            m_sPROJ_SETTINGS_FILE_PATH(sProjectFilePath)
{
    ui->setupUi(this);

    QFile projFile(m_sPROJ_SETTINGS_FILE_PATH);
    if(projFile.exists())
    {
        if(!projFile.open(QIODevice::ReadOnly))
            HyGuiLog("DlgProjectSettings::DlgProjectSettings() could not open " % m_sPROJ_SETTINGS_FILE_PATH % ": " % projFile.errorString(), LOGTYPE_Error);
    }
    else
        HyGuiLog("DlgProjectSettings::DlgProjectSettings() could not find the project file: " % m_sPROJ_SETTINGS_FILE_PATH, LOGTYPE_Error);

    QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
    projFile.close();

    m_SettingsObj = settingsDoc.object();

    if(MakeValid(m_SettingsObj) == false)
        SaveSettings();
}

DlgProjectSettings::~DlgProjectSettings()
{
    delete ui;
}

QJsonObject DlgProjectSettings::GetSettingsObj() const
{
    return m_SettingsObj;
}

bool DlgProjectSettings::MakeValid(QJsonObject &settingsObjRef)
{
    if(settingsObjRef.contains("GameName") == false ||
       settingsObjRef.contains("ClassName") == false ||
       settingsObjRef.contains("DataPath") == false ||
       settingsObjRef.contains("MetaDataPath") == false ||
       settingsObjRef.contains("SourcePath") == false)
    {
        HyGuiLog("DlgProjectSettings::CheckValidity() doesn't have the minimum requirements for a valid settings file", LOGTYPE_Error);
    }

    bool bIsValid = true;

    if(settingsObjRef.contains("DefaultCoordinateUnit") == false)
    {
        settingsObjRef.insert("DefaultCoordinateUnit", QJsonValue(1));
        bIsValid = false;
    }
    if(settingsObjRef.contains("UpdateFpsCap") == false)
    {
        settingsObjRef.insert("UpdateFpsCap", QJsonValue(0));
        bIsValid = false;
    }
    if(settingsObjRef.contains("PixelsPerMeter") == false)
    {
        settingsObjRef.insert("PixelsPerMeter", QJsonValue(80.0f));
        bIsValid = false;
    }
    if(settingsObjRef.contains("ShowCursor") == false)
    {
        settingsObjRef.insert("ShowCursor", QJsonValue(true));
        bIsValid = false;
    }
    if(settingsObjRef.contains("NumInputMappings") == false)
    {
        settingsObjRef.insert("NumInputMappings", QJsonValue(1));
        bIsValid = false;
    }
    if(settingsObjRef.contains("DebugPort") == false)
    {
        settingsObjRef.insert("DebugPort", QJsonValue(1313));
        bIsValid = false;
    }

    if(settingsObjRef.contains("UseConsole") == false)
    {
        settingsObjRef.insert("UseConsole", QJsonValue(true));
        bIsValid = false;
    }

    if(settingsObjRef.contains("ConsoleInfo") == false)
    {
        QJsonObject consoleInfoObj;
        consoleInfoObj.insert("Name", "Harmony Log Console");
        consoleInfoObj.insert("Type", QJsonValue(3));
        consoleInfoObj.insert("ResolutionX", QJsonValue(64));
        consoleInfoObj.insert("ResolutionY", QJsonValue(80));
        consoleInfoObj.insert("LocationX", QJsonValue(0));
        consoleInfoObj.insert("LocationY", QJsonValue(0));
        settingsObjRef.insert("ConsoleInfo", consoleInfoObj);
        bIsValid = false;
    }
    else
    {
        // TODO: Check contents of "ConsoleInfo"
        QJsonObject consoleInfoObj = settingsObjRef["ConsoleInfo"].toObject();
    }

    if(settingsObjRef.contains("WindowInfoArray") == false)
    {
        QJsonArray windowInfoArray;
        QJsonObject windowInfoObj;
        windowInfoObj.insert("Name", "Window 1");
        windowInfoObj.insert("Type", QJsonValue(0));
        windowInfoObj.insert("ResolutionX", QJsonValue(1280));
        windowInfoObj.insert("ResolutionY", QJsonValue(720));
        windowInfoObj.insert("LocationX", QJsonValue(0));
        windowInfoObj.insert("LocationY", QJsonValue(0));
        windowInfoArray.append(windowInfoObj);

        settingsObjRef.insert("WindowInfoArray", windowInfoArray);
        bIsValid = false;
    }
    else
    {
        // TODO: Check contents of "WindowInfoArray"
        QJsonArray consoleInfoObj = settingsObjRef["WindowInfoArray"].toArray();
    }

    return bIsValid;
}

void DlgProjectSettings::SetDefaults()
{
    for(auto iter = m_SettingsObj.begin(); iter != m_SettingsObj.end();)
    {
        if(iter.key() == "GameName" ||
           iter.key() == "ClassName" ||
           iter.key() == "DataPath" ||
           iter.key() == "MetaDataPath" ||
           iter.key() == "SourcePath")
        {
            ++iter;
            continue;
        }

        iter = m_SettingsObj.erase(iter);
    }

    MakeValid(m_SettingsObj);
}

void DlgProjectSettings::SaveSettings()
{
    QFile settingsFile(m_sPROJ_SETTINGS_FILE_PATH);
    if(settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false) {
       HyGuiLog(QString("Couldn't open ") % m_sPROJ_SETTINGS_FILE_PATH % " for writing: " % settingsFile.errorString(), LOGTYPE_Error);
    }
    else
    {
        QJsonDocument settingsDoc;
        settingsDoc.setObject(m_SettingsObj);
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten) {
            HyGuiLog(QString("Could not write to ") % m_sPROJ_SETTINGS_FILE_PATH % " file: " % settingsFile.errorString(), LOGTYPE_Error);
        }

        settingsFile.close();
    }
}

/*virtual*/ int DlgProjectSettings::exec() /*override*/
{
    MakeValid(m_SettingsObj);

    ui->txtTitleName->setText(m_SettingsObj["GameName"].toString());
    ui->txtClassName->setText(m_SettingsObj["ClassName"].toString());

    ui->txtAssetsLocation->setText(m_SettingsObj["DataPath"].toString());
    ui->txtMetaDataLocation->setText(m_SettingsObj["MetaDataPath"].toString());
    ui->txtSourceLocation->setText(m_SettingsObj["SourcePath"].toString());

    ui->sbInputMaps->setValue(m_SettingsObj["NumInputMappings"].toInt());
    ui->sbUpdateFpsCap->setValue(m_SettingsObj["UpdateFpsCap"].toInt());
    ui->sbPixelsPerMeter->setValue(m_SettingsObj["PixelsPerMeter"].toInt());
    ui->chkShowCursor->setChecked(m_SettingsObj["ShowCursor"].toBool());

    return QDialog::exec();
}
