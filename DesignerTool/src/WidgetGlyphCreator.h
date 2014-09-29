#ifndef WIDGETGLYPHCREATOR_H
#define WIDGETGLYPHCREATOR_H

#include <QWidget>

namespace Ui {
class WidgetGlyphCreator;
}

class WidgetGlyphCreator : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetGlyphCreator(QWidget *parent = 0);
    ~WidgetGlyphCreator();

private:
    Ui::WidgetGlyphCreator *ui;
};

#endif // WIDGETGLYPHCREATOR_H
