#ifndef DLGCOLORPICKER_H
#define DLGCOLORPICKER_H

#include <QDialog>

namespace Ui {
class DlgColorPicker;
}

class DlgColorPicker : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgColorPicker(QWidget *parent = 0);
    ~DlgColorPicker();
    
private:
    Ui::DlgColorPicker *ui;
};

#endif // DLGCOLORPICKER_H
