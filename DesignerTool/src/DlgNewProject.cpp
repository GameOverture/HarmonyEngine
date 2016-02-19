#include "DlgNewProject.h"
#include "ui_DlgNewProject.h"

#include "HyGlobal.h"

#include <QDir>
#include <QFileDialog>

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

QString DlgNewProject::GetProjPath()
{
    return QDir::cleanPath(ui->txtGameLocation->text() + '/' + ui->txtGameTitle->text() + HyGlobal::ItemExt(ITEM_Project));
}

void DlgNewProject::on_buttonBox_accepted()
{
    //
    //
    // TODO: Create the workspace based on new relative directories
    //
    //
    
    // Create workspace file tree
    QDir projDir(ui->txtGameLocation->text());
    
    projDir.mkdir(ui->txtGameTitle->text());
    projDir.cd(ui->txtGameTitle->text());
    
    projDir.mkdir(HYGUIPATH_RelDataDir);
    projDir.mkdir(HYGUIPATH_RelDataAtlasDir);
    projDir.mkdir(HYGUIPATH_RelMetaDataDir);
    projDir.mkdir(HYGUIPATH_RelMetaAtlasDir);
    
    // TODO: Create code projects
    projDir.mkdir(HYGUIPATH_RelSrcDataDir);

    projDir.cd(HYGUIPATH_RelDataDir);
    
    QStringList dirList = HyGlobal::SubDirNameList();
    foreach(QString sDir, dirList)
        projDir.mkdir(sDir);
    
    projDir.cd(ui->txtGameLocation->text());
    projDir.cd(ui->txtGameTitle->text());
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
    ErrorCheck();
}

void DlgNewProject::on_txtGameLocation_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewProject::SetRelativePaths()
{
    QDir rootLocation(ui->txtGameLocation->text());
    
    ui->txtAssetsLocation->setText(rootLocation.relativeFilePath(m_sAbsoluteAssetsLocation));
    if(ui->txtAssetsLocation->text().isEmpty())
        ui->txtAssetsLocation->setText("./");
    
    ui->txtMetaDataLocation->setText(rootLocation.relativeFilePath(m_sAbsoluteMetaDataLocation));
    if(ui->txtMetaDataLocation->text().isEmpty())
        ui->txtMetaDataLocation->setText("./");
    
    ui->txtSourceLocation->setText(rootLocation.relativeFilePath(m_sAbsoluteSourceLocation));
    if(ui->txtSourceLocation->text().isEmpty())
        ui->txtSourceLocation->setText("./");
    
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

        QFile projFile(GetProjPath());
        if(projFile.exists())
        {
            ui->lblError->setText("Error: Project with this name already exists at this location.");
            bIsError = true;
            break;
        }
        
        if(rootDir.exists(ui->txtAssetsLocation->text()) == false)
        {
            ui->lblError->setText("Error: Assets location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        
        if(rootDir.exists(ui->txtMetaDataLocation->text()) == false)
        {
            ui->lblError->setText("Error: Meta-data location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        
        if(rootDir.exists(ui->txtSourceLocation->text()) == false)
        {
            ui->lblError->setText("Error: Source code location (relative to project) does not exist.");
            bIsError = true;
            break;
        }
        
    }while(false);

    if(bIsError)
        ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
    else
    {
        ui->lblError->setStyleSheet("QLabel { color : black; }");
        ui->lblError->setText(GetProjPath() + "is a valid project workspace.");
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
