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

#include "HyGuiGlobal.h"
#include "MainWindow.h"

#include "Harmony/Utilities/HyStrManip.h"

#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QUuid>

DlgNewProject::DlgNewProject(QString &sDefaultLocation, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
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
    
    m_sRelativeAssetsLocation.clear();
    m_sRelativeMetaDataLocation.clear();
    m_sRelativeSourceLocation.clear();
    
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

    QString sRelDataPath = QDir::cleanPath(ui->txtAssetsLocation->text() % "/" % ui->txtAssetsDirName->text() % "/");
    QString sRelMetaDataPath = QDir::cleanPath(ui->txtMetaDataLocation->text() % "/" % ui->txtMetaDataDirName->text() % "/");
    QString sRelSourcePath = QDir::cleanPath(ui->txtSourceLocation->text() % "/" % ui->txtSourceDirName->text() % "/");

    // Create workspace file tree
    //
    // DATA
    projDir.mkdir(sRelDataPath);
    projDir.cd(sRelDataPath);
    projDir.mkdir(HyGlobal::ItemName(ITEM_DirAtlases));
    projDir.mkdir(HyGlobal::ItemName(ITEM_DirAudioBanks));

    // META-DATA
    projDir.setPath(GetProjDirPath());
    projDir.mkdir(sRelMetaDataPath);
    projDir.cd(sRelMetaDataPath);
    projDir.mkdir(HyGlobal::ItemName(ITEM_DirAtlases));

    // SOURCE
    projDir.setPath(GetProjDirPath());
    projDir.mkdir(sRelSourcePath);
    projDir.cd(sRelSourcePath);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    QJsonObject jsonObj;
    jsonObj.insert("GameName", ui->txtTitleName->text());
    jsonObj.insert("ClassName", ui->txtClassName->text());
    jsonObj.insert("DataPath", sRelDataPath);
    jsonObj.insert("MetaDataPath", sRelMetaDataPath);
    jsonObj.insert("SourcePath", sRelSourcePath);
    
    jsonObj.insert("DefaultCoordinateUnit", QJsonValue(1));
    jsonObj.insert("UpdateFpsCap", QJsonValue(0));
    jsonObj.insert("PixelsPerMeter", QJsonValue(80.0f));
    jsonObj.insert("ShowCursor", QJsonValue(true));
	jsonObj.insert("NumInputMappings", QJsonValue(1));
	jsonObj.insert("DebugPort", QJsonValue(1313));
    
    jsonObj.insert("UseConsole", QJsonValue(true));
    QJsonObject consoleInfoObj;
    consoleInfoObj.insert("Name", "Harmony Log Console");
    consoleInfoObj.insert("Type", QJsonValue(3));
    consoleInfoObj.insert("ResolutionX", QJsonValue(64));
    consoleInfoObj.insert("ResolutionY", QJsonValue(80));
    consoleInfoObj.insert("LocationX", QJsonValue(0));
    consoleInfoObj.insert("LocationY", QJsonValue(0));
    jsonObj.insert("ConsoleInfo", consoleInfoObj);

    QJsonArray windowInfoArray;
    QJsonObject windowInfoObj;
    windowInfoObj.insert("Name", "Window 1");
    windowInfoObj.insert("Type", QJsonValue(0));
    windowInfoObj.insert("ResolutionX", QJsonValue(1280));
    windowInfoObj.insert("ResolutionY", QJsonValue(720));
    windowInfoObj.insert("LocationX", QJsonValue(0));
    windowInfoObj.insert("LocationY", QJsonValue(0));
    windowInfoArray.append(windowInfoObj);
    
    jsonObj.insert("WindowInfoArray", windowInfoArray);

    QJsonObject jsonObjForSrc = jsonObj;
    QDir srcDir(GetProjDirPath() % sRelSourcePath);
    jsonObjForSrc.insert("DataPath", QJsonValue(srcDir.relativeFilePath(GetProjDirPath()) % sRelDataPath));
    jsonObjForSrc.insert("MetaDataPath", QJsonValue(srcDir.relativeFilePath(GetProjDirPath()) % sRelMetaDataPath));
    jsonObjForSrc.insert("SourcePath", QJsonValue(srcDir.relativeFilePath(GetProjDirPath()) % sRelSourcePath));

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

    QFile newProjectFileForSrc(GetProjDirPath() % sRelSourcePath % "/" % GetProjFileName());
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
    ErrorCheck();
}

