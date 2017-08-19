#ifndef WIDGETGRADIENTEDITOR_H
#define WIDGETGRADIENTEDITOR_H

#include <QWidget>

namespace Ui {
class WidgetGradientEditor;
}

class WidgetGradientEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetGradientEditor(QWidget *parent = 0);
    ~WidgetGradientEditor();
    
private:
    Ui::WidgetGradientEditor *ui;
};

#endif // WIDGETGRADIENTEDITOR_H
