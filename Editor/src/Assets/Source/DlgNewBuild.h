/**************************************************************************
 *	DlgNewBuild.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgNewBuild_H
#define DlgNewBuild_H

#include <QDialog>

namespace Ui {
class DlgNewBuild;
}

class DlgNewBuild : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;
	QString					m_sCMakeHelp;
	bool					m_bCMakeFound;

public:
	explicit DlgNewBuild(Project &projectRef, QWidget *parent = 0);
	~DlgNewBuild();

	QString GetAbsBuildDir() const;

	QStringList GetProcOptions() const;

private Q_SLOTS:

	void on_txtBuildName_textChanged(const QString &arg1);

	void on_buttonBox_accepted();

	void on_radCMake_clicked();
	void on_radCMakeGui_clicked();

	void on_cmbCMake_currentIndexChanged(int iIndex);

	void on_btnCMakeHelp_clicked();

private:
	Ui::DlgNewBuild *ui;

	void ErrorCheck();
};


#endif // DlgNewBuild_H
