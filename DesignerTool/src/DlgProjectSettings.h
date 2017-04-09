#ifndef DLGPROJECTSETTINGS_H
#define DLGPROJECTSETTINGS_H

#include <QDialog>

#include "Project.h"

namespace Ui {
class DlgProjectSettings;
}

class DlgProjectSettings : public QDialog
{
    Q_OBJECT

    Project &           m_ProjectRef;

public:
    explicit DlgProjectSettings(Project &projectRef, QWidget *parent = 0);
    ~DlgProjectSettings();

    void SaveSettings();

private:
    Ui::DlgProjectSettings *ui;
};

#endif // DLGPROJECTSETTINGS_H
