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
    
public:
    explicit DlgOpenProject(QWidget *parent = 0);
    ~DlgOpenProject();
    
    void showEvent(QShowEvent *pEvent);
    
    QString SelectedDir();
    
private slots:
    void on_treeView_clicked(const QModelIndex &index);
    
    void on_listView_doubleClicked(const QModelIndex &index);
    
    void on_txtCurDirectory_editingFinished();
    
private:
    Ui::DlgOpenProject *ui;
    
    QFileSystemModel *m_pDirModel;
    QFileSystemModel *m_pFileModel;
};

#endif // DLGOPENPROJECT_H
