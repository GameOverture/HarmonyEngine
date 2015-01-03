#include "DlgOpenProject.h"

#include "HyGlobal.h"

DlgOpenProject::DlgOpenProject(QObject *parent) : QObject(parent)
{
    m_Dlg.setFileMode(QFileDialog::Directory);
    m_Dlg.setOption(QFileDialog::ShowDirsOnly, true);

    m_Dlg.setViewMode(QFileDialog::Detail);
    m_Dlg.setWindowModality(Qt::ApplicationModal);
    m_Dlg.setModal(true);
    
    //m_Dlg.setAcceptMode(
    
    connect(&m_Dlg, SIGNAL(directoryEntered(QString)), this, SLOT(dirEntered(QString)));
}

int DlgOpenProject::Exec()
{
    return m_Dlg.exec();
}

QString DlgOpenProject::SelectedDir()
{
    return m_Dlg.selectedFiles()[0];
}

void DlgOpenProject::dirEntered(const QString &sDir)
{
    // TODO: none of this works because QFileDialog can't update once opened.
    if(HyGlobal::IsWorkspaceValid(sDir))
        m_Dlg.setLabelText(QFileDialog::Accept, QString("Valid Project!"));
    else
        m_Dlg.setLabelText(QFileDialog::Accept, QString("Invalid"));
}
