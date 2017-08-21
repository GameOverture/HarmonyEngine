#ifndef DLGPROJECTSETTINGS_H
#define DLGPROJECTSETTINGS_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DlgProjectSettings;
}

class DlgProjectSettings : public QDialog
{
    Q_OBJECT

    const QString                       m_sPROJ_SETTINGS_FILE_PATH;
    QJsonObject                         m_SettingsObj;

    static QMap<QString, QJsonValue>    sm_DefaultValues;

public:
    static void InitDefaultValues();

    explicit DlgProjectSettings(const QString sProjectFilePath, QWidget *parent = 0);
    ~DlgProjectSettings();

    QJsonObject GetSettingsObj() const;

    bool MakeValid(QJsonObject &settingsObjRef);

    void SetDefaults();

    void SaveSettings();

    void InitWidgets(QJsonObject &settingsObjRef);

public Q_SLOTS:
    virtual int exec() override;

private:
    Ui::DlgProjectSettings *ui;
};

#endif // DLGPROJECTSETTINGS_H