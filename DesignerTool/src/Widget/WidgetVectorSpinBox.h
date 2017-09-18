#ifndef WIDGETVECTORSPINBOX_H
#define WIDGETVECTORSPINBOX_H

#include <QWidget>

namespace Ui {
class WidgetVectorSpinBox;
}

class WidgetVectorSpinBox : public QWidget
{
    Q_OBJECT

    enum PageIndex
    {
        PAGE_Int = 0,
        PAGE_Double
    };

public:
    explicit WidgetVectorSpinBox(QWidget *parent = 0);
    WidgetVectorSpinBox(bool bIsIntVector, QWidget *parent = 0);
    virtual ~WidgetVectorSpinBox();

    QVariant GetValue();
    void SetValue(QVariant data);

private:
    Ui::WidgetVectorSpinBox *ui;
};

#endif // WIDGETVECTORSPINBOX_H
