/**************************************************************************
 *	DlgNewProject.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGNEWPROJECT_H
#define DLGNEWPROJECT_H

#include <QDialog>

namespace Ui {
class DlgNewProject;
}

class DlgNewProject : public QDialog
{
	Q_OBJECT
	
	QString               m_sAbsoluteAssetsDirLocation;     // Just the absolute location of where the assets directory resides, not the directory itself
	QString               m_sAbsoluteMetaDataDirLocation;   // Just the absolute location of where the meta-data directory resides, not the directory itself
	QString               m_sAbsoluteSourceDirLocation;     // Just the absolute location of where the source directory resides, not the directory itself

public:
	explicit DlgNewProject(QString &sDefaultLocation, QWidget *parent = 0);
	~DlgNewProject();

	QString GetProjFilePath();
	QString GetProjFileName();
	QString GetProjDirPath();

	bool IsCreatingGameDir();

private Q_SLOTS:

	void ErrorCheck();

	void on_buttonBox_accepted();

	void on_btnBrowse_clicked();

	void on_txtGameLocation_textChanged(const QString &arg1);

	void on_txtTitleName_textChanged(const QString &arg1);

	void on_btnBrowseAssets_clicked();
	
	void on_btnBrowseMetaData_clicked();
	
	void on_btnBrowseSource_clicked();

	void on_chkCreateGameDir_clicked();
	
	void on_txtAssetsDirName_textChanged(const QString &arg1);

	void on_txtMetaDataDirName_textChanged(const QString &arg1);

	void on_txtSourceDirName_textChanged(const QString &arg1);

private:
	Ui::DlgNewProject *ui;
	
	void UpdateAbsoluteDirLocations();
	void SetRelativePaths();
};


#endif // DLGNEWPROJECT_H
