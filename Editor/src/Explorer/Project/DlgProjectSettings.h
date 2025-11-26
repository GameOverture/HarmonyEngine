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
class WgtWindowInfo;

class DlgProjectSettings : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;

	QList<WgtWindowInfo *>	m_WindowInfoList;

public:
	explicit DlgProjectSettings(Project &projectRef, QWidget *parent = 0);
	~DlgProjectSettings();

	QJsonObject SerializeWidgets();
	bool HasSettingsChanged();

	void RemoveWindowInfo(WgtWindowInfo *pWindowInfo);

private Q_SLOTS:
	void on_btnAddWindow_pressed();
	void on_txtTitle_textChanged(const QString &arg1);

private:
	Ui::DlgProjectSettings *ui;

	void ErrorCheck();
};

#endif // DLGPROJECTSETTINGS_H
