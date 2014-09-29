#ifndef WIDGETFONTMAP_H
#define WIDGETFONTMAP_H

#include <QWidget>

namespace Ui {
class WidgetFontMap;
}

class WidgetFontMap : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetFontMap(QWidget *parent = 0);
    ~WidgetFontMap();
    
private:
    Ui::WidgetFontMap *ui;
};

#endif // WIDGETFONTMAP_H
