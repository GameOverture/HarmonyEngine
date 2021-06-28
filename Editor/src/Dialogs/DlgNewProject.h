/**************************************************************************
 *	DlgNewProject.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGNEWPROJECT_H
#define DLGNEWPROJECT_H

#include "WgtSrcDependency.h"

#include <QDialog>

namespace Ui {
class DlgNewProject;
}

class DlgNewProject : public QDialog
{
	Q_OBJECT
	
public:
	explicit DlgNewProject(QString &sDefaultLocation, QWidget *pParent = nullptr);
	virtual ~DlgNewProject();

	QString GetProjFilePath();
	QString GetProjFileName();
	QString GetProjDirPath();

	bool IsCreatingGameDir();

private Q_SLOTS:

	void on_buttonBox_accepted();

	void on_btnBrowse_clicked();

	void on_txtGameLocation_textChanged(const QString &arg1);

	void on_txtTitleName_textChanged(const QString &arg1);

	void on_chkCreateGameDir_clicked();

private:
	Ui::DlgNewProject *ui;

	void UpdateProjectDir();

	void ErrorCheck();
};


#endif // DLGNEWPROJECT_H
