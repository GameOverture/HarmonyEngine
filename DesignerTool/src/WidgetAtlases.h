#ifndef WIDGETATLASES_H
#define WIDGETATLASES_H

#include <QWidget>

namespace Ui {
class WidgetAtlases;
}

class WidgetAtlases : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetAtlases(QWidget *parent = 0);
    ~WidgetAtlases();
    
private:
    Ui::WidgetAtlases *ui;
};

#endif // WIDGETATLASES_H
