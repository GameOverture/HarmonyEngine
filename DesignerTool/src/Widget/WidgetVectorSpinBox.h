#ifndef WIDGETVECTORSPINBOX_H
#define WIDGETVECTORSPINBOX_H

#include <QWidget>

namespace Ui {
class WidgetVectorSpinBox;
}

class WidgetVectorSpinBox : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetVectorSpinBox(QWidget *parent = 0);
    virtual ~WidgetVectorSpinBox();

    void SetAsInt(bool bEnable);

private:
    Ui::WidgetVectorSpinBox *ui;
};

#endif // WIDGETVECTORSPINBOX_H
