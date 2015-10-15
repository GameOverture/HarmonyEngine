#ifndef DLGATLASGROUPSETTINGS_H
#define DLGATLASGROUPSETTINGS_H

#include <QDialog>

namespace Ui {
class DlgAtlasGroupSettings;
}

class DlgAtlasGroupSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgAtlasGroupSettings(QWidget *parent = 0);
    ~DlgAtlasGroupSettings();
    
private:
    Ui::DlgAtlasGroupSettings *ui;
};

#endif // DLGATLASGROUPSETTINGS_H
