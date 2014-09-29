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

    ErrorCheck();
}

DlgNewProject::~DlgNewProject()
{
    delete ui;
}

QString DlgNewProject::GetProjPath()
{
    return QDir::cleanPath(ui->txtGameLocation->text() + '/' + ui->txtGameTitle->text());
}

void DlgNewProject::on_buttonBox_accepted()
{
    QDir projDir(ui->txtGameLocation->text());
    projDir.mkdir(ui->txtGameTitle->text());

    projDir.cd(ui->txtGameTitle->text());
    projDir.mkdir(QString("data"));
    projDir.mkdir(QString("metaData"));
    projDir.mkdir(QString("src"));

    projDir.cd("data");
    
    QStringList dirList = HyGlobal::SubDirNameList();
    foreach(QString sDir, dirList)
        projDir.mkdir(sDir);
}

void DlgNewProject::on_btnBrowse_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this, "Choose an empty directory for new game project");
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

void DlgNewProject::ErrorCheck()
{
    QString sFullPath = GetProjPath();
    
    bool bIsError = false;
    do
    {
        QDir gameLoc(ui->txtGameLocation->text());
        if(gameLoc.exists() == false)
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

        gameLoc.setPath(sFullPath);
        if(gameLoc.exists())
        {
            ui->lblError->setText("Error: Project with this name already exists at this location.");
            bIsError = true;
            break;
        }
    }while(false);

    if(bIsError)
        ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
    else
    {
        ui->lblError->setStyleSheet("QLabel { color : black; }");
        ui->lblError->setText(sFullPath + "is a valid project workspace.");
    }
    ui->lblError->setVisible(bIsError);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

