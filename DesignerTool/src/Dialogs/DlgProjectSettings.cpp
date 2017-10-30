#include "DlgProjectSettings.h"
#include "ui_DlgProjectSettings.h"

#include "MainWindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

QMap<QString, QJsonValue> DlgProjectSettings::sm_DefaultValues;

/*static*/ void DlgProjectSettings::InitDefaultValues()
{
    sm_DefaultValues["UpdateFpsCap"] = QJsonValue(0);
    sm_DefaultValues["PixelsPerMeter"] = QJsonValue(80.0f);
    sm_DefaultValues["ShowCursor"] = QJsonValue(true);
    sm_DefaultValues["NumInputMappings"] = QJsonValue(1);
    sm_DefaultValues["DebugPort"] = QJsonValue(1313);
    sm_DefaultValues["UseConsole"] = QJsonValue(true);

    QJsonObject consoleInfoObj;
    consoleInfoObj.insert("Name", "Harmony Log Console");
    consoleInfoObj.insert("Type", QJsonValue(3));
    consoleInfoObj.insert("ResolutionX", QJsonValue(64));
    consoleInfoObj.insert("ResolutionY", QJsonValue(80));
    consoleInfoObj.insert("LocationX", QJsonValue(0));
    consoleInfoObj.insert("LocationY", QJsonValue(0));
    sm_DefaultValues["ConsoleInfo"] = consoleInfoObj;

    QJsonArray windowInfoArray;
    QJsonObject windowInfoObj;
    windowInfoObj.insert("Name", "Window 1");
    windowInfoObj.insert("Type", QJsonValue(0));
    windowInfoObj.insert("ResolutionX", QJsonValue(1280));
    windowInfoObj.insert("ResolutionY", QJsonValue(720));
    windowInfoObj.insert("LocationX", QJsonValue(0));
    windowInfoObj.insert("LocationY", QJsonValue(0));
    windowInfoArray.append(windowInfoObj);
    sm_DefaultValues["WindowInfoArray"] = windowInfoArray;
}

DlgProjectSettings::DlgProjectSettings(const QString sProjectFilePath, QWidget *parent) :   QDialog(parent),
                                                                                            ui(new Ui::DlgProjectSettings),
                                                                                            m_sPROJ_SETTINGS_FILE_PATH(sProjectFilePath),
                                                                                            m_bHasError(false)
{
    ui->setupUi(this);

    QFile projFile(m_sPROJ_SETTINGS_FILE_PATH);
    if(projFile.exists())
    {
        if(!projFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("DlgProjectSettings::DlgProjectSettings() could not open\n" % m_sPROJ_SETTINGS_FILE_PATH % ": " % projFile.errorString(), LOGTYPE_Error);
            m_bHasError = true;
        }
    }
    else
    {
        HyGuiLog("DlgProjectSettings::DlgProjectSettings() could not find the project file:\n" % m_sPROJ_SETTINGS_FILE_PATH, LOGTYPE_Error);
        m_bHasError = true;
    }

    QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
    projFile.close();
    m_SettingsObj = settingsDoc.object();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Determine if the "source" .hyproj was opened, and if so, redirect to the proper .hyproj
    if(m_SettingsObj.contains("AdjustWorkingDirectory"))
    {
        QFileInfo tmpFile(m_sPROJ_SETTINGS_FILE_PATH);
        QString sAdjustedFilePath(tmpFile.absoluteDir().absoluteFilePath(m_SettingsObj["AdjustWorkingDirectory"].toString()));
        sAdjustedFilePath += "/" + tmpFile.fileName();
        tmpFile.setFile(sAdjustedFilePath);

        HyGuiLog("Wrong .hyproj specified. Don't use the .hyproj located in the source directory. Instead use the .hyproj found here:\n" % tmpFile.absoluteFilePath(), LOGTYPE_Warning);
        m_bHasError = true;
        return;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(MakeValid(m_SettingsObj) == false)
        SaveSettings();

    setWindowTitle(m_SettingsObj["GameName"].toString() % " Game Settings");
    setWindowIcon(HyGlobal::ItemIcon(ITEM_Project, SUBICON_Settings));
}

DlgProjectSettings::~DlgProjectSettings()
{
    delete ui;
}

bool DlgProjectSettings::HasError() const
{
    return m_bHasError;
}

QJsonObject DlgProjectSettings::GetSettingsObj() const
{
    return m_SettingsObj;
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
    if(settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog(QString("Couldn't open ") % m_sPROJ_SETTINGS_FILE_PATH % " for writing: " % settingsFile.errorString(), LOGTYPE_Error);
       m_bHasError = true;
    }
    else
    {
        QJsonDocument settingsDoc;
        settingsDoc.setObject(m_SettingsObj);
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog(QString("Could not write to ") % m_sPROJ_SETTINGS_FILE_PATH % " file: " % settingsFile.errorString(), LOGTYPE_Error);
            m_bHasError = true;
        }

        settingsFile.close();
    }
}

