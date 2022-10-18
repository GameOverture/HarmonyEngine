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

#include "WgtSrcDependency.h"

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DlgProjectSettings;
}

class Project;

class DlgProjectSettings : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;

public:
	explicit DlgProjectSettings(Project &projectRef, QWidget *parent = 0);
	~DlgProjectSettings();

	bool HasSettingsChanged();
	QJsonObject GetNewSettingsObj();

private Q_SLOTS:
	void on_buttonBox_accepted();

private:
	Ui::DlgProjectSettings *ui;

	void ErrorCheck();
};

#endif // DLGPROJECTSETTINGS_H
