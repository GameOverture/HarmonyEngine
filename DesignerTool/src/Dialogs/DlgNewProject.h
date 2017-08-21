/**************************************************************************
 *	DlgNewProject.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
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
    
    QString               m_sRelativeAssetsLocation;
    QString               m_sRelativeMetaDataLocation;
    QString               m_sRelativeSourceLocation;

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

    void on_btnBrowseAssets_clicked();
    
    void on_btnBrowseMetaData_clicked();
    
    void on_btnBrowseSource_clicked();
    
    void on_txtAssetsDirName_textChanged(const QString &arg1);

    void on_txtMetaDataDirName_textChanged(const QString &arg1);

    void on_txtSourceDirName_textChanged(const QString &arg1);

    void on_chkCreateGameDir_clicked();

    void on_txtAssetsLocation_textEdited(const QString &arg1);

    void on_txtMetaDataLocation_textEdited(const QString &arg1);

    void on_txtSourceLocation_textEdited(const QString &arg1);

    void on_txtTitleName_textChanged(const QString &arg1);
    
private:
    Ui::DlgNewProject *ui;
    
    void SetRelativePaths();
};


#endif // DLGNEWPROJECT_H