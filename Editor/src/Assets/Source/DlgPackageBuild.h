/**************************************************************************
 *	DlgPackageBuild.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgPackageBuild_H
#define DlgPackageBuild_H

#include <QDialog>

namespace Ui {
class DlgPackageBuild;
}

class DlgPackageBuild : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;

public:
	explicit DlgPackageBuild(Project &projectRef, QWidget *parent = 0);
	~DlgPackageBuild();

	PackageType GetPackageType() const;
	QString GetAbsPackageDir() const;

	QString GetProc() const;
	QStringList GetProcOptions() const;

private Q_SLOTS:

	void on_buttonBox_accepted();

	void on_radDesktop_clicked();
	void on_radBrowser_clicked();

	void on_txtEmscriptenSdk_textChanged(const QString &arg1);
	void on_btnEmscriptenSdkBrowse_clicked();

private:
	Ui::DlgPackageBuild *ui;

	void ErrorCheck();
};


#endif // DlgPackageBuild_H
