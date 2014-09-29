#ifndef DLGNEWPROJECT_H
#define DLGNEWPROJECT_H

#include <QDialog>

namespace Ui {
class DlgNewProject;
}

class DlgNewProject : public QDialog
{
    Q_OBJECT

public:
    explicit DlgNewProject(QString &sDefaultLocation, QWidget *parent = 0);
    ~DlgNewProject();

    QString GetProjPath();

private slots:

    void ErrorCheck();

    void on_buttonBox_accepted();

    void on_btnBrowse_clicked();

    void on_txtGameTitle_textChanged(const QString &arg1);

    void on_txtGameLocation_textChanged(const QString &arg1);

private:
    Ui::DlgNewProject *ui;
};


#endif // DLGNEWPROJECT_H