void DlgNewProject::SetRelativePaths()
{
    QDir assetsDir(m_sRelativeAssetsLocation);
    if(assetsDir.isAbsolute())
    {
        QDir rootLocation(GetProjDirPath());
        m_sRelativeAssetsLocation = rootLocation.relativeFilePath(m_sRelativeAssetsLocation);
    }
    ui->txtAssetsLocation->setText(m_sRelativeAssetsLocation);
    if(ui->txtAssetsLocation->text().isEmpty())
    {
        ui->txtAssetsLocation->setText("./");
        m_sRelativeAssetsLocation.clear();
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QDir metaDir(m_sRelativeMetaDataLocation);
    if(metaDir.isAbsolute())
    {
        QDir rootLocation(GetProjDirPath());
        m_sRelativeMetaDataLocation = rootLocation.relativeFilePath(m_sRelativeMetaDataLocation);
    }
    ui->txtMetaDataLocation->setText(m_sRelativeMetaDataLocation);
    if(ui->txtMetaDataLocation->text().isEmpty())
    {
        ui->txtMetaDataLocation->setText("./");
        m_sRelativeMetaDataLocation = "";
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QDir sourceDir(m_sRelativeSourceLocation);
    if(sourceDir.isAbsolute())
    {
        QDir rootLocation(GetProjDirPath());
        m_sRelativeSourceLocation = rootLocation.relativeFilePath(m_sRelativeSourceLocation);
    }
    ui->txtSourceLocation->setText(m_sRelativeSourceLocation);
    if(ui->txtSourceLocation->text().isEmpty())
    {
        ui->txtSourceLocation->setText("./");
        m_sRelativeSourceLocation = "";
    }
    
    ErrorCheck();
}

void DlgNewProject::on_txtAssetsLocation_textEdited(const QString &arg1)
{
    m_sRelativeAssetsLocation = ui->txtAssetsLocation->text();

    SetRelativePaths();
    ErrorCheck();
}

void DlgNewProject::on_txtMetaDataLocation_textEdited(const QString &arg1)
{
    m_sRelativeMetaDataLocation = ui->txtMetaDataLocation->text();

    SetRelativePaths();
    ErrorCheck();
}

void DlgNewProject::on_txtSourceLocation_textEdited(const QString &arg1)
{
    m_sRelativeSourceLocation = ui->txtSourceLocation->text();

    SetRelativePaths();
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
        if((ui->chkCreateGameDir->isChecked() == false && rootDir.exists(ui->txtAssetsLocation->text()) == false) ||
           (ui->chkCreateGameDir->isChecked() && ui->txtAssetsLocation->text().left(3) == "../" && rootDir.exists(ui->txtAssetsLocation->text()) == false))
        {
            ui->lblError->setText("Error: Assets location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        if(rootDir.exists(QDir::cleanPath(ui->txtAssetsLocation->text() + "/" + ui->txtAssetsDirName->text())))
        {
            ui->lblError->setText("Error: A directory at the assets location already has the name: " + ui->txtAssetsDirName->text());
            bIsError = true;
            break;
        }
        if(ui->txtAssetsDirName->text().isEmpty())
        {
            ui->lblError->setText("Error: Assets directory name cannot be blank");
            bIsError = true;
            break;
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if((ui->chkCreateGameDir->isChecked() == false && rootDir.exists(ui->txtMetaDataLocation->text()) == false) ||
           (ui->chkCreateGameDir->isChecked() && ui->txtMetaDataLocation->text().left(3) == "../" && rootDir.exists(ui->txtMetaDataLocation->text()) == false))
        {
            ui->lblError->setText("Error: Meta-data location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        if(rootDir.exists(QDir::cleanPath(ui->txtMetaDataLocation->text() + "/" + ui->txtMetaDataDirName->text())))
        {
            ui->lblError->setText("Error: A directory at the meta-data location already has the name: " + ui->txtMetaDataDirName->text());
            bIsError = true;
            break;
        }
        if(ui->txtMetaDataDirName->text().isEmpty())
        {
            ui->lblError->setText("Error: Meta-data directory name cannot be blank");
            bIsError = true;
            break;
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if((ui->chkCreateGameDir->isChecked() == false && rootDir.exists(ui->txtSourceLocation->text()) == false) ||
           (ui->chkCreateGameDir->isChecked() && ui->txtSourceLocation->text().left(3) == "../" && rootDir.exists(ui->txtSourceLocation->text()) == false))
        {
            ui->lblError->setText("Error: Source code location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        if(rootDir.exists(QDir::cleanPath(ui->txtSourceLocation->text() + "/" + ui->txtSourceDirName->text())))
        {
            ui->lblError->setText("Error: A directory at the source code location already has the name: " + ui->txtSourceDirName->text());
            bIsError = true;
            break;
        }
        if(ui->txtSourceDirName->text().isEmpty())
        {
            ui->lblError->setText("Error: Source code directory name cannot be blank");
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
    QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's assets directory will be located");
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        m_sRelativeAssetsLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_btnBrowseMetaData_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's meta-data directory will be located");
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        m_sRelativeMetaDataLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_btnBrowseSource_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose where this game's source code directory will be located");
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        m_sRelativeSourceLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_txtAssetsDirName_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewProject::on_txtMetaDataDirName_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewProject::on_txtSourceDirName_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewProject::on_chkCreateGameDir_clicked()
{
    SetRelativePaths();
    ErrorCheck();
}

void DlgNewProject::on_txtTitleName_textChanged(const QString &arg1)
{
    ui->lblAppendHint->setText("Appends \"/" % ui->txtTitleName->text() % "/\" to above");
    ErrorCheck();
}
