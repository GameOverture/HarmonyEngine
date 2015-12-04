#ifndef DLGOPENPROJECT_H
#define DLGOPENPROJECT_H

#include <QDialog>
#include <QFileSystemModel>
#include <QShowEvent>

namespace Ui {
class DlgOpenProject;
}

class DlgOpenProject : public QDialog
{
    Q_OBJECT
    
    QFileSystemModel *m_pFileModel;

public:
    explicit DlgOpenProject(QWidget *parent = 0);
    ~DlgOpenProject();
    
    QString SelectedDir();
    
private slots:
    void on_listView_doubleClicked(const QModelIndex &index);
    
    void on_txtCurDirectory_editingFinished();
    
private:
    Ui::DlgOpenProject *ui;

    void ErrorCheck();
};

#endif // DLGOPENPROJECT_H
