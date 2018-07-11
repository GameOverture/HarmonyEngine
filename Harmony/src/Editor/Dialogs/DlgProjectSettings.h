/**************************************************************************
*	DlgProjectSettings.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
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

	bool                                m_bHasError;

public:
	static void InitDefaultValues();

	explicit DlgProjectSettings(const QString sProjectFilePath, QWidget *parent = 0);
	~DlgProjectSettings();

	bool HasError() const;

	QJsonObject GetSettingsObj() const;

	void SetDefaults();

	void SaveSettings();

	void InitWidgets(QJsonObject &settingsObjRef);

public Q_SLOTS:
	virtual int exec() override;

private:
	Ui::DlgProjectSettings *ui;

	bool MakeValid(QJsonObject &settingsObjRef);
};

#endif // DLGPROJECTSETTINGS_H