void DlgProjectSettings::InitWidgets(QJsonObject &settingsObjRef)
{
    ui->txtTitleName->setText(m_SettingsObj["GameName"].toString());
    ui->txtClassName->setText(m_SettingsObj["ClassName"].toString());

    ui->txtAssetsLocation->setText(m_SettingsObj["DataPath"].toString());
    ui->txtMetaDataLocation->setText(m_SettingsObj["MetaDataPath"].toString());
    ui->txtSourceLocation->setText(m_SettingsObj["SourcePath"].toString());

    ui->sbInputMaps->setValue(m_SettingsObj["NumInputMappings"].toInt());
    ui->sbUpdateFpsCap->setValue(m_SettingsObj["UpdateFpsCap"].toInt());
    ui->sbPixelsPerMeter->setValue(m_SettingsObj["PixelsPerMeter"].toInt());
    ui->chkShowCursor->setChecked(m_SettingsObj["ShowCursor"].toBool());
}

/*virtual*/ int DlgProjectSettings::exec() /*override*/
{
    MakeValid(m_SettingsObj);
    InitWidgets(m_SettingsObj);

    return QDialog::exec();
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
        m_bHasError = true;
    }

    bool bIsValid = true;
    for(auto iter = sm_DefaultValues.begin(); iter != sm_DefaultValues.end(); ++iter)
    {
        if(settingsObjRef.contains(iter.key()) == false)
        {
            settingsObjRef.insert(iter.key(), iter.value());
            bIsValid = false;
        }
        else if(iter.key() == "ConsoleInfo")
        {
            bool bIsConsoleInfoValid = true;
            QJsonObject currentConsoleInfoObj = settingsObjRef["ConsoleInfo"].toObject();
            QJsonObject defaultConsoleInfoObj = sm_DefaultValues["ConsoleInfo"].toObject();
            for(auto defaultConsoleIter = defaultConsoleInfoObj.begin(); defaultConsoleIter != defaultConsoleInfoObj.end(); ++defaultConsoleIter)
            {
                if(currentConsoleInfoObj.contains(defaultConsoleIter.key()) == false)
                {
                    bIsConsoleInfoValid = false;
                    bIsValid = false;
                    break;
                }
            }

            if(bIsConsoleInfoValid)
                settingsObjRef.insert(iter.key(), iter.value());    // Replaces "ConsoleInfo" with defaults
        }
        else if(iter.key() == "WindowInfoArray")
        {
            bool bIsWindowInfoValid = true;
            QJsonArray currentWindowInfoArray = settingsObjRef["WindowInfoArray"].toArray();
            QJsonObject defaultWindowInfoObj = sm_DefaultValues["WindowInfoArray"].toArray().at(0).toObject();
            for(int i = 0; i < currentWindowInfoArray.size(); ++i)
            {
                QJsonObject currentWindowInfoObj = currentWindowInfoArray[i].toObject();
                for(auto defaultWindowInfoObjIter = defaultWindowInfoObj.begin(); defaultWindowInfoObjIter != defaultWindowInfoObj.end(); ++defaultWindowInfoObjIter)
                {
                    if(currentWindowInfoObj.contains(defaultWindowInfoObjIter.key()) == false)
                    {
                        bIsWindowInfoValid = false;
                        bIsValid = false;
                        break;
                    }
                }
            }

            if(bIsWindowInfoValid)
                settingsObjRef.insert(iter.key(), iter.value());    // Replaces "WindowInfoArray" with defaults
        }
    }

    return bIsValid;
}
