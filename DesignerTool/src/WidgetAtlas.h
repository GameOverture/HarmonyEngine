#ifndef WIDGETATLAS_H
#define WIDGETATLAS_H

#include <QWidget>

namespace Ui {
class WidgetAtlas;
}

class WidgetAtlas : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetAtlas(QWidget *parent = 0);
    ~WidgetAtlas();
    
private:
    Ui::WidgetAtlas *ui;
};

#endif // WIDGETATLAS_H
