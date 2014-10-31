#ifndef WIDGETSPRITE_H
#define WIDGETSPRITE_H

#include <QWidget>

namespace Ui {
class WidgetSprite;
}

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetSprite(QWidget *parent = 0);
    ~WidgetSprite();
    
private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
