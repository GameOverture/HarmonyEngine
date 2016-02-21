#include "DlgNewProject.h"
#include "ui_DlgNewProject.h"

#include "HyGlobal.h"

#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DlgNewProject::DlgNewProject(QString &sDefaultLocation, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::DlgNewProject)
{
    ui->setupUi(this);
    ui->txtGameTitle->setText("NewGame");
    ui->txtGameTitle->setFocus();
    ui->txtGameTitle->selectAll();
    ui->txtGameTitle->setValidator(HyGlobal::FileNameValidator());// new QRegExpValidator(QRegExp("[A-Za-z0-9_]*"), this));

    ui->txtGameLocation->setText(sDefaultLocation);
    
    ui->txtAssetsDirName->setValidator(HyGlobal::FileNameValidator());
    ui->txtMetaDataDirName->setValidator(HyGlobal::FileNameValidator());
    ui->txtSourceDirName->setValidator(HyGlobal::FileNameValidator());
    
    m_sAbsoluteAssetsLocation = sDefaultLocation;
    m_sAbsoluteMetaDataLocation = sDefaultLocation;
    m_sAbsoluteSourceLocation = sDefaultLocation;
    
    SetRelativePaths();

    ErrorCheck();
}

DlgNewProject::~DlgNewProject()
{
    delete ui;
}

QString DlgNewProject::GetProjFilePath()
{
    if(ui->chkCreateGameDir->isChecked())
        return QDir::cleanPath(ui->txtGameLocation->text() + '/' + ui->txtGameTitle->text() + '/' + ui->txtGameTitle->text() + HyGlobal::ItemExt(ITEM_Project));
    else
        return QDir::cleanPath(ui->txtGameLocation->text() + '/' + ui->txtGameTitle->text() + HyGlobal::ItemExt(ITEM_Project));
}

QString DlgNewProject::GetProjDirPath()
{
    if(ui->chkCreateGameDir->isChecked())
        return QDir::cleanPath(ui->txtGameLocation->text() + '/' + ui->txtGameTitle->text() + '/');
    else
        return QDir::cleanPath(ui->txtGameLocation->text() + '/');
}

QString DlgNewProject::GetRelAssetsPath()
{
    return QDir::cleanPath(ui->txtAssetsLocation->text() + '/');
}

QString DlgNewProject::GetRelMetaDataPath()
{
    return QDir::cleanPath(ui->txtMetaDataLocation->text() + '/');
}

QString DlgNewProject::GetRelSourcePath()
{
    return QDir::cleanPath(ui->txtSourceLocation->text() + '/');
}

void DlgNewProject::on_buttonBox_accepted()
{
    QDir projDir(GetProjDirPath());
    projDir.mkpath(".");

    QString sRelAssetsPath = QDir::cleanPath(ui->txtAssetsLocation->text() % "/" % ui->txtAssetsDirName->text() % "/");
    QString sRelMetaDataPath = QDir::cleanPath(ui->txtMetaDataLocation->text() % "/" % ui->txtMetaDataDirName->text() % "/");
    QString sRelSourcePath = QDir::cleanPath(ui->txtSourceLocation->text() % "/" % ui->txtSourceDirName->text() % "/");

    QJsonObject jsonObj;
    jsonObj.insert("AssetsPath", sRelAssetsPath);
    jsonObj.insert("MetaDataPath", sRelMetaDataPath);
    jsonObj.insert("SourcePath", sRelSourcePath);

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
    
    // Create workspace file tree
    projDir.mkdir(sRelAssetsPath);
    projDir.cd(sRelAssetsPath);
    QStringList dirList = HyGlobal::SubDirNameList();
    foreach(QString sDir, dirList)
        projDir.mkdir(sDir);

    projDir.setPath(GetProjDirPath());
    projDir.mkdir(sRelMetaDataPath);
    projDir.cd(sRelMetaDataPath);
    projDir.mkdir("atlas/");

    projDir.setPath(GetProjDirPath());
    projDir.mkdir(sRelSourcePath);
    // TODO: Create code projects
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

void DlgNewProject::on_txtGameTitle_textChanged(const QString &arg1)
{
    ui->lblAppendHint->setText("Appends \"/" % ui->txtGameTitle->text() % "/\" to above");

    ErrorCheck();
}

void DlgNewProject::on_txtGameLocation_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewProject::SetRelativePaths()
{
    QDir rootLocation(GetProjDirPath());
    
    ui->txtAssetsLocation->setText(rootLocation.relativeFilePath(m_sAbsoluteAssetsLocation));
    if(ui->txtAssetsLocation->text().isEmpty())
    {
        ui->txtAssetsLocation->setText("./");
        m_sAbsoluteAssetsLocation = "";
    }
    
    ui->txtMetaDataLocation->setText(rootLocation.relativeFilePath(m_sAbsoluteMetaDataLocation));
    if(ui->txtMetaDataLocation->text().isEmpty())
    {
        ui->txtMetaDataLocation->setText("./");
        m_sAbsoluteMetaDataLocation = "";
    }
    
    ui->txtSourceLocation->setText(rootLocation.relativeFilePath(m_sAbsoluteSourceLocation));
    if(ui->txtSourceLocation->text().isEmpty())
    {
        ui->txtSourceLocation->setText("./");
        m_sAbsoluteSourceLocation = "";
    }
    
    ErrorCheck();
}

void DlgNewProject::ErrorCheck()
{
    bool bIsError = false;
    do
    {
        QDir rootDir(ui->txtGameLocation->text());
        if(rootDir.exists() == false)
        {
            ui->lblError->setText("Error: Workspace location does not exist.");
            bIsError = true;
            break;
        }

        if(ui->txtGameTitle->text().isEmpty())
        {
            ui->lblError->setText("Error: Game title cannot be blank.");
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
        m_sAbsoluteAssetsLocation = pDlg->selectedFiles()[0];
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
        m_sAbsoluteMetaDataLocation = pDlg->selectedFiles()[0];
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
        m_sAbsoluteSourceLocation = pDlg->selectedFiles()[0];
        SetRelativePaths();
    }
}

void DlgNewProject::on_txtAssetsLocation_textChanged(const QString &arg1)
{
    QDir assetsDir(ui->txtAssetsLocation->text());
    
    if(assetsDir.isAbsolute())
    {
        m_sAbsoluteAssetsLocation = ui->txtAssetsLocation->text();
        SetRelativePaths();
    }
    
    ErrorCheck();
}

void DlgNewProject::on_txtMetaDataLocation_textChanged(const QString &arg1)
{
    QDir metaDataDir(ui->txtMetaDataLocation->text());
    
    if(metaDataDir.isAbsolute())
    {
        m_sAbsoluteMetaDataLocation = ui->txtMetaDataLocation->text();
        SetRelativePaths();
    }
    
    ErrorCheck();
}

void DlgNewProject::on_txtSourceLocation_textChanged(const QString &arg1)
{
    QDir sourceDir(ui->txtSourceLocation->text());
    
    if(sourceDir.isAbsolute())
    {
        m_sAbsoluteSourceLocation = ui->txtSourceLocation->text();
        SetRelativePaths();
    }
    
    ErrorCheck();
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
