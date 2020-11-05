/**************************************************************************
 *	DlgNewPackage.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgNewPackage_H
#define DlgNewPackage_H

#include <QDialog>

namespace Ui {
class DlgNewPackage;
}

class DlgNewPackage : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;
	QString					m_sCMakeHelp;
	bool					m_bCMakeFound;

	enum PlatformPage
	{
		PLAT_Desktop = 0,
		PLAT_Browser
	};

public:
	explicit DlgNewPackage(Project &projectRef, QWidget *parent = 0);
	~DlgNewPackage();

	QString GetProc() const;
	QStringList GetProcOptions() const;

private Q_SLOTS:

	void on_buttonBox_accepted();

	void on_radDesktop_clicked();
	void on_radBrowser_clicked();

	void on_radCMake_clicked();
	void on_radCMakeGui_clicked();

	void on_cmbCMake_currentIndexChanged(int iIndex);

	void on_btnCMakeHelp_clicked();

	void on_txtEmscriptenSdk_textChanged(const QString &arg1);
	void on_btnEmscriptenSdkBrowse_clicked();

private:
	Ui::DlgNewPackage *ui;

	void ErrorCheck();
};


#endif // DlgNewPackage_H
