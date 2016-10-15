#ifndef DLGCOLORPICKER_H
#define DLGCOLORPICKER_H

#include <QDialog>

namespace Ui {
class DlgColorPicker;
}

class DlgColorPicker : public QDialog
{
    Q_OBJECT

    QColor m_TopColor;
    QColor m_BotColor;
    
public:
    explicit DlgColorPicker(QWidget *parent = 0);
    ~DlgColorPicker();

    QColor GetTopColor();
    QColor GetBotColor();
    
private:
    Ui::DlgColorPicker *ui;
};

#endif // DLGCOLORPICKER_H
