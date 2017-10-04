/**************************************************************************
 *	DlgNewProject.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgNewProject.h"
#include "ui_DlgNewProject.h"

#include "Global.h"
#include "MainWindow.h"

#include "Harmony/Utilities/HyStrManip.h"

#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QUuid>

DlgNewProject::DlgNewProject(QString &sDefaultLocation, QWidget *parent) :  QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                            ui(new Ui::DlgNewProject)
{
    ui->setupUi(this);
    ui->txtTitleName->setText("NewGame");
    ui->txtTitleName->setFocus();
    ui->txtTitleName->selectAll();
    ui->txtTitleName->setValidator(HyGlobal::FileNameValidator());
    
    ui->txtClassName->setText("NewGame");
    ui->txtClassName->setValidator(HyGlobal::CodeNameValidator());

    ui->txtGameLocation->setText(sDefaultLocation);
    
    ui->txtAssetsDirName->setValidator(HyGlobal::FileNameValidator());
    ui->txtMetaDataDirName->setValidator(HyGlobal::FileNameValidator());
    ui->txtSourceDirName->setValidator(HyGlobal::FileNameValidator());
    
    m_sAbsoluteAssetsDirLocation = m_sAbsoluteMetaDataDirLocation = m_sAbsoluteSourceDirLocation = GetProjDirPath();
    SetRelativePaths();

    ErrorCheck();
}

DlgNewProject::~DlgNewProject()
{
    delete ui;
}

QString DlgNewProject::GetProjFilePath()
{
    return GetProjDirPath() % GetProjFileName();
}

QString DlgNewProject::GetProjFileName()
{
    return ui->txtTitleName->text() % HyGlobal::ItemExt(ITEM_Project);
}

QString DlgNewProject::GetProjDirPath()
{
    if(ui->chkCreateGameDir->isChecked())
        return ui->txtGameLocation->text() + '/' + ui->txtTitleName->text() + '/';
    else
        return ui->txtGameLocation->text() + '/';
}

bool DlgNewProject::IsCreatingGameDir()
{
    return ui->chkCreateGameDir->isChecked();
}

void DlgNewProject::on_buttonBox_accepted()
{
    QDir projDir(GetProjDirPath());
    projDir.mkpath(".");

    // Create workspace file tree
    //
    // DATA
    projDir.mkdir(ui->txtRelativeAssetsLocation->text());
    projDir.cd(ui->txtRelativeAssetsLocation->text());
    projDir.mkdir(HyGlobal::ItemName(DIR_Atlases));
    projDir.mkdir(HyGlobal::ItemName(DIR_AudioBanks));

    // META-DATA
    projDir.setPath(GetProjDirPath());
    projDir.mkdir(ui->txtRelativeMetaDataLocation->text());
    projDir.cd(ui->txtRelativeMetaDataLocation->text());
    projDir.mkdir(HyGlobal::ItemName(DIR_Atlases));

    // SOURCE
    projDir.setPath(GetProjDirPath());
    projDir.mkdir(ui->txtRelativeSourceLocation->text());
    projDir.cd(ui->txtRelativeSourceLocation->text());

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Insert the minimum required fields for settings file. The project's DlgProjectSettings will fill in the rest of the defaults
    QJsonObject jsonObj;
    jsonObj.insert("GameName", ui->txtTitleName->text());
    jsonObj.insert("ClassName", ui->txtClassName->text());
    jsonObj.insert("DataPath", QString(ui->txtRelativeAssetsLocation->text() + "/"));
    jsonObj.insert("MetaDataPath", QString(ui->txtRelativeMetaDataLocation->text() + "/"));
    jsonObj.insert("SourcePath", QString(ui->txtRelativeSourceLocation->text() + "/"));

    // Development .hyproj which sits in the src directory, has only one field which indicates the relative path to the actual settings file
    QJsonObject jsonObjForSrc;
    QDir srcDir(GetProjDirPath() % ui->txtRelativeSourceLocation->text());
    jsonObjForSrc.insert("AdjustWorkingDirectory", QJsonValue(srcDir.relativeFilePath(GetProjDirPath())));
    //jsonObjForSrc.insert("DataPath", QJsonValue(srcDir.relativeFilePath(GetProjDirPath()) % sRelDataPath));
    //jsonObjForSrc.insert("MetaDataPath", QJsonValue(srcDir.relativeFilePath(GetProjDirPath()) % sRelMetaDataPath));
    //jsonObjForSrc.insert("SourcePath", QJsonValue(srcDir.relativeFilePath(GetProjDirPath()) % sRelSourcePath));

    QFile newProjectFile(GetProjFilePath());
    if(newProjectFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open new project file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonDocument newProjectDoc(jsonObj);
        qint64 iBytesWritten = newProjectFile.write(newProjectDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write new project file: " % newProjectFile.errorString(), LOGTYPE_Error);
        }

        newProjectFile.close();
    }

    QFile newProjectFileForSrc(GetProjDirPath() % ui->txtRelativeSourceLocation->text() % "/" % GetProjFileName());
    if(newProjectFileForSrc.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open new project file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonDocument newProjectDoc(jsonObjForSrc);
        qint64 iBytesWritten = newProjectFileForSrc.write(newProjectDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write new project file: " % newProjectFileForSrc.errorString(), LOGTYPE_Error);
        }

        newProjectFileForSrc.close();
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////
    QList<QDir> templateDirList;
    if(ui->chkVs2013->isChecked())
    {
        QDir templateDir(MainWindow::EngineLocation() % "templates");
        templateDir.cd("vs2013");

        templateDirList.append(templateDir);
    }

    if(ui->chkVs2015->isChecked())
    {
        QDir templateDir(MainWindow::EngineLocation() % "templates");
        templateDir.cd("vs2015");

        templateDirList.append(templateDir);
    }

    if(ui->chkVs2017->isChecked())
    {
        QDir templateDir(MainWindow::EngineLocation() % "templates");
        templateDir.cd("vs2017");

        templateDirList.append(templateDir);
    }

    QDir templateDir(MainWindow::EngineLocation() % "templates");
    templateDir.cd("common");
    templateDirList.append(templateDir);
    
    for(int iTemplateIndex = 0; iTemplateIndex < templateDirList.size(); ++iTemplateIndex)
    {
        QFileInfoList templateContentsList = templateDirList[iTemplateIndex].entryInfoList();
        for(int i = 0; i < templateContentsList.size(); ++i)
            QFile::copy(templateContentsList[i].absoluteFilePath(), projDir.absoluteFilePath(templateContentsList[i].fileName()));

        // Convert the template to use the desired game name
        //
        // Rename the files themselves
        QFileInfoList srcFileList = projDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
        for(int i = 0; i < srcFileList.size(); ++i)
        {
            if(srcFileList[i].fileName().contains("HyTitle"))
            {
                QFile file(srcFileList[i].absoluteFilePath());
                QString sNewFileName = srcFileList[i].fileName().replace("HyTitle", ui->txtTitleName->text() % "_" % templateDirList[iTemplateIndex].dirName());
                file.rename(srcFileList[i].absoluteDir().absolutePath() % "/" % sNewFileName);
                file.close();
            }

            if(srcFileList[i].fileName().contains("HyTemplate"))
            {
                QFile file(srcFileList[i].absoluteFilePath());
                QString sNewFileName = srcFileList[i].fileName().replace("HyTemplate", ui->txtClassName->text());
                file.rename(srcFileList[i].absoluteDir().absolutePath() % "/" % sNewFileName);
                file.close();
            }
        }
        // Then replace the contents
        QUuid projGUID = QUuid::createUuid();
        srcFileList = projDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
        QTextCodec *pCodec = QTextCodec::codecForLocale();
        for(int i = 0; i < srcFileList.size(); ++i)
        {
            QFile file(srcFileList[i].absoluteFilePath());
            if(!file.open(QFile::ReadOnly))
            {
                HyGuiLog("Error reading " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
                return;
            }

            QString sContents = pCodec->toUnicode(file.readAll());
            file.close();

            //QDir exeDir(GetProjDirPath() % "bin/");

            sContents.replace("[HyTitle]", ui->txtTitleName->text());
            sContents.replace("HyTemplate", ui->txtClassName->text());
            sContents.replace("HyProjGUID", projGUID.toString());
            sContents.replace("[HyHarmonyProjLocation]", srcFileList[i].dir().relativeFilePath(MainWindow::EngineLocation()) % "/");
            sContents.replace("HyHarmonyInclude", srcFileList[i].dir().relativeFilePath(MainWindow::EngineLocation() % "include"));
            sContents.replace("HyWorkingDirectory", projDir.relativeFilePath(GetProjDirPath()) % "/");

            if(!file.open(QFile::WriteOnly))
            {
                HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
                return;
            }
            file.write(pCodec->fromUnicode(sContents));
            file.close();
        }
    }
}

void DlgNewProject::on_btnBrowse_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose the \"root\" location of the game project where the game project file will appear");
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        QString sDir = pDlg->selectedFiles()[0];
        ui->txtGameLocation->setText(sDir);
    }
}

void DlgNewProject::on_txtGameLocation_textChanged(const QString &arg1)
{
    QDir sRelativeAssetsLocation(ui->txtRelativeAssetsLocation->text());
    sRelativeAssetsLocation.cdUp();

    QDir sRelativeMetaDataLocation(ui->txtRelativeMetaDataLocation->text());
    sRelativeMetaDataLocation.cdUp();

    QDir sRelativeSourceLocation(ui->txtRelativeSourceLocation->text());
    sRelativeSourceLocation.cdUp();

    // Update the asset/meta/src directories in attempt to stay relative to new directory
    QString sProjDirPath = GetProjDirPath();
    m_sAbsoluteAssetsDirLocation    = QDir::cleanPath(sProjDirPath % "/" % sRelativeAssetsLocation.path());
    m_sAbsoluteMetaDataDirLocation  = QDir::cleanPath(sProjDirPath % "/" % sRelativeMetaDataLocation.path());
    m_sAbsoluteSourceDirLocation    = QDir::cleanPath(sProjDirPath % "/" % sRelativeSourceLocation.path());

    // SetRelativePaths should technically still be correct, but calling it anyway
    SetRelativePaths();
    ErrorCheck();
}

void DlgNewProject::SetRelativePaths()
{
    QDir rootLocation(GetProjDirPath());
    
    ui->txtRelativeAssetsLocation->setText(rootLocation.relativeFilePath(QDir::cleanPath(m_sAbsoluteAssetsDirLocation % "/" % ui->txtAssetsDirName->text())));
    ui->txtRelativeMetaDataLocation->setText(rootLocation.relativeFilePath(QDir::cleanPath(m_sAbsoluteMetaDataDirLocation % "/" % ui->txtMetaDataDirName->text())));
    ui->txtRelativeSourceLocation->setText(rootLocation.relativeFilePath(QDir::cleanPath(m_sAbsoluteSourceDirLocation % "/" % ui->txtSourceDirName->text())));

    ErrorCheck();
}

void DlgNewProject::ErrorCheck()
{
    bool bIsError = false;
    do
    {
        QDir rootDir(ui->txtGameLocation->text());

        if(ui->txtTitleName->text().isEmpty())
        {
            ui->lblError->setText("Error: Game title cannot be blank.");
            bIsError = true;
            break;
        }
        
        if(ui->txtClassName->text().isEmpty())
        {
            ui->lblError->setText("Error: Class name cannot be blank.");
            bIsError = true;
            break;
        }

        QFile projFile(GetProjFilePath());
        if(projFile.exists())
        {
            ui->lblError->setText("Error: Project with this name already exists at this location.");
            bIsError = true;
            break;
        }
        
        rootDir.setPath(GetProjDirPath());


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        QDir assetsDirLocation(m_sAbsoluteAssetsDirLocation);
        if(assetsDirLocation.exists() == false)
        {
            ui->lblError->setText("Error: Assets location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        if(ui->txtAssetsDirName->text().isEmpty())
        {
            ui->lblError->setText("Error: Assets directory name cannot be blank");
            bIsError = true;
            break;
        }
        if(assetsDirLocation.exists(ui->txtAssetsDirName->text()))
        {
            ui->lblError->setText("Error: A directory at the assets location already has the name: " + ui->txtAssetsDirName->text());
            bIsError = true;
            break;
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        QDir metaDataDirLocation(m_sAbsoluteMetaDataDirLocation);
        if(metaDataDirLocation.exists() == false)
        {
            ui->lblError->setText("Error: Meta-data location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        if(ui->txtMetaDataDirName->text().isEmpty())
        {
            ui->lblError->setText("Error: Meta-data directory name cannot be blank");
            bIsError = true;
            break;
        }
        if(metaDataDirLocation.exists(ui->txtMetaDataDirName->text()))
        {
            ui->lblError->setText("Error: A directory at the meta-data location already has the name: " + ui->txtMetaDataDirName->text());
            bIsError = true;
            break;
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        QDir sourceDirLocation(m_sAbsoluteSourceDirLocation);
        if(sourceDirLocation.exists() == false)
        {
            ui->lblError->setText("Error: Source code location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        if(ui->txtSourceDirName->text().isEmpty())
        {
            ui->lblError->setText("Error: Source code directory name cannot be blank");
            bIsError = true;
            break;
        }
        if(sourceDirLocation.exists(ui->txtSourceDirName->text()))
        {
            ui->lblError->setText("Error: A directory at the source code location already has the name: " + ui->txtSourceDirName->text());
            bIsError = true;
            break;
        }
        
    }while(false);

    if(bIsError)
        ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
    else
    {
        ui->lblError->setStyleSheet("QLabel { color : black; }");
        ui->lblError->setText(GetProjFilePath() + "is a valid project workspace.");
    }
    ui->lblError->setVisible(bIsError);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}


void DlgNewProject::on_btnBrowseAssets_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's assets directory will be *created* at");
    pDlg->setDirectory(m_sAbsoluteAssetsDirLocation);
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        m_sAbsoluteAssetsDirLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_btnBrowseMetaData_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's meta-data directory will be *created* at");
    pDlg->setDirectory(m_sAbsoluteMetaDataDirLocation);
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        m_sAbsoluteMetaDataDirLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_btnBrowseSource_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's source code directory will be *created* at");
    pDlg->setDirectory(m_sAbsoluteSourceDirLocation);
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        m_sAbsoluteSourceDirLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_chkCreateGameDir_clicked()
{
    on_txtGameLocation_textChanged(QString());
}

void DlgNewProject::on_txtTitleName_textChanged(const QString &arg1)
{
    ui->lblAppendHint->setText("Appends \"/" % ui->txtTitleName->text() % "/\" to above");
    ErrorCheck();
}

void DlgNewProject::on_txtAssetsDirName_textChanged(const QString &arg1)
{
    SetRelativePaths();
}

void DlgNewProject::on_txtMetaDataDirName_textChanged(const QString &arg1)
{
    SetRelativePaths();
}

void DlgNewProject::on_txtSourceDirName_textChanged(const QString &arg1)
{
    SetRelativePaths();
}
