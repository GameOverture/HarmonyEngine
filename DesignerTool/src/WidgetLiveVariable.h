#ifndef WIDGETLIVEVARIABLE_H
#define WIDGETLIVEVARIABLE_H

#include <QWidget>

namespace Ui {
class WidgetLiveVariable;
}

class WidgetLiveVariable : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetLiveVariable(QWidget *parent = 0);
    ~WidgetLiveVariable();
    
private:
    Ui::WidgetLiveVariable *ui;
};

#endif // WIDGETLIVEVARIABLE_H
